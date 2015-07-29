#pragma once
#include <chrono>
#include <iostream>
#include "flip_fast.h"
#include "macros_hell.h"
#include "possiblemoves.h"
#include "position.h"
#include "utility.h"

// Provides two simple perft functions.
// Perft_Basic contains the simplest perft function.
// Perft_Unroll2 contains a perft function which is unrolled for the last two plies.

namespace Perft_Basic
{
	void perft(const uint8_t depth);
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth);
}

namespace Perft_Unroll2
{
	void perft(const uint8_t depth);
	uint64_t perft(const uint64_t P, const uint64_t O, const uint8_t depth);
}