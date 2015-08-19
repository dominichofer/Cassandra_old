#pragma once
#include <algorithm>
#include <set>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include "datamanipulation.h"
#include "macros_hell.h"
#include "position.h"
#include "flip_fast.h"
#include "possiblemoves.h"
#include "utility.h"
#include "search.h"

void GenerateRandomPositions(const std::string & filename, const std::size_t numPos, const int numEmpties, const bool ETH);
void GeneratePerftPositions(const std::string & filename, const int depth, const bool ETH);
void GeneratePlayedPositions(const std::string & in, const std::string & out, const std::size_t numPos, const int thinkDepth);