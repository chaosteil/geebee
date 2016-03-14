#ifndef GEEBEE_BITS_H_
#define GEEBEE_BITS_H_

#include "types.h"

namespace gb {
namespace bits {

Bytes toBytes(int value);
int assemble(Byte high, Byte low);
int toValue(const Bytes& data, int offset = 0);
void write(Bytes& bytes, int value, int offset = 0);

bool bit(int value, int bit);
bool highBit(int value, int bit);
bool lowBit(int value, int bit);
Byte high(int value);
Byte low(int value);

void setBit(Byte& value, int bit, bool flag);
void setBit(int& value, int bit, bool flag);
void setHighBit(int& value, int bit, bool flag);
void setLowBit(int& value, int bit, bool flag);
void setHigh(int& value, Byte high);
void setLow(int& value, Byte low);

int inc(Byte& high, Byte& low);
int dec(Byte& high, Byte& low);
}
}

#endif
