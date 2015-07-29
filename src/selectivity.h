#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include "position.h"
#include "utility.h"

const int NO_SELECTIVITY = 0;

struct CSelectivity
{
	float T;
	std::string percentile;
	CSelectivity(const float T, const std::string& percentile) : T(T), percentile(percentile) {}
};

const CSelectivity SelectivityTable[10] = {
	CSelectivity(99.f, "    "), // 0
	CSelectivity(3.3f, "@99%"), // 1
	CSelectivity(2.6f, "@98%"), // 2
	CSelectivity(2.0f, "@95%"), // 3
	CSelectivity(1.5f, "@87%"), // 4
	CSelectivity(1.2f, "@77%"), // 5
	CSelectivity(1.1f, "@73%"), // 6
	CSelectivity(0.7f, "@52%"), // 7
	CSelectivity(0.5f, "@38%"), // 8
	CSelectivity(0.2f, "@16%"), // 9
};

inline std::string DepthSelectivity(const int depth, const unsigned int selectivity, const uint64_t P, const uint64_t O)
{
	char buffer[16];
	if (depth < 0) return std::string("");
	//if ((depth >= static_cast<int>(Empties(P, O))) && (selectivity == 0)) return std::string("Exact");

	int d = MIN(static_cast<int>(Empties(P, O)), depth);
	sprintf(buffer, "%2d%4s", d, SelectivityTable[selectivity].percentile.c_str());
	return std::string(buffer);
}

inline std::string DepthSelectivity(const int depth, const unsigned int selectivity)
{
	char buffer[16];
	int d = MIN(99, depth);
	sprintf(buffer, "%2d%4s", d, SelectivityTable[selectivity].percentile.c_str());
	return std::string(buffer);
}
