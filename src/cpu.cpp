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

const std::array<std::string, 0x100> CPU::opcode_description_{
    "NOP",        "LD BC,d16",   "LD (BC),A",   "INC BC",      "INC B",
    "DEC B",      "LD B,d8",     "RLCA",        "LD (a16),SP", "ADD HL,BC",
    "LD A,(BC)",  "DEC BC",      "INC C",       "DEC C",       "LD C,d8",
    "RRCA",       "STOP 0",      "LD DE,d16",   "LD (DE),A",   "INC DE",
    "INC D",      "DEC D",       "LD D,d8",     "RLA",         "JR r8",
    "ADD HL,DE",  "LD A,(DE)",   "DEC DE",      "INC E",       "DEC E",
    "LD E,d8",    "RRA",         "JR NZ,r8",    "LD HL,d16",   "LD (HL+),A",
    "INC HL",     "INC H",       "DEC H",       "LD H,d8",     "DAA",
    "JR Z,r8",    "ADD HL,HL",   "LD A,(HL+)",  "DEC HL",      "INC L",
    "DEC L",      "LD L,d8",     "CPL",         "JR NC,r8",    "LD SP,d16",
    "LD (HL-),A", "INC SP",      "INC (HL)",    "DEC (HL)",    "LD (HL),d8",
    "SCF",        "JR C,r8",     "ADD HL,SP",   "LD A,(HL-)",  "DEC SP",
    "INC A",      "DEC A",       "LD A,d8",     "CCF",         "LD B,B",
    "LD B,C",     "LD B,D",      "LD B,E",      "LD B,H",      "LD B,L",
    "LD B,(HL)",  "LD B,A",      "LD C,B",      "LD C,C",      "LD C,D",
    "LD C,E",     "LD C,H",      "LD C,L",      "LD C,(HL)",   "LD C,A",
    "LD D,B",     "LD D,C",      "LD D,D",      "LD D,E",      "LD D,H",
    "LD D,L",     "LD D,(HL)",   "LD D,A",      "LD E,B",      "LD E,C",
    "LD E,D",     "LD E,E",      "LD E,H",      "LD E,L",      "LD E,(HL)",
    "LD E,A",     "LD H,B",      "LD H,C",      "LD H,D",      "LD H,E",
    "LD H,H",     "LD H,L",      "LD H,(HL)",   "LD H,A",      "LD L,B",
    "LD L,C",     "LD L,D",      "LD L,E",      "LD L,H",      "LD L,L",
    "LD L,(HL)",  "LD L,A",      "LD (HL),B",   "LD (HL),C",   "LD (HL),D",
    "LD (HL),E",  "LD (HL),H",   "LD (HL),L",   "HALT",        "LD (HL),A",
    "LD A,B",     "LD A,C",      "LD A,D",      "LD A,E",      "LD A,H",
    "LD A,L",     "LD A,(HL)",   "LD A,A",      "ADD A,B",     "ADD A,C",
    "ADD A,D",    "ADD A,E",     "ADD A,H",     "ADD A,L",     "ADD A,(HL)",
    "ADD A,A",    "ADC A,B",     "ADC A,C",     "ADC A,D",     "ADC A,E",
    "ADC A,H",    "ADC A,L",     "ADC A,(HL)",  "ADC A,A",     "SUB B",
    "SUB C",      "SUB D",       "SUB E",       "SUB H",       "SUB L",
    "SUB (HL)",   "SUB A",       "SBC A,B",     "SBC A,C",     "SBC A,D",
    "SBC A,E",    "SBC A,H",     "SBC A,L",     "SBC A,(HL)",  "SBC A,A",
    "AND B",      "AND C",       "AND D",       "AND E",       "AND H",
    "AND L",      "AND (HL)",    "AND A",       "XOR B",       "XOR C",
    "XOR D",      "XOR E",       "XOR H",       "XOR L",       "XOR (HL)",
    "XOR A",      "OR B",        "OR C",        "OR D",        "OR E",
    "OR H",       "OR L",        "OR (HL)",     "OR A",        "CP B",
    "CP C",       "CP D",        "CP E",        "CP H",        "CP L",
    "CP (HL)",    "CP A",        "RET NZ",      "POP BC",      "JP NZ,a16",
    "JP a16",     "CALL NZ,a16", "PUSH BC",     "ADD A,d8",    "RST 00H",
    "RET Z",      "RET",         "JP Z,a16",    "PREFIX CB",   "CALL Z,a16",
    "CALL a16",   "ADC A,d8",    "RST 08H",     "RET NC",      "POP DE",
    "JP NC,a16",  "---",         "CALL NC,a16", "PUSH DE",     "SUB d8",
    "RST 10H",    "RET C",       "RETI",        "JP C,a16",    "---",
    "CALL C,a16", "---",         "SBC A,d8",    "RST 18H",     "LD (a8),A",
    "POP HL",     "LD (C),A",    "---",         "---",         "PUSH HL",
    "AND d8",     "RST 20H",     "ADD SP,r8",   "JP (HL)",     "LD (a16),A",
    "---",        "---",         "---",         "XOR d8",      "RST 28H",
    "LDH A,(a8)", "POP AF",      "LD A,(C)",    "DI",          "---",
    "PUSH AF",    "OR d8",       "RST 30H",     "LD HL,SP+r8", "LD SP,HL",
    "LD A,(a16)", "EI",          "---",         "---",         "CP d8",
    "RST 38H"};

