#include "game.h"

const bool USE_IID = true;
const bool USE_PV_TTCUT = true;

const int A = 8;
const int B = 12;

inline double sigma(int D, int d, int E) { return (std::exp(-0.2770 * d) + 1.0295) * std::pow(D - d, 0.3044) * (-0.0276 * E + 2.1746); }
//inline double sigma(int D, int d, int E) { return (std::exp(-0.1726 * d) + 0.7553) * std::pow(D - d, 0.3205) * (-0.0281 * E + 2.3067); }

bool MPC(uint64_t P, uint64_t O, uint64_t& NodeCounter, int alpha, int selectivity, int depth, int empties, int& value)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	if (selectivity)
	{
		const int beta = alpha + 1;
		const double t = SelectivityTable[selectivity].T;
		const int zero_eval = Midgame::Eval_0(P, O, NodeCounter);
		int probcut_depth = (depth == empties) ? (depth / 4) * 2 - (depth & 1) - 2 : (depth / 4) * 2 + (depth & 1);
		//int probcut_depth = (depth / 4) * 2 + (depth & 1);
		if (probcut_depth <= 0) probcut_depth = depth - 2; 
		double probcut_sigma = sigma(depth, probcut_depth, empties);
		int probcut_beta = RoundInt(beta + t * probcut_sigma);
		int probcut_alpha = probcut_beta - 1;
		int score;
		int eval_error = t * 0.5 * (sigma(depth, 0, empties) + sigma(depth, probcut_depth, empties));

		if (zero_eval >= alpha + 1 - eval_error && probcut_beta <= 64)
		{
			score = ZWS(P, O, NodeCounter, probcut_alpha, NO_SELECTIVITY, probcut_depth, empties);
			if (score >= probcut_beta) { value = beta; return true; }
		}

		probcut_alpha = RoundInt(alpha - t * probcut_sigma);
		if (zero_eval < alpha + eval_error && probcut_alpha >= -64)
		{
			score = ZWS(P, O, NodeCounter, probcut_alpha, NO_SELECTIVITY, probcut_depth, empties);
			if (score <= probcut_alpha) { value = alpha; return true; }
		}
	}
	return false;
}

int ZWS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int selectivity, const int depth, const int empties)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);
	
	if (depth <= 3 && depth < empties) {
		if (depth == 3) return Midgame::ZWS_3(P, O, NodeCounter, alpha);
		if (depth == 2) return Midgame::ZWS_2(P, O, NodeCounter, alpha);
		if (depth == 1) return Midgame::ZWS_1(P, O, NodeCounter, alpha);
		if (depth == 0) return Midgame::Eval_0(P, O, NodeCounter);
	}
	if (empties <= B && depth == empties) return Endgame::ZWS_B(P, O, NodeCounter, alpha, empties);

	int score;
	int bestscore = -64;
	uint8_t BestMove = 64;
	uint64_t BitBoardPossible = PossibleMoves(P, O);
	uint64_t LocalNodeCounter = NodeCounter;
	CHashTableValueType ttValue;
	NodeCounter++;

	if (!BitBoardPossible){
		if (HasMoves(O, P))
			return -ZWS(O, P, NodeCounter, -alpha - 1, selectivity, depth, empties);
		else
			return EvalGameOver(P, empties);
	}

	if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
	if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, depth, selectivity, score)) return score;
	if (MPC(P, O, NodeCounter, alpha, selectivity, depth, empties, score)) return score;
		
	CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, alpha, ttValue, false);
	for (const auto& mv : mvList) // ETC
	{
		if (StabilityCutoff_ZWS(mv.P, mv.O, -alpha - 1, score)) {
			UpdateTT(P, O, 0, alpha, alpha + 1, -score, depth, NO_SELECTIVITY, mv.move, mvList.BestMove(), mvList.NextBestMove());
			return -score;
		}
		if (LookUpTT(mv.P, mv.O, ttValue) && UseTTValue(ttValue, -alpha - 1, -alpha, depth - 1, selectivity, score) && (-score > alpha)) {
			UpdateTT(P, O, 0, alpha, alpha+1, -score, depth, selectivity, mv.move, mvList.BestMove(), mvList.NextBestMove());
			return -score;
		}
	}
	for (const auto& mv : mvList)
	{
		score = -ZWS(mv.P, mv.O, NodeCounter, -alpha-1, selectivity, depth-1, empties-1);
		if (score > bestscore)
		{
			bestscore = score;
			BestMove = mv.move;
			if (score > alpha) break;
		}
	}
	
	if (empties-1 <= B)
		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
	else
		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
	return bestscore;
}

