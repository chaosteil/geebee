#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

#include <array>
#include <functional>

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
  using Opcodes = std::array<std::function<int()>, 0xFF>;
  void setupOpcodes();
  void setupCbOpcodes();
  void readInstruction();

  void handleOpcode(Byte op);
  void clearFlags();
  void serializeFlags();

  int load16Data(int& reg);
  int load8DataHigh(int& reg);
  int load8DataLow(int& reg);

  int handleBit(int bit, Byte byte);
  int handleBitData(int bit, int address);

  const Program& program_;
  Memory memory_;

  Opcodes opcodes_;
  Opcodes cb_opcodes_;

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
