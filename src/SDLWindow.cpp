#include "SDLWindow.h"

#include <iostream>
#include <functional>

#include <SDL.h>

#include "Joypad.h"

namespace gb {

const int SDLWindow::frames_per_second_ = 60;
const int SDLWindow::ticks_per_frame_ = 1000 / frames_per_second_;

SDLWindow::SDLWindow(const std::string& title)
    : Window(),
      window_(SDL_CreateWindow(title.c_str(), 0, 0, 160, 144, SDL_WINDOW_SHOWN),
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

void SDLWindow::setPixel(int x, int y, int color) {
  auto format = surface_->format;
  auto pixels = reinterpret_cast<uint32_t*>(surface_->pixels);
  pixels[y * 160 + x] = SDL_MapRGBA(format, color, color, color, 255);
}

bool SDLWindow::handleEvents(Joypad& joypad) {
  timer_ = SDL_GetTicks();
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

    if (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) {
      std::function<void (Joypad::Key)> key;
      if (e.type == SDL_KEYDOWN) {
        key = std::bind(&Joypad::press, &joypad, std::placeholders::_1);
      } else {  // (e.type == SDL_KEYUP)
        key = std::bind(&Joypad::release, &joypad, std::placeholders::_1);
      }

      switch (e.key.keysym.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
          key(Joypad::Key::Up);
          break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
          key(Joypad::Key::Left);
          break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
          key(Joypad::Key::Right);
          break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
          key(Joypad::Key::Down);
          break;
        case SDL_SCANCODE_RETURN:
          key(Joypad::Key::Start);
          break;
        case SDL_SCANCODE_SPACE:
          key(Joypad::Key::Select);
          break;
        case SDL_SCANCODE_N:
          key(Joypad::Key::B);
          break;
        case SDL_SCANCODE_M:
          key(Joypad::Key::A);
          break;
        default:
          break;
      }
    }
  }

  return false;
}

void SDLWindow::draw() {
  SDL_UpdateTexture(texture_.get(), NULL, surface_->pixels, surface_->pitch);
  if (SDL_RenderClear(renderer_.get())) {
    return;
  }
  if (SDL_RenderCopy(renderer_.get(), texture_.get(), &position_, NULL)) {
    return;
  }
  SDL_RenderPresent(renderer_.get());

  uint32_t ticks = SDL_GetTicks() - timer_;
  if (ticks < ticks_per_frame_) {
    SDL_Delay(ticks_per_frame_ - ticks);
  }
}

}  // namespace gb
