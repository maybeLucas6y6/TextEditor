#include "raylib.h"
#include <cstring>
#include <iostream>

#include "multitextarea.h"

void MultiLineTextArea::MergeLines(Line* destination, Line* source) {
	destination->InsertString(destination->size(), source->data(), false);
	for (size_t i = lin + 1; i < numLines - 1; i++) {
		text[i] = text[i + 1];
	}
	numLines--;
	text[numLines] = nullptr;
}
void MultiLineTextArea::InsertLine(size_t pos, char* str) {
	if (numLines < maxLines) {
		for (size_t i = maxLines - 1; i > pos; i--) {
			text[i] = text[i - 1];
		}
	}
	else {
		maxLines += maxLines;
		Line** newText = new Line * [maxLines]; // TODO: newText ar putea da nullptr
		for (size_t i = 0; i < numLines; i++) {
			newText[i + (i >= pos)] = text[i];
		}
		delete[] text;
		text = newText;
	}
	text[pos] = new Line(str);
	numLines++;
}
void MultiLineTextArea::InsertLine(size_t pos, Line* line) {
	if (numLines < maxLines) {
		for (size_t i = maxLines - 1; i > pos; i--) {
			text[i] = text[i - 1];
		}
	}
	else {
		maxLines += maxLines;
		Line** newText = new Line * [maxLines]; // TODO: newText ar putea da nullptr
		for (size_t i = 0; i < numLines; i++) {
			newText[i + (i >= pos)] = text[i];
		}
		delete[] text;
		text = newText;
	}
	text[pos] = line;
	numLines++;
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
		if (nlin > 0) {
			for (size_t i = sl + 1; i + nlin < numLines; i++) {
				delete text[i];
				if (i + sl < numLines) {
					text[i] = text[i + nlin];
				}
				else {
					text[i] = nullptr;
				}
			}
		}
		numLines -= nlin;

		text[sl]->EraseString(sc);
		text[sl]->InsertString(sc, text[el]->data() + ec, false);
		delete text[el];
		for (size_t i = el; i < numLines; i++) {
			text[i] = text[i + 1];
		}
		numLines--;
	}
	else {
		char* temp = new char[text[sl]->size() - ec + 1];
		strcpy(temp, text[sl]->data() + ec);
		text[sl]->InsertString(sc, temp, false);
		delete[] temp;
	}
	selStartLin = selStartCol = selEndLin = selEndCol = 0;
}
inline void MultiLineTextArea::ToPreviousLine() {
	if (lin > 0) {
		lin--;
		if (lin < firstLin) {
			firstLin = lin;
		}
		if (col > text[lin]->size()) {
			col = text[lin]->size();
			if (col < firstCol) {
				firstCol = col;
			}
		}
	}
}
inline void MultiLineTextArea::ToNextLine() {
	if (lin < numLines - 1) {
		lin++;
		if (lin >= firstLin + visLin) {
			firstLin++;
		}
		if (col > text[lin]->size()) {
			col = text[lin]->size();
			if (col < firstCol) {
				firstCol = col;
			}
		}
	}
}
inline void MultiLineTextArea::ToPreviousColumn() {
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
		col = text[lin]->size();
		if (col >= firstCol + visCol) {
			firstCol = col - visCol + 1;
		}
	}
}
inline void MultiLineTextArea::ToNextColumn() {
	if (col < text[lin]->size()) {
		col++;
		if (col >= firstCol + visCol) {
			firstCol++;
		}
	}
	else if (lin < numLines - 1) {
		col = 0;
		firstCol = 0;
		lin++;
		if (lin >= firstLin + visLin) {
			firstLin++;
		}
	}
}
inline void MultiLineTextArea::Enter() {
	lin++;
	if (lin >= firstLin + visLin) {
		firstLin++;
	}
	InsertLine(lin, text[lin - 1]->data() + col);
	text[lin - 1]->EraseString(col);
	col = 0;
	firstCol = 0;
}
inline void MultiLineTextArea::Backspace() {
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
		text[lin]->EraseCharacter(col);
		if (col < firstCol) {
			firstCol--;
		}
	}
	else if (lin > 0) {
		lin--;
		if (lin < firstLin) {
			firstLin = lin;
		}
		col = text[lin]->size();
		if (col >= firstCol + visCol) {
			firstCol = col - 1;
		}
		MergeLines(text[lin], text[lin + 1]);
	}
}
void MultiLineTextArea::Paste() {
	const char* clipboard = GetClipboardText();
	size_t n = strlen(clipboard);
	if (n == 0) return;

	if (!(selStartLin == selEndLin && selStartCol == selEndCol)) {
		EraseSelection();
	}

	char* cb = new char[n + 1];
	strcpy(cb, clipboard);

	size_t x;
	Line** cbtext = nullptr;
	if (strchr(cb, '\n') != nullptr) {
		cbtext = ParseText(cb, x);
	}
	else {
		text[lin]->InsertString(col, cb, true);
		col = col + n;
		return;
	}

	char* temp = nullptr;
	if (text[lin]->size() > 0) {
		temp = new char[text[lin]->size() - col + 1];
		strcpy(temp, text[lin]->data() + col);
	}

	size_t l = 0;
	text[lin]->InsertString(col, cbtext[l]->data(), false);
	l++;
	for (; l < x; l++) {
		InsertLine(lin + l, cbtext[l]);
	}
	if (temp != nullptr) {
		InsertLine(lin + l, temp);
	}

	lin = lin + x - 1;
	if (lin >= firstLin + visLin) {
		firstLin = lin - visLin + 1;
	}
	col = cbtext[x - 1]->size();
	if (col < firstCol) {
		firstCol = col - 1;
	}
	else if (col >= firstCol + visCol) {
		firstCol = col - visCol + 1;
	}

	if (cb != nullptr) delete[] cb;
	if (temp != nullptr) delete[] temp;
	if (cbtext != nullptr) delete[] cbtext;
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
		size_t n = ec - sc;
		char* str = new char[n + 1];
		memcpy(str, text[sl]->data() + sc, n);
		str[n] = 0;
		SetClipboardText(str);
		delete[] str;
	}
	else {
		size_t n = 0, nls = 1;
		n += text[sl]->size() - sc;
		for (size_t i = sl + 1; i < el; i++) {
			n += text[i]->size();
			nls++;
		}
		n += ec;
		if (ec == text[el]->size()) nls++;
		char* str = new char[n + 2 * nls + 1];
		size_t pos = 0;
		memcpy(str, text[sl]->data() + sc, text[sl]->size());
		pos += text[sl]->size() - sc;
		str[pos] = '\r';
		str[pos + 1] = '\n';
		pos += 2;
		for (size_t i = sl + 1; i < el; i++) {
			memcpy(str + pos, text[i]->data(), text[i]->size());
			pos += text[i]->size();
			str[pos] = '\r';
			str[pos + 1] = '\n';
			pos += 2;
		}
		memcpy(str + pos, text[el]->data(), ec);
		pos += ec;
		if (ec == text[el]->size()) {
			str[pos] = '\r';
			str[pos + 1] = '\n';
			pos += 2;
		}
		str[pos] = 0;
		SetClipboardText(str);
		delete[] str;
	}

	selStartLin = selStartCol = selEndLin = selEndCol = 0;
}
MultiLineTextArea::MultiLineTextArea(float startX, float startY, size_t visibleLines, size_t visibleColumns, const char* fontPath, float fontSize_, float padding_, Color color_) {
	if (fontPath != nullptr) {
		font = new Font(LoadFont(fontPath));
	}
	else {
		font = new Font(GetFontDefault());
	}
	fontSize = fontSize_;

	numLines = 1;
	maxLines = 1;
	text = new Line * [maxLines];
	text[0] = new Line;
	lin = col = 0;

	focused = true;
	selStartLin = selStartCol = selEndLin = selEndCol = 0;

	padding = padding_;
	visLin = visibleLines;
	visCol = visibleColumns;
	firstLin = firstCol = 0;
	rec = new Rectangle{ startX, startY, visibleColumns * fontSize / 2.0f + 2.0f * padding, visibleLines * fontSize + 2.0f * padding };// TODO: this won t work for any font
	recColor = new Color(color_);
}
MultiLineTextArea::~MultiLineTextArea() {
	if (text != nullptr) {
		for (size_t i = 0; i < numLines; i++) {
			if (text[i] != nullptr) {
				delete text[i];
			}
		}
		delete[] text;
	}
	if (font != nullptr) {
		UnloadFont(*font);
		delete font;
	}
	delete rec;
	delete recColor;
}
void MultiLineTextArea::Draw() {
	DrawRectangle((int)rec->x, (int)rec->y, (int)rec->width, (int)rec->height, *recColor);
	float chw = fontSize / 2.0f;

	for (size_t i = firstLin, posLin = 0; i < numLines && i < firstLin + visLin; i++, posLin++) {
		if (text[i] == nullptr) continue;
		if (text[i]->size() < firstCol) continue;

		char* cropped = new char[visCol];
		size_t n = (firstCol + visCol) > text[i]->size() ? text[i]->size() - firstCol + 1 : visCol;
		strncpy(cropped, text[i]->data() + firstCol, n);
		cropped[n - 1] = 0;

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
				DrawRectangleRec({ rec->x + padding, rec->y + posLin * fontSize + padding, n * chw, fontSize }, selColor);
			}
			if (i == sl && sl == el) {
				DrawRectangleRec({ rec->x + padding + (sc - firstCol) * chw, rec->y + padding + posLin * fontSize, (ec - sc) * chw, fontSize }, selColor);
			}
			else if (i == sl) {
				DrawRectangleRec({ rec->x + padding + (sc - firstCol) * chw, rec->y + padding + posLin * fontSize, (n - sc + firstCol) * chw, fontSize }, selColor);
			}
			else if (i == el) {
				DrawRectangleRec({ rec->x + padding, rec->y + padding + posLin * fontSize, (ec - firstCol) * chw, fontSize }, selColor);
			}
		}
		// TODO: font m_size should not be always default
		DrawTextEx(*font, cropped, { rec->x + padding, rec->y + posLin * fontSize + padding }, fontSize, 2, BLACK);
		delete[] cropped;
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

	// TODO: figure out how to calculate text width based on a given font
	DrawLineV({ rec->x + padding + (col - firstCol) * chw, rec->y + padding + (lin - firstLin) * fontSize }, { rec->x + padding + (col - firstCol) * chw, rec->y + padding + (lin - firstLin + 1) * fontSize }, RED);
}
Line** MultiLineTextArea::ParseText(char* str, size_t& size) {
	size = 0;
	size_t n = strlen(str);
	if (n == 0) {
		return nullptr;
	}

	size_t ts = 0;
	for (size_t i = 0; i < n; i++) {
		if (str[i] == '\n') {
			size++;
		}
		else if (str[i] == '\t') {
			ts++;
		}
	}
	size++;

	size_t j = 0, m = n + 4 * ts;
	char* temp = new char[m + 1];
	for (size_t i = 0; i <= n; i++) {
		if (str[i] == '\t') {
			temp[j++] = ' ';
			temp[j++] = ' ';
			temp[j++] = ' ';
			temp[j++] = ' ';
		}
		else {
			temp[j++] = str[i];
		}
	}

	Line** res = new Line * [size];
	size_t prev = 0, l = 0;
	for (size_t i = 0; i < m; i++) {
		if (temp[i] == '\n') {
			temp[i - 1] = 0;
			res[l] = new Line(temp + prev);
			l++;
			prev = i + 1;
		}
	}
	res[l] = new Line(temp + prev);
	return res;
}
void MultiLineTextArea::PushLine(char* m_data) {
	if (numLines == maxLines) {
		maxLines += maxLines;
		Line** newText = new Line * [maxLines];
		for (size_t i = 0; i < numLines; i++) {
			newText[i] = text[i];
		}
		delete[] text;
		text = newText;
	}
	text[numLines] = new Line(m_data);
	numLines++;
}
void MultiLineTextArea::PushLine(Line* line) {
	if (numLines == maxLines) {
		maxLines += maxLines;
		Line** newText = new Line * [maxLines];
		for (size_t i = 0; i < numLines; i++) {
			newText[i] = text[i];
		}
		delete[] text;
		text = newText;
	}
	text[numLines] = line;
	numLines++;
}
void MultiLineTextArea::Edit() {
	auto mpos = GetMousePosition();
	if ((mpos.x >= rec->x && mpos.x < rec->x + rec->width) && (mpos.y >= rec->y && mpos.y < rec->y + rec->height)) {
		SetMouseCursor(MOUSE_CURSOR_IBEAM); // TODO: shouldn t set this that often
	}
	else {
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
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
				text[lin]->InsertCharacter(col, chr);
				if (text[lin]->size() >= visCol) {
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
		selEndLin = numLines - 1;
		selEndCol = text[selEndLin]->size();
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