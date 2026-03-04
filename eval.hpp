#pragma once

#include "bitboard.hpp"
#include "movegen.hpp"
#include <algorithm>
#include <bit>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>

namespace bzz::heuristic {

using eval_t = int32_t;

constexpr eval_t EVAL_WON = 10000;
constexpr eval_t EVAL_LOST = -EVAL_WON;
constexpr eval_t EVAL_DRAW = -500;
constexpr eval_t EVAL_DRAW_CONTEMT = -500; // draw is essentially as bad as losing.

constexpr eval_t EVAL_IMMOVABLE_PIECE = 100;

static __attribute__((always_inline)) inline eval_t eval(bitboard_t bitboard) {

  const bitmask_t occupant = bitboard.white | bitboard.black;

  const bitmask_t blockers1 = occupant | ~movegen::table::BITBOARD_FIELDS_MASK;
  const bitmask_t blockers0 =
      occupant | ~(movegen::table::BITBOARD_FIELDS_MASK | 1ull);

  eval_t score = 0;
  eval_t movability_delta = 0;
  { // white pieces.
    bitmask_t it = bitboard.white;
    while (it) {
      const bitmask_t b = it & -it;
      it ^= b;
      const pos_t p = std::countr_zero(b);
      const bitmask_t n = movegen::table::NEIGHBOR_MASK[p] & occupant;
      if (n == 0) {
        score -= EVAL_IMMOVABLE_PIECE;
        continue;
      }

      // const bitmask_t g = movegen::table::NEIGHBOR_MASK[p] & blockers1;
      // if (std::popcount(g) == 6) {
      //   score -= EVAL_IMMOVABLE_PIECE;
      //   continue;
      // }

      const bitmask_t nb = ~b;
      const int cnt = std::popcount(n);
      if (cnt == 6) {
        continue; // <- bad branch
      }
      const bitmask_t blockers1_nb = blockers1 & nb;
      const bitmask_t blockers0_nb = blockers0 & nb;

      bitmask_t ml = movegen::table::RAY_LEFT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(ml & blockers1_nb);
        ml &= ~movegen::table::RAY_LEFT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mtl = movegen::table::RAY_TOP_LEFT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(mtl & blockers1_nb);
        mtl &=
            ~movegen::table::RAY_TOP_LEFT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mtr = movegen::table::RAY_TOP_RIGHT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(mtr & blockers1_nb);
        mtr &= ~movegen::table::RAY_TOP_RIGHT[movegen::table::RAY_MAX_LENGTH]
                                             [cpos];
      }
      bitmask_t mr = movegen::table::RAY_RIGHT[cnt][p];
      {
        int cpos = std::countr_zero(mr & blockers0_nb);
        mr &= ~movegen::table::RAY_RIGHT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mbr = movegen::table::RAY_BOTTOM_RIGHT[cnt][p];
      {
        int cpos = std::countr_zero(mbr & blockers0_nb);
        mbr &= ~movegen::table::RAY_BOTTOM_RIGHT[movegen::table::RAY_MAX_LENGTH]
                                                [cpos];
      }
      bitmask_t mbl = movegen::table::RAY_BOTTOM_LEFT[cnt][p];
      {
        int cpos = std::countr_zero(mbl & blockers0_nb);
        mbl &= ~movegen::table::RAY_BOTTOM_LEFT[movegen::table::RAY_MAX_LENGTH]
                                               [cpos];
      }
      bitmask_t m = (ml | mtl | mtr | mr | mbr | mbl) & nb;
      if (m == 0) {
        score -= EVAL_IMMOVABLE_PIECE;
      }
      uint32_t move_count = std::popcount(m);
      movability_delta += move_count;
    }
  }
  { // black pieces
    bitmask_t it = bitboard.black;
    while (it) {
      const bitmask_t b = it & -it;
      it ^= b;
      const pos_t p = std::countr_zero(b);
      const bitmask_t n = movegen::table::NEIGHBOR_MASK[p] & occupant;
      if (n == 0) {
        score += EVAL_IMMOVABLE_PIECE;
        continue;
      }

      // const bitmask_t g = movegen::table::NEIGHBOR_MASK[p] & blockers1;
      // if (std::popcount(g) == 6) {
      //   score += EVAL_IMMOVABLE_PIECE;
      //   continue;
      // }

      const bitmask_t nb = ~b;

      const int cnt = std::popcount(n);
      if (cnt == 6) {
        continue; // <- bad branch
      }

      const bitmask_t blockers1_nb = blockers1 & nb;
      const bitmask_t blockers0_nb = blockers0 & nb;

      bitmask_t ml = movegen::table::RAY_LEFT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(ml & blockers1_nb);
        ml &= ~movegen::table::RAY_LEFT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mtl = movegen::table::RAY_TOP_LEFT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(mtl & blockers1_nb);
        mtl &=
            ~movegen::table::RAY_TOP_LEFT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mtr = movegen::table::RAY_TOP_RIGHT[cnt][p];
      {
        int cpos = 63 - std::countl_zero(mtr & blockers1_nb);
        mtr &= ~movegen::table::RAY_TOP_RIGHT[movegen::table::RAY_MAX_LENGTH]
                                             [cpos];
      }
      bitmask_t mr = movegen::table::RAY_RIGHT[cnt][p];
      {
        int cpos = std::countr_zero(mr & blockers0_nb);
        mr &= ~movegen::table::RAY_RIGHT[movegen::table::RAY_MAX_LENGTH][cpos];
      }
      bitmask_t mbr = movegen::table::RAY_BOTTOM_RIGHT[cnt][p];
      {
        int cpos = std::countr_zero(mbr & blockers0_nb);
        mbr &= ~movegen::table::RAY_BOTTOM_RIGHT[movegen::table::RAY_MAX_LENGTH]
                                                [cpos];
      }
      bitmask_t mbl = movegen::table::RAY_BOTTOM_LEFT[cnt][p];
      {
        int cpos = std::countr_zero(mbl & blockers0_nb);
        mbl &= ~movegen::table::RAY_BOTTOM_LEFT[movegen::table::RAY_MAX_LENGTH]
                                               [cpos];
      }
      bitmask_t m = (ml | mtl | mtr | mr | mbr | mbl) & nb;
      if (m == 0) {
        score += EVAL_IMMOVABLE_PIECE;
      }
      uint32_t move_count = std::popcount(m);
      movability_delta -= move_count;
    }
  }
  // if (std::abs(movability_delta / 2) > 10) { 
  //   std::cout << movability_delta << std::endl;
  //   std::terminate();
  // }
  // assert(std::abs(movability_delta / 100000) < 10);
  score += movability_delta / 2;

  return score;
}

} // namespace bzz::heuristic
