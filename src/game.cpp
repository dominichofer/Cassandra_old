#include "game.h"

const int A = 8;

inline double sigma(int D, int d, int E) { return (std::exp(-0.1726 * d) + 0.7553) * std::pow(D - d, 0.3205) * (-0.0281 * E + 2.3067); }

bool MPC(uint64_t P, uint64_t O, uint64_t& NodeCounter, int alpha, int selectivity, int depth, int empties, int& value)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	if (selectivity == 0) return false;

	if (((depth == empties) && (empties >= 10)) || ((depth != empties) && (depth >= 4)))
	{
		NodeCounter++;
		const int beta = alpha + 1;
		const double t = SelectivityTable[selectivity].T;
		const int zero_eval = EvaluateFeatures(P, O);
		int probcut_depth = (depth == empties) ? depth/2 - 2 : (depth / 4) * 2 + (depth & 1);
		double probcut_sigma = sigma(depth, probcut_depth, empties);
		int probcut_beta = RoundInt(beta + t * probcut_sigma);
		int probcut_alpha = probcut_beta - 1;
		int score;

		if (zero_eval > probcut_beta && probcut_beta <= 64)
		{
			     if (probcut_depth == 1) score = Midgame::ZWS_1(P, O, NodeCounter, probcut_alpha);
			else if (probcut_depth == 2) score = Midgame::ZWS_2(P, O, NodeCounter, probcut_alpha);
			else                         score = ZWS(P, O, NodeCounter, probcut_alpha, NO_SELECTIVITY, probcut_depth, empties);
			if (score >= probcut_beta) { value = beta; return true; }
		}

		probcut_alpha = RoundInt(alpha - t * probcut_sigma);
		if (zero_eval < probcut_alpha && probcut_alpha >= -64)
		{
			     if (probcut_depth == 1) score = Midgame::ZWS_1(P, O, NodeCounter, probcut_alpha);
			else if (probcut_depth == 2) score = Midgame::ZWS_2(P, O, NodeCounter, probcut_alpha);
			else                         score = ZWS(P, O, NodeCounter, probcut_alpha, NO_SELECTIVITY, probcut_depth, empties);
			if (score <= probcut_alpha) { value = alpha; return true; }
		}
	}
	return false;

	//if ((depth == empties) && (empties >= 10) && (empties <= 26))
	//{
	//	//static const float sigma[] = { 6.188, 6.084, 5.198, 5.193, 5.08, 4.716, 4.547, 4.626, 4.561, 4.53, 4.505, 3.947, 4.178, 3.914, 4.089, 3.320, 4.190 };
	//}
	//if ((depth == 4) && (empties >= 11) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 4.505, 3.902, 4.022, 4.497, 3.57, 4.054, 3.632, 3.075, 3.533, 3.304, 3.157, 3.293, 3.209, 2.783, 2.831, 2.731, 2.551, 2.744, 2.914, 2.331, 2.254, 2.482, 2.273, 2.141, 2.596, 2.195, 2.071, 2.235, 1.989, 1.885, 2.099, 1.887, 2.08, 2.071, 1.638, 1.958, 2.063, 1.928, 1.442 };
	//}
	//if ((depth == 5) && (empties >= 12) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.884, 3.926, 4.158, 3.476, 3.916, 3.384, 2.799, 3.235, 2.853, 3.007, 3.099, 2.708, 2.646, 2.603, 2.304, 2.456, 2.603, 2.552, 2.499, 2.075, 2.096, 2.254, 1.983, 2.179, 2.125, 1.887, 1.794, 2.01, 1.92, 1.723, 2.121, 1.991, 2.127, 2.078, 1.803, 2.27, 2.377, 1.782 };
	//}
	//if ((depth == 6) && (empties >= 13) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 5.004, 5.108, 4.537, 4.823, 4.483, 4.101, 4.409, 3.918, 3.994, 4.261, 3.839, 3.569, 3.842, 3.547, 3.361, 3.49, 3.464, 3.141, 3.096, 3.121, 3.036, 2.908, 2.967, 2.86, 2.681, 2.675, 2.686, 2.69, 2.507, 2.567, 2.649, 2.883, 2.496, 2.398, 2.549, 2.699, 2.209 };
	//}
	//if ((depth == 7) && (empties >= 14) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.884, 3.926, 4.158, 3.476, 3.916, 3.384, 2.799, 3.235, 2.853, 3.007, 3.099, 2.708, 2.646, 2.603, 2.304, 2.456, 2.603, 2.552, 2.499, 2.075, 2.096, 2.254, 1.983, 2.179, 2.125, 1.887, 1.794, 2.01, 1.92, 1.723, 2.121, 1.991, 2.127, 2.078, 1.803, 2.27, 2.377, 1.782 };
	//}
	//if ((depth == 8) && (empties >= 15) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.845, 3.589, 3.612, 3.391, 3.289, 3.317, 3.149, 3.131, 3.012, 3.127, 3.051, 2.951, 3.062, 2.945, 2.76, 2.799, 2.724, 2.64, 2.797, 2.703, 2.456, 2.323, 2.309, 2.382, 2.268, 2.258, 2.259, 2.363, 2.116, 2.541, 2.559, 1.846, 2.249, 2.198 };
	//}
	//if ((depth == 9) && (empties >= 16) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.441, 3.249, 3.206, 3.043, 2.953, 3.118, 2.856, 2.737, 2.867, 2.876, 2.857, 2.605, 2.833, 2.733, 2.573, 2.587, 2.601, 2.55, 2.611, 2.543, 2.199, 2.222, 2.192, 2.191, 2.027, 2.079, 1.955, 2.047, 2.264, 2.118, 1.863, 2.064, 1.609, 1.676 };
	//}
	//if ((depth == 10) && (empties >= 17) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 4.227, 3.892, 3.765, 3.676, 3.602, 3.65, 3.47, 3.475, 3.399, 3.6, 3.512, 3.362, 3.444, 3.357, 3.182, 3.28, 3.318, 3.157, 3.084, 2.814, 2.779, 2.716, 2.623, 2.583, 2.67, 2.567, 2.335, 2.939, 2.807, 2.103, 2.605, 2.424, 2.374 };
	//}
	//if ((depth == 11) && (empties >= 18) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.688, 3.535, 3.398, 3.499, 3.403, 3.197, 3.312, 3.2, 3.141, 3.336, 3.253, 3.213, 3.267, 3.115, 3.051, 3.204, 3.097, 3.042, 2.767, 2.759, 2.661, 2.558, 2.417, 2.406, 2.309, 2.217, 2.376, 2.539, 2.068, 2.302, 1.973, 2.046 };
	//}
	//if ((depth == 12) && (empties >= 19) && (empties <= 49))
	//{
	//	//static const float sigma[] = { 3.5, 3.325, 3.259, 3.246, 3.09, 3.062, 3.123, 2.993, 2.952, 3.184, 3.157, 3.022, 3.076, 2.979, 2.926, 3.01, 3.016, 2.851, 2.725, 2.567, 2.509, 2.324, 2.353, 2.136, 2.182, 2.08, 2.222, 2.052, 2.282, 1.83, 1.951 };
	//}

	//return false;
}

