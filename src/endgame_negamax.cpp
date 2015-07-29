#include "endgame_negamax.h"

namespace Endgame
{
	int NegaMax_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x)
	{
		int Score, DiffCount;
		Score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		
		NodeCounter++;
		if (DiffCount = count_last_flip(P, x))
		{
			NodeCounter++;
			return Score + DiffCount + 1;
		}
		else if (DiffCount = count_last_flip(O, x))
		{
			NodeCounter += 2; // One for passing, one for playing
			return Score - DiffCount - 1;
		}
		else
			return (Score > 0) ? Score + 1 : Score - 1;
	}
	int NegaMax_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -NegaMax_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -NegaMax_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1));
			played = true;
		}

		if (played) return Score;

		NodeCounter++;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, NegaMax_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, NegaMax_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<2>(P);
	}
	int NegaMax_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -NegaMax_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -NegaMax_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3));
			played = true;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			Score = MAX(Score, -NegaMax_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2));
			played = true;
		}

		if (played) return Score;

		NodeCounter++;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, NegaMax_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, NegaMax_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3));
			played = true;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			Score = MIN(Score, NegaMax_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<3>(P);
	}
	int NegaMax_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4)
	{
		uint64_t flipped;
		int Score = -64;
		bool played = false;
		++NodeCounter;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			Score = MAX(Score, -NegaMax_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3, x4));
			played = true;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			Score = MAX(Score, -NegaMax_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3, x4));
			played = true;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			Score = MAX(Score, -NegaMax_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2, x4));
			played = true;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
		{
			Score = MAX(Score, -NegaMax_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, x1, x2, x3));
			played = true;
		}

		if (played) return Score;

		++NodeCounter;
		Score = 64;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			Score = MIN(Score, NegaMax_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, x2, x3, x4));
			played = true;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			Score = MIN(Score, NegaMax_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, x1, x3, x4));
			played = true;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			Score = MIN(Score, NegaMax_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, x1, x2, x4));
			played = true;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
		{
			Score = MIN(Score, NegaMax_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, x1, x2, x3));
			played = true;
		}

		if (played)
			return Score;
		else
			return EvalGameOver<4>(P);
	}
	int NegaMax  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter)
	{
		const uint64_t empties = Empties(P, O);
		if (empties == 4)
			return NegaMax_4(P, O, NodeCounter);

		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		unsigned long Move;
		int Score = -64;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -NegaMax(O, P, NodeCounter);
			else //Game is over
				return EvalGameOver(P, empties);
		}

		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			Score = MAX(Score, -NegaMax(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter));
		}

		return Score;
	}


	int AlphaBeta_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x)
	{
		int Score, DiffCount;
		Score = (PopCount(P) << 1) - 63; // == PopCount(P) - PopCount(O)
		
		NodeCounter++;
		if (DiffCount = count_last_flip(P, x))
		{
			NodeCounter++;
			return Score + DiffCount + 1;
		}
		else
		{
			if (Score < alpha)
				return alpha;
			else if (DiffCount = count_last_flip(O, x))
			{
				NodeCounter += 2; // One for passing, one for playing
				return Score - DiffCount - 1;
			}
			else
				return (Score > 0) ? Score + 1 : Score - 1;
		}
	}
	int AlphaBeta_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2)
	{
		int score = -128;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -AlphaBeta_1(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, x2);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -AlphaBeta_1(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, x1);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128) return alpha;

		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = AlphaBeta_1(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, x2);
			if (score <= alpha) return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = AlphaBeta_1(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, x1);
			if (score <= alpha) return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score == -128)
			return EvalGameOver<2>(P);
		else
			return beta;

	}
	int AlphaBeta_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3)
	{
		int score = -128;
		uint64_t flipped;
		NodeCounter++;

		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -AlphaBeta_2(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, -alpha, x2, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -AlphaBeta_2(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, -alpha, x1, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -AlphaBeta_2(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -beta, -alpha, x1, x2);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128) return alpha;

		NodeCounter++;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = AlphaBeta_2(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, beta, x2, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = AlphaBeta_2(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, beta, x1, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = AlphaBeta_2(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, beta, x1, x2);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score == -128)
			return EvalGameOver<3>(P);
		else
			return beta;

	}
	int AlphaBeta_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4)
	{
		int score = -128;
		unsigned long long flipped;
		++NodeCounter;
		
		//Play on x1
		if ((O & neighbour[x1]) && (flipped = flip(P, O, x1)))
		{
			score = -AlphaBeta_3(O ^ flipped, P ^ (1ULL << x1) ^ flipped, NodeCounter, -beta, -alpha, x2, x3, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x2
		if ((O & neighbour[x2]) && (flipped = flip(P, O, x2)))
		{
			score = -AlphaBeta_3(O ^ flipped, P ^ (1ULL << x2) ^ flipped, NodeCounter, -beta, -alpha, x1, x3, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x3
		if ((O & neighbour[x3]) && (flipped = flip(P, O, x3)))
		{
			score = -AlphaBeta_3(O ^ flipped, P ^ (1ULL << x3) ^ flipped, NodeCounter, -beta, -alpha, x1, x2, x4);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		//Play on x4
		if ((O & neighbour[x4]) && (flipped = flip(P, O, x4)))
		{
			score = -AlphaBeta_3(O ^ flipped, P ^ (1ULL << x4) ^ flipped, NodeCounter, -beta, -alpha, x1, x2, x3);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
			return alpha;
		}

		if (score != -128)
			return alpha;

		++NodeCounter;

		//Play on x1
		if ((P & neighbour[x1]) && (flipped = flip(O, P, x1)))
		{
			score = AlphaBeta_3(P ^ flipped, O ^ (1ULL << x1) ^ flipped, NodeCounter, alpha, beta, x2, x3, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x2
		if ((P & neighbour[x2]) && (flipped = flip(O, P, x2)))
		{
			score = AlphaBeta_3(P ^ flipped, O ^ (1ULL << x2) ^ flipped, NodeCounter, alpha, beta, x1, x3, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x3
		if ((P & neighbour[x3]) && (flipped = flip(O, P, x3)))
		{
			score = AlphaBeta_3(P ^ flipped, O ^ (1ULL << x3) ^ flipped, NodeCounter, alpha, beta, x1, x2, x4);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
		}

		//Play on x4
		if ((P & neighbour[x4]) && (flipped = flip(O, P, x4)))
		{
			score = AlphaBeta_3(P ^ flipped, O ^ (1ULL << x4) ^ flipped, NodeCounter, alpha, beta, x1, x2, x3);
			if (score <= alpha)	return alpha;
			if (score < beta) beta = score;
			return beta;
		}

		if (score == -128) //Nobody could play
			return EvalGameOver(P, 4);
		else
			return beta;
	}
	int AlphaBeta  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, int alpha, int beta)
	{
		const uint64_t empties = Empties(P, O);
		if (empties == 4)
			return AlphaBeta_4(P, O, NodeCounter, alpha, beta);

		uint64_t flipped;
		uint64_t BitBoardPossible = PossibleMoves(P, O);
		unsigned long Move;
		int score = -64;
		NodeCounter++;

		if (!BitBoardPossible){
			if (HasMoves(O, P))
				return -AlphaBeta(O, P, NodeCounter, -beta, -alpha);
			else //Game is over
				return BIND(EvalGameOver(P, empties), alpha, beta);
		}

		while (BitBoardPossible)
		{
			Move = BitScanLSB(BitBoardPossible);
			RemoveLSB(BitBoardPossible);
			flipped = flip(P, O, Move);
			score = -AlphaBeta(O ^ flipped, P ^ (1ULL << Move) ^ flipped, NodeCounter, -beta, -alpha);
			if (score >= beta) return beta;
			if (score > alpha) alpha = score;
		}

		return alpha;
	}
}