const std::array<std::string, 0x100> CPU::prefix_opcode_description_{
    "RLC B",      "RLC C",   "RLC D",      "RLC E",   "RLC H",      "RLC L",
    "RLC (HL)",   "RLC A",   "RRC B",      "RRC C",   "RRC D",      "RRC E",
    "RRC H",      "RRC L",   "RRC (HL)",   "RRC A",   "RL B",       "RL C",
    "RL D",       "RL E",    "RL H",       "RL L",    "RL (HL)",    "RL A",
    "RR B",       "RR C",    "RR D",       "RR E",    "RR H",       "RR L",
    "RR (HL)",    "RR A",    "SLA B",      "SLA C",   "SLA D",      "SLA E",
    "SLA H",      "SLA L",   "SLA (HL)",   "SLA A",   "SRA B",      "SRA C",
    "SRA D",      "SRA E",   "SRA H",      "SRA L",   "SRA (HL)",   "SRA A",
    "SWAP B",     "SWAP C",  "SWAP D",     "SWAP E",  "SWAP H",     "SWAP L",
    "SWAP (HL)",  "SWAP A",  "SRL B",      "SRL C",   "SRL D",      "SRL E",
    "SRL H",      "SRL L",   "SRL (HL)",   "SRL A",   "BIT 0,B",    "BIT 0,C",
    "BIT 0,D",    "BIT 0,E", "BIT 0,H",    "BIT 0,L", "BIT 0,(HL)", "BIT 0,A",
    "BIT 1,B",    "BIT 1,C", "BIT 1,D",    "BIT 1,E", "BIT 1,H",    "BIT 1,L",
    "BIT 1,(HL)", "BIT 1,A", "BIT 2,B",    "BIT 2,C", "BIT 2,D",    "BIT 2,E",
    "BIT 2,H",    "BIT 2,L", "BIT 2,(HL)", "BIT 2,A", "BIT 3,B",    "BIT 3,C",
    "BIT 3,D",    "BIT 3,E", "BIT 3,H",    "BIT 3,L", "BIT 3,(HL)", "BIT 3,A",
    "BIT 4,B",    "BIT 4,C", "BIT 4,D",    "BIT 4,E", "BIT 4,H",    "BIT 4,L",
    "BIT 4,(HL)", "BIT 4,A", "BIT 5,B",    "BIT 5,C", "BIT 5,D",    "BIT 5,E",
    "BIT 5,H",    "BIT 5,L", "BIT 5,(HL)", "BIT 5,A", "BIT 6,B",    "BIT 6,C",
    "BIT 6,D",    "BIT 6,E", "BIT 6,H",    "BIT 6,L", "BIT 6,(HL)", "BIT 6,A",
    "BIT 7,B",    "BIT 7,C", "BIT 7,D",    "BIT 7,E", "BIT 7,H",    "BIT 7,L",
    "BIT 7,(HL)", "BIT 7,A", "RES 0,B",    "RES 0,C", "RES 0,D",    "RES 0,E",
    "RES 0,H",    "RES 0,L", "RES 0,(HL)", "RES 0,A", "RES 1,B",    "RES 1,C",
    "RES 1,D",    "RES 1,E", "RES 1,H",    "RES 1,L", "RES 1,(HL)", "RES 1,A",
    "RES 2,B",    "RES 2,C", "RES 2,D",    "RES 2,E", "RES 2,H",    "RES 2,L",
    "RES 2,(HL)", "RES 2,A", "RES 3,B",    "RES 3,C", "RES 3,D",    "RES 3,E",
    "RES 3,H",    "RES 3,L", "RES 3,(HL)", "RES 3,A", "RES 4,B",    "RES 4,C",
    "RES 4,D",    "RES 4,E", "RES 4,H",    "RES 4,L", "RES 4,(HL)", "RES 4,A",
    "RES 5,B",    "RES 5,C", "RES 5,D",    "RES 5,E", "RES 5,H",    "RES 5,L",
    "RES 5,(HL)", "RES 5,A", "RES 6,B",    "RES 6,C", "RES 6,D",    "RES 6,E",
    "RES 6,H",    "RES 6,L", "RES 6,(HL)", "RES 6,A", "RES 7,B",    "RES 7,C",
    "RES 7,D",    "RES 7,E", "RES 7,H",    "RES 7,L", "RES 7,(HL)", "RES 7,A",
    "SET 0,B",    "SET 0,C", "SET 0,D",    "SET 0,E", "SET 0,H",    "SET 0,L",
    "SET 0,(HL)", "SET 0,A", "SET 1,B",    "SET 1,C", "SET 1,D",    "SET 1,E",
    "SET 1,H",    "SET 1,L", "SET 1,(HL)", "SET 1,A", "SET 2,B",    "SET 2,C",
    "SET 2,D",    "SET 2,E", "SET 2,H",    "SET 2,L", "SET 2,(HL)", "SET 2,A",
    "SET 3,B",    "SET 3,C", "SET 3,D",    "SET 3,E", "SET 3,H",    "SET 3,L",
    "SET 3,(HL)", "SET 3,A", "SET 4,B",    "SET 4,C", "SET 4,D",    "SET 4,E",
    "SET 4,H",    "SET 4,L", "SET 4,(HL)", "SET 4,A", "SET 5,B",    "SET 5,C",
    "SET 5,D",    "SET 5,E", "SET 5,H",    "SET 5,L", "SET 5,(HL)", "SET 5,A",
    "SET 6,B",    "SET 6,C", "SET 6,D",    "SET 6,E", "SET 6,H",    "SET 6,L",
    "SET 6,(HL)", "SET 6,A", "SET 7,B",    "SET 7,C", "SET 7,D",    "SET 7,E",
    "SET 7,H",    "SET 7,L", "SET 7,(HL)", "SET 7,A"};

