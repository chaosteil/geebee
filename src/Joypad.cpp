#include "Joypad.h"

#include "bits.h"
#include "Memory.h"

namespace gb {

Joypad::Joypad(Memory& memory) : memory_(memory) {
  for (bool& b : keys_) {
    b = false;
  }

  memory_.registerHandler(this);
}

Joypad::~Joypad() { memory_.unregisterHandler(this); }

void Joypad::press(Key key) { keys_[key] = true; }

void Joypad::release(Key key) { keys_[key] = false; }

bool Joypad::handlesAddress(Word address) const {
  return address == Register::Joyp;
}

Byte Joypad::read(Word address) {
  Byte byte = memory_.read(address);
  if (!bits::bit(byte, 4)) {
    bits::setBit(byte, 0, !keys_[Key::Right]);
    bits::setBit(byte, 1, !keys_[Key::Left]);
    bits::setBit(byte, 2, !keys_[Key::Up]);
    bits::setBit(byte, 3, !keys_[Key::Down]);
  } else {
    bits::setBit(byte, 0, !keys_[Key::A]);
    bits::setBit(byte, 1, !keys_[Key::B]);
    bits::setBit(byte, 2, !keys_[Key::Select]);
    bits::setBit(byte, 3, !keys_[Key::Start]);
  }
  return byte;
}

void Joypad::write(Word address, Byte byte) {
  return memory_.write(address, byte);
}

}  // namespace gb
