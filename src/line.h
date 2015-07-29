#pragma once
#include "position.h"
#include <algorithm>
#include <cassert>

class CLine
{
public:
	uint8_t size;
	uint8_t * line;

	CLine() : size(0), line(0) {}
	CLine(const CLine& o) : size(o.size) { line = new uint8_t[size]; std::copy_n(o.line, size, line); }
	CLine(const uint8_t size) : size(size) { line = new uint8_t[size ? size : 1]; std::fill_n(line, size, 64); }
	CLine & operator=(const CLine& other) {
		if (this != &other){
			size = other.size;
			delete[] line;
			line = new unsigned char[size];
			std::copy_n(other.line, size, line);
		}
		return *this;
	}
	CLine & operator=(CLine&& other) {
		assert(this != &other);
		size = other.size;
		other.size = 0;
		delete[] line;
		line = other.line;
		other.line = nullptr;
		return *this;
	}
	~CLine() { delete[] line; }

	inline void NewPV(const uint8_t newMove, const CLine* newLine) {
		assert(size == newLine->size+1);
		line[0] = newMove;
		std::copy_n(newLine->line, newLine->size, line+1);
	}
	inline void NewPV(const uint8_t newMove) { line[0] = newMove; }
	inline void NoMoves() { std::fill_n(line, size, 64); }
	inline uint8_t BestMove() const { return line[0]; }
	
	inline const std::string GetPV(const int depth) const
	{
		assert(line[depth] >= 0);
		assert(line[depth] <= 64);
		if (depth >= size)
			return "--";
		else
			return field_name[line[depth]];
	}
	inline const std::string GetPV(const int StartDepth, const int Count) const
	{
		std::string s;
		for (int i = StartDepth; i < StartDepth + Count; i++)
			s.append(GetPV(i)).append(" ");
		return s.substr(0, s.length() - 1);
	}
	inline const std::string GetPV() const { return GetPV(0, size); }
	
};
