#include "SDL.h"

#include <SDL2/SDL.h>

namespace gb {

SDL::SDL() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER |
           SDL_INIT_EVENTS);
}

SDL::~SDL() { SDL_Quit(); }
}
