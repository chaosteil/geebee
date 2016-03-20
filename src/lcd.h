#ifndef GEEBEE_LCD_H_
#define GEEBEE_LCD_H_

#include <array>

#include "types.h"

namespace gb {

class Memory;
class Window;

class LCD {
 public:
  LCD(Window& window, Memory& memory);
  ~LCD() = default;

  void advance(int timing);
  void draw();

 private:
  enum class Mode : int { HBlank = 0, VBlank = 1, OAM = 2, VRAM = 3 };

  enum Register : Word {
    Lcdc = 0xFF40,
    Stat = 0xFF41,
    Scy = 0xFF42,
    Scx = 0xFF43,
    Ly = 0xFF44,
    Lyc = 0xFF45,
    Dma = 0xFF46,
    Bgp = 0xFF47,
    Obp0 = 0xFF48,
    Obp1 = 0xFF49,
    Wy = 0xFF4A,
    Wx = 0xFF4B
  };
  static const std::array<int, 4> color_map_;

  void resetInterruptFlags();
  void setMode(Mode mode);
  void updateMemoryAccess();
  void updateLyc();

  Window& window_;
  Memory& memory_;

  bool enabled_{false};
  Mode mode_{Mode::HBlank};
  int mode_timing_{0};
};
}

#endif
