#include <iostream>
#include <cstdint>
#include <functional>
#include <random>
#include <atomic>
#include "line.h"
#include "flip_fast.h"
#include "macros_hell.h"
#include "possiblemoves.h"
#include "utility.h"
#include "perft_basic.h"
#include "datamanipulation.h"
#include "endgame.h"
#include "hashtable.h"


void benchmark_flip()
{
	const int SEED = 12;
	const int SIZE = 4 * 1024 * 1024;
	auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0, -1), std::mt19937_64(SEED));
	uint64_t * pos = new uint64_t[SIZE];
	uint64_t t;
	uint64_t sum = 0;
	uint64_t tmp = 0;

	for (unsigned int move = 0; move < 64; move++)
	{
		for (unsigned int i = 0; i < SIZE; i++) pos[i] = rnd() & ~(1ULL << move); // Initialize data

		t = ticks();
		for (unsigned int i = 0; i < SIZE; i += 8)
		{
			tmp ^= flip(pos[i + 0] & pos[i + 0], pos[i + 0] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 0] & pos[i + 1], pos[i + 0] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 0] & pos[i + 2], pos[i + 0] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 0] & pos[i + 3], pos[i + 0] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 0] & pos[i + 4], pos[i + 0] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 0] & pos[i + 5], pos[i + 0] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 0] & pos[i + 6], pos[i + 0] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 0] & pos[i + 7], pos[i + 0] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 1] & pos[i + 0], pos[i + 1] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 1] & pos[i + 1], pos[i + 1] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 1] & pos[i + 2], pos[i + 1] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 1] & pos[i + 3], pos[i + 1] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 1] & pos[i + 4], pos[i + 1] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 1] & pos[i + 5], pos[i + 1] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 1] & pos[i + 6], pos[i + 1] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 1] & pos[i + 7], pos[i + 1] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 2] & pos[i + 0], pos[i + 2] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 2] & pos[i + 1], pos[i + 2] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 2] & pos[i + 2], pos[i + 2] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 2] & pos[i + 3], pos[i + 2] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 2] & pos[i + 4], pos[i + 2] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 2] & pos[i + 5], pos[i + 2] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 2] & pos[i + 6], pos[i + 2] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 2] & pos[i + 7], pos[i + 2] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 3] & pos[i + 0], pos[i + 3] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 3] & pos[i + 1], pos[i + 3] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 3] & pos[i + 2], pos[i + 3] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 3] & pos[i + 3], pos[i + 3] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 3] & pos[i + 4], pos[i + 3] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 3] & pos[i + 5], pos[i + 3] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 3] & pos[i + 6], pos[i + 3] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 3] & pos[i + 7], pos[i + 3] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 4] & pos[i + 0], pos[i + 4] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 4] & pos[i + 1], pos[i + 4] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 4] & pos[i + 2], pos[i + 4] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 4] & pos[i + 3], pos[i + 4] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 4] & pos[i + 4], pos[i + 4] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 4] & pos[i + 5], pos[i + 4] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 4] & pos[i + 6], pos[i + 4] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 4] & pos[i + 7], pos[i + 4] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 5] & pos[i + 0], pos[i + 5] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 5] & pos[i + 1], pos[i + 5] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 5] & pos[i + 2], pos[i + 5] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 5] & pos[i + 3], pos[i + 5] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 5] & pos[i + 4], pos[i + 5] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 5] & pos[i + 5], pos[i + 5] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 5] & pos[i + 6], pos[i + 5] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 5] & pos[i + 7], pos[i + 5] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 6] & pos[i + 0], pos[i + 6] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 6] & pos[i + 1], pos[i + 6] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 6] & pos[i + 2], pos[i + 6] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 6] & pos[i + 3], pos[i + 6] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 6] & pos[i + 4], pos[i + 6] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 6] & pos[i + 5], pos[i + 6] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 6] & pos[i + 6], pos[i + 6] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 6] & pos[i + 7], pos[i + 6] ^ pos[i + 7], move);
			tmp ^= flip(pos[i + 7] & pos[i + 0], pos[i + 7] ^ pos[i + 0], move);
			tmp ^= flip(pos[i + 7] & pos[i + 1], pos[i + 7] ^ pos[i + 1], move);
			tmp ^= flip(pos[i + 7] & pos[i + 2], pos[i + 7] ^ pos[i + 2], move);
			tmp ^= flip(pos[i + 7] & pos[i + 3], pos[i + 7] ^ pos[i + 3], move);
			tmp ^= flip(pos[i + 7] & pos[i + 4], pos[i + 7] ^ pos[i + 4], move);
			tmp ^= flip(pos[i + 7] & pos[i + 5], pos[i + 7] ^ pos[i + 5], move);
			tmp ^= flip(pos[i + 7] & pos[i + 6], pos[i + 7] ^ pos[i + 6], move);
			tmp ^= flip(pos[i + 7] & pos[i + 7], pos[i + 7] ^ pos[i + 7], move);
			
			tmp ^= flip(pos[i + 0] ^ pos[i + 0], pos[i + 0] & pos[i + 0], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 1], pos[i + 0] & pos[i + 1], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 2], pos[i + 0] & pos[i + 2], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 3], pos[i + 0] & pos[i + 3], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 4], pos[i + 0] & pos[i + 4], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 5], pos[i + 0] & pos[i + 5], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 6], pos[i + 0] & pos[i + 6], move);
			tmp ^= flip(pos[i + 0] ^ pos[i + 7], pos[i + 0] & pos[i + 7], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 0], pos[i + 1] & pos[i + 0], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 1], pos[i + 1] & pos[i + 1], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 2], pos[i + 1] & pos[i + 2], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 3], pos[i + 1] & pos[i + 3], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 4], pos[i + 1] & pos[i + 4], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 5], pos[i + 1] & pos[i + 5], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 6], pos[i + 1] & pos[i + 6], move);
			tmp ^= flip(pos[i + 1] ^ pos[i + 7], pos[i + 1] & pos[i + 7], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 0], pos[i + 2] & pos[i + 0], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 1], pos[i + 2] & pos[i + 1], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 2], pos[i + 2] & pos[i + 2], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 3], pos[i + 2] & pos[i + 3], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 4], pos[i + 2] & pos[i + 4], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 5], pos[i + 2] & pos[i + 5], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 6], pos[i + 2] & pos[i + 6], move);
			tmp ^= flip(pos[i + 2] ^ pos[i + 7], pos[i + 2] & pos[i + 7], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 0], pos[i + 3] & pos[i + 0], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 1], pos[i + 3] & pos[i + 1], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 2], pos[i + 3] & pos[i + 2], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 3], pos[i + 3] & pos[i + 3], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 4], pos[i + 3] & pos[i + 4], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 5], pos[i + 3] & pos[i + 5], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 6], pos[i + 3] & pos[i + 6], move);
			tmp ^= flip(pos[i + 3] ^ pos[i + 7], pos[i + 3] & pos[i + 7], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 0], pos[i + 4] & pos[i + 0], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 1], pos[i + 4] & pos[i + 1], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 2], pos[i + 4] & pos[i + 2], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 3], pos[i + 4] & pos[i + 3], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 4], pos[i + 4] & pos[i + 4], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 5], pos[i + 4] & pos[i + 5], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 6], pos[i + 4] & pos[i + 6], move);
			tmp ^= flip(pos[i + 4] ^ pos[i + 7], pos[i + 4] & pos[i + 7], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 0], pos[i + 5] & pos[i + 0], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 1], pos[i + 5] & pos[i + 1], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 2], pos[i + 5] & pos[i + 2], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 3], pos[i + 5] & pos[i + 3], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 4], pos[i + 5] & pos[i + 4], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 5], pos[i + 5] & pos[i + 5], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 6], pos[i + 5] & pos[i + 6], move);
			tmp ^= flip(pos[i + 5] ^ pos[i + 7], pos[i + 5] & pos[i + 7], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 0], pos[i + 6] & pos[i + 0], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 1], pos[i + 6] & pos[i + 1], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 2], pos[i + 6] & pos[i + 2], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 3], pos[i + 6] & pos[i + 3], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 4], pos[i + 6] & pos[i + 4], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 5], pos[i + 6] & pos[i + 5], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 6], pos[i + 6] & pos[i + 6], move);
			tmp ^= flip(pos[i + 6] ^ pos[i + 7], pos[i + 6] & pos[i + 7], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 0], pos[i + 7] & pos[i + 0], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 1], pos[i + 7] & pos[i + 1], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 2], pos[i + 7] & pos[i + 2], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 3], pos[i + 7] & pos[i + 3], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 4], pos[i + 7] & pos[i + 4], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 5], pos[i + 7] & pos[i + 5], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 6], pos[i + 7] & pos[i + 6], move);
			tmp ^= flip(pos[i + 7] ^ pos[i + 7], pos[i + 7] & pos[i + 7], move);
		}
		t = ticks() - t;
		sum += t;

		if (tmp)
			printf("flip %s  %02.1f ticks.\n", field_name[move].c_str(), static_cast<double>(t) / SIZE / 16);
		else
			printf("flip %s  %02.1f ticks.\n", field_name[move].c_str(), static_cast<double>(t) / SIZE / 16);
	}
	printf("---------------------------\n");
	printf("flip avg %02.1f ticks.\n", static_cast<double>(sum) / SIZE / 16 / 64);

	delete[] pos;
}

