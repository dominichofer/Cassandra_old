#pragma once
#include <cstdint>
#include "macros_hell.h"
#include "flip_fast.h"
#include "count_last_flip.h"
#include "utility.h"
#include "possiblemoves.h"
#include "position.h"
#include "game.h"

namespace Endgame
{	
	int NegaMax_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x);
	int NegaMax_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2);
	int NegaMax_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3);
	int NegaMax_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4);
	int NegaMax  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	
	int AlphaBeta_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x);
	int AlphaBeta_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2);
	int AlphaBeta_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3);
	int AlphaBeta_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4);
	int AlphaBeta  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta);
	
	
	// ####################
	// # Helper functions #
	// ####################
	inline int NegaMax_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties);

		return NegaMax_1(P, O, NodeCounter, x1);
	}
	inline int NegaMax_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x2 = BitScanLSB(empties);

		return NegaMax_2(P, O, NodeCounter, x1, x2);
	}
	inline int NegaMax_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x2 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x3 = BitScanLSB(empties);

		return NegaMax_3(P, O, NodeCounter, x1, x2, x3);
	}
	inline int NegaMax_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		uint64_t BitBoardEmpty = ~(P | O);
		const unsigned int x1 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x2 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x3 = BitScanLSB(BitBoardEmpty); RemoveLSB(BitBoardEmpty);
		const unsigned int x4 = BitScanLSB(BitBoardEmpty);

		return NegaMax_4(P, O, NodeCounter, x1, x2, x3, x4);
	}
	
	inline int AlphaBeta_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties);

		return AlphaBeta_1(P, O, NodeCounter, alpha, x1);
	}
	inline int AlphaBeta_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x2 = BitScanLSB(empties);

		return AlphaBeta_2(P, O, NodeCounter, alpha, beta, x1, x2);
	}
	inline int AlphaBeta_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta)
	{
		uint64_t empties = ~(P | O);
		unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x2 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x3 = BitScanLSB(empties);

		return AlphaBeta_3(P, O, NodeCounter, alpha, beta, x1, x2, x3);
	}
	inline int AlphaBeta_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta)
	{
		uint64_t empties = ~(P | O);
		uint64_t parity = Parity(empties);
		unsigned int x1 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x2 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x3 = BitScanLSB(empties); RemoveLSB(empties);
		unsigned int x4 = BitScanLSB(empties);

		if (parity & quadrant_id_4_bit[x1]){ // Either 1 or 3 empties in x1's quadrant
			if (parity & quadrant_id_4_bit[x2]){ // Either 1 or 3 empties in x2's quadrant
				if (parity & quadrant_id_4_bit[x3]){ // Either 1 or 3 empties in x3's quadrant
					if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x4]))
						std::swap(x1,x2);
					else if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x4]))
						std::swap(x1,x3);
					else if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]))
						std::swap(x1,x4);
				}
			}
			else{ // (x1) (..) (x2 ..)
				if (parity & quadrant_id_4_bit[x3]) // (x1) (x3) (x2 x4)
					std::swap(x2,x3); // Now playing x1, x3, x2, x4
				else // (x1) (x4) (x2 x3)
					std::swap(x2,x4); // Now playing x1, x4, x3, x2
			}
		}
		else { // Either 2 or 4 in x1's quadrant 
			if (parity & quadrant_id_4_bit[x2]){ // (x2) (..) (x1 ..)
				if (parity & quadrant_id_4_bit[x3]) // (x2) (x3) (x1 x4)
					std::swap(x1,x3); // Now playing x3, x2, x1, x4
				else // (x2) (x4) (x1 x3)
					std::swap(x1,x4); // Now playing x4, x2, x3, x1
			}
			else if (parity & quadrant_id_4_bit[x3]){ // (x3) (x4) (x1 x2)
				std::swap(x1,x3);
				std::swap(x2,x4); // Now playing x3, x4, x1, x2
			}
		}

		return AlphaBeta_4(P, O, NodeCounter, alpha, beta, x1, x2, x3, x4);
	}
}