#pragma once

#include <string> // TODO: nu stiu cum sa nu pun asta aici
#include <vector> // TODO: nu stiu cum sa nu pun asta aici
#include "raylib.h"

struct Font;
struct Rectangle;
struct Color;

class MultiLineTextArea {
private:
	Font font; // TODO: should be able to use more than one font
	float fontSize, chWidth;

	std::vector<std::string> text;
	size_t visLin, visCol, firstLin, firstCol;
	size_t lin, col;

	bool focused;
	bool validSelection;
	size_t selStartLin, selStartCol, selEndLin, selEndCol;

	Rectangle rec;
	float padding;
	Color textColor, bgColor;
	void MergeLines(size_t destination, size_t source);
	void InsertLine(size_t pos, std::string line);
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
	MultiLineTextArea(float startX, float startY, size_t visibleLines, size_t visibleColumns, Font font, float fontSize, float padding, Color textColor, Color bgColor);
	~MultiLineTextArea();
	void Draw();
	std::vector<std::string> ParseText(std::string strToParse);
	void PushLine(std::string line = "");
	void Edit();
};