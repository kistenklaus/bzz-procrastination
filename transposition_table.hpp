#pragma once

#include "bitboard.hpp"
#include "eval.hpp"
#include <cstdint>
#include <vector>
namespace bzz {

class TranspositionTable {
public:
  TranspositionTable(size_t size)
      : m_mask((std::bit_ceil(size)) - 1), m_entries(std::bit_ceil(size)) {
    assert((m_entries.size() & (m_entries.size() - 1)) == 0);
  }

  enum class Flag {
    EXACT,
    LOWER,
    UPPER,
  };
  struct Entry {
    uint64_t key = 0;
    move_t best_move{};
    heuristic::eval_t score{};
    uint32_t depth{};
    Flag flag = Flag::EXACT;
  };

  Entry *probe(uint64_t key) {
    Entry &e = m_entries[key & m_mask];
    if (e.key == key) {
      return &e;
    }
    return nullptr;
  }

  void store(uint64_t key, move_t best_move, heuristic::eval_t score,
             uint32_t depth, Flag flag) {
    Entry &e = m_entries[key & m_mask];
    if (e.key == 0 || depth >= e.depth) {
      e.key = key;
      e.best_move = best_move;
      e.score = score;
      e.depth = depth;
      e.flag = flag;
    }
  }

  void clear() {
    for (auto &e : m_entries) {
      e.key = 0;
    }
  }

private:
  size_t m_mask;
  std::vector<Entry> m_entries;
};

} // namespace bzz
