#ifndef GEEBEE_SRC_IOHANDLER_H
#define GEEBEE_SRC_IOHANDLER_H

#include "types.h"

namespace gb {
class IOHandler {
 public:
  IOHandler() = default;
  virtual ~IOHandler() = default;

  virtual bool handlesAddress(Word address) const = 0;
  virtual Byte read(Word address) = 0;
  virtual void write(Word address, Byte byte) = 0;
};
}  // namespace gb

#endif
