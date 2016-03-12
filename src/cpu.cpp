#include "cpu.h"

#include <iostream>

#include "program.h"

namespace gb {

CPU::CPU(const Program& program)
    : program_(program), ram_(0x2000, 0), vram_(0x2000, 0) {
  reset();
}

CPU::~CPU() {}

void CPU::reset() {}

void CPU::cycle() {}
}
