#pragma once
#include <algorithm>
#include <set>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include "datamanipulation.h"
#include "macros_hell.h"
#include "position.h"
#include "flip_fast.h"
#include "possiblemoves.h"
#include "utility.h"

void GenerateRandomPositions(const std::string & filename, const std::size_t numPos, const int numEmpties, const bool ETH);
void GeneratePerftPositions(const std::string & filename, const int depth, const bool ETH);
