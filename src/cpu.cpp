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
            << "SP: " << sp_ << " "
            << "PC: " << pc_ << " " << std::dec
            << " F: " << (zero_ ? "Zero " : "") << (add_ ? "Add " : "")
            << (half_carry_ ? "Half " : "") << (carry_ ? "Carry " : "")
            << std::endl;
}

void CPU::setupOpcodes() {
  // clang-format off
  opcodes_[0x00] = [&]() { return 4; };
  opcodes_[0x10] = [&]() { std::runtime_error("STOP"); return 4; };

  opcodes_[0x20] = [&]() { return jumpRelative8Data(!zero_); };
  opcodes_[0x30] = [&]() { return jumpRelative8Data(!carry_); };

  opcodes_[0x01] = [&]() { return load16Data(bc_); };
  opcodes_[0x11] = [&]() { return load16Data(de_); };
  opcodes_[0x21] = [&]() { return load16Data(hl_); };
  opcodes_[0x31] = [&]() { return load16Data(sp_); };

  opcodes_[0x02] = [&]() { return writeData(bc_, bits::high(af_)); };
  opcodes_[0x12] = [&]() { return writeData(de_, bits::high(af_)); };
  opcodes_[0x22] = [&]() { int result = writeData(hl_, bits::high(af_)); hl_++; return result; };
  opcodes_[0x32] = [&]() { int result = writeData(hl_, bits::high(af_)); hl_--; return result; };

  opcodes_[0x03] = [&]() { return incReg(bc_); };
  opcodes_[0x13] = [&]() { return incReg(de_); };
  opcodes_[0x23] = [&]() { return incReg(hl_); };
  opcodes_[0x33] = [&]() { return incReg(sp_); };
  
  opcodes_[0x04] = [&]() { return incHigh(bc_); };
  opcodes_[0x14] = [&]() { return incHigh(de_); };
  opcodes_[0x24] = [&]() { return incHigh(hl_); };
  // INC (HL)
  
  opcodes_[0x05] = [&]() { return decHigh(bc_); };
  opcodes_[0x15] = [&]() { return decHigh(de_); };
  opcodes_[0x25] = [&]() { return decHigh(hl_); };
  // DEC (HL

  opcodes_[0x06] = [&]() { return load8DataHigh(bc_); };
  opcodes_[0x16] = [&]() { return load8DataHigh(de_); };
  opcodes_[0x26] = [&]() { return load8DataHigh(hl_); };
  opcodes_[0x36] = [&]() { memory_.write(hl_, memory_.read(pc_++)); return 12; };

  // RLCA
  opcodes_[0x17] = [&]() { return rotateLeftCarryHigh(af_); };
  // DAA
  // SCF

  // LD
  opcodes_[0x18] = [&]() { return jumpRelative8Data(true); };
  opcodes_[0x28] = [&]() { return jumpRelative8Data(zero_); };
  opcodes_[0x38] = [&]() { return jumpRelative8Data(carry_); };
  
  // ADD

  opcodes_[0x0A] = [&]() { return load8High(af_, memory_.read(bc_)) + 4; };
  opcodes_[0x1A] = [&]() { return load8High(af_, memory_.read(de_)) + 4; };
  opcodes_[0x2A] = [&]() { int result = load8High(af_, memory_.read(hl_)); hl_++; return result + 4; };
  opcodes_[0x3A] = [&]() { int result = load8High(af_, memory_.read(hl_)); hl_--; return result + 4; };

  opcodes_[0x0B] = [&]() { return decReg(bc_); };
  opcodes_[0x1B] = [&]() { return decReg(de_); };
  opcodes_[0x2B] = [&]() { return decReg(hl_); };
  opcodes_[0x3B] = [&]() { return decReg(sp_); };
  
  opcodes_[0x0C] = [&]() { return incLow(bc_); };
  opcodes_[0x1C] = [&]() { return incLow(de_); };
  opcodes_[0x2C] = [&]() { return incLow(hl_); };
  opcodes_[0x3C] = [&]() { return incHigh(af_); };

  opcodes_[0x0D] = [&]() { return decLow(bc_); };
  opcodes_[0x1D] = [&]() { return decLow(de_); };
  opcodes_[0x2D] = [&]() { return decLow(hl_); };
  opcodes_[0x3D] = [&]() { return decHigh(af_); };

  opcodes_[0x0E] = [&]() { return load8DataLow(bc_); };
  opcodes_[0x1E] = [&]() { return load8DataLow(de_); };
  opcodes_[0x2E] = [&]() { return load8DataLow(hl_); };
  opcodes_[0x3E] = [&]() { return load8DataHigh(af_); };

  // RRCA
  // RRA
  // CPL
  // CCF

  opcodes_[0x40] = [&]() { return load8High(bc_, bits::high(bc_)); };
  opcodes_[0x41] = [&]() { return load8High(bc_, bits::low(bc_)); };
  opcodes_[0x42] = [&]() { return load8High(bc_, bits::high(de_)); };
  opcodes_[0x43] = [&]() { return load8High(bc_, bits::low(de_)); };
  opcodes_[0x44] = [&]() { return load8High(bc_, bits::high(hl_)); };
  opcodes_[0x45] = [&]() { return load8High(bc_, bits::low(hl_)); };
  opcodes_[0x46] = [&]() { return load8High(bc_, memory_.read(hl_)) + 4; };
  opcodes_[0x47] = [&]() { return load8High(bc_, bits::high(af_)); };
  opcodes_[0x48] = [&]() { return load8Low(bc_, bits::high(bc_)); };
  opcodes_[0x49] = [&]() { return load8Low(bc_, bits::low(bc_)); };
  opcodes_[0x4A] = [&]() { return load8Low(bc_, bits::high(de_)); };
  opcodes_[0x4B] = [&]() { return load8Low(bc_, bits::low(de_)); };
  opcodes_[0x4C] = [&]() { return load8Low(bc_, bits::high(hl_)); };
  opcodes_[0x4D] = [&]() { return load8Low(bc_, bits::low(hl_)); };
  opcodes_[0x4E] = [&]() { return load8Low(bc_, memory_.read(hl_)) + 4; };
  opcodes_[0x4F] = [&]() { return load8Low(bc_, bits::high(af_)); };

  opcodes_[0x50] = [&]() { return load8High(de_, bits::high(bc_)); };
  opcodes_[0x51] = [&]() { return load8High(de_, bits::low(bc_)); };
  opcodes_[0x52] = [&]() { return load8High(de_, bits::high(de_)); };
  opcodes_[0x53] = [&]() { return load8High(de_, bits::low(de_)); };
  opcodes_[0x54] = [&]() { return load8High(de_, bits::high(hl_)); };
  opcodes_[0x55] = [&]() { return load8High(de_, bits::low(hl_)); };
  opcodes_[0x56] = [&]() { return load8High(de_, memory_.read(hl_)) + 4; };
  opcodes_[0x57] = [&]() { return load8High(de_, bits::high(af_)); };
  opcodes_[0x58] = [&]() { return load8Low(de_, bits::high(bc_)); };
  opcodes_[0x59] = [&]() { return load8Low(de_, bits::low(bc_)); };
  opcodes_[0x5A] = [&]() { return load8Low(de_, bits::high(de_)); };
  opcodes_[0x5B] = [&]() { return load8Low(de_, bits::low(de_)); };
  opcodes_[0x5C] = [&]() { return load8Low(de_, bits::high(hl_)); };
  opcodes_[0x5D] = [&]() { return load8Low(de_, bits::low(hl_)); };
  opcodes_[0x5E] = [&]() { return load8Low(de_, memory_.read(hl_)) + 4; };
  opcodes_[0x5F] = [&]() { return load8Low(de_, bits::high(af_)); };

  opcodes_[0x60] = [&]() { return load8High(hl_, bits::high(bc_)); };
  opcodes_[0x61] = [&]() { return load8High(hl_, bits::low(bc_)); };
  opcodes_[0x62] = [&]() { return load8High(hl_, bits::high(de_)); };
  opcodes_[0x63] = [&]() { return load8High(hl_, bits::low(de_)); };
  opcodes_[0x64] = [&]() { return load8High(hl_, bits::high(hl_)); };
  opcodes_[0x65] = [&]() { return load8High(hl_, bits::low(hl_)); };
  opcodes_[0x66] = [&]() { return load8High(hl_, memory_.read(hl_)) + 4; };
  opcodes_[0x67] = [&]() { return load8High(hl_, bits::high(af_)); };
  opcodes_[0x68] = [&]() { return load8Low(hl_, bits::high(bc_)); };
  opcodes_[0x69] = [&]() { return load8Low(hl_, bits::low(bc_)); };
  opcodes_[0x6A] = [&]() { return load8Low(hl_, bits::high(de_)); };
  opcodes_[0x6B] = [&]() { return load8Low(hl_, bits::low(de_)); };
  opcodes_[0x6C] = [&]() { return load8Low(hl_, bits::high(hl_)); };
  opcodes_[0x6D] = [&]() { return load8Low(hl_, bits::low(hl_)); };
  opcodes_[0x6E] = [&]() { return load8Low(hl_, memory_.read(hl_)) + 4; };
  opcodes_[0x6F] = [&]() { return load8Low(hl_, bits::high(af_)); };

  opcodes_[0x70] = [&]() { return writeData(hl_, bits::high(bc_)); };
  opcodes_[0x71] = [&]() { return writeData(hl_, bits::low(bc_)); };
  opcodes_[0x72] = [&]() { return writeData(hl_, bits::high(de_)); };
  opcodes_[0x73] = [&]() { return writeData(hl_, bits::low(de_)); };
  opcodes_[0x74] = [&]() { return writeData(hl_, bits::high(hl_)); };
  opcodes_[0x75] = [&]() { return writeData(hl_, bits::low(hl_)); };

  opcodes_[0x77] = [&]() { return writeData(hl_, bits::high(af_)); };

  opcodes_[0x78] = [&]() { return load8High(af_, bits::high(bc_)); };
  opcodes_[0x79] = [&]() { return load8High(af_, bits::low(bc_)); };
  opcodes_[0x7A] = [&]() { return load8High(af_, bits::high(de_)); };
  opcodes_[0x7B] = [&]() { return load8High(af_, bits::low(de_)); };
  opcodes_[0x7C] = [&]() { return load8High(af_, bits::high(hl_)); };
  opcodes_[0x7D] = [&]() { return load8High(af_, bits::low(hl_)); };
  opcodes_[0x7E] = [&]() { return load8High(af_, memory_.read(hl_)) + 4; };
  opcodes_[0x7F] = [&]() { return load8High(af_, bits::high(af_)); };

  opcodes_[0xA8] = [&]() { return handleXor(bits::high(bc_)); };
  opcodes_[0xA9] = [&]() { return handleXor(bits::low(bc_)); };
  opcodes_[0xAA] = [&]() { return handleXor(bits::high(de_)); };
  opcodes_[0xAB] = [&]() { return handleXor(bits::low(de_)); };
  opcodes_[0xAC] = [&]() { return handleXor(bits::high(hl_)); };
  opcodes_[0xAD] = [&]() { return handleXor(bits::low(hl_)); };
  opcodes_[0xAE] = [&]() { return handleXor(memory_.read(hl_)) + 4; };
  opcodes_[0xAF] = [&]() { return handleXor(bits::high(af_)); };

  opcodes_[0xB8] = [&]() { return compare(bits::high(bc_)); };
  opcodes_[0xB9] = [&]() { return compare(bits::low(bc_)); };
  opcodes_[0xBA] = [&]() { return compare(bits::high(de_)); };
  opcodes_[0xBB] = [&]() { return compare(bits::low(de_)); };
  opcodes_[0xBC] = [&]() { return compare(bits::high(hl_)); };
  opcodes_[0xBD] = [&]() { return compare(bits::low(hl_)); };
  opcodes_[0xBE] = [&]() { return compare(memory_.read(hl_)) + 4; };
  opcodes_[0xBF] = [&]() { return compare(bits::high(af_)); };

  opcodes_[0xC0] = [&]() { return ret(!zero_); };
  opcodes_[0xD0] = [&]() { return ret(!carry_); };
  opcodes_[0xE0] = [&]() { writeData(0xFF00 + memory_.read(pc_++), bits::high(af_)); return 12; };
  opcodes_[0xF0] = [&]() { load8High(af_, memory_.read(0xFF00 + memory_.read(pc_++))); return 12; };

  opcodes_[0xC1] = [&]() { return pop(bc_); };
  opcodes_[0xD1] = [&]() { return pop(de_); };
  opcodes_[0xE1] = [&]() { return pop(hl_); };
  opcodes_[0xF1] = [&]() { return pop(af_); };

  // JP
  // JP
  opcodes_[0xE2] = [&]() { return writeData(bits::low(bc_) + 0xFF00, bits::high(af_)); };
  opcodes_[0xF2] = [&]() { return load8High(af_, memory_.read(bits::low(bc_) + 0xFF00)) + 4; };
  // LD
  // LD

  // JP
  // DI

  opcodes_[0xC4] = [&]() { return call16Data(!zero_); };
  opcodes_[0xD4] = [&]() { return call16Data(!carry_); };

  opcodes_[0xC5] = [&]() { return push(bc_); };
  opcodes_[0xD5] = [&]() { return push(de_); };
  opcodes_[0xE5] = [&]() { return push(hl_); };
  opcodes_[0xF5] = [&]() { return push(af_); };

  // ADD
  // SUB
  // AND
  // OR

  // RST

  opcodes_[0xC8] = [&]() { return ret(zero_); };
  opcodes_[0xD8] = [&]() { return ret(carry_); };
  opcodes_[0xE8] = [&]() { return add8Stack(); };
  // LD

  opcodes_[0xC9] = [&]() { ret(true); return 16; };
  // RETI
  // JP (HL)
  // LD

  // JP
  // JP
  opcodes_[0xEA] = [&]() { return write16DataAddress(); };
  opcodes_[0xFA] = [&]() { return load16DataAddress(); };

  opcodes_[0xCB] = [&]() {
    Byte op = memory_.read(pc_++);
    std::cout << "CBOP: " << prefix_opcode_description_[op] << " - 0x" << std::hex << (int)op << std::endl;
    return cb_opcodes_[op]();
  };
  // EI

  opcodes_[0xCC] = [&]() { return call16Data(zero_); };
  opcodes_[0xDC] = [&]() { return call16Data(carry_); };

  opcodes_[0xCD] = [&]() { return call16Data(true); };

  // ADC
  // SBC
  opcodes_[0xEE] = [&]() { return handleXor(memory_.read(pc_++)) + 4; };
  opcodes_[0xFE] = [&]() { return compare(memory_.read(pc_++)) + 4; };
  
  // RST

  // clang-format on
}

