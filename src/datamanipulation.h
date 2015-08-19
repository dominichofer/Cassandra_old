#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "macros_hell.h"
#include "utility.h"
#include "position.h"

const  int8_t DATASET_DEFAULT_depth       = -1;
const  int8_t DATASET_depth_END           = 99;
const uint8_t DATASET_DEFAULT_selectivity = 0;
const  int8_t DATASET_DEFAULT_score       = -99;
const uint8_t DATASET_DEFAULT_PV          = 64;

const std::string DATASET_ENDING_POSITION_SCORE_PV = "psp";
const std::string DATASET_ENDING_POSITION_FULLSCORE = "pfs";
const std::string DATASET_ENDING_POSITION_HR = "obf";

inline bool validEnding(const std::string& ending)
{
	return (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0) ||
	       (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0) ||
	       (ending.compare(DATASET_ENDING_POSITION_HR) == 0);
}

class CDataset_Position_FullScore;
class CDataset_Position_Score_PV;


// .psp
// 192 Bit = 24 Byte
class CDataset_Position_Score_PV
{
public:
	uint64_t P, O;
	 int8_t depth;
	uint8_t selectivity;
	 int8_t score;
	uint8_t PV[5];

	inline void Reset() {
		depth = DATASET_DEFAULT_depth;
		selectivity = DATASET_DEFAULT_selectivity;
		score = DATASET_DEFAULT_score;
		for (int i = 0; i < 5; i++) PV[i] = DATASET_DEFAULT_PV;
	}
	
	CDataset_Position_Score_PV() : P(0), O(0) { Reset(); }
	CDataset_Position_Score_PV(const uint64_t P, const uint64_t O) : P(P), O(O) { Reset(); }
	CDataset_Position_Score_PV(const uint64_t P, const uint64_t O, const int8_t depth, const uint8_t selectivity, const int8_t score, const uint8_t PV0, const uint8_t PV1, const uint8_t PV2, const uint8_t PV3, const uint8_t PV4) : P(P), O(O), depth(depth), selectivity(selectivity), score(score) { PV[0] = PV0; PV[1] = PV1; PV[2] = PV2; PV[3] = PV3; PV[4] = PV4; }
	CDataset_Position_Score_PV(const CPosition & o) : P(o.P), O(o.O) { Reset(); }
	explicit CDataset_Position_Score_PV(const CDataset_Position_FullScore& o);
	explicit CDataset_Position_Score_PV(std::string s);
	explicit operator CPosition() const { return CPosition(P, O); }
	explicit operator CPositionScore() const { return CPositionScore(P, O, score); }

	inline std::string to_string() const { return board1D(P, O).append(" X; " + field_name[PV[0]] + ":" + ((score >= 0) ? "+" : "") + std::to_string(score) + ";"); }
};

// .pfs
// 648 Bit = 81 Byte
class CDataset_Position_FullScore
{
public:
	uint64_t P, O;
	 int8_t score[61];
	uint8_t PV[5];

	inline void Reset() { 
		for (int i = 0; i < 61; ++i) score[i] = DATASET_DEFAULT_score;
		for (int i = 0; i < 5; i++) PV[i] = DATASET_DEFAULT_PV;
	}

	CDataset_Position_FullScore() : P(0), O(0) { Reset(); }
	CDataset_Position_FullScore(const uint64_t P, const uint64_t O) : P(P), O(O) { Reset(); }
	CDataset_Position_FullScore(const CPosition & o) : P(o.P), O(o.O) { Reset(); }
	explicit CDataset_Position_FullScore(const CDataset_Position_Score_PV& o);
	explicit operator CPosition() const { return CPosition(P, O); }
	explicit operator CPositionScore() const { return CPositionScore(P, O, DeepestScore()); }

	int8_t DeepestScore() const { for (int i = 60; i >= 0; i--) if (score[i] != DATASET_DEFAULT_score) return score[i]; return DATASET_DEFAULT_score; }
	int8_t DeepestDepth() const { for (int i = 60; i >= 0; i--) if (score[i] != DATASET_DEFAULT_score) return i; return DATASET_DEFAULT_depth; }
};


std::vector<CDataset_Position_Score_PV> read_hr_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL);
void write_to_hr_file(const std::string & filename, const std::vector<CDataset_Position_Score_PV>& vector);

template <class T>
std::vector<T> read_vector(const std::string & filename, std::size_t size = 0xFFFFFFFFFFFFFFFFULL)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file){
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return std::vector<T>();
	}

	const std::size_t N = 4 * 1024;
	std::size_t ValidData;
	std::vector<T> vector;
	T* DataArray = new T[N];
	while (ValidData = fread(DataArray, sizeof(T), (N < size ? N : size), file)){
		vector.insert(vector.end(), DataArray, DataArray + ValidData);
		size -= ValidData;
	}

	fclose(file);
	delete[] DataArray;

	return vector;
}

template <class T>
void write_to_file(const std::string & filename, const std::vector<T>& vector)
{
	FILE* file = fopen(filename.c_str(), "wb");
	if (!file){
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return;
	}

	for (const T& data : vector) fwrite(&data, sizeof(T), 1, file);

	fclose(file);
}

template <> std::vector<CPositionScore> read_vector(const std::string & filename, std::size_t size);
