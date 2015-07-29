#include "flip_loop.h"

uint64_t flip_loop_dir(const uint64_t P, const uint64_t O, const uint8_t move, const int dX, const int dY)
{
	uint64_t flips = 0;
	uint64_t tmp;
	int i = (move % 8) + dX; // Starting index in x direction
	int j = (move / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		tmp = 1ULL << (j * 8 + i); // The bit to look at
		if (O & tmp) // The bit belongs to the opponent
			flips |= tmp; // Add to possible flips
		else if (P & tmp) // The bit belongs to the player
			return flips; // All possible flips become real flips
		else // The bit belongs to no player
			return 0; // There are no possible flips
		i += dX; // Advance in direction
		j += dY; // Advance in direction
	}
	return 0;
}

uint64_t flip_loop(const uint64_t P, const uint64_t O, const uint8_t move)
{
	return flip_loop_dir(P, O, move, -1, -1)
		 | flip_loop_dir(P, O, move, -1,  0)
		 | flip_loop_dir(P, O, move, -1, +1)
		 | flip_loop_dir(P, O, move,  0, -1)
		 | flip_loop_dir(P, O, move,  0, +1)
		 | flip_loop_dir(P, O, move, +1, -1)
		 | flip_loop_dir(P, O, move, +1,  0)
		 | flip_loop_dir(P, O, move, +1, +1);
}