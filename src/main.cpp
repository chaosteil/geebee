#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <SDL2/SDL.h>

#include "program.h"
#include "sdl.h"
#include "window.h"

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
  std::cout << "rom size: " << program.rom().size() << std::endl;
  std::cout << "bootrom size: " << program.bootrom().size() << std::endl;

  gb::Window window;
  SDL_Delay(1000);

  return 0;
}
