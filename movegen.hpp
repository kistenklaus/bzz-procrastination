#pragma once
#include "bitboard.hpp"
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdio>

namespace bzz::movegen {

namespace table {

constexpr uint32_t RAY_LOOKUP_SIZE = 65;
constexpr uint32_t RAY_MAX_LENGTH = 5;
constexpr uint32_t RAY_LOOKUP_DEPTH = RAY_MAX_LENGTH + 1;
constexpr bitmask_t RAY_LEFT_BORDER = 0x10408204102081ull;
constexpr bitmask_t RAY_TOP_LEFT_BORDER = 0x1000800400207Full;
constexpr bitmask_t RAY_TOP_RIGHT_BORDER = 0x40020010008007Full;
constexpr bitmask_t RAY_RIGHT_BORDER = 0x408204102081040ull;
constexpr bitmask_t RAY_BOTTOM_RIGHT_BORDER = 0x7F0200100080040ull;
constexpr bitmask_t RAY_BOTTOM_LEFT_BORDER = 0x7F0008004002001ull;

constexpr bitmask_t BITBOARD_TOP_LEFT_BORDER = 0x410411Cull;
constexpr bitmask_t BITBOARD_TOP_RIGHT_BORDER = 0x10204081Cull;
constexpr bitmask_t BITBOARD_RIGHT_BORDER = 0x104104102040810ull;
constexpr bitmask_t BITBOARD_BOTTOM_RIGHT_BORDER = 0x1C4104100000000ull;
constexpr bitmask_t BITBOARD_BOTTOM_LEFT_BORDER = 0x1C0810204000000ull;
constexpr bitmask_t BITBOARD_LEFT_BORDER = 0x40810204104104ull;

// blocker mask
// constexpr bitmask_t BITBOARD_OUTSIDE_MASK = 0x63860A4012830E3ull; // 21,24,34,37
// constexpr bitmask_t BITBOARD_OUTSIDE_MASK = 0x6386080200830E3ull; // 29
constexpr bitmask_t BITBOARD_OUTSIDE_MASK = 0x6386080000830E3ull;
constexpr bitmask_t BITBOARD_SPECIAL_MASK = 0;
constexpr bitmask_t BITBOARD_PADDING_MASK = 0xF800000000000000ull;
constexpr bitmask_t BITBOARD_FIELDS_MASK =
    ~(BITBOARD_OUTSIDE_MASK | BITBOARD_PADDING_MASK);

consteval std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
build_ray_lookup(bitmask_t border, int32_t step) {
  std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH> lookup{};
  for (pos_t pos = 0; pos < RAY_LOOKUP_SIZE - 1; ++pos) {
    lookup[0][pos] = (bitmask_t(1u) << pos);
  }
  for (uint32_t len = 1; len < RAY_LOOKUP_DEPTH; ++len) {
    for (pos_t pos = 0; pos < RAY_LOOKUP_SIZE - 1; ++pos) {
      bitmask_t prev = lookup[len - 1][pos];
      if (prev & border) {
        lookup[len][pos] = prev;
      } else {
        if (step < 0) {
          lookup[len][pos] = (prev >> -step) | prev;
        } else {
          lookup[len][pos] = (prev << step) | prev;
        }
      }
    }
  }

  for (uint32_t len = 0; len < RAY_LOOKUP_DEPTH; ++len) {
    for (pos_t pos = 0; pos < RAY_LOOKUP_SIZE; ++pos) {
      lookup[len][pos] |= 1ull;
    }
  }

  return lookup;
}

consteval std::array<bitmask_t, RAY_LOOKUP_SIZE> build_neighbor_mask() {
  std::array<bitmask_t, RAY_LOOKUP_SIZE> lookup{};
  for (uint64_t pos = 0; pos < RAY_LOOKUP_SIZE - 1; ++pos) {

    // -1, -7, -6, +1, 7, 6
    bitmask_t tile = bitmask_t(1u) << pos;
    bitmask_t neighbor_mask = 0;

    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_LEFT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos - 1);
    }
    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_TOP_LEFT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos - 7);
    }
    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_TOP_RIGHT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos - 6);
    }
    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_RIGHT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos + 1);
    }
    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_BOTTOM_RIGHT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos + 7);
    }

    if (tile & BITBOARD_FIELDS_MASK & ~BITBOARD_BOTTOM_LEFT_BORDER) {
      neighbor_mask |= bitmask_t(1u) << (pos + 6);
    }

    lookup[pos] = neighbor_mask | 1ull;
  }
  return lookup;
}

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_LEFT = build_ray_lookup(RAY_LEFT_BORDER, -1);

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_TOP_LEFT = build_ray_lookup(RAY_TOP_LEFT_BORDER, -7);

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_TOP_RIGHT = build_ray_lookup(RAY_TOP_RIGHT_BORDER, -6);

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_RIGHT = build_ray_lookup(RAY_RIGHT_BORDER, +1);

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_BOTTOM_RIGHT = build_ray_lookup(RAY_BOTTOM_RIGHT_BORDER, +7);

constexpr std::array<std::array<bitmask_t, RAY_LOOKUP_SIZE>, RAY_LOOKUP_DEPTH>
    RAY_BOTTOM_LEFT = build_ray_lookup(RAY_BOTTOM_LEFT_BORDER, +6);

