#include "catch.hpp"

#include <iostream>
#include <string>

#include "CPU.h"
#include "Program.h"
#include "Window.h"

using namespace gb;
using namespace std;

TEST_CASE("Emulator passes the test roms", "[testroms],[!mayfail]") {
  Window window;

  auto run_until_done = [](CPU& cpu) {
    const string& data = cpu.memory().serial_data();
    while (data.find("Passed") == string::npos &&
           data.find("Failed") == string::npos) {
      cpu.cycle();
    }
  };

  SECTION("Can load test roms") {
    Program program{"roms/cpu_instrs.gb"};

    REQUIRE(program.rom().size() > 0);

    SECTION("CPU instructions are correct") {
      Program program{"roms/cpu_instrs.gb"};
      CPU cpu{window, program};
      run_until_done(cpu);

      REQUIRE(cpu.memory().serial_data().find("Passed") != string::npos);
    }

    SECTION("CPU instructions are correctly timed") {
      Program program{"roms/instr_timing.gb"};
      CPU cpu{window, program};
      run_until_done(cpu);

      REQUIRE(cpu.memory().serial_data().find("Passed") != string::npos);
    }

    SECTION("Memory instructions are correctly timed") {
      Program program{"roms/mem_timing.gb"};
      CPU cpu{window, program};
      run_until_done(cpu);

      REQUIRE(cpu.memory().serial_data().find("Passed") != string::npos);
    }
  }
}