CPU::CPU(LCD& lcd, const Program& program) : program_(program), memory_(program) {
  setupOpcodes();
  setupCbOpcodes();
  reset();
}

CPU::~CPU() {}

void CPU::reset() {
  memory_.reset();

  a_ = 0; f_ = 0;
  b_ = 0; c_ = 0;
  d_ = 0; e_ = 0;
  h_ = 0; l_ = 0;
  sp_ = 0;
  pc_ = 0;

  interrupts_ = false;
  halt_ = false;

  clearFlags();
}

void CPU::cycle() { if (!halt_) readInstruction(); }

void CPU::printState() {
  std::cout << std::hex << "AF: " << (int)a_ << (int)f_ << " "
            << "BC: " << (int)b_ << (int)c_ << " "
            << "DE: " << (int)d_ << (int)e_ << " "
            << "HL: " << (int)h_ << (int)l_ << " "
            << "SP: " << (int)sp_ << " "
            << "PC: " << (int)pc_ << " " << std::dec
            << " F: " << (zero_ ? "Zero " : "") << (add_ ? "Add " : "")
            << (half_carry_ ? "Half " : "") << (carry_ ? "Carry " : "")
            << std::endl;
}

void CPU::setupOpcodes() {
  // Helper functions
  auto bc = [&]() { return bits::assemble(b_, c_); };
  auto de = [&]() { return bits::assemble(d_, e_); };
  auto hl = [&]() { return bits::assemble(h_, l_); };

  // clang-format off
  opcodes_[0x00] = [&]() { return 4; };
  opcodes_[0x10] = [&]() { halt_ = true; /* HALT LCD */ return 4; };

  opcodes_[0x20] = [&]() { return jumpRelative8Data(!zero_); };
  opcodes_[0x30] = [&]() { return jumpRelative8Data(!carry_); };

  opcodes_[0x01] = [&]() { return load16Data(b_, c_); };
  opcodes_[0x11] = [&]() { return load16Data(d_, e_); };
  opcodes_[0x21] = [&]() { return load16Data(h_, l_); };
  opcodes_[0x31] = [&]() { Byte high, low; load16Data(high, low); sp_ = bits::assemble(high, low); return 12; };

  opcodes_[0x02] = [&]() { memory_.write(bc(), a_); return 8; };
  opcodes_[0x12] = [&]() { memory_.write(de(), a_); return 8; };
  opcodes_[0x22] = [&]() { memory_.write(hl(), a_); bits::inc(h_, l_); return 8; };
  opcodes_[0x32] = [&]() { memory_.write(hl(), a_); bits::dec(h_, l_); return 8; };

  opcodes_[0x03] = [&]() { bits::inc(b_, c_); return 8; };
  opcodes_[0x13] = [&]() { bits::inc(d_, e_); return 8; };
  opcodes_[0x23] = [&]() { bits::inc(h_, l_); return 8; };
  opcodes_[0x33] = [&]() { sp_++; sp_ &= 0xFFFF; return 8; };
  
  opcodes_[0x04] = [&]() { return inc(b_); };
  opcodes_[0x14] = [&]() { return inc(d_); };
  opcodes_[0x24] = [&]() { return inc(h_); };
  opcodes_[0x34] = [&]() { Byte byte = memory_.read(hl()); inc(byte); memory_.write(hl(), byte); return 12; };
  
  opcodes_[0x05] = [&]() { return dec(b_); };
  opcodes_[0x15] = [&]() { return dec(d_); };
  opcodes_[0x25] = [&]() { return dec(h_); };
  opcodes_[0x35] = [&]() { Byte byte = memory_.read(hl()); dec(byte); memory_.write(hl(), byte); return 12; };

  opcodes_[0x06] = [&]() { b_ = memory_.read(pc_++); return 8; };
  opcodes_[0x16] = [&]() { d_ = memory_.read(pc_++); return 8; };
  opcodes_[0x26] = [&]() { h_ = memory_.read(pc_++); return 8; };
  opcodes_[0x36] = [&]() { memory_.write(hl(), memory_.read(pc_++)); return 12; };

  opcodes_[0x07] = [&]() { rotateLeft(a_); zero_ = false; return 4; };
  opcodes_[0x17] = [&]() { rotateLeftCarry(a_); zero_ = false; return 4; };
  opcodes_[0x27] = [&]() { daa(); return 4; };
  opcodes_[0x37] = [&]() { add_ = false; half_carry_ = false; carry_ = true; return 4; };

  opcodes_[0x08] = [&]() { Byte low = memory_.read(pc_++); Byte high = memory_.read(pc_++); int address = bits::assemble(high, low); memory_.write(++address, bits::low(sp_)); memory_.write(++address, bits::high(sp_)); return 20; };
  opcodes_[0x18] = [&]() { return jumpRelative8Data(true); };
  opcodes_[0x28] = [&]() { return jumpRelative8Data(zero_); };
  opcodes_[0x38] = [&]() { return jumpRelative8Data(carry_); };
  
  // ADD

  opcodes_[0x0A] = [&]() { a_ = memory_.read(bc()); return 8; };
  opcodes_[0x1A] = [&]() { a_ = memory_.read(de()); return 8; };
  opcodes_[0x2A] = [&]() { a_ = memory_.read(hl()); bits::inc(h_, l_); return 8; };
  opcodes_[0x3A] = [&]() { a_ = memory_.read(hl()); bits::dec(h_, l_); return 8; };

  opcodes_[0x0B] = [&]() { bits::dec(b_, c_); return 8; };
  opcodes_[0x1B] = [&]() { bits::dec(d_, e_); return 8; };
  opcodes_[0x2B] = [&]() { bits::dec(h_, l_); return 8; };
  opcodes_[0x3B] = [&]() { sp_--; sp_ &= 0xFFFF; return 8; };
  
  opcodes_[0x0C] = [&]() { return inc(c_); };
  opcodes_[0x1C] = [&]() { return inc(e_); };
  opcodes_[0x2C] = [&]() { return inc(l_); };
  opcodes_[0x3C] = [&]() { return inc(a_); };

  opcodes_[0x0D] = [&]() { return dec(c_); };
  opcodes_[0x1D] = [&]() { return dec(e_); };
  opcodes_[0x2D] = [&]() { return dec(l_); };
  opcodes_[0x3D] = [&]() { return dec(a_); };

  opcodes_[0x0E] = [&]() { c_ = memory_.read(pc_++); return 8; };
  opcodes_[0x1E] = [&]() { e_ = memory_.read(pc_++); return 8; };
  opcodes_[0x2E] = [&]() { l_ = memory_.read(pc_++); return 8; };
  opcodes_[0x3E] = [&]() { a_ = memory_.read(pc_++); return 8; };

  // RRCA
  // RRA
  opcodes_[0x2F] = [&]() { add_ = true; half_carry_ = true; a_ = ~a_; return 4; };
  opcodes_[0x3F] = [&]() { add_ = false; half_carry_ = false; carry_ = !carry_; return 4; };

  opcodes_[0x40] = [&]() { b_ = b_; return 4; };
  opcodes_[0x41] = [&]() { b_ = c_; return 4; };
  opcodes_[0x42] = [&]() { b_ = d_; return 4; };
  opcodes_[0x43] = [&]() { b_ = e_; return 4; };
  opcodes_[0x44] = [&]() { b_ = h_; return 4; };
  opcodes_[0x45] = [&]() { b_ = l_; return 4; };
  opcodes_[0x46] = [&]() { b_ = memory_.read(hl()); return 8; };
  opcodes_[0x47] = [&]() { b_ = a_; return 4; };
  opcodes_[0x48] = [&]() { c_ = b_; return 4; };
  opcodes_[0x49] = [&]() { c_ = c_; return 4; };
  opcodes_[0x4A] = [&]() { c_ = d_; return 4; };
  opcodes_[0x4B] = [&]() { c_ = e_; return 4; };
  opcodes_[0x4C] = [&]() { c_ = h_; return 4; };
  opcodes_[0x4D] = [&]() { c_ = l_; return 4; };
  opcodes_[0x4E] = [&]() { c_ = memory_.read(hl()); return 8; };
  opcodes_[0x4F] = [&]() { c_ = a_; return 4; };

  opcodes_[0x50] = [&]() { d_ = b_; return 4; };
  opcodes_[0x51] = [&]() { d_ = c_; return 4; };
  opcodes_[0x52] = [&]() { d_ = d_; return 4; };
  opcodes_[0x53] = [&]() { d_ = e_; return 4; };
  opcodes_[0x54] = [&]() { d_ = h_; return 4; };
  opcodes_[0x55] = [&]() { d_ = l_; return 4; };
  opcodes_[0x56] = [&]() { d_ = memory_.read(hl()); return 8; };
  opcodes_[0x57] = [&]() { d_ = a_; return 4; };
  opcodes_[0x58] = [&]() { e_ = b_; return 4; };
  opcodes_[0x59] = [&]() { e_ = c_; return 4; };
  opcodes_[0x5A] = [&]() { e_ = d_; return 4; };
  opcodes_[0x5B] = [&]() { e_ = e_; return 4; };
  opcodes_[0x5C] = [&]() { e_ = h_; return 4; };
  opcodes_[0x5D] = [&]() { e_ = l_; return 4; };
  opcodes_[0x5E] = [&]() { e_ = memory_.read(hl()); return 8; };
  opcodes_[0x5F] = [&]() { e_ = a_; return 4; };

  opcodes_[0x60] = [&]() { h_ = b_; return 4; };
  opcodes_[0x61] = [&]() { h_ = c_; return 4; };
  opcodes_[0x62] = [&]() { h_ = d_; return 4; };
  opcodes_[0x63] = [&]() { h_ = e_; return 4; };
  opcodes_[0x64] = [&]() { h_ = h_; return 4; };
  opcodes_[0x65] = [&]() { h_ = l_; return 4; };
  opcodes_[0x66] = [&]() { h_ = memory_.read(hl()); return 8; };
  opcodes_[0x67] = [&]() { h_ = a_; return 4; };
  opcodes_[0x68] = [&]() { l_ = b_; return 4; };
  opcodes_[0x69] = [&]() { l_ = c_; return 4; };
  opcodes_[0x6A] = [&]() { l_ = d_; return 4; };
  opcodes_[0x6B] = [&]() { l_ = e_; return 4; };
  opcodes_[0x6C] = [&]() { l_ = h_; return 4; };
  opcodes_[0x6D] = [&]() { l_ = l_; return 4; };
  opcodes_[0x6E] = [&]() { l_ = memory_.read(hl()); return 8; };
  opcodes_[0x6F] = [&]() { l_ = a_; return 4; };

  opcodes_[0x70] = [&]() { memory_.write(hl(), b_); return 8; };
  opcodes_[0x71] = [&]() { memory_.write(hl(), c_); return 8; };
  opcodes_[0x72] = [&]() { memory_.write(hl(), d_); return 8; };
  opcodes_[0x73] = [&]() { memory_.write(hl(), e_); return 8; };
  opcodes_[0x74] = [&]() { memory_.write(hl(), h_); return 8; };
  opcodes_[0x75] = [&]() { memory_.write(hl(), l_); return 8; };

  opcodes_[0x76] = [&]() { halt_ = true; return 4; };

  opcodes_[0x77] = [&]() { memory_.write(hl(), a_); return 8; };

  opcodes_[0x78] = [&]() { a_ = b_; return 4; };
  opcodes_[0x79] = [&]() { a_ = c_; return 4; };
  opcodes_[0x7A] = [&]() { a_ = d_; return 4; };
  opcodes_[0x7B] = [&]() { a_ = e_; return 4; };
  opcodes_[0x7C] = [&]() { a_ = h_; return 4; };
  opcodes_[0x7D] = [&]() { a_ = l_; return 4; };
  opcodes_[0x7E] = [&]() { a_ = memory_.read(hl()); return 8; };
  opcodes_[0x7F] = [&]() { a_ = a_; return 4; };

  // ADD
  
  // ADC
  
  // SUB
  
  // SBC
  
  opcodes_[0xA0] = [&]() { return handleAnd(b_); };
  opcodes_[0xA1] = [&]() { return handleAnd(c_); };
  opcodes_[0xA2] = [&]() { return handleAnd(d_); };
  opcodes_[0xA3] = [&]() { return handleAnd(e_); };
  opcodes_[0xA4] = [&]() { return handleAnd(h_); };
  opcodes_[0xA5] = [&]() { return handleAnd(l_); };
  opcodes_[0xA6] = [&]() { return handleAnd(memory_.read(hl())) + 4; };
  opcodes_[0xA7] = [&]() { return handleAnd(a_); };

  opcodes_[0xA8] = [&]() { return handleXor(b_); };
  opcodes_[0xA9] = [&]() { return handleXor(c_); };
  opcodes_[0xAA] = [&]() { return handleXor(d_); };
  opcodes_[0xAB] = [&]() { return handleXor(e_); };
  opcodes_[0xAC] = [&]() { return handleXor(h_); };
  opcodes_[0xAD] = [&]() { return handleXor(l_); };
  opcodes_[0xAE] = [&]() { return handleXor(memory_.read(hl())) + 4; };
  opcodes_[0xAF] = [&]() { return handleXor(a_); };

  opcodes_[0xB0] = [&]() { return handleOr(b_); };
  opcodes_[0xB1] = [&]() { return handleOr(c_); };
  opcodes_[0xB2] = [&]() { return handleOr(d_); };
  opcodes_[0xB3] = [&]() { return handleOr(e_); };
  opcodes_[0xB4] = [&]() { return handleOr(h_); };
  opcodes_[0xB5] = [&]() { return handleOr(l_); };
  opcodes_[0xB6] = [&]() { return handleOr(memory_.read(hl())) + 4; };
  opcodes_[0xB7] = [&]() { return handleOr(a_); };
  
  opcodes_[0xB8] = [&]() { return compare(b_); };
  opcodes_[0xB9] = [&]() { return compare(c_); };
  opcodes_[0xBA] = [&]() { return compare(d_); };
  opcodes_[0xBB] = [&]() { return compare(e_); };
  opcodes_[0xBC] = [&]() { return compare(h_); };
  opcodes_[0xBD] = [&]() { return compare(l_); };
  opcodes_[0xBE] = [&]() { return compare(memory_.read(hl())) + 4; };
  opcodes_[0xBF] = [&]() { return compare(a_); };

  opcodes_[0xC0] = [&]() { return ret(!zero_); };
  opcodes_[0xD0] = [&]() { return ret(!carry_); };
  opcodes_[0xE0] = [&]() { memory_.write(0xFF00 + memory_.read(pc_++), a_); return 12; };
  opcodes_[0xF0] = [&]() { a_ = memory_.read(0xFF00 + memory_.read(pc_++)); return 12; };

  opcodes_[0xC1] = [&]() { return pop(b_, c_); };
  opcodes_[0xD1] = [&]() { return pop(d_, e_); };
  opcodes_[0xE1] = [&]() { return pop(h_, l_); };
  opcodes_[0xF1] = [&]() { return pop(a_, f_); };

  opcodes_[0xC2] = [&]() { return jump16Data(!zero_); };
  opcodes_[0xD2] = [&]() { return jump16Data(!carry_); };
  opcodes_[0xE2] = [&]() { memory_.write(0xFF00 + c_, a_); return 8; };
  opcodes_[0xF2] = [&]() { a_ = memory_.read(0xFF00 + c_); return 8; };

  opcodes_[0xC3] = [&]() { return jump16Data(true); };
  opcodes_[0xF3] = [&]() { interrupts_ = false; return 4; };

  opcodes_[0xC4] = [&]() { return call16Data(!zero_); };
  opcodes_[0xD4] = [&]() { return call16Data(!carry_); };

  opcodes_[0xC5] = [&]() { return push(b_, c_); };
  opcodes_[0xD5] = [&]() { return push(d_, e_); };
  opcodes_[0xE5] = [&]() { return push(h_, l_); };
  opcodes_[0xF5] = [&]() { return push(a_, f_); };

  // ADD
  // SUB
  opcodes_[0xE6] = [&]() { return handleAnd(memory_.read(pc_++)) + 4; };
  opcodes_[0xF6] = [&]() { return handleOr(memory_.read(pc_++)) + 4; };

  opcodes_[0xC7] = [&]() { return handleRst(0x00); };
  opcodes_[0xD7] = [&]() { return handleRst(0x10); };
  opcodes_[0xE7] = [&]() { return handleRst(0x20); };
  opcodes_[0xF7] = [&]() { return handleRst(0x30); };

  opcodes_[0xC8] = [&]() { return ret(zero_); };
  opcodes_[0xD8] = [&]() { return ret(carry_); };
  opcodes_[0xE8] = [&]() { return add8Stack(); };
  opcodes_[0xF8] = [&]() { int sp = sp_; add8Stack(); h_ = bits::high(sp_); l_ = bits::low(sp_); sp_ = sp; return 12; };

  opcodes_[0xC9] = [&]() { ret(true); return 16; };
  opcodes_[0xD9] = [&]() { ret(true); interrupts_ = true; return 16; };
  opcodes_[0xE9] = [&]() { pc_ = hl(); return 4; };
  opcodes_[0xE9] = [&]() { sp_ = hl(); return 8; };

  opcodes_[0xCA] = [&]() { return jump16Data(zero_); };
  opcodes_[0xDA] = [&]() { return jump16Data(carry_); };
  opcodes_[0xEA] = [&]() { return write16DataAddress(); };
  opcodes_[0xFA] = [&]() { return load16DataAddress(); };

  opcodes_[0xCB] = [&]() {
    Byte op = memory_.read(pc_++);
    std::cout << "CBOP: " << prefix_opcode_description_[op] << " - 0x" << std::hex << (int)op << std::endl;
    return cb_opcodes_[op]();
  };
  opcodes_[0xFB] = [&]() { interrupts_ = true; return 4; };

  opcodes_[0xCC] = [&]() { return call16Data(zero_); };
  opcodes_[0xDC] = [&]() { return call16Data(carry_); };

  opcodes_[0xCD] = [&]() { return call16Data(true); };

  // ADC
  // SBC
  opcodes_[0xEE] = [&]() { return handleXor(memory_.read(pc_++)) + 4; };
  opcodes_[0xFE] = [&]() { return compare(memory_.read(pc_++)) + 4; };
  
  opcodes_[0xCF] = [&]() { return handleRst(0x08); };
  opcodes_[0xDF] = [&]() { return handleRst(0x18); };
  opcodes_[0xEF] = [&]() { return handleRst(0x28); };
  opcodes_[0xFF] = [&]() { return handleRst(0x38); };

  // clang-format on
}