void CPU::setupCbOpcodes() {
  // clang-format off

  cb_opcodes_[0x10] = [&]() { return rotateLeftCarryHigh(bc_); };
  cb_opcodes_[0x11] = [&]() { return rotateLeftCarryLow(bc_); };
  cb_opcodes_[0x12] = [&]() { return rotateLeftCarryHigh(de_); };
  cb_opcodes_[0x13] = [&]() { return rotateLeftCarryLow(de_); };
  cb_opcodes_[0x14] = [&]() { return rotateLeftCarryHigh(hl_); };
  cb_opcodes_[0x15] = [&]() { return rotateLeftCarryLow(hl_); };
  //cb_opcodes_[0x16] = [&]() { return rotateLeftCarryHigh(bc_); };
  cb_opcodes_[0x17] = [&]() { return rotateLeftCarryHigh(af_); };

  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0x40 + i * 8] = [i, this]() { return handleBit(i, bits::high(bc_)); };
    cb_opcodes_[0x41 + i * 8] = [i, this]() { return handleBit(i, bits::low(bc_)); };
    cb_opcodes_[0x42 + i * 8] = [i, this]() { return handleBit(i, bits::high(de_)); };
    cb_opcodes_[0x43 + i * 8] = [i, this]() { return handleBit(i, bits::low(de_)); };
    cb_opcodes_[0x44 + i * 8] = [i, this]() { return handleBit(i, bits::high(hl_)); };
    cb_opcodes_[0x45 + i * 8] = [i, this]() { return handleBit(i, bits::low(hl_)); };
    cb_opcodes_[0x46 + i * 8] = [i, this]() { return handleBit(i, memory_.read(hl_)) + 8; };
    cb_opcodes_[0x47 + i * 8] = [i, this]() { return handleBit(i, bits::high(af_)); };
  }
  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0x80 + i * 8] = [i, this]() { return handleResHigh(i, bc_); };
    cb_opcodes_[0x81 + i * 8] = [i, this]() { return handleResLow(i, bc_); };
    cb_opcodes_[0x82 + i * 8] = [i, this]() { return handleResHigh(i, de_); };
    cb_opcodes_[0x83 + i * 8] = [i, this]() { return handleResLow(i, de_); };
    cb_opcodes_[0x84 + i * 8] = [i, this]() { return handleResHigh(i, hl_); };
    cb_opcodes_[0x85 + i * 8] = [i, this]() { return handleResLow(i, hl_); };
    cb_opcodes_[0x86 + i * 8] = [i, this]() { return handleResMemory(i, hl_); };
    cb_opcodes_[0x87 + i * 8] = [i, this]() { return handleResHigh(i, af_); };
  }
  for (int i = 0; i < 8; i++) {
    cb_opcodes_[0xC0 + i * 8] = [i, this]() { return handleSetHigh(i, bc_); };
    cb_opcodes_[0xC1 + i * 8] = [i, this]() { return handleSetLow(i, bc_); };
    cb_opcodes_[0xC2 + i * 8] = [i, this]() { return handleSetHigh(i, de_); };
    cb_opcodes_[0xC3 + i * 8] = [i, this]() { return handleSetLow(i, de_); };
    cb_opcodes_[0xC4 + i * 8] = [i, this]() { return handleSetHigh(i, hl_); };
    cb_opcodes_[0xC5 + i * 8] = [i, this]() { return handleSetLow(i, hl_); };
    cb_opcodes_[0xC6 + i * 8] = [i, this]() { return handleSetMemory(i, hl_); };
    cb_opcodes_[0xC7 + i * 8] = [i, this]() { return handleSetHigh(i, af_); };
  }
  // clang-format on
}

