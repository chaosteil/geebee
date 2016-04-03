#ifndef GEEBEE_SRC_JOYPAD_H
#define GEEBEE_SRC_JOYPAD_H

#include <array>

#include "IOHandler.h"
#include "types.h"

namespace gb {

class Memory;

class Joypad : public IOHandler {
 public:
  enum Key : int { Up = 0, Down, Left, Right, Start, Select, A, B, Max };

  Joypad(Memory& memory);
  Joypad(const Joypad& joypad) = delete;
  Joypad(Joypad&& joypad) = delete;
  ~Joypad();

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
