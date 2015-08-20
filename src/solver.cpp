#pragma once
#include <cstdint>
#include <functional>
#include <random>
#include <set>
#include <csignal>
#include <vector>
#include <omp.h>
#include "configfile.h"
#include "endgame_negamax.h"
#include "game.h"
#include "macros_hell.h"
#include "perft_basic.h"
#include "datamanipulation.h"
#include "generatepositions.h"
#include "hashtable.h"
#include "selectivity.h"
#include "search.h"
#include "worker.h"

std::atomic_bool user_wants_to_quit;
void signalHandler(int signum) { user_wants_to_quit.store(true, std::memory_order_release); }

void TTUsePV(uint64_t P, uint64_t O, int depth, const int selectivity, const int score, const uint8_t PV1, const uint8_t PV2, const uint8_t PV3, const uint8_t PV4, const uint8_t PV5)
{
	if (PV1 == 64) return;
	UpdateTT(P, O, 0, score, score, score, depth, selectivity, PV1, 64, 64);
	if (!HasMoves(P, O)) std::swap(P, O);
	PlayStone(P, O, PV1); depth--;

	if (PV2 == 64) return;
	UpdateTT(P, O, 0, -score, -score, -score, depth, selectivity, PV2, 64, 64);
	if (!HasMoves(P, O)) std::swap(P, O);
	PlayStone(P, O, PV2); depth--;

	if (PV3 == 64) return;
	UpdateTT(P, O, 0, score, score, score, depth, selectivity, PV3, 64, 64);
	if (!HasMoves(P, O)) std::swap(P, O);
	PlayStone(P, O, PV3); depth--;

	if (PV4 == 64) return;
	UpdateTT(P, O, 0, -score, -score, -score, depth, selectivity, PV4, 64, 64);
	if (!HasMoves(P, O)) std::swap(P, O);
	PlayStone(P, O, PV4); depth--;

	if (PV5 == 64) return;
	UpdateTT(P, O, 0, score, score, score, depth, selectivity, PV5, 64, 64);
	if (!HasMoves(P, O)) std::swap(P, O);
	PlayStone(P, O, PV5); depth--;
	
	UpdateTT(P, O, 0, -score, -score, -score, depth, selectivity, 64, 64, 64);
}

void FForum_Benchmark(const std::string & filename)
{
	uint64_t NodeCounter = 0;
	std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);

	Screen.Initialize(vec.size(), vec.size());
	Screen.printHead();
	for (int i = 0; i < vec.size(); i++)
	{
		CSearch search(vec[i].P, vec[i].O);
		Screen.printPosition(i+1, vec[i].P, vec[i].O);
		search.Evaluate();
		search.print_result(i+1);
		if (search.score != vec[i].score) printf("ERROR: Miscalculation! Score should be %+2.2d.\n", vec[i].score);
		NodeCounter += search.NodeCounter;
	}
	Screen.printTail();
	Screen.printSummary(NodeCounter);
}

void FForum()
{
	verbose = 2;
	ConfigFile::Initialize("C:\\Cassandra\\config.ini");
	Features::Initialize();
	CountLastFlip::Initialize();
	Stability::Initialize();
	TT = CHashTable(1024 * 1024 * 1024 / sizeof(CHashTable::NodeType));
	TTPV = CHashTable(1024 * 1024 * 1024 / 16 / sizeof(CHashTable::NodeType));
	omp_set_num_threads(1);
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-1-19.obf");
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-20-39.obf");
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-40-59.obf");
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-60-79.obf");
	TT.print_stats();
	TTPV.print_stats();
}

void SolveStartPosition()
{
	verbose = 3;
	ConfigFile::Initialize("C:\\Cassandra\\config.ini");
	Features::Initialize();
	CountLastFlip::Initialize();
	Stability::Initialize();
	TT = CHashTable(1024 * 1024 * 1024 / sizeof(CHashTable::NodeType));
	TTPV = CHashTable(1024 * 1024 * 1024 / 16 / sizeof(CHashTable::NodeType));
	omp_set_num_threads(1);

	Screen.Initialize(0, 0);
	Screen.printHead();
	CSearch search("-----------------O-O--O--XOOOOOO---OO-------O--------O--------O-", -64, 64, 5);
	//CSearch search;
	//PlayStone(search.P, search.O, BitScanLSB(PossibleMoves(search.P, search.O)));
	Screen.printPosition(0, search.P, search.O);
	search.Evaluate();
	search.print_result();
	Screen.printTail();
	Screen.printSummary(search.NodeCounter);

	TT.print_stats();
}

void DistibutedComputingDoWork(CWorker<CDataset_Position_Score_PV,CDataset_Position_Score_PV>& worker)
{
    uint64_t WU_id;
    CDataset_Position_Score_PV position;

    while (worker.RequestProblem(position, WU_id))
    {
        CSearch search(position.P, position.O);
		Screen.printPosition(WU_id, position.P, position.O);
		search.Evaluate();
		position.score = search.score;
		for (int j = 0; j < 5; ++j) position.PV[j] = search.PV_line.line[j];
		position.depth = 99;
		position.selectivity = 0;
		search.print_result(WU_id);
        worker.ReportSolution(position, WU_id);
	}
    printf("No more work.\n");
}

