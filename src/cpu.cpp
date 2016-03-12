#include "cpu.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>

#include "bits.h"
#include "program.h"

namespace gb {

CPU::CPU(const Program& program) : program_(program), memory_(program) {
  reset();
}

CPU::~CPU() {}

void CPU::reset() {
  memory_.reset();

  af_ = 0;
  bc_ = 0;
  de_ = 0;
  hl_ = 0;
  sp_ = 0;
  pc_ = 0;

  clearFlags();
}

void CPU::cycle() { readInstruction(); }

void CPU::printState() {
  std::cout << std::hex << "AF: " << af_ << " "
            << "BC: " << bc_ << " "
            << "DE: " << de_ << " "
            << "HL: " << hl_ << " "
            << "SP: " << sp_ << " " << std::dec << std::endl;
}

void CPU::readInstruction() {
  Byte op = memory_.read(pc_);
  pc_++;
  handleOpcode(op);
}

void CPU::handleOpcode(Byte op) {
  if (op == 0x0) {  // NOP
  } else if (op == 0x0c) {
    Byte value = bits::low(bc_);
    add_ = false;
    if (value == 0x0F) {
      half_carry_ = true;
      value++;
    } else if (value == 0xFF) {
      carry_ = true;
      zero_ = true;
      value = 0;
    } else {
      value++;
    }
    serializeFlags();

  } else if (op == 0x0e) {  // LD C,d8
    Byte value = memory_.read(pc_++);
    bits::set_low(bc_, value);

  } else if (op == 0x11) {  // LD DE,d16
    Byte low = memory_.read(pc_++);
    Byte high = memory_.read(pc_++);
    de_ = bits::assemble(high, low);

  } else if (op == 0x1a) {  // LD A,(DE)
    Byte value = memory_.read(de_);
    bits::set_high(af_, value);

  } else if (op == 0x20) {  // JR NZ,r8
    Byte addr = memory_.read(pc_++);
    if (!zero_) {
      pc_ += addr;
      // timer
    }

  } else if (op == 0x21) {  // LD HL,d16
    Byte low = memory_.read(pc_++);
    Byte high = memory_.read(pc_++);
    hl_ = bits::assemble(high, low);

  } else if (op == 0x31) {  // LD SP,d16
    Byte low = memory_.read(pc_++);
    Byte high = memory_.read(pc_++);
    sp_ = bits::assemble(high, low);

  } else if (op == 0x32) {  // LD (HL-),A
    Byte high = bits::high(af_);
    memory_.write(hl_, high);
    hl_--;

  } else if (op == 0x3e) {  // LD A,d8
    Byte value = memory_.read(pc_++);
    bits::set_high(af_, value);

  } else if (op == 0x77) {  // LD (HL),A
    Byte high = bits::high(af_);
    memory_.write(hl_, high);

  } else if (op == 0xaf) {  // XOR A
    Byte high = bits::high(af_);
    high ^= high;
    clearFlags();
    if (high == 0) {
      zero_ = true;
    }
    serializeFlags();

  } else if (op == 0xcb) {  // PREFIX CB
    Byte prefixOp = memory_.read(pc_++);
    handlePrefixOpcode(prefixOp);

  } else if (op == 0xcd) {  // CALL a16
    Byte low = memory_.read(pc_++);
    Byte high = memory_.read(pc_++);
    int pos = bits::assemble(high, low);

  } else if (op == 0xe0) {  // LDH (a8),A
    Byte value = memory_.read(pc_++);
    Byte high = bits::high(af_);
    memory_.write(0xFF00 + value, high);

  } else if (op == 0xe2) {  // LD (C),A
    Byte value = bits::high(af_);
    Byte c = bits::low(bc_);
    memory_.write(0xFF00 + c, value);

  } else if (op == 0x0f) {  // RRCA
    clearFlags();
    Byte high = bits::high(af_);
    if (high & 1) {
      carry_ = true;
    }
    high >>= 1;
    bits::set_high(af_, high);
    bits::set_bit(af_, 15, carry_);
    serializeFlags();

  } else {
    std::cout << "^--" << std::endl;
    throw std::runtime_error("Unimplemented opcode " + std::to_string(op));
  }

  // timer_ += timingtable[op];
}

void CPU::handlePrefixOpcode(Byte op) {
  std::cout << "PREFIX: 0x" << std::hex << ((int)(op)&0xFF) << std::endl;
  if (op == 0x20) {  // SLA B
    clearFlags();
    char high = bits::high(bc_);
    if (high & 0x80) {
      carry_ = true;
    }
    high <<= 1;
    bits::set_high(bc_, high & 0xFF);
    serializeFlags();

  } else if (op == 0x7c) {  // BIT 7,H
    zero_ = bits::highbit(hl_, 7);
    add_ = false;
    half_carry_ = true;
    serializeFlags();

  } else {
    std::cout << "^-- NOPREFIX" << std::endl;
    throw int();
  }
  // timer_ += prefixtimingtable[op];
}

void CPU::clearFlags() {
  zero_ = false;
  add_ = false;
  half_carry_ = false;
  carry_ = false;
}

void CPU::serializeFlags() {
  bits::set_bit(af_, 7, zero_);
  bits::set_bit(af_, 6, add_);
  bits::set_bit(af_, 5, half_carry_);
  bits::set_bit(af_, 4, carry_);
}
}
