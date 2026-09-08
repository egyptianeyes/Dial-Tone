#pragma once

namespace DarkPalette
{
	inline COLORREF Window() { return RGB(30, 34, 38); }
	inline COLORREF Surface() { return RGB(42, 47, 52); }
	inline COLORREF Active() { return RGB(52, 58, 64); }
	inline COLORREF Selected() { return RGB(61, 68, 75); }
	inline COLORREF Border() { return RGB(78, 85, 92); }
	inline COLORREF Separator() { return RGB(60, 66, 72); }
	inline COLORREF Text() { return RGB(235, 238, 241); }
	inline COLORREF SecondaryText() { return RGB(175, 183, 191); }
	inline COLORREF DisabledText() { return RGB(115, 123, 131); }
	inline COLORREF Input() { return RGB(35, 40, 45); }
	inline COLORREF Accent(bool darkMode) { return darkMode ? RGB(64, 102, 136) : RGB(48, 96, 140); }
	inline COLORREF SemanticSuccess(bool darkMode) { return darkMode ? RGB(92, 190, 120) : RGB(38, 120, 72); }
	inline COLORREF SemanticProgress(bool darkMode) { return darkMode ? RGB(225, 170, 75) : RGB(176, 112, 20); }
	inline COLORREF SemanticWarning(bool darkMode) { return darkMode ? RGB(235, 180, 80) : RGB(166, 101, 12); }
	inline COLORREF SemanticError(bool darkMode) { return darkMode ? RGB(235, 105, 105) : RGB(175, 55, 55); }
	inline COLORREF SecondaryText(bool darkMode) { return darkMode ? SecondaryText() : RGB(100, 100, 100); }
	inline COLORREF Link(bool darkMode) { return darkMode ? RGB(130, 160, 185) : RGB(55, 95, 130); }
}
