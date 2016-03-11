#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, const char** argv) {
  cout << "GeeBee 0.1" << endl;
  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "Show the help message")(
      "file,f", po::value<string>(), "The .gb file to read");

  po::positional_options_description pos_desc;
  pos_desc.add("file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_desc)
                .run(),
            vm);
  po::notify(vm);

  if (vm.find("help") != vm.end()) {
    cout << desc << endl;
    return 1;
  }

  SDL_Init(SDL_INIT_VIDEO);
  {
    auto window_deleter = [](SDL_Window* win) { SDL_DestroyWindow(win); };
    auto window = unique_ptr<SDL_Window, decltype(window_deleter)>(SDL_CreateWindow("GeeBee", 0, 0, 160, 144, SDL_WINDOW_SHOWN), window_deleter);
    SDL_Delay(1000);
  }

  SDL_Quit();

  return 0;
}
