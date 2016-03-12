#ifndef GEEBEE_MEMORY_H_
#define GEEBEE_MEMORY_H_

#include "types.h"

namespace gb {

class Program;

class Memory {
 public:
  Memory(const Program& program);
  ~Memory() = default;

  const Bytes& ram() const { return ram_; }
  const Bytes& vram() const { return vram_; }
  const Bytes& sat() const { return sat_; }
  const Bytes& io() const { return io_; }
  const Bytes& hram() const { return hram_; }

  Bytes& ram() { return ram_; }
  Bytes& vram() { return vram_; }
  Bytes& sat() { return sat_; }
  Bytes& io() { return io_; }
  Bytes& hram() { return hram_; }

  void reset();
  Byte read(int address) const;
  void write(int address, Byte byte);

 private:
  static int in(int address, int from, int to);
  const Program& program_;

  Bytes ram_;
  Bytes vram_;
  Bytes sat_;
  Bytes io_;
  Bytes hram_;
};
}

#endif
