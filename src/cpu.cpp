#include "cpu.h"

#include <algorithm>
#include <iostream>

#include "program.h"

namespace gb {

CPU::CPU(const Program& program)
    : program_(program) {
  reset();
}

CPU::~CPU() {}

void CPU::reset() {
  ram_.assign(0x2000, 0);
  vram_.assign(0x2000, 0);

  if (program_.bootrom().size() != 0) {
    std::copy(program_.bootrom().begin(), program_.bootrom().end(),
              ram_.begin());
  } else {
    // TODO
  }
}

void CPU::cycle() {}
}
