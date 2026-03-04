#pragma once

#include "bitboard.hpp"
#include "eval.hpp"
#include "hash.hpp"
#include "movegen.hpp"
#include "repetition.hpp"
#include "transposition_table.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace bzz {

static constexpr size_t MAX_MOVES = 100;

static heuristic::eval_t
naive_negamax(bitboard_t bitboard, uint32_t depth,
              heuristic::eval_t alpha = heuristic::EVAL_LOST,
              heuristic::eval_t beta = heuristic::EVAL_WON) {
  if (depth == 0) {
    return heuristic::eval(bitboard);
  }
  move_t moves[MAX_MOVES];
  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);

  bitmask_t black = bitboard.black;
  for (uint32_t m = 0; m < move_count; ++m) {
    // this is all that we need to make (apply) a move.
    bitboard_t next{
        .white = black,
        .black = moves[m],
    };

    heuristic::eval_t score = -naive_negamax(next, depth - 1, -beta, -alpha);
    if (score >= beta) {
      return score;
    }
    alpha = std::max(alpha, score);
  }
  return alpha;
}

static heuristic::eval_t negamax(bitboard_t bitboard, uint32_t depth,
                                 TranspositionTable *tt,
                                 heuristic::eval_t alpha,
                                 heuristic::eval_t beta) {
  heuristic::eval_t alpha0 = alpha;
  heuristic::eval_t beta0 = beta;

  uint64_t key = hash::hash_pos(bitboard);

  auto *tte = tt->probe(key);
  if (tte && tte->depth >= depth) {
    const heuristic::eval_t s = tte->score;
    switch (tte->flag) {
    case TranspositionTable::Flag::EXACT:
      return s;
    case TranspositionTable::Flag::LOWER:
      alpha = std::max(alpha, s);
      break;
    case TranspositionTable::Flag::UPPER:
      beta = std::min(beta, s);
      break;
    }
    if (alpha >= beta) {
      return s;
    }
  }

  if (depth == 0) {
    return heuristic::eval(bitboard);
  }
  move_t moves[MAX_MOVES];
  assert(movegen::count_moves(bitboard) < MAX_MOVES);
  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);


  if (move_count == 0) {
    return heuristic::EVAL_LOST;
  }

  heuristic::eval_t best = alpha;
  bitmask_t best_move = 0;
  bitmask_t tte_move = 0;
  if (tte) {
    tte_move = tte->best_move;
  }

  for (uint32_t i = 0; i < move_count; ++i) {
    if (moves[i] == tte_move) {
      std::swap(moves[0], moves[i]);
      break;
    }
  }

  for (uint32_t m = 0; m < move_count; ++m) {
    // this is all that we need to make a move.
    bitboard_t next{
        .white = bitboard.black,
        .black = moves[m],
    };

    heuristic::eval_t score = -negamax(next, depth - 1, tt, -beta, -alpha);

    if (score > best) {
      best = score;
      best_move = moves[m];
    }
    alpha = std::max(alpha, score);
    if (alpha >= beta) {
      break;
    }
  }

  TranspositionTable::Flag flag;
  if (best <= alpha0) {
    flag = TranspositionTable::Flag::UPPER;
  } else if (best >= beta0) {
    flag = TranspositionTable::Flag::LOWER;
  } else {
    flag = TranspositionTable::Flag::EXACT;
  }
  tt->store(key, best_move, best, depth, flag);
  return best;
}

static heuristic::eval_t negascout(bitboard_t bitboard, uint32_t depth,
                                   TranspositionTable *tt,
                                   Repetition* rr,
                                   heuristic::eval_t alpha,
                                   heuristic::eval_t beta) {
  heuristic::eval_t alpha0 = alpha;
  heuristic::eval_t beta0 = beta;

  uint64_t key = hash::hash_pos(bitboard);

  if (rr->would_draw(key)) {
    return heuristic::EVAL_DRAW;
  }

  auto *tte = tt->probe(key);
  if (tte && tte->depth >= depth) {
    const heuristic::eval_t s = tte->score;
    switch (tte->flag) {
    case TranspositionTable::Flag::EXACT:
      return s;
    case TranspositionTable::Flag::LOWER:
      alpha = std::max(alpha, s);
      break;
    case TranspositionTable::Flag::UPPER:
      beta = std::min(beta, s);
      break;
    }
    if (alpha >= beta) {
      return s;
    }
  }


  if (depth == 0) {
    return heuristic::eval(bitboard);
  }
  move_t moves[MAX_MOVES];
  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);

  if (move_count == 0) {
    return heuristic::EVAL_LOST;
  }


  heuristic::eval_t best = heuristic::EVAL_LOST;
  bitmask_t best_move = moves[0];
  bitmask_t killer_move = 0;

  if (tte && tte->best_move) {
    killer_move = tte->best_move;
  } else {
    const bool pv_node = (beta - alpha) > 1;
    if (pv_node && depth >= 6) {
      negascout(bitboard, depth - 2, tt,rr, alpha, beta);
      if (auto *e2 = tt->probe(key)) {
        killer_move = e2->best_move;
      }
    }
  }
  rr->push(key);

  if (killer_move) {
    // we have a good candidate for a the best move.
    
#ifndef NDEBUG
    if (killer_move) {
      bool ok = false;
      for (uint32_t i = 0; i < move_count; ++i)
        ok |= (moves[i] == killer_move);
      assert(ok);
    }
#endif

    bitboard_t next{
        .white = bitboard.black,
        .black = killer_move,
    };

    // full window on tte move.
    heuristic::eval_t score = -negascout(next, depth - 1, tt,rr, -beta, -alpha);
    best = score;
    best_move = killer_move;
    alpha = std::max(alpha, score);

    if (alpha < beta) {
      for (uint32_t m = 0; m < move_count; ++m) {
        if (moves[m] == killer_move) {
          continue;
        }
        bitboard_t next{
            .white = bitboard.black,
            .black = moves[m],
        };
        // null window search
        heuristic::eval_t score =
            -negascout(next, depth - 1, tt, rr,-(alpha + 1), -alpha);

        if (score > alpha && score < beta) {
          // full window search
          score = -negascout(next, depth - 1, tt, rr, -beta, -alpha);
        }

        if (score > best) {
          best = score;
          best_move = moves[m];
        }
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
          break;
        }
      }
    }
  } else {
    // normal search
    for (uint32_t m = 0; m < move_count; ++m) {
      bitboard_t next{
          .white = bitboard.black,
          .black = moves[m],
      };
      heuristic::eval_t score = -negascout(next, depth - 1, tt, rr, -beta, -alpha);
      if (score > best) {
        best = score;
        best_move = moves[m];
      }
      alpha = std::max(alpha, score);
      if (alpha >= beta) {
        break;
      }
    }
  }
  TranspositionTable::Flag flag;
  if (best <= alpha0) {
    flag = TranspositionTable::Flag::UPPER;
  } else if (best >= beta0) {
    flag = TranspositionTable::Flag::LOWER;
  } else {
    flag = TranspositionTable::Flag::EXACT;
  }
  tt->store(key, best_move, best, depth, flag);

  rr->pop(key);
  return best;
}

} // namespace bzz