void CPU::setupCbOpcodes() {
  // Helper functions
  auto bc = [&]() { return bits::assemble(b_, c_); };
  auto de = [&]() { return bits::assemble(d_, e_); };
  auto hl = [&]() { return bits::assemble(h_, l_); };

  // clang-format off
  cb_opcodes_[0x00] = [&]() { return rotateLeft(b_); };
  cb_opcodes_[0x01] = [&]() { return rotateLeft(c_); };
  cb_opcodes_[0x02] = [&]() { return rotateLeft(d_); };
  cb_opcodes_[0x03] = [&]() { return rotateLeft(e_); };
  cb_opcodes_[0x04] = [&]() { return rotateLeft(h_); };
  cb_opcodes_[0x05] = [&]() { return rotateLeft(l_); };
  cb_opcodes_[0x06] = [&]() { Byte byte = memory_.read(hl()); rotateLeft(byte); memory_.write(hl(), byte); return 16; };
  cb_opcodes_[0x07] = [&]() { return rotateLeft(a_); };

  // RRC

  cb_opcodes_[0x10] = [&]() { return rotateLeftCarry(b_); };
  cb_opcodes_[0x11] = [&]() { return rotateLeftCarry(c_); };
  cb_opcodes_[0x12] = [&]() { return rotateLeftCarry(d_); };
  cb_opcodes_[0x13] = [&]() { return rotateLeftCarry(e_); };
  cb_opcodes_[0x14] = [&]() { return rotateLeftCarry(h_); };
  cb_opcodes_[0x15] = [&]() { return rotateLeftCarry(l_); };
  cb_opcodes_[0x16] = [&]() { Byte byte = memory_.read(hl()); rotateLeftCarry(byte); memory_.write(hl(), byte); return 16; };
  cb_opcodes_[0x17] = [&]() { return rotateLeftCarry(a_); };

  // RR

  // SLA

  // SRA

  cb_opcodes_[0x30] = [&]() { return handleSwap(b_); };
  cb_opcodes_[0x31] = [&]() { return handleSwap(c_); };
  cb_opcodes_[0x32] = [&]() { return handleSwap(d_); };
  cb_opcodes_[0x33] = [&]() { return handleSwap(e_); };
  cb_opcodes_[0x34] = [&]() { return handleSwap(h_); };
  cb_opcodes_[0x35] = [&]() { return handleSwap(l_); };
  cb_opcodes_[0x36] = [&]() { Byte byte = memory_.read(hl()); handleSwap(byte); memory_.write(hl(), byte); return 16; };
  cb_opcodes_[0x37] = [&]() { return handleSwap(a_); };

  cb_opcodes_[0x38] = [&]() { return shiftRightLogical(b_); };
  cb_opcodes_[0x39] = [&]() { return shiftRightLogical(c_); };
  cb_opcodes_[0x3A] = [&]() { return shiftRightLogical(d_); };
  cb_opcodes_[0x3B] = [&]() { return shiftRightLogical(e_); };
  cb_opcodes_[0x3C] = [&]() { return shiftRightLogical(h_); };
  cb_opcodes_[0x3D] = [&]() { return shiftRightLogical(l_); };
  cb_opcodes_[0x3E] = [&]() { Byte byte = memory_.read(hl()); shiftRightLogical(byte); memory_.write(hl(), byte); return 16; };
  cb_opcodes_[0x3F] = [&]() { return shiftRightLogical(a_); };

  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0x40 + i * 8] = [i, this]() { return handleBit(i, b_); };
    cb_opcodes_[0x41 + i * 8] = [i, this]() { return handleBit(i, c_); };
    cb_opcodes_[0x42 + i * 8] = [i, this]() { return handleBit(i, d_); };
    cb_opcodes_[0x43 + i * 8] = [i, this]() { return handleBit(i, e_); };
    cb_opcodes_[0x44 + i * 8] = [i, this]() { return handleBit(i, h_); };
    cb_opcodes_[0x45 + i * 8] = [i, this]() { return handleBit(i, l_); };
    cb_opcodes_[0x46 + i * 8] = [&hl, i, this]() { return handleBit(i, memory_.read(hl())) + 8; };
    cb_opcodes_[0x47 + i * 8] = [i, this]() { return handleBit(i, a_); };
  }
  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0x80 + i * 8] = [i, this]() { return handleRes(i, b_); };
    cb_opcodes_[0x81 + i * 8] = [i, this]() { return handleRes(i, c_); };
    cb_opcodes_[0x82 + i * 8] = [i, this]() { return handleRes(i, d_); };
    cb_opcodes_[0x83 + i * 8] = [i, this]() { return handleRes(i, e_); };
    cb_opcodes_[0x84 + i * 8] = [i, this]() { return handleRes(i, h_); };
    cb_opcodes_[0x85 + i * 8] = [i, this]() { return handleRes(i, l_); };
    cb_opcodes_[0x86 + i * 8] = [&hl, i, this]() { Byte byte = memory_.read(hl()); handleRes(i, byte); memory_.write(hl(), byte); return 16; };
    cb_opcodes_[0x87 + i * 8] = [i, this]() { return handleRes(i, a_); };
  }
  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0xC0 + i * 8] = [i, this]() { return handleSet(i, b_); };
    cb_opcodes_[0xC1 + i * 8] = [i, this]() { return handleSet(i, c_); };
    cb_opcodes_[0xC2 + i * 8] = [i, this]() { return handleSet(i, d_); };
    cb_opcodes_[0xC3 + i * 8] = [i, this]() { return handleSet(i, e_); };
    cb_opcodes_[0xC4 + i * 8] = [i, this]() { return handleSet(i, h_); };
    cb_opcodes_[0xC5 + i * 8] = [i, this]() { return handleSet(i, l_); };
    cb_opcodes_[0xC6 + i * 8] = [&hl, i, this]() { Byte byte = memory_.read(hl()); handleSet(i, byte); memory_.write(hl(), byte); return 16; };
    cb_opcodes_[0xC7 + i * 8] = [i, this]() { return handleSet(i, a_); };
  }
  // clang-format on
}

