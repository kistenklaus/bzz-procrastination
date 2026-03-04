
#include "bitboard.hpp"
#include "bot.hpp"
#include "diag.hpp"
#include "eval.hpp"
#include "gamestate.hpp"
#include "hash.hpp"
#include "minmax.hpp"
#include "movegen.hpp"
#include "perf.hpp"
#include "transposition_table.hpp"
#include <chrono>
#include <iostream>
#include <map>

int main() {
  using namespace bzz;

  // bzz::minmax_perfT();

  GameState gamestate;

  uint32_t turn = 0;
  TranspositionTable tt(1 << 22);
  std::map<uint64_t, uint32_t> rep;
  while (true) {

    Repetition rr = gamestate.history();
    rr.reserve(2048);

    bot_move_t move;
    auto s = std::chrono::high_resolution_clock::now();
    if (turn % 2 == 0) {
      move = bzz::best_move(gamestate.bitboard(), 8, &tt, &rr,
                            heuristic::EVAL_DRAW_CONTEMT);
    } else {
      move = bzz::best_move(gamestate.bitboard(), 8, &tt, &rr, heuristic::EVAL_DRAW_CONTEMT);
    }

    auto e = std::chrono::high_resolution_clock::now();
    auto d =
        std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(e -
                                                                             s);
    if (turn % 2 == 0) {
      printf("whites-turn: %d    [took: %fms]\n", turn, d.count());
      print_bitboard(gamestate.bitboard());
    } else {
      printf("blacks-turn: %d    [took: %fms]\n", turn, d.count());
      bitboard_t board = gamestate.bitboard();
      std::swap(board.white, board.black);
      print_bitboard(board);
    }

    if (move.from == 0 && move.to == 0) {
      if (turn % 2 == 0) {
        std::cout << "black won" << std::endl;
      } else {
        std::cout << "white won" << std::endl;
      }
      break;
    }
    if (move.from == 0 && move.to == 1) {
      std::cout << "draw" << std::endl;
      break;
    }

    gamestate.turn(move);

    ++turn;
  }

  bzz::minmax_perfT();
}
