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
	int alpha_w, beta_w, window;

	if (depth >= empties) // Endgame
	{
		if (empties <= 16)
		{
			score = Eval(P, O, NodeCounter, alpha, beta, selectivity, empties, empties, &PV_line);
			print_stats(empties, selectivity);
		}
		else if (empties < 22)
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

			//// Iterative broadening
			//for (int s = MAX_SELECTIVITY; s >= selectivity; s -= MAX_SELECTIVITY)
			//{
			//	score = Eval(P, O, NodeCounter, alpha, beta, s, D, empties, &PV_line);
			//	print_stats(D, s);
			//}

			// Iterative broadening
			for (int s = MAX_SELECTIVITY; s >= selectivity; s -= MAX_SELECTIVITY)
			{
				window = 2;
				alpha_w = MAX(-64, score - window);
				beta_w = MIN(64, score + window);

				score = Eval(P, O, NodeCounter, alpha_w, beta_w, s, D, empties, &PV_line);
				print_stats(D, s);
				if (score >= beta_w && beta_w != 64)
				{
					score = Eval(P, O, NodeCounter, score-1, 64, s, D, empties, &PV_line);
					print_stats(D, s);
				}
				else if (score <= alpha_w && alpha_w != -64)
				{
					score = Eval(P, O, NodeCounter, -64, score+1, s, D, empties, &PV_line);
					print_stats(D, s);
				}
			}
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

			//// Iterative broadening
			//for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
			//{
			//	score = Eval(P, O, NodeCounter, alpha, beta, s, D, empties, &PV_line);
			//	print_stats(D, s);
			//}

			//// Iterative broadening
			//for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
			//{
			//	window = 2;
			//	alpha_w = MAX(-64, score - window);
			//	beta_w = MIN(64, score + window);

			//	score = Eval(P, O, NodeCounter, alpha_w, beta_w, s, D, empties, &PV_line);
			//	print_stats(D, s);
			//	if (score >= beta_w && beta_w != 64)
			//	{
			//		score = Eval(P, O, NodeCounter, score-1, 64, s, D, empties, &PV_line);
			//		print_stats(D, s);
			//	}
			//	else if (score <= alpha_w && alpha_w != -64)
			//	{
			//		score = Eval(P, O, NodeCounter, -64, score+1, s, D, empties, &PV_line);
			//		print_stats(D, s);
			//	}
			//}

			// Iterative broadening
			for (int s = MAX_SELECTIVITY; s >= selectivity; s -= 2)
			{
				window = 2;
				alpha_w = MAX(-64, score - window);
				beta_w = MIN(64, score + window);

				while (true)
				{
					score = Eval(P, O, NodeCounter, alpha_w, beta_w, s, D, empties, &PV_line);
					print_stats(D, s);
					if (score >= beta_w && beta_w != 64)
					{
						alpha_w = score - 1;
						beta_w = MIN(64, alpha_w + 2 * window);
					}
					else if (score <= alpha_w && alpha_w != -64)
					{
						beta_w = score + 1;
						alpha_w = MAX(-64, beta_w - 2 * window);
					}
					else
						break;
					window += 2;
				}
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
