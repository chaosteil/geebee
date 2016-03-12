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
  setupOpcodes();
  setupCbOpcodes();
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

void CPU::setupOpcodes() {
  // clang-format off
  opcodes_[0x00] = [&]() { return 4; };
  opcodes_[0x10] = [&]() { std::runtime_error("STOP"); return 4; };

  opcodes_[0x01] = [&]() { return load16Data(bc_); };
  opcodes_[0x11] = [&]() { return load16Data(de_); };
  opcodes_[0x21] = [&]() { return load16Data(hl_); };
  opcodes_[0x31] = [&]() { return load16Data(sp_); };

  opcodes_[0x06] = [&]() { return load8DataHigh(bc_); };
  opcodes_[0x16] = [&]() { return load8DataHigh(de_); };
  opcodes_[0x26] = [&]() { return load8DataHigh(hl_); };

  opcodes_[0x0E] = [&]() { return load8DataLow(bc_); };
  opcodes_[0x1E] = [&]() { return load8DataLow(de_); };
  opcodes_[0x2E] = [&]() { return load8DataLow(hl_); };

  opcodes_[0xCB] = [&]() {
    Byte op = memory_.read(pc_++);
    return cb_opcodes_[op]();
  };

  // clang-format on
}

void CPU::setupCbOpcodes() {
  // clang-format off
  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0x40 + i * 8] = [&]() { return handleBit(i, bits::high(bc_)); };
    cb_opcodes_[0x41 + i * 8] = [&]() { return handleBit(i, bits::low(bc_)); };
    cb_opcodes_[0x42 + i * 8] = [&]() { return handleBit(i, bits::high(de_)); };
    cb_opcodes_[0x43 + i * 8] = [&]() { return handleBit(i, bits::low(de_)); };
    cb_opcodes_[0x44 + i * 8] = [&]() { return handleBit(i, bits::high(hl_)); };
    cb_opcodes_[0x45 + i * 8] = [&]() { return handleBit(i, bits::low(hl_)); };
    cb_opcodes_[0x46 + i * 8] = [&]() { return handleBitData(i, hl_); };
    cb_opcodes_[0x47 + i * 8] = [&]() { return handleBit(i, bits::high(af_)); };
  }
  // clang-format on
}

void CPU::readInstruction() {
  Byte op = memory_.read(pc_++);
  std::cout << "OP: 0x" << std::hex << (int)op << std::endl;
  handleOpcode(op);
}

void CPU::handleOpcode(Byte op) {
  opcodes_[op]();
  return;

  if (op == 0x0) {  // NOP

  } else if (op == 0x1a) {  // LD A,(DE)
    Byte value = memory_.read(de_);
    bits::set_high(af_, value);

  } else if (op == 0x20) {  // JR NZ,r8
    Byte addr = memory_.read(pc_++);
    if (!zero_) {
      pc_ += addr;
      // timer
    }

  } else if (op == 0x32) {  // LD (HL-),A
    Byte high = bits::high(af_);
    memory_.write(hl_, high);
    hl_--;

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
    //handlePrefixOpcode(prefixOp);

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

int CPU::load16Data(int& reg) {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  reg = bits::assemble(high, low);

  return 12;
}

int CPU::load8DataHigh(int& reg) {
  Byte value = memory_.read(pc_++);

  bits::set_high(reg, value);

  return 8;
}

int CPU::load8DataLow(int& reg) {
  Byte value = memory_.read(pc_++);

  bits::set_low(reg, value);

  return 8;
}

int CPU::handleBit(int bit, Byte byte) {
  zero_ = (byte & (1 << bit)) == 0;
  add_ = false;
  half_carry_ = true;
  serializeFlags();

  return 8;
}

int CPU::handleBitData(int bit, int address) {
  Byte value = memory_.read(address);

  handleBit(bit, value);

  return 16;
}
}
