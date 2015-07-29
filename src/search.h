#pragma once
#include <cstdint>
#include <chrono>
#include <utility>
#include "macros_hell.h"
#include "line.h"
#include "utility.h"
#include "configfile.h"
#include "game.h"
#include "selectivity.h"
#include "screen.h"

class CSearch
{
public:
	enum NodeType { PV_Node, All_Node, Cut_Node };

	uint64_t P, O;                                                     // 128 Bit
	uint64_t NodeCounter;                                              //  64 Bit
	 int8_t alpha, beta;                                               //  16 Bit
	 int8_t depth;                                                     //   8 Bit
	uint8_t selectivity;                                               //   8 Bit
	 int8_t score;                                                     //   8 Bit
	std::chrono::high_resolution_clock::time_point startTime, endTime; // 128 Bit
	bool stop;                                                         //   8 Bit
	NodeType nodeType;                                                 //
	CLine PV_line;                                                     //
	                                                                   // -------
	                                                                   // 368 Bit

	CSearch(uint64_t P, uint64_t O, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity, uint8_t PV_depth, std::chrono::high_resolution_clock::time_point endTime) :
		P(P), O(O),
		NodeCounter(0),
		alpha(alpha), beta(beta),
		depth(depth), selectivity(selectivity),
		score(-128),
		startTime(std::chrono::high_resolution_clock::now()),
		endTime(endTime),
		stop(false),
		nodeType(nodeType),
		PV_line(CLine(PV_depth))
	{}

	CSearch(uint64_t P, uint64_t O, int8_t alpha, int8_t beta, int8_t depth, uint8_t selectivity, uint8_t PV_depth, std::chrono::milliseconds duration)
		: CSearch(P, O, alpha, beta, depth, selectivity, PV_depth, std::chrono::high_resolution_clock::now() + duration) {}

	CSearch(uint64_t P = START_POSITION_P, uint64_t O = START_POSITION_O, int8_t alpha= -64, int8_t beta = 64, int8_t depth = 99, uint8_t selectivity = 0, uint8_t PV_depth = 5)
		: CSearch(P, O, alpha, beta, depth, selectivity, PV_depth, std::chrono::hours(24 * 365)) {}

	CSearch(const CSearch& o) : CSearch(o.P, o.O, o.alpha, o.beta, o.depth, o.selectivity, o.PV_line.size, o.endTime) {}


	inline bool TestTimeOut() { return stop |= (std::chrono::high_resolution_clock::now() > endTime); }

	inline uint8_t PV(const unsigned int index) const { return PV_line.line[index]; }
	inline std::string GetPV() const { return PV_line.GetPV(); }
	inline std::string GetPV(const int depth) const { return PV_line.GetPV(depth); }
	inline std::string GetPV(const int StartDepth, const int num) const { return PV_line.GetPV(StartDepth, num); }
	void print_result(const int number = 0);
	void Evaluate();
private:
	void print_stats(const int8_t depth, const uint8_t selectivity);
};