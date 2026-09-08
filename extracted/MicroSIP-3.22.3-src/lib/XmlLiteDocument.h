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

#pragma once

#include <vector>

namespace DialToneXml
{
	struct Attribute
	{
		CString name;
		CString value;
	};

	struct Element
	{
		CString name;
		CString text;
		std::vector<Attribute> attributes;
		std::vector<Element> children;

		Element();
		explicit Element(LPCTSTR elementName);

		Element& AddChild(LPCTSTR elementName);
		void SetAttribute(LPCTSTR attributeName, const CString& value);
		bool HasAttribute(LPCTSTR attributeName) const;
		CString GetAttribute(LPCTSTR attributeName) const;
		const Element* FindChild(LPCTSTR elementName, size_t start = 0) const;
	};

	bool Parse(const CStringA& xml, Element& root);
	bool Write(const Element& root, CStringA& xml);
}
