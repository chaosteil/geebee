#ifndef GEEBEE_BITS_H_
#define GEEBEE_BITS_H_

#include "types.h"

namespace gb {
namespace bits {

Bytes to_bytes(int value);
int assemble(Byte high, Byte low);
int to_value(const Bytes& data, int offset = 0);
void write(Bytes& bytes, int value, int offset = 0);

bool bit(int value, int bit);
bool highbit(int value, int bit);
bool lowbit(int value, int bit);
Byte high(int value);
Byte low(int value);

void set_bit(Byte& value, int bit, bool flag);
void set_bit(int& value, int bit, bool flag);
void set_highbit(int& value, int bit, bool flag);
void set_lowbit(int& value, int bit, bool flag);
void set_high(int& value, Byte high);
void set_low(int& value, Byte low);
}
}

#endif
