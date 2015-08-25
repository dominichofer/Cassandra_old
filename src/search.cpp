#include "search.h"

void CSearch::print_stats(const int8_t depth, const uint8_t selectivity)
{
	if (verbose >= 3)
	{
		std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
		Screen.printStats(depth, selectivity, P, O, score, duration, NodeCounter, GetPV());
	}
}

void CSearch::print_result(const int number)
{
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	Screen.printResult(number, depth, selectivity, P, O, score, duration, NodeCounter, GetPV());
	TT.AdvanceDate();
}

int CSearch::Aspiration_Search(uint64_t P, uint64_t O, uint64_t& NodeCounter, int alpha, int beta, int score, int selectivity, int depth, int empties, CLine& PV_line)
{
	int width = (depth == empties ? 2 : 1);
	int down = width;
	int up = width;

	while (true)
	{
		int low = MAX(alpha, score - down);
		int high = MIN(beta, score + up);

		score = Eval(P, O, NodeCounter, low, high, selectivity, depth, empties, &PV_line);
		print_stats(depth, selectivity);

		if (score <= low && score > alpha && down > 0) // fail low
		{
			down *= 2;
			up = 0;
		}
		else if (score >= high && score < beta && up > 0) // fail high
		{
			down = 0;
			up *= 2;
		}
		else
			break;
	}
	return score;
}

void CSearch::Evaluate()
{
	const int empties = Empties(P, O);
	const int MAX_SELECTIVITY = 6;
	int D = MIN(static_cast<int>(depth), empties);

	if (depth >= empties) // Endgame
	{
		if (empties <= 14)
		{
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, empties, empties, &PV_line);
			print_stats(empties, selectivity);
		}
		else if (empties <= 20)
		{
			// Iterative deepening
			for (int d = D % 2; d <= D; d += 2)
			{
				if (empties - d >= 10)
				{
					score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, d, empties, &PV_line);
					print_stats(d, MAX_SELECTIVITY);
				}
			}

			// Iterative broadening
			for (int s : {6, 0})
				score = Aspiration_Search(P, O, NodeCounter, alpha, beta, score, s, D, empties, PV_line);
		}
		else
		{
			// Initial Eval
			for (int d = D % 2; d <= D % 2 + 4; d += 2)
			{
				score = Eval(P, O, NodeCounter, alpha, beta, NO_SELECTIVITY, d, empties, &PV_line);
				print_stats(d, NO_SELECTIVITY);
			}

			// Iterative deepening
			for (int d = D % 2 + 4; d < D; d += 2)
				if (empties - d >= 10)
					score = Aspiration_Search(P, O, NodeCounter, alpha, beta, score, MAX_SELECTIVITY, d, empties, PV_line);

			if (empties < 30)
			{
				// Iterative broadening
				for (int s : {6, 4, 0})
					score = Aspiration_Search(P, O, NodeCounter, alpha, beta, score, s, D, empties, PV_line);
			}
			else
			{
				// Iterative broadening
				for (int s : {6, 4, 3, 2, 1, 0})
					score = Aspiration_Search(P, O, NodeCounter, alpha, beta, score, s, D, empties, PV_line);
			}
		}
	}
	else
	{
		if (D <= 3)
		{
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, D, empties, &PV_line);
			print_stats(D, selectivity);
		}
		else if (depth <= 10)
		{
			for (int d = D % 2; d <= D; d += 2)
			{
				score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, d, empties, &PV_line);
				print_stats(d, MAX_SELECTIVITY);
			}
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, D, empties, &PV_line);
			print_stats(D, selectivity);
		}
		else
		{
			for (int d = D % 2; d < D; d += 2)
			{
				if (empties - d >= 10)
				{
					score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, d, empties, &PV_line);
					print_stats(d, MAX_SELECTIVITY);
				}
			}

			// Iterative broadening
			for (int s : {6, 3, 0})
				score = Aspiration_Search(P, O, NodeCounter, alpha, beta, score, s, D, empties, PV_line);
		}
	}

	//// Iterative deepening
	//for (int d = D % 2; d < D; d += 2)
	//{
	//	if (empties - d >= 10)
	//	{
	//		score = Eval(P, O, NodeCounter, m_alpha, m_beta, MAX_SELECTIVITY, d, empties, &PV_line);
	//		print_stats(d, MAX_SELECTIVITY);
	//	}
	//}

	//// Iterative broadening
	//for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
	//{
	//	score = Eval(P, O, NodeCounter, alpha, beta, s, D, empties, &PV_line);
	//	print_stats(D, s);
	//}

	endTime = std::chrono::high_resolution_clock::now();
}

void CSearch::EvaluateDirect()
{
	const int empties = Empties(P, O);
	int D = MIN(static_cast<int>(depth), empties);
	
	score = Eval(P, O, NodeCounter, alpha, beta, NO_SELECTIVITY, D, empties, &PV_line);
	print_stats(D, NO_SELECTIVITY);

	endTime = std::chrono::high_resolution_clock::now();
}