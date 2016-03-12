#include "bits.h"

namespace gb {
namespace bits {

Bytes to_bytes(int value) {
  Bytes data(2, 0);

  data[0] = high(value);
  data[1] = low(value);

  return data;
}

int assemble(Byte high, Byte low) { return low + high * 0x100; }

int to_value(const Bytes& data, int offset) {
  return assemble(data[offset + 1], data[offset]);
}

void write(Bytes& bytes, int value, int offset) {
  bytes[offset] = high(value);
  bytes[offset + 1] = low(value);
}

bool bit(int value, int bit) { return (value >> bit) & 1; }

bool highbit(int value, int bit_) { return bit(value, bit_ + 8); }

bool lowbit(int value, int bit_) { return bit(value, bit_); }

Byte high(int value) { return (value >> 8) & 0xFF; }

Byte low(int value) { return (value)&0xFF; }

void set_bit(int& value, int bit, bool flag) {
  value ^= (-flag ^ value) & (1 << bit);
}

void set_highbit(int& value, int bit, bool flag) {
  set_bit(value, bit + 8, flag);
}

void set_lowbit(int& value, int bit, bool flag) { set_bit(value, bit, flag); }

void set_high(int& value, Byte high) {
  value &= 0xFF;
  value |= high << 4;
}

void set_low(int& value, Byte low) {
  value &= 0xFF00;
  value |= low;
}
}
}
