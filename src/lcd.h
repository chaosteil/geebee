#ifndef GEEBEE_LCD_H_
#define GEEBEE_LCD_H_

#include "types.h"

namespace gb {

class Window;

class LCD {
 public:
  LCD(Window& window);
  ~LCD() = default;

  void write(int address, Byte byte);
  Byte read(int address);
  int control() const { return control_; }

 private:
  void clearFlags();
  void serializeFlags();

  Window& window_;

  Byte control_;
  bool enable_;
  bool window_tile_map_display_;
  bool window_display_;
  bool bg_window_tile_data_;
  bool bg_tile_map_display_;
  bool obj_size_;
  bool obj_display_;
  bool bg_display_;
};
}

#endif
