#ifndef GEEBEE_SRC_WINDOW_H
#define GEEBEE_SRC_WINDOW_H

namespace gb {

class Window {
 public:
  Window() = default;
  virtual ~Window() = default;

  virtual void setPixel(int /*x*/, int /*y*/, int /*color*/) {}
};

}  // namespace gb

#endif
