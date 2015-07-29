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

void CSearch::Evaluate()
{
	const int empties = Empties(P, O);
	const int MAX_SELECTIVITY = 6;
	const int D = MIN(static_cast<int>(depth), empties);

	if (depth >= empties) // Endgame
	{
		if (empties <= 16)
		{
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, empties, empties, &PV_line);
			print_stats(empties, selectivity);
		}
		else if (empties <= 22)
		{
			// Iterative deepening
			for (int d = D % 2; d < D; d += 2)
			{
				if (empties - d >= 10)
				{
					score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, d, empties, &PV_line);
					print_stats(d, MAX_SELECTIVITY);
				}
			}
			
			score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, empties, empties, &PV_line);
			print_stats(empties, MAX_SELECTIVITY);

			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, empties, empties, &PV_line);
			print_stats(empties, selectivity);
		}
		else
		{
			// Iterative deepening
			for (int d = D % 2; d < D; d += 2)
			{
				if (empties - d >= 10)
				{
					score = Eval(P, O, NodeCounter, alpha, beta, MAX_SELECTIVITY, d, empties, &PV_line);
					print_stats(d, MAX_SELECTIVITY);
				}
			}

			// Iterative broadening
			for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
			{
				score = Eval(P, O, NodeCounter, alpha, beta, s, D, empties, &PV_line);
				print_stats(D, s);
			}
		}
	}
	else
	{
		if (depth <= 3)
		{
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, depth, empties, &PV_line);
			print_stats(depth, selectivity);
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
			for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
			{
				score = Eval(P, O, NodeCounter, alpha, beta, s, D, empties, &PV_line);
				print_stats(D, s);
			}

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
