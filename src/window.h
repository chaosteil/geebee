#ifndef GEEBEE_WINDOW_H_
#define GEEBEE_WINDOW_H_

#include <memory>
#include <string>

struct SDL_Window;

namespace gb {

class Window {
 public:
  Window(const std::string& title = "GeeBee");
  ~Window();

 private:
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
};
}

#endif
