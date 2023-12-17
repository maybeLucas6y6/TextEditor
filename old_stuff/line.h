#pragma once

class Line {
private:
	char* m_data;
	size_t m_size, m_capacity;
public:
	Line();
	Line(char* str);
	~Line();
	size_t size() const;
	size_t capacity() const;
	char* data() const;
	void InsertCharacter(size_t pos, char c);
	void EraseCharacter(size_t pos);
	void InsertString(size_t pos, char* str, bool keepLeftoverText);
	void EraseString(size_t pos);
	void SetData(char* str);
};