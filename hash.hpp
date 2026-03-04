#pragma once

#include "bitboard.hpp"
#include <cstdint>
namespace bzz::hash {

static inline bitmask_t mix64(uint64_t x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

static inline bitmask_t mix_pos(bitmask_t white, bitmask_t black) {
  return mix64(white) ^ mix64(black + 0x9e3779b97f4a7c15ULL);
}

static inline bitmask_t fmix64(bitmask_t k) {
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccdULL;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53ULL;
  k ^= k >> 33;
  return k;
}

static inline bitmask_t fmix_pos(bitmask_t white, bitmask_t black) {
  bitmask_t h = white ^ (black * 0x9e3779b97f4a7c15ULL);
  return fmix64(h);
}

static __attribute__((always_inline)) inline bitmask_t
hash_pos(bitmask_t white, bitmask_t black) {
  return fmix_pos(white, black);
}

static __attribute__((always_inline)) inline bitmask_t
hash_pos(bitboard_t bitboard) {
  return hash_pos(bitboard.white, bitboard.black);
}

} // namespace bzz::hash