void CPU::readInstruction() {
  Byte op = memory_.read(pc_++);
  std::cout << "OP: " << opcode_description_[op] << " - 0x" << std::hex
            << (int)op << std::endl;
  handleOpcode(op);
  serializeFlags();
  printState();
}

void CPU::handleOpcode(Byte op) { int timing = opcodes_[op](); }

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
}

int CPU::load16Data(Byte& high, Byte& low) {
  low = memory_.read(pc_++);
  high = memory_.read(pc_++);

  return 12;
}

int CPU::write16DataAddress() {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  memory_.write(address, a_);

  return 16;
}

int CPU::load16DataAddress() {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  a_ = memory_.read(address);

  return 16;
}

int CPU::jumpRelative8Data(bool jump) {
  SByte address = memory_.read(pc_++);
  if (jump) {
    pc_ += address;
    return 12;
  } else {
    return 8;
  }
}

int CPU::jump16Data(bool jump) {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  if (jump) {
    pc_ = address;
    return 16;
  } else {
    return 12;
  }
}

int CPU::call16Data(bool jump) {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  if (jump) {
    push(bits::high(pc_), bits::low(pc_));
    pc_ = address;
    return 24;
  } else {
    return 12;
  }
}

int CPU::ret(bool jump) {
  if (jump) {
    Byte high, low;
    pop(high, low);
    pc_ = bits::assemble(high, low);
    return 20;
  } else {
    return 8;
  }
}

