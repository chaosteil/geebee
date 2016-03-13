#include <iostream>
#include <stdexcept>
#include <string>

#include "memory.h"
#include "program.h"

namespace gb {

Memory::Memory(const Program& program) : program_(program) {}

void Memory::reset() {
  ram_.assign(0x2000, 0);
  vram_.assign(0x2000, 0);
  sat_.assign(0xFEA0 - 0xFE00, 0);
  io_.assign(0xFF80 - 0xFF00, 0);
  hram_.assign(0xFFFF - 0xFF80, 0);
}

Byte Memory::read(int address) const {
  // Bootrom hacks
  if (in(address, 0x0000, 0x0100)) {
    return program_.bootrom()[address];

  // 16kB ROM Bank 00
  } else if (in(address, 0x0000, 0x3FFF)) {
    return program_.rom()[address];

  // 16kB ROM Bank 01..NN
  } else if (in(address, 0x4000, 0x7FFF)) {
    return program_.rom()[address];

  // 8kB Video RAM (VRAM)
  } else if (in(address, 0x8000, 0x9FFF)) {
    return vram_[address - 0x8000];

  // 8kB External RAM
  } else if (in(address, 0xA000, 0xBFFF)) {
    throw std::runtime_error("Reading External RAM");

  // 4KB Work RAM Bank 0 (WRAM)
  } else if (in(address, 0xC000, 0xCFFF)) {
    return ram_[address - 0xC000];

  // 4KB Work RAM Bank 1 (WRAM)
  } else if (in(address, 0xD000, 0xDFFF)) {
    return ram_[address - 0xC000];

  // Same as C000-DDFF (ECHO)
  } else if (in(address, 0xE000, 0xFDFF)) {
    return ram_[address - 0xC000];

  // Sprite Attribute Table (OAM)
  } else if (in(address, 0xFE00, 0xFE9F)) {
    return sat_[address - 0xFE00];

  // Not Usable
  } else if (in(address, 0xFEA0, 0xFEFF)) {
    throw std::runtime_error("Reading Not Usable Memory");

  // I/O Ports
  } else if (in(address, 0xFF00, 0xFF7F)) {
    return io_[address - 0xFF00];

  // High RAM (HRAM)
  } else if (in(address, 0xFF80, 0xFFFE)) {
    return hram_[address - 0xFF80];

  // Interrupt Enable Register
  } else if (in(address, 0xFFFF, 0xFFFF)) {
    throw std::runtime_error("Reading Interrupt Enable Register");

  } else {
    throw std::runtime_error("Completely invalid address " + std::to_string(address));
  }
}

void Memory::write(int address, Byte byte) {
  // 32kB ROM
  if (in(address, 0x0000, 0x7FFF)) {
    throw std::runtime_error("Writing ROM");

  // 8kB Video RAM (VRAM)
  } else if (in(address, 0x8000, 0x9FFF)) {
    vram_[address - 0x8000] = byte;

  // 8kB External RAM
  } else if (in(address, 0xA000, 0xBFFF)) {
    throw std::runtime_error("Writing External RAM");

  // 4KB Work RAM Bank 0 (WRAM)
  } else if (in(address, 0xC000, 0xCFFF)) {
    ram_[address - 0xC000] = byte;

  // 4KB Work RAM Bank 1 (WRAM)
  } else if (in(address, 0xD000, 0xDFFF)) {
    ram_[address - 0xC000] = byte;

  // Same as C000-DDFF (ECHO)
  } else if (in(address, 0xE000, 0xFDFF)) {
    ram_[address - 0xC000] = byte;

  // Sprite Attribute Table (OAM)
  } else if (in(address, 0xFE00, 0xFE9F)) {
    sat_[address - 0xFE00] = byte;

  // Not Usable
  } else if (in(address, 0xFEA0, 0xFEFF)) {
    throw std::runtime_error("Writing Not Usable Memory");

  // I/O Ports
  } else if (in(address, 0xFF00, 0xFF7F)) {
    io_[address - 0xFF00] = byte;

  // High RAM (HRAM)
  } else if (in(address, 0xFF80, 0xFFFE)) {
    hram_[address - 0xFF80] = byte;

  // Interrupt Enable Register
  } else if (in(address, 0xFFFF, 0xFFFF)) {
    throw std::runtime_error("Writing Interrupt Enable Register");

  } else {
    throw std::runtime_error("Completely invalid address " + std::to_string(address));
  }
}

int Memory::in(int address, int from, int to) {
  return address >= from && address <= to;
}

}
