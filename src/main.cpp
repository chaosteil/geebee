#include <boost/program_options.hpp>
#include <string>
#include <iostream>

using namespace std;
namespace po = boost::program_options;

int main(int argc, const char** argv) {
  cout << "GeeBee 0.1" << endl;
  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "Show the help message")(
      "file,f", po::value<string>(), "The .gb file to read");

  po::positional_options_description pos_desc;
  pos_desc.add("file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_desc)
                .run(),
            vm);
  po::notify(vm);

  if (vm.find("help") != vm.end()) {
    cout << desc << endl;
    return 1;
  }

  return 0;
}
