#include "line.h"

#include <cstring>

Line::Line() {
	m_data = new char[1];
	m_data[0] = 0;
	m_size = 1;
	m_capacity = 1;
}
Line::Line(char* str) {
	if (str != nullptr) {
		size_t n = strlen(str) + 1;
		m_data = new char[n];
		strcpy(m_data, str);
		m_size = m_capacity = n;
	}
	else {
		m_data = nullptr;
		m_size = 0;
		m_capacity = 0;
	}
}
Line::~Line() {
	if (m_data != nullptr) {
		delete[] m_data;
	}
}
size_t Line::size() const {
	return m_size - 1;
}
size_t Line::capacity() const {
	return m_capacity;
}
char* Line::data() const {
	return m_data;
}
void Line::InsertCharacter(size_t pos, char c) {
	if (m_size == m_capacity) {
		m_capacity += m_capacity;
		char* temp = new char[m_capacity];
		memcpy(temp, m_data, m_size);
		delete[] m_data;
		m_data = temp;
	}
	char* temp = new char[m_capacity - pos];
	memcpy(temp, m_data + pos, m_size - pos);
	memcpy(m_data + pos + 1, temp, m_size - pos);
	m_data[pos] = c;
	delete[] temp;
	m_size++;
}
void Line::EraseCharacter(size_t pos) {
	strcpy(m_data + pos, m_data + pos + 1);
	m_size--;
}
void Line::InsertString(size_t pos, char* str, bool keepLeftoverText) {
	size_t n = strlen(str) + 1;
	/*if (n == 1) {
		return;
	}*/
	if (keepLeftoverText == false) {
		if (pos + n > m_capacity) {
			m_capacity = pos + n;
			char* temp = new char[m_capacity];
			memcpy(temp, m_data, m_size);
			delete[] m_data;
			m_data = temp;
		}
		memcpy(m_data + pos, str, n);
		m_size = pos + n;
	}
	else {
		if (m_size + n - 1 > m_capacity) {
			m_capacity = m_size - 1 + n;
		}
		char* temp = new char[m_capacity];
		memcpy(temp, m_data, pos);
		memcpy(temp + pos, str, n - 1);
		memcpy(temp + pos + n - 1, m_data + pos, m_size - pos);
		delete[] m_data;
		m_data = temp;
		m_size = m_size + n;
	}
}
void Line::EraseString(size_t pos) {
	if (pos < m_size) {
		m_data[pos] = 0;
		m_size = pos + 1;
	}
}
void Line::SetData(char* str) {
	size_t n = strlen(str) + 1;
	char* temp = new char[m_capacity];
	memcpy(temp, m_data, m_size);
	delete[] m_data;
	m_data = temp;
	m_size = m_capacity = n;
}