void benchmark_hasmoves()
{
	const int SEED = 12;
	const int SIZE = 4 * 1024 * 1024;
	auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0, -1), std::mt19937_64(SEED));
	uint64_t * pos = new uint64_t[SIZE];
	uint64_t t;
	uint64_t tmp = 0;

	for (unsigned int i = 0; i < SIZE; i++) pos[i] = rnd(); // Initialize data

	t = ticks();
	for (unsigned int i = 0; i < SIZE; i += 8)
	{
		tmp ^= HasMoves(pos[i + 0] & pos[i + 0], pos[i + 0] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 1], pos[i + 0] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 2], pos[i + 0] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 3], pos[i + 0] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 4], pos[i + 0] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 5], pos[i + 0] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 6], pos[i + 0] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 0] & pos[i + 7], pos[i + 0] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 0], pos[i + 1] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 1], pos[i + 1] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 2], pos[i + 1] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 3], pos[i + 1] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 4], pos[i + 1] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 5], pos[i + 1] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 6], pos[i + 1] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 1] & pos[i + 7], pos[i + 1] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 0], pos[i + 2] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 1], pos[i + 2] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 2], pos[i + 2] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 3], pos[i + 2] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 4], pos[i + 2] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 5], pos[i + 2] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 6], pos[i + 2] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 2] & pos[i + 7], pos[i + 2] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 0], pos[i + 3] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 1], pos[i + 3] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 2], pos[i + 3] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 3], pos[i + 3] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 4], pos[i + 3] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 5], pos[i + 3] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 6], pos[i + 3] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 3] & pos[i + 7], pos[i + 3] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 0], pos[i + 4] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 1], pos[i + 4] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 2], pos[i + 4] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 3], pos[i + 4] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 4], pos[i + 4] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 5], pos[i + 4] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 6], pos[i + 4] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 4] & pos[i + 7], pos[i + 4] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 0], pos[i + 5] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 1], pos[i + 5] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 2], pos[i + 5] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 3], pos[i + 5] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 4], pos[i + 5] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 5], pos[i + 5] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 6], pos[i + 5] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 5] & pos[i + 7], pos[i + 5] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 0], pos[i + 6] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 1], pos[i + 6] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 2], pos[i + 6] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 3], pos[i + 6] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 4], pos[i + 6] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 5], pos[i + 6] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 6], pos[i + 6] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 6] & pos[i + 7], pos[i + 6] ^ pos[i + 7]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 0], pos[i + 7] ^ pos[i + 0]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 1], pos[i + 7] ^ pos[i + 1]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 2], pos[i + 7] ^ pos[i + 2]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 3], pos[i + 7] ^ pos[i + 3]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 4], pos[i + 7] ^ pos[i + 4]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 5], pos[i + 7] ^ pos[i + 5]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 6], pos[i + 7] ^ pos[i + 6]);
		tmp ^= HasMoves(pos[i + 7] & pos[i + 7], pos[i + 7] ^ pos[i + 7]);

		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 0], pos[i + 0] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 1], pos[i + 0] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 2], pos[i + 0] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 3], pos[i + 0] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 4], pos[i + 0] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 5], pos[i + 0] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 6], pos[i + 0] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 0] ^ pos[i + 7], pos[i + 0] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 0], pos[i + 1] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 1], pos[i + 1] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 2], pos[i + 1] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 3], pos[i + 1] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 4], pos[i + 1] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 5], pos[i + 1] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 6], pos[i + 1] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 1] ^ pos[i + 7], pos[i + 1] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 0], pos[i + 2] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 1], pos[i + 2] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 2], pos[i + 2] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 3], pos[i + 2] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 4], pos[i + 2] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 5], pos[i + 2] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 6], pos[i + 2] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 2] ^ pos[i + 7], pos[i + 2] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 0], pos[i + 3] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 1], pos[i + 3] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 2], pos[i + 3] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 3], pos[i + 3] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 4], pos[i + 3] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 5], pos[i + 3] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 6], pos[i + 3] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 3] ^ pos[i + 7], pos[i + 3] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 0], pos[i + 4] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 1], pos[i + 4] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 2], pos[i + 4] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 3], pos[i + 4] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 4], pos[i + 4] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 5], pos[i + 4] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 6], pos[i + 4] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 4] ^ pos[i + 7], pos[i + 4] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 0], pos[i + 5] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 1], pos[i + 5] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 2], pos[i + 5] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 3], pos[i + 5] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 4], pos[i + 5] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 5], pos[i + 5] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 6], pos[i + 5] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 5] ^ pos[i + 7], pos[i + 5] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 0], pos[i + 6] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 1], pos[i + 6] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 2], pos[i + 6] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 3], pos[i + 6] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 4], pos[i + 6] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 5], pos[i + 6] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 6], pos[i + 6] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 6] ^ pos[i + 7], pos[i + 6] & pos[i + 7]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 0], pos[i + 7] & pos[i + 0]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 1], pos[i + 7] & pos[i + 1]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 2], pos[i + 7] & pos[i + 2]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 3], pos[i + 7] & pos[i + 3]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 4], pos[i + 7] & pos[i + 4]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 5], pos[i + 7] & pos[i + 5]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 6], pos[i + 7] & pos[i + 6]);
		tmp ^= HasMoves(pos[i + 7] ^ pos[i + 7], pos[i + 7] & pos[i + 7]);
	}
	t = ticks() - t;

	if (tmp)
		printf("has moves  %02.1f ticks.\n", static_cast<double>(t) / SIZE / 16);
	else
		printf("has moves  %02.1f ticks.\n", static_cast<double>(t) / SIZE / 16);

	delete[] pos;
}

