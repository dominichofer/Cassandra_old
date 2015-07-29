#pragma once
#include <cstdint>

// Provides a flip function that can run in parallel.
// The code is the same for every input.
// Especially useful for GPUs.

namespace Flip_Parallel
{
	void Initialize();
}

uint64_t flip(const uint64_t P, const uint64_t O, const uint8_t move);
