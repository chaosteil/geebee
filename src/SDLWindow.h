#ifndef GEEBEE_SRC_SDLWINDOW_H
#define GEEBEE_SRC_SDLWINDOW_H

#include <memory>
#include <string>

#include <SDL_rect.h>

#include "Window.h"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Window;

namespace gb {
class SDLWindow : public Window {
 public:
  explicit SDLWindow(const std::string& title = "GeeBee");
  virtual ~SDLWindow() = default;

  virtual void setPixel(int x, int y, int color) override;

  bool handleEvents();
  void draw();

 private:
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;
  std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> texture_;
  std::shared_ptr<SDL_Surface> surface_;

  SDL_Rect position_;
};
}

#endif
