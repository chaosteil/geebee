#ifndef GEEBEE_SRC_BITS_H
#define GEEBEE_SRC_BITS_H

#include "types.h"

namespace gb {
namespace bits {

Bytes toBytes(Word value);
Word assemble(Byte high, Byte low);
Word toValue(const Bytes& data, int offset = 0);
void write(Bytes& bytes, Word value, int offset = 0);

bool bit(Word value, int bit);
bool highBit(Word value, int bit);
bool lowBit(Word value, int bit);
Byte high(Word value);
Byte low(Word value);

void setBit(Byte& value, int bit, bool flag);
void setBit(Word& value, int bit, bool flag);
void setHighBit(Word& value, int bit, bool flag);
void setLowBit(Word& value, int bit, bool flag);
void setHigh(Word& value, Byte high);
void setLow(Word& value, Byte low);

Word inc(Byte& high, Byte& low);
Word dec(Byte& high, Byte& low);

}  // namespace bits
}  // namespace gb

#endif
