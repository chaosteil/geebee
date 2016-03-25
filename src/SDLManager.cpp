#include "SDLManager.h"

#include <SDL.h>

namespace gb {

SDLManager::SDLManager() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER |
           SDL_INIT_EVENTS);
}

SDLManager::~SDLManager() { SDL_Quit(); }

}  // namespace gb
