#include "Timer.h"

#include <iostream>

#include "Memory.h"
#include "bits.h"

namespace gb {

const std::array<int, 4> Timer::clocks_{1024, 16, 64, 256};

Timer::Timer(Memory& memory) : memory_(memory) {}

void Timer::advance(int timing) {
  Byte control = memory_.read(Register::Control);
  if (!bits::bit(control, 2)) {
    return;
  }

  divider_ += timing;
  while (divider_ >= clocks_[0]) {
    divider_ -= clocks_[0];
    Byte divider = memory_.read(Register::Divider);
    divider++;
    memory_.io()[Register::Divider - 0xFF00] = divider;
  }

  counter_ += timing;
  int current_clock = clocks_[control & 0x03];
  while (counter_ >= current_clock) {
    counter_ -= clocks_[control & 0x03];
    Byte counter = memory_.read(Register::Counter);
    counter++;
    if (counter == 0) {
      Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
      bits::setBit(interrupts, 2, true);
      memory_.write(Memory::Register::InterruptFlag, interrupts);

      counter = memory_.read(Register::Modulo);
    }
    memory_.write(Register::Counter, counter);
  }
}

}  // namespace gb