void DistibutedComputing(const std::string& IP, const int port)
{
	int threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threadlist;
	CWorker<CDataset_Position_Score_PV,CDataset_Position_Score_PV> worker;
	worker.Initialize(IP.c_str(), port);

	Screen.Initialize(0, 0);
	Screen.printHead();

	// Create workers
	for (unsigned int i = 0; i < threads - 1; ++i)
		threadlist.push_back(std::thread(DistibutedComputingDoWork, std::ref(worker)));
	DistibutedComputingDoWork(worker);

	// Join workers
	for (auto& thread : threadlist) thread.join();
}

uint64_t ParseRAM(const std::string& s)
{
	std::string number = s.substr(0, s.length() - 2);
	std::string prefix = s.substr(s.length() - 2, 1);
	uint64_t num = std::stoi(number);
	if (prefix.compare("k") == 0) return num * 1024;
	if (prefix.compare("M") == 0) return num * 1024 * 1024;
	if (prefix.compare("G") == 0) return num * 1024 * 1024 * 1024;
	return 0;
}

void print_help()
{
	std::cout << "Solves a file of reversi positions.\n" <<
		"Arguments:\n" <<
		"-f abc.xyz      Filename.\n" <<
		"-n 123          Number of positions to solve (default: 1G).\n" <<
		"-d 123          Depth to solve for (default: Exact).\n" <<
		"-s 123          Selectivity to solve for (default: no selectivity).\n" <<
		"-ram 123GB      RAM to use in transposition table (default: 1GB).\n" <<
		"-verbose 123    Verbosity level.\n" <<
		"-v              Verbose (-verbose 2).\n" <<
		"-vv             Very Verbose (-verbose 3).\n" <<
		"-q              Quiet (-verbose 0).\n" <<
		"-t 123          Threads to use.\n" <<
		"-test           Run as test.\n" <<
		"-noskip         Not skipping solved positions.\n" <<
		"-nosave         Not saving results.\n\n" <<
		"-noPV           Not using PV from file.\n\n" <<
		"-ip 1.2.3.4     TCP/IP\n" <<
		"-p 123          port\n\n"<<
		"-h              Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	// Preheat
	int tmp = 4;
	printf("Preheating CPU");
	for (unsigned int i = 0; i < 100000000; i++)
		tmp = (static_cast<int>(std::exp(std::sin(tmp))) * 123456789 + 987) / 101 + 13;
	printf(" to %dC.\n", tmp % 100);

	//SolveStartPosition(); return 0;
	FForum(); return 0;

	uint64_t NodeCounter = 0;
	unsigned int index = 0;

	int n = 1000000000;
	int d = 99;
	int s = 0;
	int t = std::thread::hardware_concurrency();
	uint64_t ram = 1024 * 1024 * 1024;
	bool b_Test = false;
	bool b_SkipSolved = true;
	bool b_Save = true;
	bool b_UsePV = true;
	std::string filename;
	std::string IP = "127.0.0.1";
	int port = 0;

	for (int i = 0; i < argc; ++i)
	{
		     if (std::string(argv[i]) == "-f") filename = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-n") n = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-d") d = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-s") s = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-ram") ram = ParseRAM(std::string(argv[++i]));
		else if (std::string(argv[i]) == "-verbose") verbose = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-v") verbose = 2;
		else if (std::string(argv[i]) == "-vv") verbose = 3;
		else if (std::string(argv[i]) == "-q") verbose = 0;
		else if (std::string(argv[i]) == "-t") t = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-test") { b_SkipSolved = false; b_Save = false; b_Test = true; }
		else if (std::string(argv[i]) == "-noskip") b_SkipSolved = false;
		else if (std::string(argv[i]) == "-nosave") b_Save = false;
		else if (std::string(argv[i]) == "-noPV") b_UsePV = false;
		else if (std::string(argv[i]) == "-ip") IP = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-p") port = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-h") { print_help(); return 0; }
	}

	{ b_SkipSolved = false; b_Save = false; b_Test = true; }
	filename = "C:\\Cassandra\\pos\\play5_e21_1M.psp";
	
	ConfigFile::Initialize("C:\\Cassandra\\config.ini");
	Features::Initialize();
	CountLastFlip::Initialize();
	Stability::Initialize();
	TT = CHashTable(ram / sizeof(CHashTable::NodeType));
	TTPV = CHashTable(ram / 8 / sizeof(CHashTable::NodeType));
	omp_set_num_threads(t);

	if (port != 0) // Distributed computing mode
	{
		DistibutedComputing(IP, port);
		return 0;
	}

	if (verbose)
	{
		if (b_Test)        std::cout << "Running as a TEST!" << std::endl;
		if (!b_SkipSolved) std::cout << "Not skipping solved positions!" << std::endl;
		if (!b_Save)       std::cout << "Not saving results!" << std::endl;
	}

    // Install signal handler
	user_wants_to_quit.store(false, std::memory_order_release);
	signal(SIGINT, signalHandler);

    // Get and test ending
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; return -1; }

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		if (b_SkipSolved)
			while ((index < vec.size()) && (vec[index].depth >= d) || ((vec[index].depth == d) && (vec[index].selectivity > s))) index++; // Skip solved positions

		Screen.Initialize(vec.size(), MIN(vec.size() - index, static_cast<std::size_t>(n)));
		Screen.printHead();

		const long long i_max = MIN(vec.size(), static_cast<std::size_t>(index + n));
		#pragma omp parallel for schedule(static, 1) reduction(+ : NodeCounter)
		for (long long i = index; i < i_max; i++)
		{
			if (user_wants_to_quit.load(std::memory_order_acquire)) continue;
			if (b_UsePV) TTUsePV(vec[i].P, vec[i].O, vec[i].depth, vec[i].selectivity, vec[i].score, vec[i].PV[0], vec[i].PV[1], vec[i].PV[2], vec[i].PV[3], vec[i].PV[4]);
			CSearch search(vec[i].P, vec[i].O, -64, 64, d, s);
			Screen.printPosition(i, vec[i].P, vec[i].O);
			search.Evaluate();
			vec[i].score = search.score;
			for (int j = 0; j < 5; ++j) vec[i].PV[j] = search.PV_line.line[j];
			vec[i].depth = d;
			vec[i].selectivity = s;
			search.print_result(i);
			NodeCounter += search.NodeCounter;
		}

		if (b_Save)
			write_to_file(filename, vec);

		Screen.printTail();
		Screen.printSummary(NodeCounter);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
        std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		if (b_SkipSolved)
			while ((index < vec.size()) && (vec[index].score[d] != DATASET_DEFAULT_score)) index++; // Skip solved positions

		Screen.Initialize(vec.size(), MIN(vec.size() - index, static_cast<std::size_t>(n)));
		Screen.printHead();

		const long long i_max = MIN(vec.size(), static_cast<std::size_t>(index + n));
		#pragma omp parallel for schedule(static, 1) reduction(+ : NodeCounter)
		for (long long i = index; i < i_max; i++)
		{
			if (user_wants_to_quit.load(std::memory_order_acquire)) continue;
			if (b_UsePV) TTUsePV(vec[i].P, vec[i].O, vec[i].DeepestDepth(), DATASET_DEFAULT_selectivity, vec[i].DeepestScore(), vec[i].PV[0], vec[i].PV[1], vec[i].PV[2], vec[i].PV[3], vec[i].PV[4]);
			CSearch search(vec[i].P, vec[i].O, -64, 64, d, NO_SELECTIVITY);
			Screen.printPosition(i, vec[i].P, vec[i].O);
			search.Evaluate();
			vec[i].score[d] = search.score;
			if (d >= vec[i].DeepestDepth()) for (int j = 0; j < 5; ++j) vec[i].PV[j] = search.PV_line.line[j];
			search.print_result(i);
			NodeCounter += search.NodeCounter;
		}

		if (b_Save)
			write_to_file(filename, vec);

		Screen.printTail();
		Screen.printSummary(NodeCounter);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
        std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		if (b_SkipSolved)
			while ((index < vec.size()) && (vec[index].depth >= d) || ((vec[index].depth == d) && (vec[index].selectivity > s))) index++; // Skip solved positions

		Screen.Initialize(vec.size(), MIN(vec.size() - index, static_cast<std::size_t>(n)));
		Screen.printHead();

		const long long i_max = MIN(vec.size(), static_cast<std::size_t>(index + n));
		#pragma omp parallel for schedule(static, 1) reduction(+ : NodeCounter)
		for (long long i = index; i < i_max; i++)
		{
			if (user_wants_to_quit.load(std::memory_order_acquire)) continue;
			if (b_UsePV) TTUsePV(vec[i].P, vec[i].O, vec[i].depth, vec[i].selectivity, vec[i].score, vec[i].PV[0], vec[i].PV[1], vec[i].PV[2], vec[i].PV[3], vec[i].PV[4]);
			CSearch search(vec[i].P, vec[i].O, -64, 64, d, s);
			Screen.printPosition(i, vec[i].P, vec[i].O);
			search.Evaluate();
			vec[i].score = search.score;
			for (int j = 0; j < 5; ++j) vec[i].PV[j] = search.PV_line.line[j];
			vec[i].depth = DATASET_depth_END;
			vec[i].selectivity = 0;
			search.print_result(i);
			NodeCounter += search.NodeCounter;
		}

		if (b_Save)
			write_to_hr_file(filename, vec);

		Screen.printTail();
		Screen.printSummary(NodeCounter);
	}
	return 0;
}