int PVS(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const int depth, const int empties, CLine* pline)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(-64 <= beta ); assert(beta  <= 64);
	assert(alpha <= beta);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);
	
	if (depth <= 2 && depth < empties) {
		if (depth == 2) return Midgame::PVS_2(P, O, NodeCounter, alpha, beta, pline);
		if (depth == 1) return Midgame::PVS_1(P, O, NodeCounter, alpha, beta, pline);
		if (depth == 0) return Midgame::Eval_0(P, O, NodeCounter);
	}
	if (empties <= A && depth == empties) return Endgame::PVS_A(P, O, NodeCounter, alpha, beta, empties, pline);

	int lower = alpha;
	int score;
	int bestscore = -65;
	uint8_t BestMove = 64;
	uint64_t BitBoardPossible = PossibleMoves(P, O);
	uint64_t LocalNodeCounter = NodeCounter;
	CHashTableValueType ttValue;
	NodeCounter++;

	if (!BitBoardPossible){
		if (HasMoves(O, P))
			return -PVS(O, P, NodeCounter, -beta, -alpha, selectivity, depth, empties, pline);
		else {
			if (pline) pline->NoMoves();
			return EvalGameOver(P, empties);
		}
	}

	if (!pline && StabilityCutoff_PVS(P, O, alpha, score)) return score;
	if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
		if (USE_PV_TTCUT && !pline && UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
			return score;
	if (USE_IID && ttValue.PV == 64) // IID
	{
		int reduced_depth = (depth == empties) ? depth - 10 : depth - 2;
		if (reduced_depth >= 3)
		{
			PVS(P, O, NodeCounter, -64, 64, 6, reduced_depth, empties, nullptr);
			if (LookUpTTPV(P, O, ttValue))
				if (USE_PV_TTCUT && !pline && UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
					return score;
		}
	}

	CLine * line = nullptr;
	if (pline && pline->size) line = new CLine(pline->size-1);
	CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, alpha, ttValue, true);
	for (const auto& mv : mvList)
	{
		if (bestscore == -65)
			score = -PVS(mv.P, mv.O, NodeCounter, -beta, -lower, selectivity, depth-1, empties-1, line);
		else
		{
			score = -ZWS(mv.P, mv.O, NodeCounter, -lower-1, selectivity, depth-1, empties-1);
			if (score > lower && score < beta)
				score = -PVS(mv.P, mv.O, NodeCounter, -beta, -lower, selectivity, depth-1, empties-1, line); // OPTIMIZATION: -lower -> -score
		}
		if (score > bestscore)
		{
			bestscore = score;
			BestMove = mv.move;
			if (line) pline->NewPV(mv.move, line);
			if (score >= beta) break;
			if (score > lower) lower = score;
		}
	}
	
	if (empties-1 <= B)
	{
		UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
	}
	else
	{
		UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
		UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
	}
	delete line;
	return bestscore;
}

int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, int depth, const int empties, CLine* pline)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(-64 <= beta ); assert(beta  <= 64);
	assert(alpha <= beta);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	return PVS(P, O, NodeCounter, alpha, beta, selectivity, depth, empties, pline);
}

namespace Midgame
{
	int Eval_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		assert((P & O) == 0);

		NodeCounter++;
		return EvaluateFeatures(P, O);
	}

	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int score;
		int bestscore = -64;
		unsigned long Move;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_1(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, Empties(P, O));
		}

		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			score = -Eval_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		return bestscore;
	}
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int score;
		int bestscore = -64;
		unsigned long Move;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_2(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, Empties(P, O));
		}

		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			score = -ZWS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha - 1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		return bestscore;
	}
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_3(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, Empties(P, O));
		}
		
		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, 3, NO_SELECTIVITY, score)) return score;

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, 3, alpha, ttValue, false);
		for (const auto& mv : mvList)
		{
			score = -ZWS_2(mv.P, mv.O, NodeCounter, -alpha-1);
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (score > alpha) break;
			}
		}
	
		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, 3, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		return bestscore;
	}

	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		int score;
		int bestscore = -65;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		unsigned long Move;
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_1(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, Empties(P, O));
			}
		}

		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			score = -Eval_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			if (score > bestscore)
			{
				bestscore = score;
				if (pline) pline->NewPV(Move);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		return bestscore;
	}
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		int lower = alpha;
		int score;
		int bestscore = -65;
		unsigned long Move;
		uint8_t BestMove = 64;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_2(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, Empties(P, O));
			}
		}
		
		uint64_t LocalNodeCounter = NodeCounter;
		CLine * line = nullptr;
		if (pline && pline->size) line = new CLine(pline->size-1);
		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			if (bestscore == -65)
				score = -PVS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -lower, line);
			else
			{
				score = -ZWS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -lower-1);
				if (score > lower && score < beta)
					score = -PVS_1(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -lower, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = Move;
				if (line) pline->NewPV(Move, line);
				if (score >= beta) break;
				if (score > lower) lower = score;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, beta, bestscore, 2, NO_SELECTIVITY, BestMove, 64, 64);
		delete line;
		return bestscore;
	}
}

