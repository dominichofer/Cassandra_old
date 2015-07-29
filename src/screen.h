#pragma once
#include <cstdint>
#include <chrono>
#include <utility>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include "macros_hell.h"
#include "line.h"
#include "utility.h"
#include "configfile.h"
#include "selectivity.h"

class CScreen
{
	// verbose == 0 : quiet          show only over all statistics
    // verbose == 1 : normal         show progress
    // verbose == 2 : verbose        show position results
    // verbose == 3 : very verbose   show position progress
public:
	std::atomic<int> positionCounter;
	void Initialize(int max_number, int positions);
	void printHead();
	void printTail();
	void printStats (                  const int depth, const int selectivity, const uint64_t P, const uint64_t O, const int score, const std::chrono::milliseconds duration, const uint64_t NodeCounter, const std::string& PV);
	void printResult(const int number, const int depth, const int selectivity, const uint64_t P, const uint64_t O, const int score, const std::chrono::milliseconds duration, const uint64_t NodeCounter, const std::string& PV);
	void printPosition(const int number, const uint64_t P, const uint64_t O);
	void printSummary(const uint64_t NodeCounter);
private:
	std::chrono::high_resolution_clock::time_point startTime, lastPrint;
	int width_number;
	int width_nodes;
	int width_nps;
	int width_PV;
	int positions;
	std::mutex mtx;
	std::atomic_flag flag;

	void GetLines(std::string& L1, std::string& L2);
	//std::string StatsString(int number, int depth, int selectivity, uint64_t P, uint64_t O, int score, std::chrono::milliseconds duration, uint64_t NodeCounter, std::string addendum);
	std::string ETA();
};

extern CScreen Screen;
