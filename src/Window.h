#ifndef __GEEBEE_SRC_WINDOW_H
#define __GEEBEE_SRC_WINDOW_H

#include <memory>
#include <string>

#include <SDL2/SDL_rect.h>

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Window;

namespace gb {

class Window {
 public:
  explicit Window(const std::string& title = "GeeBee");
  ~Window() = default;

  std::weak_ptr<SDL_Surface> surface() const { return surface_; }

  bool handleEvents();
  void draw();

 private:
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
  std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;
  std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> texture_;
  std::shared_ptr<SDL_Surface> surface_;

  SDL_Rect position_;
};

}  // namespace gb

#endif