void CPU::readInstruction() {
  Byte op = memory_.read(pc_++);
  std::cout << "OP: " << opcode_description_[op] << " - 0x" << std::hex
            << (int)op << std::endl;
  handleOpcode(op);
}

void CPU::handleOpcode(Byte op) { int timing = opcodes_[op](); }

void CPU::clearFlags() {
  zero_ = false;
  add_ = false;
  half_carry_ = false;
  carry_ = false;
}

void CPU::serializeFlags() {
  bits::setBit(af_, 7, zero_);
  bits::setBit(af_, 6, add_);
  bits::setBit(af_, 5, half_carry_);
  bits::setBit(af_, 4, carry_);
}

int CPU::load8High(int& reg, Byte byte) {
  bits::setHigh(reg, byte);
  return 4;
}

int CPU::load8Low(int& reg, Byte byte) {
  bits::setLow(reg, byte);
  return 4;
}

int CPU::load16Data(int& reg) {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  reg = bits::assemble(high, low);

  return 12;
}

int CPU::load8DataHigh(int& reg) {
  Byte value = memory_.read(pc_++);

  bits::setHigh(reg, value);

  return 8;
}

int CPU::load8DataLow(int& reg) {
  Byte value = memory_.read(pc_++);

  bits::setLow(reg, value);

  return 8;
}

