#include "LCD.h"

#include <algorithm>
#include <iostream>

#include "Memory.h"
#include "Window.h"
#include "bits.h"

namespace gb {

const std::array<int, 4> LCD::color_map_{255, 170, 85, 0};

LCD::LCD(Window& window, Memory& memory)
    : IOHandler(), window_(window), memory_(memory) {
  memory_.registerHandler(this);
}

LCD::~LCD() { memory_.unregisterHandler(this); }

void LCD::advance(int timing) {
  done_frame_ = false;
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
  }
  if (!enabled_) {
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

      Byte ly = memory_.read(Register::Ly);
      drawLine(ly);
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

bool LCD::handlesAddress(Word address) const {
  return address == Register::Dma;
}

Byte LCD::read(Word address) { return memory_.read(address); }

void LCD::write(Word address, Byte byte) {
  if (address == Register::Dma) {
    Word source_start = byte << 8 | 0x00;
    Word source_end = byte << 8 | 0x9F;
    Word destination = 0xFE00;
    for (Word i = source_start; i <= source_end; i++, destination++) {
      memory_.write(destination, read(i));
    }
  } else {
    memory_.write(address, byte);
  }
}

LCD::SpriteInfo::SpriteInfo(const Memory& memory, int id)
    : x(memory.read(0xFE00 + id * 4 + 1)),
      y(memory.read(0xFE00 + id * 4 + 0)),
      tile(memory.read(0xFE00 + id * 4 + 2)),
      flags(memory.read(0xFE00 + id * 4 + 3)) {}

void LCD::drawLine(int ly) {
  if (ly >= 144) {
    return;
  }
  Byte bgp_data = memory_.read(Register::Bgp);
  Byte obp0_data = memory_.read(Register::Obp0);
  Byte obp1_data = memory_.read(Register::Obp1);

  auto palette = [](Byte palette, int color) -> int {
    int value = palette >> (color * 2);
    value &= 0x3;
    return color_map_[value];
  };
  auto color_number = [](int bit, Byte top, Byte bottom) -> int {
    return (((top >> bit) & 1) << 1) | ((bottom >> bit) & 1);
  };

  Byte lcdc = memory_.read(Register::Lcdc);
  Byte scx = memory_.read(Register::Scx);
  Byte scy = memory_.read(Register::Scy);
  Byte wx = memory_.read(Register::Wx);
  Byte wy = memory_.read(Register::Wy);

  bool signed_tile = bits::bit(lcdc, 4);
  Word bg_tile_data = !signed_tile ? 0x9000 : 0x8000;
  Word bg_tile_map = !bits::bit(lcdc, 3) ? 0x9800 : 0x9C00;

  // BG
  if (bits::bit(lcdc, 0)) {
    int y = (ly + scy) % 256;
    for (int i = 0; i < 160; i++) {
      int x = (i + scx) % 256;
      int tile_x = x / 8;
      int tile_y = y / 8;
      int pixel_x = 8 - x % 8 - 1;
      int pixel_y = y % 8;

      Byte tile = memory_.read(bg_tile_map + (tile_y * 32) + tile_x);
      int offset = tile;
      if (!signed_tile) {
        offset = static_cast<SByte>(tile);
      }
      Byte bottom = memory_.read(bg_tile_data + (offset * 16) + (pixel_y * 2));
      Byte top = memory_.read(bg_tile_data + (offset * 16) + (pixel_y * 2) + 1);

      Byte pixel = palette(bgp_data, color_number(pixel_x, top, bottom));
      window_.setPixel(i, ly, pixel);
    }
  } else {
    for (int i = 0; i < 160; i++) {
      window_.setPixel(i, ly, 255);
    }
  }

  // OBJ
  std::vector<SpriteInfo> sprites;
  for (int i = 0; i < 40; i++) {
    sprites.push_back(SpriteInfo{memory_, i});
  }
  // Remove all sprites from consideration for this line if they don't fit in y
  sprites.erase(std::remove_if(std::begin(sprites), std::end(sprites),
                               [ly](const SpriteInfo& info) {
                                 return info.y == 0 || info.y >= 160 ||
                                        ly < info.y - 16 || ly >= info.y - 8;
                               }),
                sprites.end());
  // Sort all leftover sprites by x coordinate
  std::sort(std::begin(sprites), std::end(sprites),
            [](const SpriteInfo& left, const SpriteInfo& right) {
              return left.x < right.x;
            });

  // Keep at most 10 sprites
  int size = std::min(10, static_cast<int>(sprites.size()));
  // Draw them!
  for (int i = size - 1; i >= 0; i--) {
    SpriteInfo& info = sprites[i];
    int pixel_y = ly - info.y + 16;
    if (bits::bit(info.flags, 6)) {
      pixel_y = 8 - pixel_y - 1;
    }
    for (int x = 0; x < 8; x++) {
      if (info.x + x - 8 < 0) {
        continue;
      }
      int pixel_x = 8 - x % 8 - 1;
      if (bits::bit(info.flags, 5)) {
        pixel_x = 8 - pixel_x - 1;
      }

      Byte bottom = memory_.read(0x8000 + (static_cast<int>(info.tile) * 16) +
                                 (pixel_y * 2));
      Byte top = memory_.read(0x8000 + (static_cast<int>(info.tile) * 16) +
                              (pixel_y * 2) + 1);

      Byte obp = bits::bit(info.flags, 4) ? obp1_data : obp0_data;
      Byte pixel = palette(obp, color_number(pixel_x, top, bottom));

      if (pixel != 0xFF) {
        window_.setPixel(info.x + x - 8, ly, pixel);
      }
    }
  }
}

void LCD::resetInterruptFlags() {
  Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
  bits::setBit(interrupts, 0, false);
  bits::setBit(interrupts, 1, false);
  memory_.write(Memory::Register::InterruptFlag, interrupts);
}

void LCD::setMode(Mode mode) {
  if (mode == mode_) {
    return;
  }
  mode_ = mode;

  Byte stat = memory_.read(Register::Stat);
  // Write current mode
  stat &= 0xFC;
  stat |= static_cast<int>(mode_) & 0x03;

  // Request interrupt for mode if available
  Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
  if ((mode == Mode::HBlank && bits::bit(stat, 3)) ||
      (mode == Mode::VBlank && bits::bit(stat, 4)) ||
      (mode == Mode::OAM && bits::bit(stat, 5))) {
    bits::setBit(interrupts, 1, true);
  }
  if (mode == Mode::VBlank) {
    bits::setBit(interrupts, 0, true);
    done_frame_ = true;
  }

  memory_.write(Memory::Register::InterruptFlag, interrupts);
  memory_.write(Register::Stat, stat);
}

void LCD::updateMemoryAccess() {
  memory_.setOAMAccess(true);
  memory_.setVRAMAccess(true);

  if (!enabled_) {
    return;
  }

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

  if (ly == lyc) {
    Byte interrupts = memory_.read(Memory::Register::InterruptFlag);
    bits::setBit(interrupts, 1, true);
    memory_.write(Memory::Register::InterruptFlag, interrupts);
  }
}

}  // namespace gb