void benchmark_possiblemoves()
{
	const int SEED = 12;
	const int SIZE = 4 * 1024 * 1024;
	auto rnd = std::bind(std::uniform_int_distribution<uint64_t>(0, -1), std::mt19937_64(SEED));
	uint64_t * pos = new uint64_t[SIZE];
	uint64_t t;
	uint64_t tmp = 0;

	for (unsigned int i = 0; i < SIZE; i++) pos[i] = rnd(); // Initialize data

	t = ticks();
	for (unsigned int i = 0; i < SIZE; i += 8)
	{
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 0], pos[i + 0] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 1], pos[i + 0] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 2], pos[i + 0] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 3], pos[i + 0] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 4], pos[i + 0] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 5], pos[i + 0] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 6], pos[i + 0] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 0] & pos[i + 7], pos[i + 0] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 0], pos[i + 1] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 1], pos[i + 1] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 2], pos[i + 1] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 3], pos[i + 1] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 4], pos[i + 1] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 5], pos[i + 1] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 6], pos[i + 1] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 1] & pos[i + 7], pos[i + 1] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 0], pos[i + 2] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 1], pos[i + 2] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 2], pos[i + 2] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 3], pos[i + 2] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 4], pos[i + 2] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 5], pos[i + 2] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 6], pos[i + 2] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 2] & pos[i + 7], pos[i + 2] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 0], pos[i + 3] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 1], pos[i + 3] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 2], pos[i + 3] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 3], pos[i + 3] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 4], pos[i + 3] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 5], pos[i + 3] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 6], pos[i + 3] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 3] & pos[i + 7], pos[i + 3] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 0], pos[i + 4] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 1], pos[i + 4] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 2], pos[i + 4] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 3], pos[i + 4] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 4], pos[i + 4] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 5], pos[i + 4] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 6], pos[i + 4] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 4] & pos[i + 7], pos[i + 4] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 0], pos[i + 5] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 1], pos[i + 5] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 2], pos[i + 5] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 3], pos[i + 5] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 4], pos[i + 5] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 5], pos[i + 5] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 6], pos[i + 5] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 5] & pos[i + 7], pos[i + 5] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 0], pos[i + 6] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 1], pos[i + 6] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 2], pos[i + 6] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 3], pos[i + 6] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 4], pos[i + 6] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 5], pos[i + 6] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 6], pos[i + 6] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 6] & pos[i + 7], pos[i + 6] ^ pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 0], pos[i + 7] ^ pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 1], pos[i + 7] ^ pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 2], pos[i + 7] ^ pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 3], pos[i + 7] ^ pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 4], pos[i + 7] ^ pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 5], pos[i + 7] ^ pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 6], pos[i + 7] ^ pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 7] & pos[i + 7], pos[i + 7] ^ pos[i + 7]);

		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 0], pos[i + 0] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 1], pos[i + 0] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 2], pos[i + 0] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 3], pos[i + 0] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 4], pos[i + 0] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 5], pos[i + 0] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 6], pos[i + 0] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 0] ^ pos[i + 7], pos[i + 0] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 0], pos[i + 1] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 1], pos[i + 1] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 2], pos[i + 1] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 3], pos[i + 1] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 4], pos[i + 1] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 5], pos[i + 1] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 6], pos[i + 1] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 1] ^ pos[i + 7], pos[i + 1] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 0], pos[i + 2] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 1], pos[i + 2] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 2], pos[i + 2] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 3], pos[i + 2] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 4], pos[i + 2] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 5], pos[i + 2] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 6], pos[i + 2] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 2] ^ pos[i + 7], pos[i + 2] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 0], pos[i + 3] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 1], pos[i + 3] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 2], pos[i + 3] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 3], pos[i + 3] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 4], pos[i + 3] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 5], pos[i + 3] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 6], pos[i + 3] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 3] ^ pos[i + 7], pos[i + 3] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 0], pos[i + 4] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 1], pos[i + 4] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 2], pos[i + 4] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 3], pos[i + 4] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 4], pos[i + 4] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 5], pos[i + 4] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 6], pos[i + 4] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 4] ^ pos[i + 7], pos[i + 4] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 0], pos[i + 5] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 1], pos[i + 5] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 2], pos[i + 5] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 3], pos[i + 5] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 4], pos[i + 5] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 5], pos[i + 5] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 6], pos[i + 5] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 5] ^ pos[i + 7], pos[i + 5] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 0], pos[i + 6] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 1], pos[i + 6] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 2], pos[i + 6] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 3], pos[i + 6] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 4], pos[i + 6] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 5], pos[i + 6] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 6], pos[i + 6] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 6] ^ pos[i + 7], pos[i + 6] & pos[i + 7]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 0], pos[i + 7] & pos[i + 0]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 1], pos[i + 7] & pos[i + 1]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 2], pos[i + 7] & pos[i + 2]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 3], pos[i + 7] & pos[i + 3]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 4], pos[i + 7] & pos[i + 4]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 5], pos[i + 7] & pos[i + 5]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 6], pos[i + 7] & pos[i + 6]);
		tmp ^= PossibleMoves(pos[i + 7] ^ pos[i + 7], pos[i + 7] & pos[i + 7]);
	}
	t = ticks() - t;

	if (tmp)
		printf("possible moves  %02.1f ticks.\n", static_cast<double>(t) / SIZE / 16);
	else
		printf("possible moves  %02.1f ticks.\n", static_cast<double>(t) / SIZE / 16);

	delete[] pos;
}

