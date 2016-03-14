#include "bits.h"

namespace gb {
namespace bits {

Bytes toBytes(int value) {
  Bytes data(2, 0);

  data[0] = high(value);
  data[1] = low(value);

  return data;
}

int assemble(Byte high, Byte low) { return low + high * 0x100; }

int toValue(const Bytes& data, int offset) {
  return assemble(data[offset + 1], data[offset]);
}

void write(Bytes& bytes, int value, int offset) {
  bytes[offset] = high(value);
  bytes[offset + 1] = low(value);
}

bool bit(int value, int bit) { return (value >> bit) & 1; }

bool highBit(int value, int bit_) { return bit(value, bit_ + 8); }

bool lowBit(int value, int bit_) { return bit(value, bit_); }

Byte high(int value) { return (value >> 8) & 0xFF; }

Byte low(int value) { return (value)&0xFF; }

void setBit(Byte& value, int bit, bool flag) {
  value ^= (-flag ^ value) & (1 << bit);
}

void setBit(int& value, int bit, bool flag) {
  value ^= (-flag ^ value) & (1 << bit);
}

void setHighBit(int& value, int bit, bool flag) {
  setBit(value, bit + 8, flag);
}

void setLowBit(int& value, int bit, bool flag) { setBit(value, bit, flag); }

void setHigh(int& value, Byte high) {
  value &= 0xFF;
  value |= (int)(high) << 8;
}

void setLow(int& value, Byte low) {
  value &= 0xFF00;
  value |= low;
}

int inc(Byte& high, Byte& low) {
  int value = bits::assemble(high, low);
  value++;
  value &= 0xFFFF;
  high = bits::high(value);
  low = bits::low(value);

  return value;
}

int dec(Byte& high, Byte& low) {
  int value = bits::assemble(high, low);
  value--;
  value &= 0xFFFF;
  high = bits::high(value);
  low = bits::low(value);

  return value;
}
}
}