int CPU::inc(Byte& byte) {
  add_ = false;

  half_carry_ = ((byte & 0x0F) == 0x0F);
  byte++;
  byte &= 0xFF;
  zero_ = byte == 0;

  return 4;
}

int CPU::dec(Byte& byte) {
  add_ = true;

  half_carry_ = ((byte & 0x0F) == 0x00);
  byte--;
  byte &= 0xFF;
  zero_ = byte == 0;

  return 4;
}

int CPU::compare(Byte byte) {
  add_ = true;

  carry_ = a_ < byte;
  // TODO
  zero_ = a_ == byte;

  return 4;
}

int CPU::handleAnd(Byte byte) {
  a_ &= byte;

  zero_ = a_ == 0;
  add_ = 0;
  half_carry_ = 1;
  carry_ = 0;

  return 4;
}

int CPU::handleXor(Byte byte) {
  a_ ^= byte;
  clearFlags();
  zero_ = a_ == 0;

  return 4;
}

int CPU::handleOr(Byte byte) {
  a_ |= byte;

  zero_ = a_ == 0;
  add_ = 0;
  half_carry_ = 0;
  carry_ = 0;

  return 4;
}

int CPU::shiftRightLogical(Byte& byte) {
  carry_ = byte & 1;
  byte >>= 1;

  zero_ = byte == 0;
  add_ = false;
  half_carry_ = false;

  return 8;
}

