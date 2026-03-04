#pragma once

#include "bitboard.hpp"
#include "bot.hpp"
#include "minmax.hpp"
#include "repetition.hpp"
#include "transposition_table.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>

namespace bzz {

static uint64_t perfT_run(bitboard_t bitboard, uint32_t depth) {
  if (depth == 0) {
    return 1;
  }
  uint64_t nodes = 0;
  move_t moves[MAX_MOVES];

  uint32_t move_count = movegen::enumerate_moves(bitboard, moves);

  bitmask_t black = bitboard.black;
  for (uint32_t m = 0; m < move_count; ++m) {
    bitboard_t next{
        .white = black,
        .black = moves[m],
    };
    nodes += perfT_run(next, depth - 1);
  }
  return nodes;
}

static void perfT() {

  for (uint32_t d = 1; d < 20; ++d) {
    auto s = std::chrono::high_resolution_clock::now();
    uint64_t count = bzz::perfT_run(BITBOARD_START_POSITION, d);
    auto e = std::chrono::high_resolution_clock::now();
    auto latency =
        std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(e -
                                                                             s);
    std::cout << "perf-" << d << "  " << count
              << " positions     took:" << latency << std::endl;
  }
}

static void minmax_perfT() {

  TranspositionTable tt(1 << 22);
  for (uint32_t d = 1; d < 30; ++d) {
    Repetition rr;
    auto s = std::chrono::high_resolution_clock::now();
    bzz::best_move(BITBOARD_START_POSITION, d, &tt, &rr, 0);
    auto e = std::chrono::high_resolution_clock::now();
    auto latency =
        std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(e -
                                                                             s);
    std::cout << "perf-" << d << "  took:" << latency << std::endl;
  }
}

} // namespace bzz
