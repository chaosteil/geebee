#ifndef GEEBEE_SRC_PROGRAM_H
#define GEEBEE_SRC_PROGRAM_H

#include <string>

#include "types.h"

namespace gb {

class Program {
 public:
  explicit Program(const std::string& rom_filename,
                   const std::string& bootrom_filename = "");
  ~Program() = default;

  const std::string& title() const { return title_; }
  int type() const { return type_; }
  int rom_size() const { return rom_size_; }
  int ram_size() const { return ram_size_; }

  bool is_valid() const { return !rom_.empty(); }

  const Bytes& rom() const { return rom_; }
  const Bytes& bootrom() const { return bootrom_; }

 private:
  Bytes rom_;
  Bytes bootrom_;

  std::string title_;
  int type_{0};
  int rom_size_{0};
  int ram_size_{0};
};

}  // namespace gb

#endif
