#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

#include <vector>

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

  std::vector<char> ram_;
  std::vector<char> vram_;
};
}

#endif