int CPU::handleSwap(Byte& byte) {
  Byte temp = byte & 0x0F;
  byte >>= 4;
  byte |= (temp << 4);

  zero_ = byte == 0;
  add_ = false;
  half_carry_ = false;
  carry_ = false;

  return 8;
}

int CPU::handleBit(int bit, Byte byte) {
  zero_ = !bits::bit(byte, bit);
  add_ = false;
  half_carry_ = true;

  return 8;
}

int CPU::handleRes(int bit, Byte& byte) {
  bits::setBit(byte, bit, false);

  return 8;
}

int CPU::handleSet(int bit, Byte& byte) {
  bits::setBit(byte, bit, true);

  return 8;
}

int CPU::handleRst(Byte offset) {
  push(bits::high(pc_), bits::low(pc_));
  pc_ = offset;

  return 16;
}

int CPU::pop(Byte& high, Byte& low) {
  low = memory_.read(sp_++);
  high = memory_.read(sp_++);

  return 12;
}

int CPU::push(Byte high, Byte low) {
  memory_.write(--sp_, high);
  memory_.write(--sp_, low);

  return 16;
}

int CPU::add8Stack() {
  Byte byte = memory_.read(pc_++);
  zero_ = false;
  add_ = false;

  half_carry_ = (((sp_ & 0x000F) + byte & 0x000F) & 0x00F0) > 0;
  carry_ = (((sp_ & 0x00FF) + byte & 0x00FF) & 0x0F00) > 0;

  sp_ += byte;
  sp_ &= 0xFFFF;

  return 16;
}

int CPU::rotateLeft(Byte& byte) {
  carry_ = bits::bit(byte, 7);
  byte <<= 1;
  if (carry_) {
    byte |= 1;
  }
  zero_ = byte == 0;
  add_ = false;
  half_carry_ = false;

  return 8;
}

int CPU::rotateLeftCarry(Byte& byte) {
  bool carry = carry_;
  carry_ = bits::bit(byte, 7);
  byte <<= 1;
  if (carry) {
    byte |= 1;
  }

  zero_ = byte == 0;
  add_ = false;
  half_carry_ = false;

  return 8;
}

int CPU::daa() {
  Byte low = a_ & 0x0F;
  if (low > 9 || half_carry_) {
    a_ += 0x06;
  }
  half_carry_ = false;

  Byte high = (a_ & 0xF0) >> 4;
  if (high > 9 || carry_) {
    a_ += 0x60;
    carry_ = true;
  } else {
    carry_ = false;
  }

  zero_ = a_ == 0;

  return 4;
}

}
