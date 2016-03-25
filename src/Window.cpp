#include "Window.h"

#include <iostream>

#include <SDL.h>

namespace gb {

Window::Window(const std::string& title)
    : window_(SDL_CreateWindow(title.c_str(), 0, 0, 160, 144, SDL_WINDOW_SHOWN),
              [](SDL_Window* win) { SDL_DestroyWindow(win); }),
      renderer_(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_SOFTWARE),
                [](SDL_Renderer* ren) { SDL_DestroyRenderer(ren); }),
      texture_(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, 160, 144),
               [](SDL_Texture* texture) { SDL_DestroyTexture(texture); }),
      surface_(SDL_CreateRGBSurface(0, 160, 144, 32, 0, 0, 0, 0),
               [](SDL_Surface* surface) { SDL_FreeSurface(surface); }),
      position_{0, 0, 160, 144} {
  SDL_FillRect(surface_.get(), NULL,
               SDL_MapRGBA(surface_->format, 255, 255, 255, 255));
}

void Window::setPixel(int x, int y, int color) {
  auto format = surface_->format;
  auto pixels = reinterpret_cast<uint32_t*>(surface_->pixels);
  pixels[y * 160 + x] = SDL_MapRGBA(format, color, color, color, 255);
}

bool Window::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return true;
    }
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        return true;
      }
    }
  }

  return false;
}

void Window::draw() {
  SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);
  if (SDL_RenderClear(renderer_.get())) {
    return;
  }
  if (SDL_RenderCopy(renderer_.get(), texture_.get(), &position_, NULL)) {
    return;
  }
  SDL_RenderPresent(renderer_.get());
}

}  // namespace gb
