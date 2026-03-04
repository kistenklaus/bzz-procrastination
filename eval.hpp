#pragma once

#include "bitboard.hpp"
#include "movegen.hpp"
#include <cstdint>
#include <limits>

namespace bzz::heuristic {

using eval_t = int32_t;

constexpr eval_t EVAL_WON = 1000;
constexpr eval_t EVAL_LOST = -1000;

static volatile eval_t acc = 0;

static __attribute__((always_inline)) inline eval_t eval(bitboard_t bitboard) {
  // TODO: More interessting heuristic.
  eval_t white_moves = static_cast<eval_t>(movegen::count_moves(bitboard));
  bitboard_t swapped{.white = bitboard.black, .black = bitboard.white};
  eval_t black_moves = static_cast<eval_t>(movegen::count_moves(swapped));
  if (white_moves == 0) {
    return EVAL_LOST;
  }
  // return 0;
  return white_moves - black_moves;
}

} // namespace bzz::heuristic
