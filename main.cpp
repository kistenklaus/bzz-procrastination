
#include "bitboard.hpp"
#include "movegen.hpp"
#include "bot.hpp"
#include "minmax.hpp"
#include "perf.hpp"
#include <chrono>
#include <iostream>

int main() {
  using namespace bzz;

  bitboard_t test{
      .white = (1 << 8),
      .black = (1 << 30),
  };

  bzz::minmax_perfT();
  // bzz::perfT();
  //
  // bot_move_t move = bzz::best_move(BITBOARD_START_POSITION, 20);
  // std::cout << "best-move: " << move.from << " -> " << move.to << std::endl;
}
