#pragma once

#include "bitboard.hpp"
#include "diag.hpp"
#include "eval.hpp"
#include "hash.hpp"
#include "minmax.hpp"
#include "repetition.hpp"
#include <bit>
#include <cassert>
namespace bzz {

struct bot_move_t {
  uint32_t from;
  uint32_t to;
};

static bot_move_t best_move(bitboard_t bitboard, uint32_t max_depth,
                            TranspositionTable *tt, Repetition *rr,
                            int32_t draw_contemt) {
  move_t moves[MAX_MOVES];
  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);
  if (move_count == 0)
    return {.from = 0, .to = 0};

  bitmask_t final_best_move = moves[0];
  heuristic::eval_t final_best_score = heuristic::EVAL_LOST;

  const uint64_t root_key = hash::hash_pos(bitboard);

  for (uint32_t d = std::min(6u, max_depth); d <= max_depth; ++d) {
    heuristic::eval_t alpha = heuristic::EVAL_LOST;
    heuristic::eval_t beta = heuristic::EVAL_WON;

    // TT root ordering
    if (auto *e = tt->probe(root_key)) {
      const bitmask_t tt_move = e->best_move;
      if (tt_move) {
        for (uint32_t i = 0; i < move_count; ++i) {
          if (moves[i] == tt_move) {
            std::swap(moves[0], moves[i]);
            break;
          }
        }
      }
    }

    bitmask_t best_move = moves[0];
    heuristic::eval_t best_score = heuristic::EVAL_LOST;

    // First move full-window
    {
      bitboard_t next{.white = bitboard.black, .black = moves[0]};
      const uint64_t child_key = hash::hash_pos(next);
      const bool is_rep = rr->would_draw(child_key);

      heuristic::eval_t score =
          is_rep ? (draw_contemt)
                 : -negascout(next, d - 1, tt, rr, -beta, -alpha);

      best_score = score;
      best_move = moves[0];
      alpha = std::max(alpha, score);
    }

    // Remaining moves with PVS
    for (uint32_t m = 1; m < move_count; ++m) {
      bitboard_t next{.white = bitboard.black, .black = moves[m]};
      const uint64_t child_key = hash::hash_pos(next);
      const bool is_rep = rr->would_draw(child_key);

      heuristic::eval_t score;
      if (is_rep) {
        score = draw_contemt;
      } else {
        score = -negascout(next, d - 1, tt, rr, -(alpha + 1), -alpha);

        if (score > alpha && score < beta) {
          score = -negascout(next, d - 1, tt, rr, -beta, -alpha);
        }
      }

      if (score > best_score) {
        best_score = score;
        best_move = moves[m];
      }

      alpha = std::max(alpha, score);
      if (alpha >= beta)
        break;
    }

    final_best_move = best_move;
    final_best_score = best_score;
  }

  bitboard_t final_next{.white = bitboard.black, .black = final_best_move};

  if (rr->would_draw(hash::hash_pos(final_next))) {
    return {.from = 0, .to = 1};
  }

  bitmask_t delta = bitboard.white ^ final_best_move;
  bitmask_t from_mask = delta & bitboard.white;
  bitmask_t dst_mask = delta & final_best_move;

  return {
      .from = static_cast<uint32_t>(std::countr_zero(from_mask)),
      .to = static_cast<uint32_t>(std::countr_zero(dst_mask)),
  };
}

} // namespace bzz
