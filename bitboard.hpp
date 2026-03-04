#pragma once

#include <cstdint>

namespace bzz {

using bitmask_t = uint64_t;
using pos_t = uint32_t;

struct bitboard_t {
  bitmask_t white;
  bitmask_t black;
};

static constexpr bitboard_t BITBOARD_START_POSITION{
    .white = (1ull << 8) | (1ull << 9) | (1ull << 10) | (1ull << 11),
    .black = (1ull << 47) | (1ull << 48) | (1ull << 49) | (1ull << 50),
};

using move_t = bitmask_t;

} // namespace bzz
