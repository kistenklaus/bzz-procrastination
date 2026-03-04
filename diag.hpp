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

static void print_bitboard(bitboard_t bitboard) {

  static constexpr char EMPTY = ' ';
  static constexpr char WHITE = 'w';
  static constexpr char BLACK = 'b';

  printf("\x1B[90m");
  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 1) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 7;
  bitboard.black >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 6;
  bitboard.black >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 6) {
      printf("\x1B[90m");
    }

    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 7;
  bitboard.black >>= 7;

  printf("\x1B[0m");
  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {

    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }

    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 6;
  bitboard.black >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 7;
  bitboard.black >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 6;
  bitboard.black >>= 6;

  printf("\x1B[90m");
  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 6) {
      printf("\x1B[90m");
    }
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 7;
  bitboard.black >>= 7;

  std::putchar(' ');
  std::putchar('|');
  for (uint32_t i = 0; i < 6; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 0) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }
  std::putchar('\n');
  bitboard.white >>= 6;
  bitboard.black >>= 6;

  std::putchar('|');
  for (uint32_t i = 0; i < 7; ++i) {
    if (i == 5) {
      printf("\x1B[90m");
    }
    bool w = (bitboard.white & (bitmask_t(1) << i));
    bool b = (bitboard.black & (bitmask_t(1) << i));
    if (w) {
      std::putchar(WHITE);
    } else if (b) {
      std::putchar(BLACK);
    } else {
      std::putchar(EMPTY);
    }
    if (i == 1) {
      printf("\x1B[0m");
    }
    std::putchar('|');
  }

  printf("\x1B[0m");
  std::putchar('\n');
}

} // namespace bzz
