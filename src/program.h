#ifndef GEEBEE_PROGRAM_H_
#define GEEBEE_PROGRAM_H_

#include <string>
#include <vector>

namespace gb {

class Program {
 public:
  Program(const std::string& rom_filename,
          const std::string& bootrom_filename = "");
  ~Program();

  const std::string& title() const { return title_; }
  bool is_valid() const { return !rom_.empty(); }

  const std::vector<char>& rom() const { return rom_; }
  const std::vector<char>& bootrom() const { return bootrom_; }

 private:
  std::vector<char> rom_;
  std::vector<char> bootrom_;

  std::string title_;
};
}

#endif
