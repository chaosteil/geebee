#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <SDL2/SDL.h>

#include "CPU.h"
#include "LCD.h"
#include "Program.h"
#include "SDL.h"
#include "Window.h"

using namespace std;
namespace po = boost::program_options;

static gb::SDL sdl;

int main(int argc, const char** argv) {
  cout << "GeeBee 0.1" << endl;
  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "Show the help message")(
      "file,f", po::value<string>(), "The .gb file to read")(
      "bootrom,b", po::value<string>()->default_value(""),
      "The .bin file to read for the boot rom");

  po::positional_options_description pos_desc;
  pos_desc.add("file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_desc)
                .run(),
            vm);
  po::notify(vm);

  if (vm.find("help") != vm.end() || vm.find("file") == vm.end()) {
    cout << desc << endl;
    return 1;
  }

  gb::Program program(vm["file"].as<string>(), vm["bootrom"].as<string>());

  if (!program.is_valid()) {
    cout << "Invalid rom!" << endl;
    return 2;
  }

  cout << program.title() << " " << program.type() << " " << program.ram_size()
       << endl;
  cout << "rom size: " << program.rom().size() << endl;
  cout << "bootrom size: " << program.bootrom().size() << endl;
  cout << "info: " << program.type() << " " << program.rom_size() << " "
       << program.ram_size() << endl;

  gb::Window window;
  gb::CPU cpu(window, program);
  while (true) {
    if (window.handleEvents()) {
      break;
    }
    cpu.cycle();
    window.draw();
  }

  return 0;
}