int ZWS  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int selectivity, const int depth, const int empties)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	if ((empties == A) && (depth == empties)) return Endgame::ZWS_A(P, O, NodeCounter, alpha, empties);
	if (depth == 3)                           return Midgame::ZWS_3(P, O, NodeCounter, alpha);

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

    //else if ((depth > 6) && (empties > 14)) // IID
    //{
    //    Eval(P, O, NodeCounter, alpha, alpha+1, selectivity, depth == empties ? empties-12 : depth-4, empties);
    //    if (LookUpTT(P, O, ttValue))
    //        if (UseTTValue(ttValue, alpha, alpha+1, depth, selectivity, score))
    //            return score;
    //}
	
	CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, ttValue, false);
	for (const auto& mv : mvList) // ETC
	{
		if (StabilityCutoff_ZWS(mv.P, mv.O, -alpha-1, score)) return -score;
		if (LookUpTT(mv.P, mv.O, ttValue) && UseTTValue(ttValue, -alpha - 1, -alpha, depth - 1, selectivity, score) && (-score > alpha))
		{
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
	
	UpdateTT(P, O, NodeCounter-LocalNodeCounter, alpha, alpha+1, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
	return bestscore;
}

int PVS  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, const int selectivity, const int depth, const int empties, CLine* pline)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(-64 <= beta ); assert(beta  <= 64);
	assert(alpha <= beta);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	if ((empties == A) && (depth == empties)) return Endgame::PVS_A(P, O, NodeCounter, alpha, beta, empties, pline);
	if (depth == 3)                           return Midgame::PVS_3(P, O, NodeCounter, alpha, beta, pline);

	const int original_alpha = alpha;
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

	if (StabilityCutoff_PVS(P, O, alpha, score)) return score;
	if (LookUpTT(P, O, ttValue))
	{
		if (!pline)
			if (UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
				return score;
	}
	else if ((depth >= 5) && (empties >= 12)) // IID
	{
		Eval(P, O, NodeCounter, alpha, beta, selectivity, depth == empties ? empties-10 : depth-2, empties);
		if (LookUpTT(P, O, ttValue))
			if (!pline)
				if (UseTTValue(ttValue, alpha, beta, depth, selectivity, score))
					return score;
	}

	CLine * line = nullptr;
	if (pline && pline->size) line = new CLine(pline->size-1);
	CMoveList mvList(P, O, NodeCounter, BitBoardPossible, depth, ttValue, true);
	for (const auto& mv : mvList)
	{
		if (bestscore == -65)
			score = -PVS(mv.P, mv.O, NodeCounter, -beta, -alpha, selectivity, depth-1, empties-1, line);
		else
		{
			score = -ZWS(mv.P, mv.O, NodeCounter, -alpha-1, selectivity, depth-1, empties-1);
			if (score > alpha && score < beta)
				score = -PVS(mv.P, mv.O, NodeCounter, -beta, -alpha, selectivity, depth-1, empties-1, line);
		}
		if (score > bestscore)
		{
			bestscore = score;
			BestMove = mv.move;
			if (line) pline->NewPV(mv.move, line);
			if (score >= beta) break;
			if (score > alpha) alpha = score;
		}
	}
	
	UpdateTT(P, O, NodeCounter-LocalNodeCounter, original_alpha, beta, bestscore, depth, selectivity, BestMove, mvList.BestMove(), mvList.NextBestMove());
	delete line;
	return bestscore;
}

int Eval(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const int depth, const int empties, CLine* pline)
{
	assert((P & O) == 0);
	assert(-64 <= alpha); assert(alpha <= 64);
	assert(-64 <= beta ); assert(beta  <= 64);
	assert(alpha <= beta);
	assert(0 <= depth); assert(depth <= 60);
	assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));
	assert(depth <= empties);

	if (empties <= A)
	{
		     if (empties == 0) return Endgame::PVS_0(P, O, NodeCounter);
		else if (empties == 1) return Endgame::PVS_1(P, O, NodeCounter, alpha, pline);
		else                   return Endgame::PVS_A(P, O, NodeCounter, alpha, beta, empties, pline);
	}
	else if (depth <= 3)
	{
		     if (depth == 0) return Midgame::PVS_0(P, O, NodeCounter);
		else if (depth == 1) return Midgame::PVS_1(P, O, NodeCounter, alpha, beta, pline);
		else if (depth == 2) return Midgame::PVS_2(P, O, NodeCounter, alpha, beta, pline);
		else                 return Midgame::PVS_3(P, O, NodeCounter, alpha, beta, pline);
	}
	else
        return PVS(P, O, NodeCounter, alpha, beta, selectivity, depth, empties, pline);
}

