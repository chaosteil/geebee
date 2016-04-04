#ifndef GEEBEE_SRC_LCD_H
#define GEEBEE_SRC_LCD_H

#include <array>
#include <unordered_set>

#include "IOHandler.h"
#include "types.h"

namespace gb {

class Memory;
class Window;

class LCD : public IOHandler {
 public:
  LCD(Window& window, Memory& memory);
  LCD(const LCD& lcd) = delete;
  LCD(LCD&& lcd) = delete;
  ~LCD() override;
  LCD& operator=(const LCD& lcd) = delete;
  LCD& operator=(const LCD&& lcd) = delete;

  bool doneFrame() const { return done_frame_; }
  void advance(int timing);

  bool handlesAddress(Word address) const override;
  Byte read(Word address) override;
  void write(Word address, Byte byte) override;

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

  struct SpriteInfo {
    Byte y{0};
    Byte x{0};
    Byte tile{0};
    Byte flags{0};

    SpriteInfo(const Memory& memory, int id);
  };
  static const std::array<int, 4> color_map_;

  void drawLine(int ly);
  void resetInterruptFlags();
  void setMode(Mode mode);
  void updateMemoryAccess();
  void updateLyc();

  Window& window_;
  Memory& memory_;

  bool enabled_{false};
  Mode mode_{Mode::HBlank};
  int mode_timing_{0};
  bool done_frame_{true};
};

}  // namespace gb

#endif