void benchmark_perft()
{
	Perft_Unroll2::perft(13);
}

void printHeader_PV(const int width_num)
{
	const int width_Nodes = 16;
	const int width_nps = 11;
	const int width_PV = 5 * 3 + 1;
	const int width_PV_front = static_cast<int>((width_PV - 2) / 2.0f + 0.5f);
	std::string L1, L2;

	if (width_num)
	{
		L1.append(std::string(width_num - 2, ' ')).append("# |");
		L2.append(std::string(width_num, '-')).append("+");
	}

	L1.append(" depth|score|       time (s) |");
	L2.append("------+-----+----------------+");

	L1.append(std::string(width_Nodes - 10, ' ')).append("nodes (N) |");
	L2.append(std::string(width_Nodes, '-')).append("+");

	L1.append(std::string(width_nps - 8, ' ')).append("N/s     |");
	L2.append(std::string(width_nps, '-')).append("+");

	L1.append(std::string(width_PV_front, ' ')).append("PV").append(std::string(width_PV - width_PV_front - 2, ' '));
	L2.append(std::string(width_PV, '-'));

	printf("%s\n%s\n", L1.c_str(), L2.c_str());
}
void printHeader_FullScore(const int width_num)
{
	const int width_Nodes = 16;
	const int width_nps = 11;
	const int width_PV = 5 * 3 + 1;
	const int width_PV_front = static_cast<int>((width_PV - 2) / 2.0f + 0.5f);
	std::string L1, L2;

	if (width_num)
	{
		L1.append(std::string(width_num - 2, ' ')).append("# |");
		L2.append(std::string(width_num, '-')).append("+");
	}

	L1.append(" depth|score|       time (s) |");
	L2.append("------+-----+----------------+");

	L1.append(std::string(width_Nodes - 10, ' ')).append("nodes (N) |");
	L2.append(std::string(width_Nodes, '-')).append("+");

	L1.append(std::string(width_nps - 8, ' ')).append("N/s     |");
	L2.append(std::string(width_nps, '-')).append("+");
	
	L1.append("scores");
	L2.append("-------");

	printf("%s\n%s\n", L1.c_str(), L2.c_str());
}

