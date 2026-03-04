#pragma once

#include "bitboard.hpp"
#include <cstdio>

namespace bzz {

static void print_bitmask(bitmask_t bitmask) {

  printf("\x1B[90m");
  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 1) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 6) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 7;

  printf("\x1B[0m");
  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 6;

  printf("\x1B[90m");
  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 6) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitmask >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool b = (bitmask & (bitmask_t(1) << i));
    if (b) {
      std::putchar('1');
    } else {
      std::putchar('0');
    }
    if (i == 1) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }

  printf("\x1B[0m");
  std::putchar('\n');
}


}
