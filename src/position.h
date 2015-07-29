#pragma once
#include "flip_fast.h"
#include "possiblemoves.h"
#include "utility.h"
#include "macros_hell.h"
#include <utility>

#define PLAY_STONE_SWAP


const uint64_t START_POSITION_P = 0x0000000810000000ULL;
const uint64_t START_POSITION_O = 0x0000001008000000ULL;
const uint64_t START_POSITION_ETH_P = 0x0000001800000000ULL;
const uint64_t START_POSITION_ETH_O = 0x0000000018000000ULL;

const std::string field_name[65] = {
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8", "--"
};
const uint64_t neighbour[64] = { // Neighbours to the input-field
	0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,	0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
	0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,	0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
	0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,	0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
	0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,	0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
	0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,	0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
	0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,	0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
	0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,	0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
	0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,	0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
};
const uint8_t quadrant_id_2_bit[64] = {   
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3
};
const uint8_t quadrant_id_4_bit[64] = {   
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	1, 1, 1, 1, 2, 2, 2, 2,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8,
	4, 4, 4, 4, 8, 8, 8, 8
};
const uint64_t quadrant_mask[64] = {
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x000000000F0F0F0FULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL, 0x00000000F0F0F0F0ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
	0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0x0F0F0F0F00000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL, 0xF0F0F0F000000000ULL,
};
const uint64_t quadrant[16] = {   
	0x0000000000000000ULL, //0000
	0x000000000F0F0F0FULL, //0001
	0x00000000F0F0F0F0ULL, //0010
	0x00000000FFFFFFFFULL, //0011
	0x0F0F0F0F00000000ULL, //0100
	0x0F0F0F0F0F0F0F0FULL, //0101
	0x0F0F0F0FF0F0F0F0ULL, //0110
	0x0F0F0F0FFFFFFFFFULL, //0111
	0xF0F0F0F000000000ULL, //1000
	0xF0F0F0F00F0F0F0FULL, //1001
	0xF0F0F0F0F0F0F0F0ULL, //1010
	0xF0F0F0F0FFFFFFFFULL, //1011
	0xFFFFFFFF00000000ULL, //1100
	0xFFFFFFFF0F0F0F0FULL, //1101
	0xFFFFFFFFF0F0F0F0ULL, //1110
	0xFFFFFFFFFFFFFFFFULL  //1111
};

uint64_t FlipDiagonal(uint64_t b);
uint64_t FlipCodiagonal(uint64_t b);
uint64_t FlipVertical(uint64_t b);
uint64_t FlipHorizontal(uint64_t b);

std::string board1D(const uint64_t P, const uint64_t O);
std::string board2D(const uint64_t P, const uint64_t O);

inline void ResetPosition(uint64_t & P, uint64_t & O, const bool ETH)
{
	P = ETH ? START_POSITION_ETH_P : START_POSITION_P;
	O = ETH ? START_POSITION_ETH_O : START_POSITION_O;
}

FORCE_INLINE uint64_t Empties(const uint64_t P, const uint64_t O) { return PopCount(~(P | O)); }
inline uint64_t Parity(uint64_t E)
{
	#ifdef HAS_PEXT
		// 4 x SHIFT, 4 x XOR, 1 x PEXT
		// = 9 OPs
		E ^= E >>  1;
		E ^= E >>  2;
		E ^= E >>  8;
		E ^= E >> 16;
		return PExt(E, 0x0000001100000011ULL);
	#else
		// 6 x SHIFT, 4 x XOR, 2 x AND, 2 x OR 
		// = 14 OPs
		E ^= E >>  1;
		E ^= E >>  2;
		E ^= E >>  8;
		E ^= E >> 16;
		E &= 0x0000001100000011ULL;
		E |= E >>  3;
		E |= E >> 30;
		return E & 0xFULL;
	#endif
}
inline uint64_t Parity(const uint64_t P, const uint64_t O) { return Parity(~(P | O)); }

