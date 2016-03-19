#include "lcd.h"

#include <iostream>

#include <SDL2/SDL.h>

#include "bits.h"
#include "memory.h"
#include "window.h"

namespace gb {

const std::array<int, 4> color_map_{255, 170, 85, 0};

LCD::LCD(Window& window, Memory& memory) : window_(window), memory_(memory) {}

void LCD::advance(int timing) {
  resetInterruptFlags();

  memory_.setOAMAccess(true);
  memory_.setVRAMAccess(true);

  Byte lcdc = memory_.read(Register::Lcdc);
  // Check if screen is enabled
  if (!bits::bit(lcdc, 7)) {
    if (enabled_) {
      setMode(Mode::HBlank);
      mode_timing_ = 0;
      enabled_ = false;
    }
    return;
  } else if (!enabled_) {
    setMode(Mode::OAM);
    mode_timing_ = 0;
    enabled_ = true;
  }

  mode_timing_ += timing;
  if (mode_ == Mode::OAM) {
    if (mode_timing_ >= 79) {
      mode_timing_ -= 79;
      setMode(Mode::VRAM);
    }
  } else if (mode_ == Mode::VRAM) {
    if (mode_timing_ >= 172) {
      mode_timing_ -= 172;
      setMode(Mode::HBlank);
    }
  } else if (mode_ == Mode::HBlank) {
    if (mode_timing_ >= 205) {
      mode_timing_ -= 205;
      Byte ly = memory_.read(Register::Ly);
      ly++;
      memory_.write(Register::Ly, ly);
      if (ly >= 144) {
        setMode(Mode::VBlank);
      } else {
        setMode(Mode::OAM);
      }
    }
  } else {
    Byte ly = (mode_timing_ / 456) + 144;
    if (mode_timing_ >= 4560) {
      setMode(Mode::OAM);
      mode_timing_ -= 4560;
      memory_.write(Register::Ly, 0);
    } else {
      memory_.write(Register::Ly, ly);
    }
  }

  updateLyc();
  updateMemoryAccess();
}

void LCD::draw() {
  Byte bg_palette_data = memory_.read(Register::Bgp);
  Byte obp0_palette_data = memory_.read(Register::Obp0);
  Byte obp1_palette_data = memory_.read(Register::Obp1);

  auto palette = [](Byte palette, int color) -> int {
    int value = palette >> (color * 2);
    value &= 0x3;
    return color_map_[value];
  };
  auto color_number = [](int bit, Byte top, Byte bottom) -> int {
    return (((top >> bit) & 1) << 1) | ((bottom >> bit) & 1);
  };
  auto surface = window_.surface().lock();
  auto format = surface->format;
  uint32_t* pixels = (uint32_t*)surface->pixels;

  // int z = 0;
  // for (int y = 0; y < 32; ++y) {
  // for (int x = 0; x < 32; ++x) {
  // Byte tile = memory_.vram()[z + 0x1800];
  // for (int i = 0; i < 8; ++i) {
  // Byte bottom = memory_.vram()[tile * 16 + i * 2];
  // Byte top = memory_.vram()[tile * 16 + 1 + i * 2];

  // int a = color(7, bottom, top);
  // int b = color(6, bottom, top);
  // int c = color(5, bottom, top);
  // int d = color(4, bottom, top);
  // int e = color(3, bottom, top);
  // int f = color(2, bottom, top);
  // int g = color(1, bottom, top);
  // int h = color(0, bottom, top);
  // pixels[x * 8 + 0 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, a, a, a, 255);
  // pixels[x * 8 + 1 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, b, b, b, 255);
  // pixels[x * 8 + 2 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, c, c, c, 255);
  // pixels[x * 8 + 3 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, d, d, d, 255);
  // pixels[x * 8 + 4 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, e, e, e, 255);
  // pixels[x * 8 + 5 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, f, f, f, 255);
  // pixels[x * 8 + 6 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, g, g, g, 255);
  // pixels[x * 8 + 7 + ((y * 8) + i) * 256] =
  // SDL_MapRGBA(format, h, h, h, 255);
  //}
  // z++;
  //}
  //}
}

void LCD::resetInterruptFlags() {
  Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
  bits::setBit(interrupts, 0, false);
  bits::setBit(interrupts, 1, false);
  memory_.write(Memory::Register::InterruptFlag, interrupts);
}

void LCD::setMode(Mode mode) {
  if (mode == mode_) return;
  mode_ = mode;

  Byte stat = memory_.read(Register::Stat);
  // Write current mode
  stat &= 0xFC;
  stat |= (int)mode_ & 0x03;

  // Request interrupt for mode if available
  Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
  if ((mode == Mode::HBlank && bits::bit(stat, 3)) ||
      (mode == Mode::VBlank && bits::bit(stat, 4)) ||
      (mode == Mode::OAM && bits::bit(stat, 5))) {
    bits::setBit(interrupts, 1, true);
  }
  if ((mode == Mode::VBlank)) {
    bits::setBit(interrupts, 0, true);
  }

  memory_.write(Memory::Register::InterruptFlag, interrupts);
  memory_.write(Register::Stat, stat);
}

void LCD::updateMemoryAccess() {
  memory_.setOAMAccess(true);
  memory_.setVRAMAccess(true);

  if (!enabled_) return;

  if (mode_ == Mode::OAM) {
    memory_.setOAMAccess(false);
  } else if (mode_ == Mode::VRAM) {
    memory_.setOAMAccess(false);
    memory_.setVRAMAccess(false);
  }
}

void LCD::updateLyc() {
  Byte ly = memory_.read(Register::Ly);
  Byte lyc = memory_.read(Register::Lyc);

  Byte stat = memory_.read(Register::Stat);
  bits::setBit(stat, 2, ly == lyc);
}
}
