#ifndef GEEBEE_SRC_JOYPAD_H
#define GEEBEE_SRC_JOYPAD_H

#include <array>

#include "IOHandler.h"
#include "types.h"

namespace gb {

class Memory;

class Joypad : public IOHandler {
 public:
  enum Key : int {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
    Start = 4,
    Select = 5,
    A = 6,
    B = 7,

    Max = 8
  };

  explicit Joypad(Memory& memory);
  Joypad(const Joypad& joypad) = delete;
  Joypad(Joypad&& joypad) = delete;
  ~Joypad() override;

  void press(Key key);
  void release(Key key);

  bool handlesAddress(Word address) const override;
  Byte read(Word address) override;
  void write(Word address, Byte byte) override;

 private:
  enum Register : Word { Joyp = 0xFF00 };
  Memory& memory_;

  std::array<bool, Key::Max> keys_;
};
}  // namespace gb

#endif