int CPU::writeData(int address, Byte byte) {
  memory_.write(address, byte);

  return 8;
}

int CPU::write16DataAddress() {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  memory_.write(address, bits::high(af_));

  return 16;
}

int CPU::load16DataAddress() {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  bits::setHigh(af_, memory_.read(address));

  return 16;
}

int CPU::jumpRelative8Data(bool jump) {
  SByte address = memory_.read(pc_++);
  if (jump) {
    std::cout << "JMP" << std::endl;
    pc_ += address;
    return 12;
  } else {
    std::cout << "NO JMP" << std::endl;
    return 8;
  }
}

int CPU::call16Data(bool jump) {
  Byte low = memory_.read(pc_++);
  Byte high = memory_.read(pc_++);
  int address = bits::assemble(high, low);

  if (jump) {
    push(pc_);
    pc_ = address;
    return 24;
  } else {
    return 12;
  }
}

int CPU::ret(bool jump) {
  if (jump) {
    pop(pc_);
    return 20;
  } else {
    return 8;
  }
}

int CPU::incReg(int& reg) {
  reg++;
  reg &= 0xFFFF;

  return 8;
}

int CPU::incHigh(int& reg) {
  add_ = false;

  Byte high = bits::high(reg);
  half_carry_ = ((high & 0x0F) == 0x0F);
  high++;
  high &= 0xFF;
  zero_ = high == 0;
  bits::setHigh(reg, high);
  serializeFlags();

  return 4;
}

