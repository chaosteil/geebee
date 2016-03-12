#include "cpu.h"

#include "program.h"

namespace gb{

CPU::CPU(const Program& program) : program_(program) {
  reset();
}

CPU::~CPU() {
}

void CPU::reset() {
}

void CPU::cycle() {
}

}
