#pragma once

#include "line.h"

struct Font;
struct Rectangle;
struct Color;

class MultiLineTextArea {
private:
	Font* font; // TODO: should be able to use more than one font
	float fontSize, chWidth;

	Line** text;
	size_t numLines, maxLines;
	size_t visLin, visCol, firstLin, firstCol;
	size_t lin, col;

	bool focused;
	size_t selStartLin, selStartCol, selEndLin, selEndCol;

	Rectangle* rec;
	float padding;
	Color* recColor;
	void MergeLines(Line* destination, Line* source);
	void InsertLine(size_t pos, char* str);
	void InsertLine(size_t pos, Line* line);
	void EraseSelection();
	void ToPreviousLine();
	void ToNextLine();
	void ToPreviousColumn();
	void ToNextColumn();
	void Enter();
	void Backspace();
	void Paste();
	void Copy();
public:
	MultiLineTextArea(float startX, float startY, size_t visibleLines, size_t visibleColumns, const char* fontPath, float fontSize_, float padding_, Color color_);
	~MultiLineTextArea();
	void Draw();
	Line** ParseText(char* str, size_t& size);
	void PushLine(char* m_data = nullptr);
	void PushLine(Line* line);
	void Edit();
};