int CPU::incLow(int& reg) {
  add_ = false;

  Byte low = bits::low(reg);
  half_carry_ = ((low & 0x0F) == 0x0F);
  low++;
  low &= 0xFF;
  zero_ = low == 0;
  bits::setLow(reg, low);
  serializeFlags();

  return 4;
}

int CPU::decReg(int& reg) {
  reg--;
  reg &= 0xFFFF;

  return 8;
}

int CPU::decHigh(int& reg) {
  add_ = true;

  Byte high = bits::high(reg);
  half_carry_ = ((high & 0x0F) == 0x00);
  high--;
  high &= 0xFF;
  zero_ = high == 0;
  bits::setHigh(reg, high);
  serializeFlags();

  return 4;
}

int CPU::decLow(int& reg) {
  add_ = true;

  Byte low = bits::low(reg);
  half_carry_ = ((low & 0x0F) == 0x00);
  low--;
  low &= 0xFF;
  zero_ = low == 0;
  bits::setLow(reg, low);
  serializeFlags();

  return 4;
}

int CPU::compare(Byte byte) {
  add_ = true;

  Byte value = bits::high(af_);
  carry_ = value < byte;
  // TODO
  zero_ = value == byte;

  serializeFlags();

  return 4;
}

int CPU::handleXor(Byte byte) {
  Byte high = bits::high(af_);
  high ^= byte;
  bits::setHigh(af_, high);
  clearFlags();
  if (high == 0) {
    zero_ = true;
  }
  serializeFlags();

  return 4;
}

