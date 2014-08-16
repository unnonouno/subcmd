#include <string>

#include "subcmd.hpp"

int main(int argc, char* argv[]) {
  subcmd::parser parser;
  {
    cmdline::parser& p = parser.add_subcmd("saitama", "Go saitama");
    p.add<std::string>("name", 'n', "name");
  }
  {
    cmdline::parser& p = parser.add_subcmd("gumma", "Go gumma");
    p.add<std::string>("name", 'n', "name");
  }

  parser.parse_check(argc, argv);

  if (parser.get_command() == "saitama") {
    std::cout << parser.get_parser().get<std::string>("name") << std::endl;
  }
}
