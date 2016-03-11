#ifndef GEEBEE_CPU_H_
#define GEEBEE_CPU_H_

namespace gb {

class Program;

class CPU {
 public:
  CPU();
  ~CPU();

  void load(const Program& program);
  void cycle();
  void reset();
};
}

#endif
