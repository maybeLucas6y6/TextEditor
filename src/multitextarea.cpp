#include "multitextarea.h"

#include "raylib.h"
#include <iostream>

void MultiLineTextArea::MergeLines(size_t destination, size_t source) {
	text[destination] += text[source];
	text.erase(text.begin() + source);
}
void MultiLineTextArea::InsertLine(size_t pos, std::string line) {
	text.insert(text.begin() + pos, line);
}
void MultiLineTextArea::EraseSelection() {
	size_t sl, sc, el, ec;
	if (selStartLin < selEndLin) {
		sl = selStartLin;
		el = selEndLin;
	}
	else {
		sl = selEndLin;
		el = selStartLin;
	}
	if (selStartCol < selEndCol) {
		sc = selStartCol;
		ec = selEndCol;
	}
	else {
		sc = selEndCol;
		ec = selStartCol;
	}

	if (sl != el) {
		size_t nlin = el - sl - 1;
		text.erase(text.begin() + sl + 1, text.begin() + sl + 1 + nlin);

		text[sl].erase(sc);
		text[sl] += text[el].substr(ec);
		text.erase(text.begin() + el);
	}
	else {
		text[sl].erase(sc, ec - sc);
	}
	selStartLin = selStartCol = selEndLin = selEndCol = 0;
}
void MultiLineTextArea::ToPreviousLine() {
	if (lin > 0) {
		lin--;
		if (lin < firstLin) {
			firstLin = lin;
		}
		if (col > text[lin].size()) {
			col = text[lin].size();
			if (col < firstCol) {
				firstCol = col;
			}
		}
	}
}
void MultiLineTextArea::ToNextLine() {
	if (lin < text.size() - 1) {
		lin++;
		if (lin >= firstLin + visLin) {
			firstLin++;
		}
		if (col > text[lin].size()) {
			col = text[lin].size();
			if (col < firstCol) {
				firstCol = col;
			}
		}
	}
}
void MultiLineTextArea::ToPreviousColumn() {
	if (col > 0) {
		col--;
		if (col < firstCol) {
			firstCol = col;
		}
	}
	else if (lin > 0) {
		lin--;
		if (lin < firstLin) {
			firstLin = lin;
		}
		col = text[lin].size();
		if (col >= firstCol + visCol) {
			firstCol = col - visCol + 1;
		}
	}
}
void MultiLineTextArea::ToNextColumn() {
	if (col < text[lin].size()) {
		col++;
		if (col >= firstCol + visCol) {
			firstCol++;
		}
	}
	else if (lin < text.size() - 1) {
		col = 0;
		firstCol = 0;
		lin++;
		if (lin >= firstLin + visLin) {
			firstLin++;
		}
	}
}
void MultiLineTextArea::Enter() {
	lin++;
	if (lin >= firstLin + visLin) {
		firstLin++;
	}
	InsertLine(lin, text[lin - 1].substr(col));
	text[lin - 1].erase(col);
	col = 0;
	firstCol = 0;
}
void MultiLineTextArea::Backspace() {
	if (!(selStartLin == selEndLin && selStartCol == selEndCol)) {
		col = selStartCol < selEndCol ? selStartCol : selEndCol;
		if (col < firstCol) {
			firstCol = col;
		}
		lin = selStartLin < selEndLin ? selStartLin : selEndLin;
		if (lin < firstLin) {
			firstLin = lin;
		}
		EraseSelection();
	}
	else if (col > 0) {
		col--;
		text[lin].erase(col, 1);
		if (col < firstCol) {
			firstCol--;
		}
	}
	else if (lin > 0) {
		lin--;
		if (lin < firstLin) {
			firstLin = lin;
		}
		col = text[lin].size();
		if (col >= firstCol + visCol) {
			firstCol = col - 1;
		}
		MergeLines(lin, lin + 1);
	}
}
void MultiLineTextArea::Paste() {
	std::string clipboard = GetClipboardText();
	size_t n = clipboard.size();
	if (n == 0) return;

	if (!(selStartLin == selEndLin && selStartCol == selEndCol)) {
		EraseSelection();
	}

	std::vector<std::string> temp = ParseText(clipboard);
	lin = lin + temp.size() - 1;
	col = temp[temp.size() - 1].size();

	text = temp;
}
void MultiLineTextArea::Copy() {
	if (selStartLin == selEndLin && selStartCol == selEndCol) return;
	size_t sl, sc, el, ec;
	if (selStartLin < selEndLin) {
		sl = selStartLin;
		el = selEndLin;
	}
	else {
		sl = selEndLin;
		el = selStartLin;
	}
	if (selStartCol < selEndCol) {
		sc = selStartCol;
		ec = selEndCol;
	}
	else {
		sc = selEndCol;
		ec = selStartCol;
	}

	if (sl == el) {
		SetClipboardText(text[sl].substr(sc, ec - sc).c_str());
	}
	else {
		std::string res;
		res += text[sl].substr(sc);
		res += "\r\n";
		for (size_t i = sl + 1; i < el; i++) {
			res += text[i];
			res += "\r\n";
		}
		res += text[el].substr(0, ec);
		if (text[el].size() == ec) {
			res += "\r\n";
		}
		SetClipboardText(res.c_str());
	}

	selStartLin = selStartCol = selEndLin = selEndCol = 0;
}
MultiLineTextArea::MultiLineTextArea(float startX, float startY, size_t visibleLines, size_t visibleColumns, Font font, float fontSize, float padding, Color textColor, Color bgColor)
	: visLin(visibleLines)
	, visCol(visibleColumns)
	//, startX, startY
	, font(font)
	, fontSize(fontSize)
	, padding(padding)
	, textColor(textColor)
	, bgColor(bgColor)
{
	chWidth = (fontSize * font.recs->width / font.baseSize) + font.glyphPadding / 2.0f;

	col = lin = 0;
	text.push_back(std::string());

	focused = false;
	selStartLin = selStartCol = selEndLin = selEndCol = 0;

	firstLin = firstCol = 0;
	rec = { startX, startY, visibleColumns * chWidth + 2.0f * padding, visibleLines * fontSize + 2.0f * padding }; // TODO: this won t work for any font
}
MultiLineTextArea::~MultiLineTextArea() {
	
}
void MultiLineTextArea::Draw() {
	DrawRectangle((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, bgColor);

	for (size_t i = firstLin, posLin = 0; i < text.size() && i < firstLin + visLin; i++, posLin++) {
		if (text[i].empty()) continue;
		if (text[i].size() < firstCol) continue;

		size_t n = (firstCol + visCol) > text[i].size() ? text[i].size() - firstCol + 1 : visCol;
		std::string cropped = text[i].substr(firstCol, n);

		if (!(selStartLin == selEndLin && selStartCol == selEndCol)) {
			size_t sl, sc, el, ec;
			if (selStartLin < selEndLin) {
				sl = selStartLin;
				el = selEndLin;
			}
			else {
				sl = selEndLin;
				el = selStartLin;
			}
			if (selStartCol < selEndCol) {
				sc = selStartCol;
				ec = selEndCol;
			}
			else {
				sc = selEndCol;
				ec = selStartCol;
			}

			Color selColor = { 0, 120, 250, 100 };
			if (i > sl && i < el) {
				DrawRectangleRec({ rec.x + padding, rec.y + posLin * fontSize + padding, n * chWidth, fontSize }, selColor);
			}
			if (i == sl && sl == el) {
				DrawRectangleRec({ rec.x + padding + (sc - firstCol) * chWidth, rec.y + padding + posLin * fontSize, (ec - sc) * chWidth, fontSize }, selColor);
			}
			else if (i == sl) {
				DrawRectangleRec({ rec.x + padding + (sc - firstCol) * chWidth, rec.y + padding + posLin * fontSize, (n - sc + firstCol) * chWidth, fontSize }, selColor);
			}
			else if (i == el) {
				DrawRectangleRec({ rec.x + padding, rec.y + padding + posLin * fontSize, (ec - firstCol) * chWidth, fontSize }, selColor);
			}
		}
		// TODO: font m_size should not be always default
		DrawTextEx(font, cropped.c_str(), {rec.x + padding, rec.y + posLin * fontSize + padding}, fontSize, 2, textColor);
	}

	char x[8] = { 0 };
	char pos[64] = { 0 };
	_itoa((int)lin, x, 10);
	strcat(pos, x);
	_itoa((int)col, x, 10);
	strcat(pos, ":");
	strcat(pos, x);
	_itoa((int)firstLin, x, 10);
	strcat(pos, " / ");
	strcat(pos, x);
	_itoa((int)firstCol, x, 10);
	strcat(pos, ":");
	strcat(pos, x);
	strcat(pos, " / ");
	_itoa((int)selStartLin, x, 10);
	strcat(pos, x);
	strcat(pos, ":");
	_itoa((int)selStartCol, x, 10);
	strcat(pos, x);
	strcat(pos, "|");
	_itoa((int)selEndLin, x, 10);
	strcat(pos, x);
	strcat(pos, ":");
	_itoa((int)selEndCol, x, 10);
	strcat(pos, x);
	DrawText(pos, 300, 300, 20, BLACK);

	if (focused) {
		// TODO: figure out how to calculate text width based on a given font
		DrawLineV({ rec.x + padding + (col - firstCol) * chWidth, rec.y + padding + (lin - firstLin) * fontSize }, { rec.x + padding + (col - firstCol) * chWidth, rec.y + padding + (lin - firstLin + 1) * fontSize }, RED);
	}
}
std::vector<std::string> MultiLineTextArea::ParseText(std::string strToParse) {
	size_t n = strToParse.size();
	std::vector<std::string> temp;
	size_t prev = 0;
	for (size_t i = 0; i < n; i++) {
		if (strToParse[i] == '\n') {
			temp.push_back(strToParse.substr(prev, i - prev - 1));
			prev = i + 1;
		}
	}
	temp.push_back(strToParse.substr(prev, strToParse[n - 1] == '\n' ? n - prev - 2 : n - prev)); // TODO: might break

	for (std::string& str : temp) {
		size_t s = str.size();
		for (size_t i = 0; i < str.size();) {
			if (str[i] == '\t') {
				str.erase(i, 1);
				str.insert(i, 4, ' ');
				i += 4;
			}
			else {
				i++;
			}
		}
	}

	return temp;
}
void MultiLineTextArea::PushLine(std::string line) {
	text.push_back(line);
}
void MultiLineTextArea::Edit() {
	auto mpos = GetMousePosition();
	bool inBounds = (mpos.x >= rec.x && mpos.x < rec.x + rec.width) && (mpos.y >= rec.y && mpos.y < rec.y + rec.height);
	if (inBounds) {
		SetMouseCursor(MOUSE_CURSOR_IBEAM); // TODO: shouldn t set this that often
	}
	else {
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (inBounds) {
			focused = true;
		}
		else {
			focused = false;
		}
	}

	int chr = GetCharPressed();
	int key = GetKeyPressed();
	if (focused && (chr != 0 || key != 0)) {
		switch (key) {
		case KEY_UP: { ToPreviousLine(); break; }
		case KEY_DOWN: { ToNextLine(); break; }
		case KEY_LEFT: { ToPreviousColumn(); break; }
		case KEY_RIGHT: { ToNextColumn(); break; }
		case KEY_ENTER: { Enter(); break; }
		case KEY_BACKSPACE: { Backspace(); break; }
		default:
			if (chr != 0) {
				text[lin].insert(col, 1, chr);
				if (text[lin].size() >= visCol) {
					firstCol++;
				}
				col++;
			}
		}
		/*for (size_t i = 0; i < numLines; i++) {
			std::cout << i << "/" << maxLines << ". " << text[i]->data() << ", " << text[i]->size() << "/" << text[i]->capacity() << "\n";
		}
		std::cout << "\n";*/
	}
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
		// TODO: save to file
		std::cout << "Command\n";
	}
	else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		Paste();
	}
	else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
		Copy();
	}
	else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
		selStartLin = selEndCol = 0;
		selEndLin = text.size() - 1;
		selEndCol = text[selEndLin].size();
	}

	if (IsKeyPressed(KEY_LEFT_SHIFT)) {
		selStartLin = lin;
		selStartCol = col;
	}
	if (IsKeyDown(KEY_LEFT_SHIFT)) {
		selEndLin = lin;
		selEndCol = col;
	}
}