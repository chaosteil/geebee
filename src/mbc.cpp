#include "mbc.h"

#include <stdexcept>

#include "program.h"

namespace gb {

MBC::MBC(const Program& program) : program_(program) { reset(); }

void MBC::reset() {
  int type = program_.type();

  switch (type) {
    case 0x00:
      mbc_ = 0;
      break;

    case 0x01:
    case 0x02:
    case 0x03:
      mbc_ = 1;
      break;

    case 0x05:
    case 0x06:
      mbc_ = 2;
      break;

    case 0x0F:
    case 0x10:
    case 0x12:
    case 0x13:
      mbc_ = 3;
      break;

    case 0x15:
    case 0x16:
    case 0x17:
      mbc_ = 4;
      break;

    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
      mbc_ = 5;
      break;

    default:
      throw std::runtime_error("Invalid/Unknown MBC");
  }

  switch (type) {
    case 0x02:
    case 0x03:
    case 0x08:
    case 0x09:
    case 0x0C:
    case 0x0D:
    case 0x10:
    case 0x13:
    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x1B:
    case 0x1D:
    case 0x1E:
    case 0xFF:
      has_ram_ = true;
      break;

    default:
      break;
  }

  if (mbc_ == 2) {
    has_ram_ = true;
  }

  if (has_ram_) {
    prepareRam();
  }
}

Byte MBC::read(Word address) const {
  switch (address & 0xF000) {
    case 0x0000:
    case 0x1000:
    case 0x2000:
    case 0x3000:
      return program_.rom()[address];
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      return program_.rom()[translateRomAddress(address)];
    case 0xA000:
    case 0xB000:
      return ram_enable_ ? ram_[address - 0xA000] : 0;

    default:
      throw std::runtime_error("Invalid MBC read");
      return 0x0;
  }
}

void MBC::write(Word address, Byte byte) {
  switch (address & 0xF000) {
    case 0x0000:
    case 0x1000:
      enableRam(byte);
      break;
    case 0x2000:
    case 0x3000:
      selectRomBank(byte);
      break;
    case 0x4000:
    case 0x5000:
      selectModeSpecificSelect(byte);
      break;
    case 0x6000:
    case 0x7000:
      selectModeSelect(byte);
      break;
    case 0xA000:
    case 0xB000:
      if (ram_enable_) {
        ram_[address - 0xA000] = byte;
      }
      break;

    default:
      throw std::runtime_error("Invalid MBC write");
  }
}

uint32_t MBC::translateRomAddress(Word address) const {
  if (rom_bank_ <= 0x01) {
    return address;
  }

  uint32_t bank = rom_bank_;
  if (mbc_ == 1) {
    if (bank == 0x20 || bank == 0x40 || bank == 0x60) {
      bank++;
    }
  }

  bank *= 0x4000;
  return bank + (address - 0x4000);
}

void MBC::prepareRam() {
  if (mbc_ == 2) {
    ram_.assign(512, 0);
    return;
  }

  switch (program_.ram_size()) {
    case 0x01:
      ram_.assign(0x0800, 0);
      break;
    case 0x02:
      ram_.assign(0x2000, 0);
      break;
    case 0x03:
      ram_.assign(0x8000, 0);
      break;
    default:
      throw std::runtime_error("Invalid MBC RAM size");
  }
}

void MBC::enableRam(Byte byte) {
  byte &= 0x0F;
  ram_enable_ = byte == 0x0A;
}

void MBC::selectRomBank(Byte byte) {
  byte &= 0x1F;
  rom_bank_ &= 0xE0;
  rom_bank_ |= byte;
}

void MBC::selectModeSpecificSelect(Byte byte) {
  byte &= 0x03;
  if (ram_banking_) {
    ram_bank_ = byte;
  } else {
    rom_bank_ &= 0x1F;
    rom_bank_ |= byte << 5;
  }
}

void MBC::selectModeSelect(Byte byte) { ram_banking_ = byte & 0x01; }
}
