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

  int load16Data(Byte& high, Byte& low);

  int write16DataAddress();
  int load16DataAddress();

  int jumpRelative8Data(bool jump);
  int jump16Data(bool jump);
  int call16Data(bool jump);
  int ret(bool jump);

  int inc(Byte& byte);
  int dec(Byte& byte);
  int addHl(Word word);
  int add(Byte byte);
  int sub(Byte byte);

  int compare(Byte byte);

  int handleAnd(Byte byte);
  int handleXor(Byte byte);
  int handleOr(Byte byte);

  int handleSwap(Byte& byte);
  int handleBit(int bit, Byte byte);
  int handleRes(int bit, Byte& byte);
  int handleSet(int bit, Byte& byte);

  int handleRst(Byte offset);

  int pop(Byte& high, Byte& low);
  int push(Byte high, Byte low);

  int add8Stack();

  int rotateLeft(Byte& byte);
  int rotateLeftCarry(Byte& byte);
  int rotateRight(Byte& byte);
  int rotateRightCarry(Byte& byte);
  int shiftLeftLogical(Byte& byte);
  int shiftRightLogical(Byte& byte);
  int shiftRight(Byte& byte);

  int daa();

  const Program& program_;
  Memory memory_;

  Opcodes opcodes_;
  Opcodes cb_opcodes_;

  Byte a_;
  Byte f_;
  Byte b_;
  Byte c_;
  Byte d_;
  Byte e_;
  Byte h_;
  Byte l_;

  Word sp_;
  Word pc_;

  bool interrupts_;
  bool halt_;
  bool zero_;
  bool add_;
  bool half_carry_;
  bool carry_;
};
}

#endif
