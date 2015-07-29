#pragma once
#include <cstdint>
#include <cassert>
#include <atomic>
#include <iostream>
#include "datamanipulation.h"

class CHashTableValueType
{
public:
	uint8_t cost;        //   8 Bit
	 int8_t depth;       //   8 Bit
	uint8_t selectivity; //   8 Bit
	 int8_t alpha;       //   8 Bit
	 int8_t beta;        //   8 Bit
	uint8_t PV;          //   8 Bit
	uint8_t AV;          //   8 Bit
	                     // -------
	                     //  56 Bit = 7 Byte

	CHashTableValueType() : cost(0), depth(-1), selectivity(0), alpha(-99), beta(99), PV(64), AV(64) {}
	CHashTableValueType(const uint64_t NodeCounter, const int8_t depth, const uint8_t selectivity, const int8_t alpha, const int8_t beta, const uint8_t PV, const uint8_t AV) :
		cost(BitScanMSB(NodeCounter)), depth(depth), selectivity(selectivity), alpha(alpha), beta(beta), PV(PV), AV(AV) {}
};

class CTwoNode
{
	static const uint8_t OLD_AGE = 6;
public:
	uint64_t m_P1, m_O1, m_P2, m_O2;        // 256 Bit
	CHashTableValueType m_value1, m_value2;	// 112 Bit
	uint8_t m_date1, m_date2;               //  16 Bit
private:
	mutable std::atomic_flag spinlock;      //  64 Bit
	                                        // -------
	                                        // 448 Bit = 56 Byte
public:
	CTwoNode() : m_P1(0), m_O1(0), m_P2(0), m_O2(0), m_date1(0), m_date2(0) { spinlock.clear(); }
	CTwoNode(const uint64_t P1, const uint64_t O1, const CHashTableValueType& Value1,
	         const uint64_t P2, const uint64_t O2, const CHashTableValueType& Value2)
		: m_P1(0), m_O1(0), m_P2(0), m_O2(0), m_date1(0), m_date2(0), m_value1(Value1), m_value2(Value2) { spinlock.clear(); }
	CTwoNode& operator=(const CTwoNode& other);
	CTwoNode& operator=(CTwoNode&& other);

	void Update (const uint64_t P, const uint64_t O, const CHashTableValueType& Value, const uint8_t date);
	bool LookUp (const uint64_t P, const uint64_t O,       CHashTableValueType& Value) const; // The return value is stored in 'Value'
	void Refresh(const uint64_t P, const uint64_t O, const uint8_t date);
	void Clear();
};


class CHashTable
{
	inline uint64_t bucketSize(uint64_t Buckets) { while ((Buckets % 2 == 0) || (Buckets % 3 == 0) || (Buckets % 5 == 0)) Buckets--; return Buckets; }
public:
	typedef CTwoNode NodeType;
	typedef CHashTableValueType ValueType;
	CHashTable(const uint64_t Buckets) : buckets(bucketSize(Buckets)), date(0), UpdateCounter(0), LookUpCounter(0), RefreshCounter(0), HitCounter(0) { table = new NodeType[buckets]; }
	CHashTable() : CHashTable(7) {}
	~CHashTable() { delete[] table; }
	CHashTable& operator=(const CHashTable& other);
	CHashTable& operator=(CHashTable&& other);
	inline void Update (const uint64_t P, const uint64_t O, const ValueType& Value)       { UpdateCounter++; table[Hash(P, O)].Update(P, O, Value, date); }
	inline bool LookUp (const uint64_t P, const uint64_t O,       ValueType& Value) const { LookUpCounter++; bool b = table[Hash(P, O)].LookUp(P, O, Value); if (b) HitCounter++; return b; }
	inline void Refresh(const uint64_t P, const uint64_t O)                               { RefreshCounter++; table[Hash(P, O)].Refresh(P, O, date); }
	inline void AdvanceDate() { date++; }
	void Clear();
	void Save(const std::string & filename) const;
	void Load(const std::string & filename);
	void print_stats();

private:
	NodeType* table;
	uint64_t buckets;
	uint8_t date;
	mutable std::atomic<uint64_t> UpdateCounter, LookUpCounter, RefreshCounter, HitCounter;

	inline std::size_t Hash(uint64_t P, uint64_t O) const
	{
		P ^= P >> 36;
		O ^= O >> 21;
		return (P * O) % buckets;

		//P ^= P >> 33;
		//O ^= O >> 33;
		//P *= 0xFF14AFD7ED558CCDULL;
		//O *= 0xFF14AFD7ED558CCDULL;
		//O ^= O >> 33;
		//return (P + O + (O << 41)) % buckets;
	}
};

extern CHashTable TT;
