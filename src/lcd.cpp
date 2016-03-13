#include "lcd.h"

#include "bits.h"
#include "window.h"

namespace gb {

LCD::LCD(Window& window) : window_(window) {
  clearFlags();
  serializeFlags();
}

void LCD::write(int address, Byte byte) {
}

Byte LCD::read(int address) {
}

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
