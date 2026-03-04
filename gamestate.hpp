#pragma once

#include "bitboard.hpp"
#include "bot.hpp"
#include "hash.hpp"
#include "repetition.hpp"
#include <map>
namespace bzz {

class GameState {
public:
  GameState() : m_start(BITBOARD_START_POSITION), m_turn(true) {}

  void turn(bot_move_t move) { m_moves.push_back(move); }

  bitboard_t bitboard() {
    bitboard_t bitboard = m_start;
    if (!m_turn) {
      std::swap(bitboard.white, bitboard.black);
    }
    for (bot_move_t move : m_moves) {
      bitboard.white ^= (1ull << move.from);
      bitboard.white ^= (1ull << move.to);
      std::swap(bitboard.white, bitboard.black);
    }
    return bitboard;
  }

  Repetition history() const {
    Repetition rr;
    bitboard_t it = m_start;
    if (!m_turn) {
      std::swap(it.white, it.black);
    }
    rr.push(hash::hash_pos(it));
    for (bot_move_t move : m_moves) {
      it.white ^= (bitmask_t(1) << move.from);
      it.white ^= (bitmask_t(1) << move.to);
      std::swap(it.white, it.black); 

      rr.push(hash::hash_pos(it));
    }
    return rr;
  }

private:
  bitboard_t m_start;
  bool m_turn; // <- white to move.
  std::vector<bot_move_t> m_moves;
};

} // namespace bzz
