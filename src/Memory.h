#ifndef GEEBEE_MEMORY_H_
#define GEEBEE_MEMORY_H_

#include "MBC.h"
#include "types.h"

namespace gb {

class Program;

class Memory {
 public:
  enum Register : Word {
    SerialTransferData = 0xFF01,
    SerialTransferControl = 0xFF02,
    InterruptFlag = 0xFF0F,
    BootMode = 0xFF50,
    InterruptEnable = 0xFFFF
  };

  Memory(const Program& program);
  ~Memory() = default;

  const Bytes& ram() const { return ram_; }
  const Bytes& vram() const { return vram_; }
  const Bytes& sat() const { return sat_; }
  const Bytes& io() const { return io_; }
  const Bytes& hram() const { return hram_; }

  bool booting() const { return booting_; }

  Bytes& ram() { return ram_; }
  Bytes& vram() { return vram_; }
  Bytes& sat() { return sat_; }
  Bytes& io() { return io_; }
  Bytes& hram() { return hram_; }

  void reset();
  Byte read(Word address) const;
  void write(Word address, Byte byte);

  void setOAMAccess(bool enable) { oam_access_ = enable; }
  void setVRAMAccess(bool enable) { vram_access_ = enable; }

 private:
  static int in(Word address, Word from, Word to);

  const Program& program_;
  MBC mbc_;

  bool booting_{false};
  bool oam_access_{true};
  bool vram_access_{true};

  Bytes ram_;
  Bytes vram_;
  Bytes sat_;
  Bytes io_;
  Bytes hram_;
};
}

#endif
