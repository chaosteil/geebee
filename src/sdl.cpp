#include "sdl.h"

#include <SDL2/SDL.h>

namespace gb {

SDL::SDL() { SDL_Init(SDL_INIT_VIDEO); }

SDL::~SDL() { SDL_Quit(); }
}