void print_stats_PV(const int width_num, int num, int depth, int score, std::chrono::milliseconds duration, uint64_t nodes, const std::string& PV)
{
	static std::atomic_flag Spinlock;
	const int width_Nodes = 16;
	const int width_nps = 11;

	while (Spinlock.test_and_set()) ;
	printf("%*u|%6d| %+2.2d |%16s|%*s|%*s| %s\n", width_num, num, depth, score, 
		time_format(duration).c_str(), width_Nodes, ThousandsSeparator(nodes).c_str(),
		width_nps, (duration.count() == 0 ? "" : ThousandsSeparator(nodes * 1000ULL / duration.count()).c_str()),
		PV.c_str());
	Spinlock.clear();
}
void print_stats_FullScore(const int width_num, int num, int depth, std::chrono::milliseconds duration, uint64_t nodes, const std::string& scores)
{
	static std::atomic_flag Spinlock;
	const int width_Nodes = 16;
	const int width_nps = 11;

	while (Spinlock.test_and_set()) ;
	printf("%*u|%6d|%16s|%*s|%*s| %s\n", width_num, num, depth, 
		time_format(duration).c_str(), width_Nodes, ThousandsSeparator(nodes).c_str(),
		width_nps, (duration.count() == 0 ? "" : ThousandsSeparator(nodes * 1000ULL / duration.count()).c_str()),
		scores.c_str());
	Spinlock.clear();
}

