#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

#include "types.h"
#include "memory.h"

namespace gb {

class Program;

class CPU {
 public:
  CPU(const Program& program);
  ~CPU();

  void reset();
  void cycle();
  void printState();

 private:
  void readInstruction();

  void handleOpcode(Byte op);
  void handlePrefixOpcode(Byte op);
  void clearFlags();
  void serializeFlags();

  const Program& program_;
  Memory memory_;

  int af_;
  int bc_;
  int de_;
  int hl_;
  int sp_;
  int pc_;
  Byte interrupts_;

  bool zero_;
  bool add_;
  bool half_carry_;
  bool carry_;
};
}

#endif