int CPU::handleBit(int bit, Byte byte) {
  zero_ = !bits::bit(byte, bit);
  add_ = false;
  half_carry_ = true;
  serializeFlags();

  return 8;
}

int CPU::handleResHigh(int bit, int& reg) {
  bits::setHighBit(reg, bit, false);

  return 8;
}

int CPU::handleResLow(int bit, int& reg) {
  bits::setLowBit(reg, bit, false);

  return 8;
}

int CPU::handleResMemory(int bit, int address) {
  Byte byte = memory_.read(address);
  bits::setBit(byte, bit, false);
  memory_.write(address, byte);

  return 16;
}

int CPU::handleSetHigh(int bit, int& reg) {
  bits::setHighBit(reg, bit, true);

  return 8;
}

int CPU::handleSetLow(int bit, int& reg) {
  bits::setLowBit(reg, bit, true);

  return 8;
}

int CPU::handleSetMemory(int bit, int address) {
  Byte byte = memory_.read(address);
  bits::setBit(byte, bit, false);
  memory_.write(address, byte);

  return 16;
}

int CPU::pop(int& reg) {
  Byte low = memory_.read(sp_++);
  Byte high = memory_.read(sp_++);

  reg = bits::assemble(high, low);

  return 12;
}

int CPU::push(int reg) {
  memory_.write(--sp_, bits::high(reg));
  memory_.write(--sp_, bits::low(reg));

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
  serializeFlags();

  return 16;
}

int CPU::rotateLeftCarryHigh(int& reg) {
  Byte byte = bits::high(reg);
  bool carry = carry_;
  if (bits::bit(byte, 7)) {
    carry_ = true;
  }
  byte <<= 1;
  if (carry) {
    byte |= 1;
  }

  zero_ = byte == 0;

  bits::setHigh(reg, byte);

  serializeFlags();

  return 8;
}

int CPU::rotateLeftCarryLow(int& reg) {
  Byte byte = bits::low(reg);
  bool carry = carry_;
  if (bits::bit(byte, 7)) {
    carry_ = true;
  }
  byte <<= 1;
  if (carry) {
    byte |= 1;
  }

  zero_ = byte == 0;

  bits::setLow(reg, byte);

  serializeFlags();

  return 8;
}
}