void FForum_Benchmark(const std::string & filename)
{
	uint64_t OverAllNodeCounter = 0;
	uint64_t NodeCounter = 0;
	std::chrono::high_resolution_clock::time_point StartTime, OverAllStartTime;
	std::vector<CDataset_Position_FullScore> vec = read_hr_vector(filename);

	printHeader_PV(2);

	OverAllStartTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < vec.size(); i++)
	{
		NodeCounter = 0;
		CLine pline(5);
		StartTime = std::chrono::high_resolution_clock::now();
		int score = Endgame::Eval(vec[i].P, vec[i].O, NodeCounter, &pline);
		std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - StartTime);
		print_stats_PV(2, i, Empties(vec[i].P, vec[i].O), score, duration, NodeCounter, pline.GetPV());
		if (score != vec[i].BestScore())
			printf("ERROR: Miscalculation! Score should be %+2.2d.\n", vec[i].BestScore());
		OverAllNodeCounter += NodeCounter;
	}

	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - OverAllStartTime);
	printf(       "%s nodes in %s (%s N/s).\n\n", ThousandsSeparator(OverAllNodeCounter), time_format(duration).c_str(), (duration.count() == 0 ? "" : ThousandsSeparator(OverAllNodeCounter * 1000ULL / duration.count()).c_str()));
}

int main(int argc, char* argv[])
{
	TT = CHashTable(10000000);
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-1-19.obf");
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-20-39.obf");
	FForum_Benchmark("C:\\Cassandra\\pos\\fforum-40-59.obf");
	TT.print_stats();
	return 0;

	benchmark_perft();
	//benchmark_possiblemoves();
	//benchmark_hasmoves();
	benchmark_flip();
	return 0;
}