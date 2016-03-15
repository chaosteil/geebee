#include "bits.h"

namespace gb {
namespace bits {

Bytes toBytes(Word value) {
  Bytes data(2, 0);

  data[0] = high(value);
  data[1] = low(value);

  return data;
}

Word assemble(Byte high, Byte low) { return low + high * 0x100; }

Word toValue(const Bytes& data, int offset) {
  return assemble(data[offset + 1], data[offset]);
}

void write(Bytes& bytes, Word value, int offset) {
  bytes[offset] = high(value);
  bytes[offset + 1] = low(value);
}

bool bit(Word value, int bit) { return (value >> bit) & 1; }

bool highBit(Word value, int bit_) { return bit(value, bit_ + 8); }

bool lowBit(Word value, int bit_) { return bit(value, bit_); }

Byte high(Word value) { return (value >> 8) & 0xFF; }

Byte low(Word value) { return (value)&0xFF; }

void setBit(Byte& value, int bit, bool flag) {
  value ^= (-flag ^ value) & (1 << bit);
}

void setBit(Word& value, int bit, bool flag) {
  value ^= (-flag ^ value) & (1 << bit);
}

void setHighBit(Word& value, int bit, bool flag) {
  setBit(value, bit + 8, flag);
}

void setLowBit(Word& value, int bit, bool flag) { setBit(value, bit, flag); }

void setHigh(Word& value, Byte high) {
  value &= 0xFF;
  value |= (Word)(high) << 8;
}

void setLow(Word& value, Byte low) {
  value &= 0xFF00;
  value |= low;
}

Word inc(Byte& high, Byte& low) {
  Word value = bits::assemble(high, low);
  value++;
  value &= 0xFFFF;
  high = bits::high(value);
  low = bits::low(value);

  return value;
}

Word dec(Byte& high, Byte& low) {
  Word value = bits::assemble(high, low);
  value--;
  value &= 0xFFFF;
  high = bits::high(value);
  low = bits::low(value);

  return value;
}
}
}