static constexpr std::array<bitmask_t, RAY_LOOKUP_SIZE> NEIGHBOR_MASK =
    build_neighbor_mask();

// should fit int L1 (i.e. around 4KB in total)
static constexpr size_t SIZE =
    NEIGHBOR_MASK.size() * sizeof(bitmask_t) +
    RAY_LOOKUP_SIZE * RAY_LOOKUP_DEPTH * sizeof(bitmask_t);

} // namespace table

static uint64_t count_moves(bitboard_t bitboard) {
  uint64_t count = 0;
  const bitmask_t occupant = bitboard.white | bitboard.black | table::BITBOARD_SPECIAL_MASK;
  const bitmask_t blockers1 = occupant | ~table::BITBOARD_FIELDS_MASK;
  const bitmask_t blockers0 = occupant | ~(table::BITBOARD_FIELDS_MASK | 1ull);

  bitmask_t pieces = bitboard.white;

  while (pieces) {
    const bitmask_t b = pieces & -pieces;
    pieces ^= b;
    const pos_t p = std::countr_zero(b);
    const bitmask_t n = table::NEIGHBOR_MASK[p] & occupant;
    const bitmask_t nb = ~b;
    const int cnt = std::popcount(n);

    [[unlikely]]
    if (cnt == 6) {
      continue; // <- bad branch
    }

    const bitmask_t blockers1_nb = blockers1 & nb;
    const bitmask_t blockers0_nb = blockers0 & nb;

    bitmask_t ml = table::RAY_LEFT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(ml & blockers1_nb);
      ml &= ~table::RAY_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mtl = table::RAY_TOP_LEFT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(mtl & blockers1_nb);
      mtl &= ~table::RAY_TOP_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mtr = table::RAY_TOP_RIGHT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(mtr & blockers1_nb);
      mtr &= ~table::RAY_TOP_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mr = table::RAY_RIGHT[cnt][p];
    {
      int cpos = std::countr_zero(mr & blockers0_nb);
      mr &= ~table::RAY_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mbr = table::RAY_BOTTOM_RIGHT[cnt][p];
    {
      int cpos = std::countr_zero(mbr & blockers0_nb);
      mbr &= ~table::RAY_BOTTOM_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mbl = table::RAY_BOTTOM_LEFT[cnt][p];
    {
      int cpos = std::countr_zero(mbl & blockers0_nb);
      mbl &= ~table::RAY_BOTTOM_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t m = (ml | mtl | mtr | mr | mbr | mbl) & nb;
    count += std::popcount(m);
  }
  return count;
}

static uint32_t enumerate_moves(bitboard_t bitboard, move_t *const out_moves) {
  move_t *it = out_moves;
  const bitmask_t occupant = bitboard.white | bitboard.black | table::BITBOARD_SPECIAL_MASK;
  const bitmask_t blockers1 = occupant | ~table::BITBOARD_FIELDS_MASK;
  const bitmask_t blockers0 = occupant | ~(table::BITBOARD_FIELDS_MASK | 1ull);

  bitmask_t pieces = bitboard.white;

  // TODO: Consider manual unrolling
  // iterate over pieces (bits) 
  while (pieces) {
    // b is a bitmask where only the bit of the piece is set.
    const bitmask_t b = pieces & -pieces;
    pieces ^= b;
    const pos_t p = std::countr_zero(b); // <- position of the piece.
    const bitmask_t n = table::NEIGHBOR_MASK[p] & occupant;
    const bitmask_t nb = ~b;
    // amount of neighbors.
    const int cnt = std::popcount(n);
    [[unlikely]]
    if (cnt == 6) {
      continue; // <- bad branch
    }

    const bitmask_t blockers1_nb = blockers1 & nb;
    const bitmask_t blockers0_nb = blockers0 & nb;

    bitmask_t ml = table::RAY_LEFT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(ml & blockers1_nb);
      ml &= ~table::RAY_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mtl = table::RAY_TOP_LEFT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(mtl & blockers1_nb);
      mtl &= ~table::RAY_TOP_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mtr = table::RAY_TOP_RIGHT[cnt][p];
    {
      int cpos = 63 - std::countl_zero(mtr & blockers1_nb);
      mtr &= ~table::RAY_TOP_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mr = table::RAY_RIGHT[cnt][p];
    {
      int cpos = std::countr_zero(mr & blockers0_nb);
      mr &= ~table::RAY_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mbr = table::RAY_BOTTOM_RIGHT[cnt][p];
    {
      int cpos = std::countr_zero(mbr & blockers0_nb);
      mbr &= ~table::RAY_BOTTOM_RIGHT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t mbl = table::RAY_BOTTOM_LEFT[cnt][p];
    {
      int cpos = std::countr_zero(mbl & blockers0_nb);
      mbl &= ~table::RAY_BOTTOM_LEFT[table::RAY_MAX_LENGTH][cpos];
    }
    bitmask_t m = (ml | mtl | mtr | mr | mbr | mbl) & nb;

    // iterate over set bits.
    while (m) {
      bitmask_t t = m & -m;
      *it++ = bitboard.white ^ (b | t);
      m ^= t;
    }
  }
  return it - out_moves;
}

} // namespace bzz::movegen
