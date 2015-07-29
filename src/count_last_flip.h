#pragma once
#include <cstdint>
#include "macros_hell.h"

namespace CountLastFlip
{
	extern uint8_t (*CLF_fp[64])(const uint64_t);

	void Initialize();
}

FORCE_INLINE uint8_t count_last_flip(const uint64_t P, const uint8_t move) { return CountLastFlip::CLF_fp[move](P); }