namespace Endgame
{
	int Eval_0(const uint64_t P, uint64_t& NodeCounter)
	{
		NodeCounter++;
		return EvalGameOver<0>(P);
	}

	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const uint8_t x)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 1);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x < 64);

		int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		int diff;
		NodeCounter++;

		if (diff = count_last_flip(P, x))
		{
			NodeCounter++;
			return score + diff + 1;
		}
		else
		{
			if (score > 0)
			{
				if (score + 1 < alpha) return score + 1;

				if (diff = count_last_flip(O, x))
				{
					NodeCounter += 2; // One for passing, one for playing
					return score - diff - 1;
				}
				else
					return score + 1;
			}
			else
			{
				if (score - 1 < alpha) return score - 1;

				if (diff = count_last_flip(O, x))
				{
					NodeCounter += 2; // One for passing, one for playing
					return score - diff - 1;
				}
				else
					return score - 1;
			}
		}
	}
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const uint8_t x1, const uint8_t x2)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 2);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);

		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
			return MAX(bestscore, -ZWS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
			return MIN(bestscore, ZWS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1));

		if (bestscore != 65) return bestscore;
		
		return EvalGameOver<2>(P);
	}
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, uint8_t x1, uint8_t x2, uint8_t x3)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 3);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);
		assert(x3 < 64);

		if (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) //Play x3 first
			std::swap(x1, x3);
		else if (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]) //Play x2 first
			std::swap(x1, x2);
		
		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1, x3);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
			return MAX(bestscore, -ZWS_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = ZWS_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, x3);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
			return MIN(bestscore, ZWS_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2));

		if (bestscore != 65) return bestscore;
		
		return EvalGameOver<3>(P);
	}
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 4);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);
		assert(x3 < 64);
		assert(x4 < 64);
		
		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;

		//uint64_t parity = Parity(~(P, O));
		//if (!(parity & quadrant_id_4_bit[x1])) { // (..) (..) (x1 ..)
		//	if (parity & quadrant_id_4_bit[x2]) { // (x2) (..) (x1 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x2) (x3) (x1 x4)
		//			std::swap(x1, x3); // Now playing x3 x2 x1 x4
		//		else // (x2) (x4) (x1 x3)
		//			std::swap(x1, x4); //Now playing x4 x2 x3 x1
		//	}
		//	else if (parity & quadrant_id_4_bit[x3]) // (x3) (x4) (x1 x2)
		//		{std::swap(x1, x3); std::swap(x2, x4);} // Now playing x3 x4 x1 x2
		//} 
		//else { // Either 1 or 3 in x1's quadrant
		//	if (!(parity & quadrant_id_4_bit[x2])) { // (x1) (..) (x2 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x1) (x3) (x2 x4)
		//			std::swap(x2, x3); // Now playing x1 x3 x2 x4
		//		else // (x1) (x4) (x2 x3)
		//			std::swap(x2, x4); // Now playing x1 x4 x3 x2
		//	}
		//}

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			bestscore = -ZWS_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3, x4);
			if (bestscore > alpha) return bestscore;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1, x3, x4);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x4);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
			return MAX(bestscore, -ZWS_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x3));

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			bestscore = ZWS_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3, x4);
			if (bestscore <= alpha) return bestscore;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1, x3, x4);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2, x4);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
			return MIN(bestscore, ZWS_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, x1, x2, x3));

		if (bestscore != 65) return bestscore;
		
		return EvalGameOver<4>(P);
	}
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		uint64_t BBEmpties = ~(P | O);
		uint8_t x1 = BitScanLSB(BBEmpties);
		return ZWS_1(P, O, NodeCounter, alpha, x1);
	}
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		uint64_t BBEmpties = ~(P | O);
		uint8_t x1 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x2 = BitScanLSB(BBEmpties);
		return ZWS_2(P, O, NodeCounter, alpha, x1, x2);
	}
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		uint64_t BBEmpties = ~(P | O);
		uint8_t x1 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x2 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x3 = BitScanLSB(BBEmpties);
		return ZWS_3(P, O, NodeCounter, alpha, x1, x2, x3);
	}
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha)
	{
		uint64_t BBEmpties = ~(P | O);
		uint8_t x1 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x2 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x3 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		uint8_t x4 = BitScanLSB(BBEmpties);
		return ZWS_4(P, O, NodeCounter, alpha, x1, x2, x3, x4);
	}
	int ZWS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int empties)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));

		if (empties <= 4) {
			if (empties == 4) return ZWS_4(P, O, NodeCounter, alpha);
			if (empties == 3) return ZWS_3(P, O, NodeCounter, alpha);
			if (empties == 2) return ZWS_2(P, O, NodeCounter, alpha);
			if (empties == 1) return ZWS_1(P, O, NodeCounter, alpha);
			if (empties == 0) return Eval_0(P, NodeCounter);
		}

		int score;
		int bestscore = -64;
		unsigned long Move;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -ZWS_A(O, P, NodeCounter, -alpha - 1, empties);
			else
				return EvalGameOver(P, empties);
		}

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		
		const uint64_t BBParity = quadrant[Parity(P, O)];
		uint64_t BBTmp;

		BBTmp = BitBoardPossible &  BBParity;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1, empties-1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		BBTmp = BitBoardPossible & ~BBParity;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1, empties-1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		return bestscore;
	}
	int ZWS_B(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int empties)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));

		if (empties <= A) return ZWS_A(P, O, NodeCounter, alpha, empties);

		int score;
		int bestscore = -64;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -ZWS_B(O, P, NodeCounter, -alpha - 1, empties);
			else
				return EvalGameOver(P, empties);
		}

		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		if (LookUpTT(P, O, ttValue) && UseTTValue(ttValue, alpha, alpha+1, empties, NO_SELECTIVITY, score)) return score; 

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, empties, alpha, ttValue, false);
		for (const auto& mv : mvList)
		{
			score = -ZWS_B(mv.P, mv.O, NodeCounter, -alpha-1, empties-1);
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (score > alpha) break;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		return bestscore;
	}

	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x, CLine* pline)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 1);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x < 64);

		int score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		int diff;

		NodeCounter++;
		if (diff = count_last_flip(P, x))
		{
			NodeCounter++;
			if (pline) pline->NewPV(x);
			return score + diff + 1;
		}
		else
		{
			//if (!pline && (score + 1 < alpha)) return score + 1;

			if (diff = count_last_flip(O, x))
			{
				NodeCounter += 2; // One for passing, one for playing
				if (pline) pline->NewPV(x);
				return score - diff - 1;
			}
			else
			{
				if (pline) pline->NoMoves();
				return score > 0 ? score + 1 : score - 1;
			}

			//if (score > 0)
			//{
			//	if (!pline && (score + 1 < alpha)) return score + 1;

			//	if (diff = count_last_flip(O, x))
			//	{
			//		NodeCounter += 2; // One for passing, one for playing
			//		if (pline) pline->NewPV(x);
			//		return score - diff - 1;
			//	}
			//	else
			//	{
			//		if (pline) pline->NoMoves();
			//		return score + 1;
			//	}
			//}
			//else
			//{
			//	if (!pline && (score - 1 < alpha)) return score - 1;

			//	if (diff = count_last_flip(O, x))
			//	{
			//		NodeCounter += 2; // One for passing, one for playing
			//		if (pline) pline->NewPV(x);
			//		return score - diff - 1;
			//	}
			//	else
			//	{
			//		if (pline) pline->NoMoves();
			//		return score - 1;
			//	}
			//}
		}
	}
	int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, const int empties, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);
		assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));

		if (empties <= 1) {
			if (empties == 1) return PVS_1(P, O, NodeCounter, alpha, pline);
			if (empties == 0) return Eval_0(P, NodeCounter);
		}
		
		int lower = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -PVS_A(O, P, NodeCounter, -beta, -alpha, empties, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, Empties(P, O));
			}
		}

		CLine * line = nullptr;
		if (pline && pline->size) line = new CLine(pline->size-1);
		
		uint64_t LocalNodeCounter = NodeCounter;
		CHashTableValueType ttValue;
		if (LookUpTTPV(P, O, ttValue) || LookUpTT(P, O, ttValue))
			if (USE_PV_TTCUT && !pline && UseTTValue(ttValue, alpha, beta, empties, NO_SELECTIVITY, score))
				return score;

		CMoveList mvList(P, O, NodeCounter, BitBoardPossible, empties, alpha, ttValue, true);
		for (const auto& mv : mvList)
		{
			if (bestscore == -65)
				score = -PVS_A(mv.P, mv.O, NodeCounter, -beta, -lower, empties-1, line);
			else
			{
				score = -ZWS_A(mv.P, mv.O, NodeCounter, -lower-1, empties-1);
				if (score > lower && score < beta)
					score = -PVS_A(mv.P, mv.O, NodeCounter, -beta, -lower, empties-1, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv.move;
				if (line) pline->NewPV(mv.move, line);
				if (score >= beta) break;
				if (score > lower) lower = score;
			}
		}

		UpdateTTPV(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		UpdateTT(P, O, NodeCounter - LocalNodeCounter, alpha, beta, bestscore, empties, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());

		delete line;
		return bestscore;
	}
}