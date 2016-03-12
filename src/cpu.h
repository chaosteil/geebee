#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

#include "types.h"

namespace gb {

class Program;

class CPU {
 public:
  CPU(const Program& program);
  ~CPU();

  void reset();
  void cycle();

 private:
  const Program& program_;

  Bytes ram_;
  Bytes vram_;
};
}

#endif
