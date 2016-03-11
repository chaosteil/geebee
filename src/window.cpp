#include "window.h"

#include <SDL2/SDL.h>

namespace gb {

Window::Window(const std::string& title)
    : window_(SDL_CreateWindow(title.c_str(), 0, 0, 160, 144, SDL_WINDOW_SHOWN),
              [](SDL_Window* win) { SDL_DestroyWindow(win); }) {}

Window::~Window() {}
}
