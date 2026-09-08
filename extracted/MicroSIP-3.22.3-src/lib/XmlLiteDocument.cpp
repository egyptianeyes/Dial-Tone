/*
 * Copyright (C) 2026 Egyptian Eyes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "stdafx.h"
#include "XmlLiteDocument.h"

#include <xmllite.h>

namespace
{
	bool SameName(const CString& left, LPCTSTR right)
	{
		return left.Compare(right) == 0;
	}

	bool WriteElement(IXmlWriter* writer, const DialToneXml::Element& element)
	{
		if (FAILED(writer->WriteStartElement(NULL, element.name, NULL))) return false;
		for (size_t i = 0; i < element.attributes.size(); ++i) {
			const DialToneXml::Attribute& attribute = element.attributes[i];
			if (FAILED(writer->WriteAttributeString(NULL, attribute.name, NULL, attribute.value))) return false;
		}
		if (!element.text.IsEmpty() && FAILED(writer->WriteString(element.text))) return false;
		for (size_t i = 0; i < element.children.size(); ++i) {
			if (!WriteElement(writer, element.children[i])) return false;
		}
		return SUCCEEDED(writer->WriteEndElement());
	}
}

namespace DialToneXml
{
	Element::Element()
	{
	}

	Element::Element(LPCTSTR elementName) : name(elementName)
	{
	}

	Element& Element::AddChild(LPCTSTR elementName)
	{
		children.push_back(Element(elementName));
		return children.back();
	}

	void Element::SetAttribute(LPCTSTR attributeName, const CString& value)
	{
		Attribute attribute;
		attribute.name = attributeName;
		attribute.value = value;
		attributes.push_back(attribute);
	}

	bool Element::HasAttribute(LPCTSTR attributeName) const
	{
		for (size_t i = 0; i < attributes.size(); ++i) {
			if (SameName(attributes[i].name, attributeName)) return true;
		}
		return false;
	}

	CString Element::GetAttribute(LPCTSTR attributeName) const
	{
		for (size_t i = 0; i < attributes.size(); ++i) {
			if (SameName(attributes[i].name, attributeName)) return attributes[i].value;
		}
		return CString();
	}

	const Element* Element::FindChild(LPCTSTR elementName, size_t start) const
	{
		for (size_t i = start; i < children.size(); ++i) {
			if (SameName(children[i].name, elementName)) return &children[i];
		}
		return NULL;
	}

	bool Parse(const CStringA& xml, Element& root)
	{
		root = Element();
		IStream* stream = NULL;
		IXmlReader* reader = NULL;
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &stream))) return false;

		ULONG written = 0;
		if (FAILED(stream->Write((LPCSTR)xml, xml.GetLength(), &written)) || written != (ULONG)xml.GetLength()) {
			stream->Release();
			return false;
		}
		LARGE_INTEGER start = {};
		if (FAILED(stream->Seek(start, STREAM_SEEK_SET, NULL))
			|| FAILED(CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&reader), NULL))
			|| FAILED(reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))
			|| FAILED(reader->SetInput(stream))) {
			if (reader) reader->Release();
			stream->Release();
			return false;
		}

		std::vector<Element*> stack;
		bool haveRoot = false;
		XmlNodeType type;
		HRESULT result;
		while ((result = reader->Read(&type)) == S_OK) {
			if (type == XmlNodeType_Element) {
				const WCHAR* name = NULL;
				if (FAILED(reader->GetLocalName(&name, NULL)) || !name) {
					result = E_FAIL;
					break;
				}
				Element* element = NULL;
				if (stack.empty()) {
					if (haveRoot) {
						result = E_FAIL;
						break;
					}
					root = Element(name);
					element = &root;
					haveRoot = true;
				}
				else {
					stack.back()->children.push_back(Element(name));
					element = &stack.back()->children.back();
				}

				if (reader->MoveToFirstAttribute() == S_OK) {
					do {
						const WCHAR* attributeName = NULL;
						const WCHAR* value = NULL;
						if (FAILED(reader->GetLocalName(&attributeName, NULL))
							|| FAILED(reader->GetValue(&value, NULL)) || !attributeName || !value) {
							result = E_FAIL;
							break;
						}
						element->SetAttribute(attributeName, value);
					} while (reader->MoveToNextAttribute() == S_OK);
					reader->MoveToElement();
					if (FAILED(result)) break;
				}
				if (!reader->IsEmptyElement()) stack.push_back(element);
			}
			else if (type == XmlNodeType_EndElement) {
				if (stack.empty()) {
					result = E_FAIL;
					break;
				}
				stack.pop_back();
			}
			else if (type == XmlNodeType_Text || type == XmlNodeType_CDATA
				|| type == XmlNodeType_Whitespace || type == XmlNodeType_SignificantWhitespace) {
				if (!stack.empty()) {
					const WCHAR* value = NULL;
					if (FAILED(reader->GetValue(&value, NULL)) || !value) {
						result = E_FAIL;
						break;
					}
					stack.back()->text.Append(value);
				}
			}
		}

		reader->Release();
		stream->Release();
		if (result != S_FALSE || !haveRoot || !stack.empty()) {
			root = Element();
			return false;
		}
		return true;
	}

	bool Write(const Element& root, CStringA& xml)
	{
		xml.Empty();
		if (root.name.IsEmpty()) return false;

		IStream* stream = NULL;
		IXmlWriter* writer = NULL;
		IXmlWriterOutput* output = NULL;
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &stream))) return false;
		if (FAILED(CreateXmlWriter(__uuidof(IXmlWriter), reinterpret_cast<void**>(&writer), NULL))
			|| FAILED(CreateXmlWriterOutputWithEncodingName(stream, NULL, L"utf-8", &output))
			|| FAILED(writer->SetOutput(output))
			|| !WriteElement(writer, root)
			|| FAILED(writer->Flush())) {
			if (output) output->Release();
			if (writer) writer->Release();
			stream->Release();
			return false;
		}

		HGLOBAL memory = NULL;
		if (FAILED(GetHGlobalFromStream(stream, &memory))) {
			output->Release();
			writer->Release();
			stream->Release();
			return false;
		}
		STATSTG statistics = {};
		if (FAILED(stream->Stat(&statistics, STATFLAG_NONAME)) || statistics.cbSize.HighPart != 0) {
			output->Release();
			writer->Release();
			stream->Release();
			return false;
		}
		SIZE_T length = statistics.cbSize.LowPart;
		const BYTE* bytes = static_cast<const BYTE*>(GlobalLock(memory));
		if (!bytes) {
			output->Release();
			writer->Release();
			stream->Release();
			return false;
		}
		SIZE_T offset = length >= 3 && bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf ? 3 : 0;
		xml.SetString(reinterpret_cast<LPCSTR>(bytes + offset), static_cast<int>(length - offset));
		GlobalUnlock(memory);

		output->Release();
		writer->Release();
		stream->Release();
		return true;
	}
}
