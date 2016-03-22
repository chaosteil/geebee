#include "CPU.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>

#include "Program.h"
#include "bits.h"

namespace gb {

CPU::CPU(Window& window, const Program& program)
    : program_(program),
      memory_(program_),
      timer_(memory_),
      lcd_(window, memory_) {
  setupOpcodes();
  setupCbOpcodes();
  reset();
}

CPU::~CPU() {}

void CPU::reset() {
  memory_.reset();

  a_ = 0;
  f_ = 0;
  b_ = 0;
  c_ = 0;
  d_ = 0;
  e_ = 0;
  h_ = 0;
  l_ = 0;
  sp_ = 0;
  pc_ = 0;

  interrupts_ = true;
  halt_ = false;

  clearFlags();
}

void CPU::cycle() {
  do {
    step();
  } while (!lcd_.doneFrame());
}

void CPU::step() {
  int timing = 4;
  Byte iEnable = memory_.read(Memory::Register::InterruptEnable);
  Byte iFlag = memory_.read(Memory::Register::InterruptFlag);
  bool hasInterrupt = iEnable & iFlag;

  // Manage interrupts if we have any
  if (interrupts_ && hasInterrupt) {
    for (int i = 0; i <= 4; i++) {
      if (bits::bit(iEnable & iFlag, i)) {
        bits::setBit(iFlag, i, false);
        memory_.write(Memory::Register::InterruptFlag, iFlag);

        interrupts_ = false;
        push(bits::high(pc_), bits::low(pc_));
        pc_ = 0x40 + i * 0x08;

        halt_ = false;
        timing = 12;
        break;
      }
    }
  } else if (!interrupts_ && hasInterrupt && halt_) {
    halt_ = false;
  } else if (!halt_) {
    timing = readInstruction();
  }

  timer_.advance(timing);
  lcd_.advance(timing);
}

void CPU::printState() {
  std::cout << std::hex << "AF: " << (int)a_ << " " << (int)f_ << " "
            << "BC: " << (int)b_ << " " << (int)c_ << " "
            << "DE: " << (int)d_ << " " << (int)e_ << " "
            << "HL: " << (int)h_ << " " << (int)l_ << " "
            << "SP: " << (int)sp_ << " "
            << "PC: " << (int)pc_ << " " << std::dec
            << " F: " << (zero_ ? "Zero " : "") << (add_ ? "Add " : "")
            << (half_carry_ ? "Half " : "") << (carry_ ? "Carry " : "")
            << std::endl;
}

int CPU::readInstruction() {
  Byte op = memory_.read(pc_++);

  int timing = handleOpcode(op);
  serializeFlags();

  if (output_) printState();

  return timing;
}

int CPU::handleOpcode(Byte op) { return opcodes_[op](); }

void CPU::clearFlags() {
  zero_ = false;
  add_ = false;
  half_carry_ = false;
  carry_ = false;
}

void CPU::serializeFlags() {
  bits::setBit(f_, 7, zero_);
  bits::setBit(f_, 6, add_);
  bits::setBit(f_, 5, half_carry_);
  bits::setBit(f_, 4, carry_);

  f_ &= 0xF0;
}

void CPU::deserializeFlags() {
  zero_ = bits::bit(f_, 7);
  add_ = bits::bit(f_, 6);
  half_carry_ = bits::bit(f_, 5);
  carry_ = bits::bit(f_, 4);

  f_ &= 0xF0;
}

}