namespace Midgame
{
	int ZWS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
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
			score = -ZWS_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
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
		unsigned long Move;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -ZWS_3(O, P, NodeCounter, -alpha-1);
			else
				return EvalGameOver(P, Empties(P, O));
		}
		
		if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;

		// #.####.#
		// ........
		// #.####.#
		// #.#..#.#
		// #.#..#.#
		// #.####.#
		// ........
		// #.####.#
		const unsigned long long PATTERN_FIRST = 0xBD00BDA5A5BD00BDULL;
		const uint64_t BBParity = quadrant[Parity(P, O)];
		uint64_t BBTmp;

		BBTmp = BitBoardPossible &  BBParity &  PATTERN_FIRST;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha - 1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		BBTmp = BitBoardPossible &  BBParity & ~PATTERN_FIRST;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha - 1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		BBTmp = BitBoardPossible & ~BBParity;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha - 1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}
		return bestscore;
	}

	int PVS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		assert((P & O) == 0);

		NodeCounter++;
		return EvaluateFeatures(P, O);
	}
	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, CLine* pline)
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
				return EvalGameOver(P, O);
			}
		}

		CLine * line = nullptr;
		if (pline && pline->size) line = new CLine(pline->size-1);
		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			if (bestscore == -65)
				score = -PVS_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			else
			{
				score = -ZWS_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
				if (score > alpha && score < beta)
					score = -PVS_0(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter);
			}
			if (score > bestscore)
			{
				bestscore = score;
				if (line) pline->NewPV(Move, line);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		delete line;
		return bestscore;
	}
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		const int original_alpha = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_2(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, O);
			}
		}
		
		uint64_t LocalNodeCounter = NodeCounter;
		CLine * line = nullptr;
		if (pline && pline->size) line = new CLine(pline->size-1);
		CMoveListLight mvList(P, O, BitBoardPossible);
		for (const auto& mv : mvList)
		{
			flipped = flip(P, O, mv);
			if (bestscore == -65)
				score = -PVS_1(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, line);
			else
			{
				score = -ZWS_1(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				if (score > alpha && score < beta)
					score = -PVS_1(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv;
				if (line) pline->NewPV(mv, line);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, original_alpha, beta, bestscore, 2, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		delete line;
		return bestscore;
	}
	int PVS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, const int beta, CLine* pline)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(-64 <= beta ); assert(beta  <= 64);
		assert(alpha <= beta);

		const int original_alpha = alpha;
		int score;
		int bestscore = -65;
		uint8_t BestMove = 64;
		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		NodeCounter++;

		if (!BitBoardPossible) {
			if (HasMoves(O, P))
				return -PVS_3(O, P, NodeCounter, -beta, -alpha, pline);
			else {
				if (pline) pline->NoMoves();
				return EvalGameOver(P, O);
			}
		}
		
		uint64_t LocalNodeCounter = NodeCounter;
		CLine * line = nullptr;
		if (pline && pline->size) line = new CLine(pline->size-1);
		CMoveListLight mvList(P, O, BitBoardPossible);
		for (const auto& mv : mvList)
		{
			flipped = flip(P, O, mv);
			if (bestscore == -65)
				score = -PVS_2(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, line);
			else
			{
				score = -ZWS_2(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				if (score > alpha && score < beta)
					score = -PVS_2(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				BestMove = mv;
				if (line) pline->NewPV(mv, line);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		UpdateTT(P, O, NodeCounter-LocalNodeCounter, original_alpha, beta, bestscore, 3, NO_SELECTIVITY, BestMove, mvList.BestMove(), mvList.NextBestMove());
		delete line;
		return bestscore;
	}
}

namespace Endgame
{
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x)
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
			//if (score + 1 < alpha) return score + 1;

			//if (diff = count_last_flip(O, x))
			//{
			//	NodeCounter += 2; // One for passing, one for playing
			//	return score - diff - 1;
			//}
			//else
			//	return score > 0 ? score + 1: score - 1;

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
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 2);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);

		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -ZWS_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -ZWS_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -alpha - 1, x1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = ZWS_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = ZWS_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		if (bestscore != 65)
			return bestscore;
		else
			return EvalGameOver<2>(P);
	}
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2, const unsigned int x3)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 3);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(x1 < 64);
		assert(x2 < 64);
		assert(x3 < 64);

		//if (QUADRANT_ID(x1) == QUADRANT_ID(x2)) //Play x3 first
		//	std::swap(x1,x3);
		//else if (QUADRANT_ID(x1) == QUADRANT_ID(x3)) //Play x2 first
		//	std::swap(x1,x2);
		
		int score;
		int bestscore = -65;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
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
		{
			score = -ZWS_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -alpha - 1, x1, x2);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = ZWS_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
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
		{
			score = ZWS_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, x1, x2);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		if (bestscore != 65)
			return bestscore;
		else
			return EvalGameOver<3>(P);
	}
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4)
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

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -alpha - 1, x2, x3, x4);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
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
		{
			score = -ZWS_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -alpha - 1, x1, x2, x3);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		if (bestscore != -65) return bestscore;
		bestscore = 65;
		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2, x3, x4);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
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
		{
			score = ZWS_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, x1, x2, x3);
			if (score <= alpha) return score;
			if (score < bestscore) bestscore = score;
		}

		if (bestscore != 65)
			return bestscore;
		else
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
		//uint64_t parity = Parity(BBEmpties);
		unsigned int x1 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		unsigned int x2 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		unsigned int x3 = BitScanLSB(BBEmpties); RemoveLSB(BBEmpties);
		unsigned int x4 = BitScanLSB(BBEmpties);

		//if (parity & quadrant_id_4_bit[x1]){ // Either 1 or 3 empties in x1's quadrant
		//	if (parity & quadrant_id_4_bit[x2]){ // Either 1 or 3 empties in x2's quadrant
		//		if (parity & quadrant_id_4_bit[x3]){ // Either 1 or 3 empties in x3's quadrant
		//			if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x4]))
		//				std::swap(x1,x2);
		//			else if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x4]))
		//				std::swap(x1,x3);
		//			else if ((quadrant_id_4_bit[x1] == quadrant_id_4_bit[x2]) && (quadrant_id_4_bit[x1] == quadrant_id_4_bit[x3]))
		//				std::swap(x1,x4);
		//		}
		//	}
		//	else{ // (x1) (..) (x2 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x1) (x3) (x2 x4)
		//			std::swap(x2,x3); // Now playing x1, x3, x2, x4
		//		else // (x1) (x4) (x2 x3)
		//			std::swap(x2,x4); // Now playing x1, x4, x3, x2
		//	}
		//}
		//else { // Either 2 or 4 in x1's quadrant
		//	if (parity & quadrant_id_4_bit[x2]){ // (x2) (..) (x1 ..)
		//		if (parity & quadrant_id_4_bit[x3]) // (x2) (x3) (x1 x4)
		//			std::swap(x1,x3); // Now playing x3, x2, x1, x4
		//		else // (x2) (x4) (x1 x3)
		//			std::swap(x1,x4); // Now playing x4, x2, x3, x1
		//	}
		//	else if (parity & quadrant_id_4_bit[x3]){ // (x3) (x4) (x1 x2)
		//		std::swap(x1,x3);
		//		std::swap(x2,x4); // Now playing x3, x4, x1, x2
		//	}
		//}

		return ZWS_4(P, O, NodeCounter, alpha, x1, x2, x3, x4);
	}
	int ZWS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int empties)
	{
		assert((P & O) == 0);
		assert(-64 <= alpha); assert(alpha <= 64);
		assert(0 <= empties); assert(empties <= 60); assert(empties == Empties(P, O));

		if (empties == 3) return ZWS_3(P, O, NodeCounter, alpha);

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
		//if (PopCount(BitBoardPossible) == 1)
		//{
		//	unsigned long Move = BitScanLSB(BitBoardPossible);
		//	uint64_t flipped = flip(P, O, Move);
		//	return -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1, empties-1);
		//}

		//if (StabilityCutoff_ZWS(P, O, alpha, score)) return score;
		
		// #......#
		// ........
		// ..####..
		// ..#..#..
		// ..#..#..
		// ..####..
		// ........
		// #......#
		const uint64_t PATTERN_FIRST = 0x81003C24243C0081ULL;
		const uint64_t BBParity = quadrant[Parity(P, O)];
		uint64_t BBTmp;

		BBTmp = BitBoardPossible &  BBParity &  PATTERN_FIRST;
		while (BBTmp)
		{
			Move = BitScanLSB(BBTmp);
			RemoveLSB(BBTmp);
			flipped = flip(P, O, Move);
			score = -ZWS_A(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -alpha-1, empties-1);
			if (score > alpha) return score;
			if (score > bestscore) bestscore = score;
		}

		BBTmp = BitBoardPossible &  BBParity & ~PATTERN_FIRST;
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

	int PVS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		assert((P & O) == 0);
		assert(Empties(P, O) == 0);

		NodeCounter++;
		return EvalGameOver<0>(P);
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

		if (empties == 1) return PVS_1(P, O, NodeCounter, alpha, pline);

		int score;
		int bestscore = -65;
		uint64_t flipped;
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

		CMoveListLight mvList(P, O, BitBoardPossible);
		for (const auto& mv : mvList)
		{
			flipped = flip(P, O, mv);
			if (bestscore == -65)
				score = -PVS_A(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, empties-1, line);
			else
			{
				     if (empties == 2) score = -ZWS_1(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				else if (empties == 3) score = -ZWS_2(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				else if (empties == 4) score = -ZWS_3(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				else if (empties == 5) score = -ZWS_4(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1);
				else                   score = -ZWS_A(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -alpha-1, empties-1);

				if (score > alpha && score < beta)
					score = -PVS_A(O ^ flipped, P ^ (1ULL << mv) ^ flipped, NodeCounter, -beta, -alpha, empties-1, line);
			}
			if (score > bestscore)
			{
				bestscore = score;
				if (line) pline->NewPV(mv, line);
				if (score >= beta) break;
				if (score > alpha) alpha = score;
			}
		}

		delete line;
		return bestscore;
	}
}