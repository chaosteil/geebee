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
      joypad_(memory_),
      timer_(memory_),
      lcd_(window, memory_) {
  setupOpcodes();
  setupCbOpcodes();
  reset();
}

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

  if (program_.bootrom().empty()) {
    initNoboot();
  }
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
  } else if (!interrupts_ && iFlag && halt_) {
    halt_ = false;
  } else if (!halt_) {
    timing = readInstruction();
  }

  timer_.advance(timing);
  lcd_.advance(timing);
}

void CPU::printState() {
  std::cout << std::hex << "AF: " << static_cast<int>(a_) << " "
            << static_cast<int>(f_) << " "
            << "BC: " << static_cast<int>(b_) << " " << static_cast<int>(c_)
            << " "
            << "DE: " << static_cast<int>(d_) << " " << static_cast<int>(e_)
            << " "
            << "HL: " << static_cast<int>(h_) << " " << static_cast<int>(l_)
            << " "
            << "SP: " << static_cast<int>(sp_) << " "
            << "PC: " << static_cast<int>(pc_) << " " << std::dec
            << " F: " << (zero_ ? "Zero " : "") << (add_ ? "Add " : "")
            << (half_carry_ ? "Half " : "") << (carry_ ? "Carry " : "")
            << std::endl;
}

void CPU::initNoboot() {
  a_ = 0x01;
  f_ = 0xB0;
  b_ = 0x00;
  c_ = 0x13;
  d_ = 0x00;
  e_ = 0xD8;
  h_ = 0x01;
  l_ = 0x4D;
  sp_ = 0xFFFE;
  pc_ = 0x0100;
  clearFlags();

  memory_.write(0xFF05, 0x00);
  memory_.write(0xFF06, 0x00);
  memory_.write(0xFF07, 0x00);
  memory_.write(0xFF10, 0x80);
  memory_.write(0xFF11, 0xBF);
  memory_.write(0xFF12, 0xF3);
  memory_.write(0xFF14, 0xBF);
  memory_.write(0xFF16, 0xF3);
  memory_.write(0xFF17, 0x00);
  memory_.write(0xFF19, 0xBF);
  memory_.write(0xFF1A, 0x7F);
  memory_.write(0xFF1B, 0xFF);
  memory_.write(0xFF1C, 0x9F);
  memory_.write(0xFF1E, 0xBF);
  memory_.write(0xFF20, 0xFF);
  memory_.write(0xFF21, 0x00);
  memory_.write(0xFF22, 0x00);
  memory_.write(0xFF23, 0xBF);
  memory_.write(0xFF24, 0x77);
  memory_.write(0xFF25, 0xF3);
  memory_.write(0xFF26, 0xF1);
  memory_.write(0xFF40, 0x91);
  memory_.write(0xFF42, 0x00);
  memory_.write(0xFF43, 0x00);
  memory_.write(0xFF45, 0x00);
  memory_.write(0xFF47, 0xFC);
  memory_.write(0xFF48, 0xFF);
  memory_.write(0xFF49, 0xFF);
  memory_.write(0xFF4A, 0x00);
  memory_.write(0xFF4B, 0x00);
  memory_.write(0xFFFF, 0x00);

  memory_.write(0xFF50, 0x01);
}

int CPU::readInstruction() {
  Byte op = memory_.read(pc_++);

  int timing = handleOpcode(op);
  serializeFlags();

  if (output_) {
    printState();
  }

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

}  // namespace gb
