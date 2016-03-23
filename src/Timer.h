#ifndef GEEBEE_SRC_TIMER_H
#define GEEBEE_SRC_TIMER_H

#include <array>

#include "types.h"

namespace gb {

class Memory;

class Timer {
 public:
  explicit Timer(Memory& memory);
  ~Timer() = default;

  void advance(int timing);

 private:
  enum Register : Word {
    Divider = 0xFF04,
    Counter = 0xFF05,
    Modulo = 0xFF06,
    Control = 0xFF07
  };
  static const std::array<int, 4> clocks_;

  Memory& memory_;

  int divider_{0};
  int counter_{0};
};

}  // namespace gb

#endif
