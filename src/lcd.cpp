#include "lcd.h"

#include <array>
#include <iostream>

#include <SDL2/SDL.h>

#include "bits.h"
#include "window.h"

namespace gb {

LCD::LCD(Window& window) : window_(window) {
  clearFlags();
  serializeFlags();
}

void LCD::draw(const Bytes& bytes) {
  return;
  static std::array<int, 4> color_map = {255, 170, 85, 0};

  auto color = [](int bit, Byte top, Byte bottom) -> int {
    return color_map[(((top >> bit) & 1) << 1) | ((bottom >> bit) & 1)];
  };
  auto surface = window_.surface().lock();
  auto format = surface->format;
  uint32_t* pixels = (uint32_t*)surface->pixels;

  int z = 0;
  for (int y = 0; y < 32; ++y) {
    for (int x = 0; x < 32; ++x) {
      Byte tile = bytes[z + 0x1800];
      for (int i = 0; i < 8; ++i) {
        Byte bottom = bytes[tile * 16 + i * 2];
        Byte top = bytes[tile * 16 + 1 + i * 2];

        int a = color(7, bottom, top);
        int b = color(6, bottom, top);
        int c = color(5, bottom, top);
        int d = color(4, bottom, top);
        int e = color(3, bottom, top);
        int f = color(2, bottom, top);
        int g = color(1, bottom, top);
        int h = color(0, bottom, top);
        pixels[x * 8 + 0 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, a, a, a, 255);
        pixels[x * 8 + 1 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, b, b, b, 255);
        pixels[x * 8 + 2 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, c, c, c, 255);
        pixels[x * 8 + 3 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, d, d, d, 255);
        pixels[x * 8 + 4 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, e, e, e, 255);
        pixels[x * 8 + 5 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, f, f, f, 255);
        pixels[x * 8 + 6 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, g, g, g, 255);
        pixels[x * 8 + 7 + ((y * 8) + i) * 256] =
            SDL_MapRGBA(format, h, h, h, 255);
      }
      z++;
    }
  }
}

void LCD::write(int address, Byte byte) {}

Byte LCD::read(int address) {}

void LCD::clearFlags() {
  enable_ = false;
  window_tile_map_display_ = false;
  window_display_ = false;
  bg_window_tile_data_ = false;
  bg_tile_map_display_ = false;
  obj_size_ = false;
  obj_display_ = false;
  bg_display_ = false;
}

void LCD::serializeFlags() {
  bits::setBit(control_, 7, enable_);
  bits::setBit(control_, 6, window_tile_map_display_);
  bits::setBit(control_, 5, window_display_);
  bits::setBit(control_, 4, bg_window_tile_data_);
  bits::setBit(control_, 3, bg_tile_map_display_);
  bits::setBit(control_, 2, obj_size_);
  bits::setBit(control_, 1, obj_display_);
  bits::setBit(control_, 0, bg_display_);
}
}
