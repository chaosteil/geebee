#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

#include <array>
#include <functional>

#include "types.h"
#include "memory.h"

namespace gb {

class LCD;
class Program;

class CPU {
 public:
  CPU(LCD& lcd, const Program& program);
  ~CPU();

  Memory& memory() { return memory_; }
  void reset();
  void cycle();
  void printState();

 private:
  using Opcodes = std::array<std::function<int()>, 0x100>;
  
  static const std::array<std::string, 0x100> opcode_description_;
  static const std::array<std::string, 0x100> prefix_opcode_description_;

  void setupOpcodes();
  void setupCbOpcodes();
  void readInstruction();

  void handleOpcode(Byte op);
  void clearFlags();
  void serializeFlags();

  int load8High(int& reg, Byte byte);
  int load8Low(int& reg, Byte byte);

  int load16Data(int& reg);
  int load8DataHigh(int& reg);
  int load8DataLow(int& reg);

  int writeData(int address, Byte byte);

  int write16DataAddress();
  int load16DataAddress();

  int jumpRelative8Data(bool jump);
  int call16Data(bool jump);
  int ret(bool jump);

  int incReg(int& reg);
  int incHigh(int& reg);
  int incLow(int& reg);

  int decReg(int& reg);
  int decHigh(int& reg);
  int decLow(int& reg);

  int compare(Byte byte);

  int handleXor(Byte byte);
  int handleBit(int bit, Byte byte);
  int handleResHigh(int bit, int& reg);
  int handleResLow(int bit, int& reg);
  int handleResMemory(int bit, int address);
  int handleSetHigh(int bit, int& reg);
  int handleSetLow(int bit, int& reg);
  int handleSetMemory(int bit, int address);

  int pop(int& reg);
  int push(int reg);

  int add8Stack();

  int rotateLeftCarryHigh(int& reg);
  int rotateLeftCarryLow(int& reg);

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
