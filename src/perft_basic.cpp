#include "perft_basic.h"

namespace Perft_Basic
{
	void perft(const uint8_t depth)
	{
		uint64_t correct[] = {1, 4, 12, 56, 244, 1396, 8200, 55092, 390216, 3005288, 24571056, 212258216, 1939879668, 18429618408, 184041761768, 1891831332208, 20301171282452, 222742563853912, 2534535926617852, 19, 20, 21};
		std::chrono::high_resolution_clock::time_point startTime, endTime;
		uint64_t P = START_POSITION_P;
		uint64_t O = START_POSITION_O;
		PlayStone(P, O, BitScanLSB(PossibleMoves(P, O)));
		uint64_t result;

		std::cout << "depth|       Positions        |correct|      Time      " << std::endl;
		std::cout << "-----+------------------------+-------+----------------" << std::endl;

		for (uint8_t d = 1; d <= depth; ++d)
		{
			startTime = std::chrono::high_resolution_clock::now();
			result = 4ULL * perft(P, O, d-1);
			endTime = std::chrono::high_resolution_clock::now();
			
			printf(" %3u | %22s |%7s| %14s\n", d, ThousandsSeparator(result).c_str(), (correct[d] == result ? "  true " : " false "), time_format(endTime - startTime).c_str());
		}
	}

	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		if (depth == 0) return 1ULL;

		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
			if (HasMoves(O, P))
				return perft(O, P, depth-1);
			else
				return 0ULL;
		
		uint64_t sum = 0;
		unsigned long LSB; // Holds the Least Significant Bit
		uint64_t flipped;

		while (BitBoardPossible)
		{
			LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, LSB);
			sum += perft(O ^ flipped, P ^ flipped ^ (1ULL << LSB), depth - 1);
		}

		return sum;
	}
}

namespace Perft_Unroll2
{	
	void perft(const uint8_t depth)
	{
		uint64_t correct[] = {1, 4, 12, 56, 244, 1396, 8200, 55092, 390216, 3005288, 24571056, 212258216, 1939879668, 18429618408, 184041761768, 1891831332208, 20301171282452, 222742563853912, 2534535926617852, 19, 20, 21};
		std::chrono::high_resolution_clock::time_point startTime, endTime;
		uint64_t P = START_POSITION_P;
		uint64_t O = START_POSITION_O;
		PlayStone(P, O, BitScanLSB(PossibleMoves(P, O)));
		uint64_t result;

		std::cout << "depth|       Positions        |correct|      Time      " << std::endl;
		std::cout << "-----+------------------------+-------+----------------" << std::endl;

		for (uint8_t d = 3; d <= depth; ++d)
		{
			startTime = std::chrono::high_resolution_clock::now();
			result = 4ULL * perft(P, O, d-1);
			endTime = std::chrono::high_resolution_clock::now();
			
			printf(" %3u | %22s |%7s| %14s\n", d, ThousandsSeparator(result).c_str(), (correct[d] == result ? "  true " : " false "), time_format(endTime - startTime).c_str());
		}
	}

	// perft for 2 plies left
	uint64_t perft2(const uint64_t P, const uint64_t O)
	{
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0) return PopCount(PossibleMoves(O, P));

		unsigned long LSB; // Holds the Least Significant Bit
		uint64_t sum = 0;
		uint64_t flipped;
		uint64_t BitBoardPossible2;

		while (BitBoardPossible)
		{
			LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, LSB);
			BitBoardPossible2 = PossibleMoves(O ^ flipped, P ^ flipped ^ (1ULL << LSB));
			if (BitBoardPossible2)
				sum += PopCount(BitBoardPossible2);
			else
				sum += HasMoves(P ^ flipped ^ (1ULL << LSB), O ^ flipped);
		}

		return sum;
	}

	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth)
	{
		if (depth == 2) return perft2(P, O);
		
		uint64_t BitBoardPossible = PossibleMoves(P, O);

		if (BitBoardPossible == 0)
			if (HasMoves(O, P))
				return perft(O, P, depth-1);
			else
				return 0ULL;
		
		uint64_t sum = 0;
		unsigned long LSB; // Holds the Least Significant Bit
		uint64_t flipped;

		while (BitBoardPossible)
		{
			LSB = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, LSB);
			sum += perft(O ^ flipped, P ^ flipped ^ (1ULL << LSB), depth - 1);
		}

		return sum;
	}
}