#include <iostream>
#include <stdexcept>
#include <string>

#include "memory.h"
#include "program.h"

namespace gb {

Memory::Memory(const Program& program) : program_(program) { reset(); }

void Memory::reset() {
  booting_ = true;

  ram_.assign(0x2000, 0);
  vram_.assign(0x2000, 0);
  sat_.assign(0xFEA0 - 0xFE00, 0);
  io_.assign(0xFF80 - 0xFF00, 0);
  hram_.assign(0xFFFF - 0xFF80, 0);
}

Byte Memory::read(int address) const {
  switch (address & 0xF000) {
    // 16kB ROM Bank 00
    case 0x0000:
    case 0x1000:
    case 0x2000:
    case 0x3000:
      if (booting_ && in(address, 0x0000, 0x0100)) {
        return program_.bootrom()[address];
      }
      return program_.rom()[address];

    // 16kB ROM Bank 01..NN
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      return program_.rom()[address];

    // 8kB VRAM
    case 0x8000:
    case 0x9000:
      return vram_[address - 0x8000];

    // 8kB ERAM
    case 0xA000:
    case 0xB000:
      throw std::runtime_error("Reading External RAM");

    // 4KB Work RAM Bank 0 (WRAM)
    case 0xC000:
      return ram_[address - 0xC000];

    // 4KB Work RAM Bank 0 (WRAM)
    case 0xD000:
      return ram_[address - 0xC000];

    case 0xE000:
      return ram_[address - 0xC000];

    default:
      break;
  };

  // Same as C000-DDFF (ECHO)
  if (in(address, 0xE000, 0xFDFF)) {
    return ram_[address - 0xC000];

    // Sprite Attribute Table (OAM)
  } else if (in(address, 0xFE00, 0xFE9F)) {
    return sat_[address - 0xFE00];

    // Not Usable
  } else if (in(address, 0xFEA0, 0xFEFF)) {
    throw std::runtime_error("Reading Not Usable Memory");

    // I/O Ports
  } else if (in(address, 0xFF00, 0xFF7F)) {
    if (address == 0xFF44) {
      drawable_ = true;
       return 0x90;
    }
    return io_[address - 0xFF00];

    // High RAM (HRAM)
  } else if (in(address, 0xFF80, 0xFFFE)) {
    return hram_[address - 0xFF80];

    // Interrupt Enable Register
  } else if (in(address, 0xFFFF, 0xFFFF)) {
    throw std::runtime_error("Reading Interrupt Enable Register");

  } else {
    throw std::runtime_error("Completely invalid address " +
                             std::to_string(address));
  }
}

void Memory::write(int address, Byte byte) {
  switch (address & 0xF000) {
    // 32kB ROM
    case 0x0000:
    case 0x1000:
    case 0x2000:
    case 0x3000:
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      throw std::runtime_error("Writing ROM");

    // 8kB Video RAM (VRAM)
    case 0x8000:
    case 0x9000:
      vram_[address - 0x8000] = byte; return;

    // 8kB External RAM
    case 0xA000:
    case 0xB000:
      throw std::runtime_error("Writing External RAM");

    // 4KB Work RAM Bank 0 (WRAM)
    case 0xC000:
      ram_[address - 0xC000] = byte; return;

    // 4KB Work RAM Bank 1 (WRAM)
    case 0xD000:
      ram_[address - 0xC000] = byte; return;

    // Same as C000-DDFF (ECHO)
    case 0xE000:
      ram_[address - 0xC000] = byte; return;

    default:
      break;
  };

  // Same as C000-DDFF (ECHO)
  if (in(address, 0xE000, 0xFDFF)) {
    ram_[address - 0xC000] = byte;

    // Sprite Attribute Table (OAM)
  } else if (in(address, 0xFE00, 0xFE9F)) {
    sat_[address - 0xFE00] = byte;

    // Not Usable
  } else if (in(address, 0xFEA0, 0xFEFF)) {
    throw std::runtime_error("Writing Not Usable Memory");

    // I/O Ports
  } else if (in(address, 0xFF00, 0xFF7F)) {
    std::cout << "IO WRITE: " << std::hex << (int)address << " " << byte
              << std::endl;
    io_[address - 0xFF00] = byte;

    if (address == 0xFF50 && byte != 0x0) {
      booting_ = false;
    }
    // High RAM (HRAM)
  } else if (in(address, 0xFF80, 0xFFFE)) {
    hram_[address - 0xFF80] = byte;

    // Interrupt Enable Register
  } else if (in(address, 0xFFFF, 0xFFFF)) {
    //throw std::runtime_error("Writing Interrupt Enable Register");

  } else {
    throw std::runtime_error("Completely invalid address " +
                             std::to_string(address));
  }
}

int Memory::in(int address, int from, int to) {
  return address >= from && address <= to;
}
}
