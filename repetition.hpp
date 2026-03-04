#pragma once
#include "hash.hpp"
#include <bit>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cassert>
#include <limits>

namespace bzz {

// Open addressing hash table: key -> small count
class OACountTable {
public:
  using key_t = uint64_t;
  using count_t = uint16_t;

  OACountTable() = default;

  void reserve(size_t expected_items) {
    // keep load <= ~0.5 for fast linear probing
    size_t cap = std::bit_ceil<std::size_t>(expected_items * 2 + 16);
    if (cap <= m_cap) return;
    rehash_to(cap);
  }

  count_t get(key_t k) const noexcept {
    if (m_cap == 0) return 0;
    size_t idx = index_for(k);

    for (;;) {
      const uint8_t st = m_state[idx];
      if (st == EMPTY) return 0;
      if (st == FULL && m_keys[idx] == k) return m_vals[idx];
      idx = (idx + 1) & m_mask;
    }
  }

  void inc(key_t k) {
    ensure_capacity_for_insert();

    size_t idx = index_for(k);
    size_t first_tomb = npos;

    for (;;) {
      const uint8_t st = m_state[idx];

      if (st == EMPTY) {
        const size_t dst = (first_tomb != npos) ? first_tomb : idx;
        if (first_tomb != npos) --m_tomb;

        m_state[dst] = FULL;
        m_keys[dst]  = k;
        m_vals[dst]  = 1;
        ++m_size;
        return;
      }

      if (st == TOMB) {
        if (first_tomb == npos) first_tomb = idx;
      } else { // FULL
        if (m_keys[idx] == k) {
          if (m_vals[idx] != std::numeric_limits<count_t>::max())
            ++m_vals[idx];
          return;
        }
      }

      idx = (idx + 1) & m_mask;
    }
  }

  void dec(key_t k) {
    assert(m_cap != 0);

    size_t idx = index_for(k);
    for (;;) {
      const uint8_t st = m_state[idx];
      if (st == EMPTY) {
        assert(false && "OACountTable::dec on missing key");
        return;
      }
      if (st == FULL && m_keys[idx] == k) {
        assert(m_vals[idx] > 0);
        if (--m_vals[idx] == 0) {
          m_state[idx] = TOMB;
          ++m_tomb;
          --m_size;

          // too many tombstones => rehash in place
          if (m_tomb * 4 > m_cap) {
            rehash_to(m_cap);
          }
        }
        return;
      }
      idx = (idx + 1) & m_mask;
    }
  }

private:
  static constexpr uint8_t EMPTY = 0;
  static constexpr uint8_t FULL  = 1;
  static constexpr uint8_t TOMB  = 2;
  static constexpr size_t  npos  = static_cast<size_t>(-1);

  size_t m_cap  = 0;
  size_t m_mask = 0;
  size_t m_size = 0; // FULL slots
  size_t m_tomb = 0; // TOMB slots

  std::vector<key_t>   m_keys;
  std::vector<count_t> m_vals;
  std::vector<uint8_t> m_state;

  size_t __attribute__((always_inline)) inline index_for(key_t k) const noexcept {
    return static_cast<size_t>(hash::mix64(k)) & m_mask;
  }

  void ensure_capacity_for_insert() {
    if (m_cap == 0) {
      rehash_to(16);
      return;
    }

    // linear probing degrades badly with high occupancy (FULL+TOMB)
    const size_t used = m_size + m_tomb;
    if (used * 10 >= m_cap * 7) { // used/cap >= 0.7
      rehash_to(m_cap * 2);
    }
  }

  void rehash_to(size_t new_cap) {
    new_cap = std::bit_ceil<std::size_t>(new_cap);
    if (new_cap < 16) new_cap = 16;

    std::vector<key_t>   old_keys;
    std::vector<count_t> old_vals;
    std::vector<uint8_t> old_state;
    old_keys.swap(m_keys);
    old_vals.swap(m_vals);
    old_state.swap(m_state);
    const size_t old_cap = m_cap;

    m_cap  = new_cap;
    m_mask = new_cap - 1;
    m_size = 0;
    m_tomb = 0;

    m_keys.assign(m_cap, 0);
    m_vals.assign(m_cap, 0);
    m_state.assign(m_cap, EMPTY);

    if (old_cap == 0) return;

    for (size_t i = 0; i < old_cap; ++i) {
      if (old_state[i] != FULL) continue;
      const key_t k = old_keys[i];
      const count_t v = old_vals[i];

      size_t idx = index_for(k);
      while (m_state[idx] == FULL) idx = (idx + 1) & m_mask;

      m_state[idx] = FULL;
      m_keys[idx]  = k;
      m_vals[idx]  = v;
      ++m_size;
    }
  }
};

// Repetition with O(1) seen/would_draw and push/pop.
struct Repetition {
  using key_t = uint64_t;

  void reserve(size_t plies) {
    m_counts.reserve(plies);
  }

  bool seen(key_t k) const noexcept {
    return m_counts.get(k) >= 1;
  }

  // true iff key already appeared >=2 times; visiting again would be 3-fold
  bool would_draw(key_t k) const noexcept {
    return m_counts.get(k) >= 2;
  }

  void push(key_t k) {
    m_counts.inc(k);
  }

  void pop(key_t k) {
    m_counts.dec(k);
  }

private:
  std::vector<key_t> m_stack;
  OACountTable m_counts;
};

} // namespace bzz
