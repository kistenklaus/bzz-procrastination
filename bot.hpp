#pragma once

#include "bitboard.hpp"
#include "diag.hpp"
#include "eval.hpp"
#include "minmax.hpp"
#include <bit>
#include <cassert>
namespace bzz {

struct bot_move_t {
  uint32_t from;
  uint32_t to;
};

static bot_move_t best_move(bitboard_t bitboard, uint32_t max_depth) {

  TranspositionTable tt{1ull << 19};

  move_t moves[MAX_MOVES];
  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);

  bitmask_t final_best_move = 0;
  heuristic::eval_t final_best_score = heuristic::EVAL_LOST;

  for (uint32_t d = 1; d <= max_depth; ++d) {

    heuristic::eval_t alpha = heuristic::EVAL_LOST;
    heuristic::eval_t beta = heuristic::EVAL_WON;

    bitmask_t best_move = 0;

    // optional: reorder root moves via TT
    uint64_t root_key = hash::hash_pos(bitboard);
    if (auto *e = tt.probe(root_key)) {
      for (uint32_t i = 0; i < move_count; ++i) {
        if (moves[i] == e->best_move) {
          std::swap(moves[0], moves[i]);
          break;
        }
      }
    }

    for (uint32_t m = 0; m < move_count; ++m) {

      bitboard_t next{
          .white = bitboard.black,
          .black = moves[m],
      };

      heuristic::eval_t score = -negascout(next, d - 1, &tt, -beta, -alpha);

      if (score > alpha) {
        alpha = score;
        best_move = moves[m];
      }
    }

    final_best_move = best_move;
    final_best_score = alpha;

    // printf("depth %u score = %d\n", d, alpha);
  }

  printf("best-score: %d\n", final_best_score);
  assert(final_best_move != 0);

  bitmask_t delta = bitboard.white ^ final_best_move;
  bitmask_t from_mask = delta & bitboard.white;
  bitmask_t dst_mask = delta & final_best_move;

  return {.from = static_cast<uint32_t>(std::countr_zero(from_mask)),
          .to = static_cast<uint32_t>(std::countr_zero(dst_mask))};
}

} // namespace bzz
