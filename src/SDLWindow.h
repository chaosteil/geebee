#ifndef GEEBEE_SRC_SDLWINDOW_H
#define GEEBEE_SRC_SDLWINDOW_H

#include <functional>
#include <memory>
#include <string>

#include <SDL_rect.h>

#include "Window.h"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Window;

namespace gb {

class Joypad;

class SDLWindow : public Window {
 public:
  explicit SDLWindow(const std::string& title = "GeeBee");
  ~SDLWindow() override = default;

  void setPixel(int x, int y, int color) override;

  bool handleEvents(Joypad& joypad);
  void draw();

 private:
  static const int frames_per_second_;
  static const int ticks_per_frame_;

  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;
  std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> texture_;
  std::shared_ptr<SDL_Surface> surface_;

  SDL_Rect position_;
  uint32_t timer_;
};
}  // namespace gb

#endif