inline void PlayStone(uint64_t & P, uint64_t & O, const int move)
{
	#ifdef PLAY_STONE_SWAP
		const uint64_t flipped = flip(P, O, move);
		P ^= flipped ^ (1ULL << move);
		O ^= flipped;
		std::swap(P, O);
	#else
		const uint64_t flipped = flip(P, O, move);
		O ^= P;                   // O' == O ^ (P)
		P ^= O ^ flipped;         // P' == P ^ (O ^ P ^ flipped) == O ^ flipped
		O ^= P ^ (1ULL << move);  // O' == O' ^ (P' ^ (1ULL << move)) == (O ^ P) ^ (O ^ flipped) ^ (1ULL << move) == P ^ flipped ^ (1ULL << move)
	#endif
}

inline uint64_t StableStonesCornerAndCo(const uint64_t O)
{
	#ifdef HAS_PEXT
		// 1 x AND, 1 x OR, 1 x PEXT, 2 x PDEP
		// = 5 OPs
		const uint64_t tmp = PExt(O, 0x8100000000000081ULL);
		return (PDep(tmp, 0x4200000000000042ULL) | PDep(tmp, 0x0081000000008100ULL)) & O;
	#else
		// 5x AND, 4x SHIFT, 4x OR
		// = 13 OPs
		return (
				((O & 0x0100000000000001ULL) << 1) |
				((O & 0x8000000000000080ULL) >> 1) |
				((O & 0x8100000000000081ULL) << 8) |
				((O & 0x8100000000000081ULL) >> 8) |
					  0x8100000000000081ULL
				) & O;
	#endif
}
//inline uint64_t StableStones_affectables(const uint64_t P, const uint64_t O) { return O & ~AFFECTABLE(~(P | O)); }
uint64_t StableStonesFullEdges(const uint64_t P, const uint64_t O);
uint64_t StableStonesFullEdgesSecondOrder(const uint64_t P, const uint64_t O);
uint64_t StableStonesTriangles(const uint64_t O);
uint64_t StableStonesSkyline(const uint64_t O);
inline uint64_t StableStones(const uint64_t P, const uint64_t O)
{
	return StableStonesFullEdges(P, O) | StableStonesFullEdgesSecondOrder(P, O) | StableStonesTriangles(O);
}

FORCE_INLINE uint64_t SMEAR_BITBOARD(uint64_t B)
{
	// 4x SHIFT, 4x OR, 2x AND 
	// = 10 OPs
	B |= (B >> 1) & 0x7F7F7F7F7F7F7F7FULL | (B << 1) & 0xFEFEFEFEFEFEFEFEULL;
	return B | (B >> 8) | (B << 8);
}

inline uint64_t PlayersBoarder(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P) & ~(P | O); } // 13 OPs
inline uint64_t OpponentsBoarder(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(O) & ~(P | O); } // 13 OPs
inline uint64_t Boarders(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(P | O) & ~(P | O); } // 14 OPs

inline uint64_t PlayersExposed(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & P; } // 13 OPs
inline uint64_t OpponentsExposed(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & O; } // 13 OPs
inline uint64_t Exposeds(const uint64_t P, const uint64_t O) { return SMEAR_BITBOARD(~(P | O)) & (P | O); } // 14 OPs

struct CPosition
{
	uint64_t P, O;
	
	CPosition() : P(0), O(0) {}
	CPosition(uint64_t P, uint64_t O) : P(P), O(O) {}

	inline bool operator==(const CPosition& other) const { return (this->P == other.P) &&  (this->O == other.O); }
	inline bool operator!=(const CPosition& other) const { return (this->P != other.P) ||  (this->O != other.O); }
	inline bool operator<=(const CPosition& other) const { return (this->P <= other.P) || ((this->P == other.P) && (this->O <= other.O)); }
	inline bool operator>=(const CPosition& other) const { return (this->P >= other.P) || ((this->P == other.P) && (this->O >= other.O)); }
	inline bool operator< (const CPosition& other) const { return (this->P <  other.P) || ((this->P == other.P) && (this->O <  other.O)); }
	inline bool operator> (const CPosition& other) const { return (this->P >  other.P) || ((this->P == other.P) && (this->O >  other.O)); }
};

struct CPositionScore
{
	uint64_t P, O;
	 int8_t score;
	
	CPositionScore() : P(0), O(0), score(0) {}
	CPositionScore(uint64_t P, uint64_t O, int8_t score) : P(P), O(O), score(score) {}
};