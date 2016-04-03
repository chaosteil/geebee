#ifndef GEEBEE_SRC_TIMER_H
#define GEEBEE_SRC_TIMER_H

#include <array>

#include "IOHandler.h"
#include "types.h"

namespace gb {

class Memory;

class Timer : public IOHandler {
 public:
  explicit Timer(Memory& memory);
  Timer(const Timer& timer) = delete;
  Timer(Timer&& timer) = delete;
  ~Timer();

  void advance(int timing);

  bool handlesAddress(Word address) const override;
  Byte read(Word address) override;
  void write(Word address, Byte byte) override;

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
