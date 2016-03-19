#ifndef GEEBEE_MBC_H_
#define GEEBEE_MBC_H_

#include "types.h"

namespace gb {

class Program;

class Mbc {
 public:
  Mbc(const Program& program);
  ~Mbc() = default;

  void reset();
  Byte read(Word address) const;
  void write(Word address, Byte byte);

 private:
  uint32_t translateRomAddress(Word address) const;

  void prepareRam();

  void enableRam(Byte byte);
  void selectRomBank(Byte byte);
  void selectModeSpecificSelect(Byte byte);
  void selectModeSelect(Byte byte);

  const Program& program_;

  int mbc_{0};

  bool has_ram_{false};
  bool has_battery_{false};
  bool has_timer_{false};

  bool ram_enable_{false};
  Byte rom_bank_{0};
  Byte ram_bank_{0};
  bool ram_banking_{false};

  Bytes ram_;
};
}

#endif
