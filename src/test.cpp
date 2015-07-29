#include <iostream>
#include "flip_fast.h"
#include "flip_loop.h"
#include "macros_hell.h"
#include "perft_basic.h"
#include "possiblemoves.h"
#include "utility.h"

uint64_t line(int move, int dX, int dY)
{
	uint64_t ret = 0;
	int i = (move % 8) + dX; // Starting index in x direction
	int j = (move / 8) + dY; // Starting index in y direction

	while ((i >= 0) && (i < 8) && (j >= 0) && (j < 8)) // In between boundaries
	{
		ret |= 1ULL << (j * 8 + i);
		i += dX;
		j += dY;
	}

	return ret;
}

bool test_flip()
{
	uint64_t P, O;
	uint64_t mask[64];
	int max_i = 1 << 21;
	for (int move = 0; move < 64; move++)
		mask[move] = line(move, -1, -1)
		           | line(move, -1,  0)
		           | line(move, -1, +1)
		           | line(move,  0, -1)
		           | line(move,  0, +1)
		           | line(move, +1, -1)
		           | line(move, +1,  0)
		           | line(move, +1, +1);


	for (int move = 0; move < 64; move++)
		for (int i = 0; i < max_i; i++)
		{
			P = PDep(i, mask[move]);
			int max_j = 1 << (21 - PopCount(i));
			for (int j = 0; j < max_j; j++)
			{
				O = PDep(j, mask[move] ^ P);
				if (flip(P, O, move) != flip_loop(P, O, move)) return false;
			}
		}

	return true;
}

int main(int argc, char* argv[])
{
	std::cout << "Test flip: " << (test_flip() ? "passed" : "failed") << std::endl;
	Perft_Unroll2::perft(15);
	return 0;
}
