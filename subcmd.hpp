/*

Copyright (c) 2014 Yuya Unno.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SUBCMD_HPP_5642C751_7D39_481F_8F21_18493526E02C_
#define SUBCMD_HPP_5642C751_7D39_481F_8F21_18493526E02C_

#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include "./cmdline.h"

namespace subcmd {

class parser {
 public:
  parser()
      : parser_(NULL) {
  }

  ~parser() {
    for (command_map_type::const_iterator it = commands_.begin();
         it != commands_.end(); ++it) {
      delete it->second.parser;
    }
  }

  cmdline::parser& add_subcmd(
      const std::string& cmd,
      const std::string& help) {
    if (commands_.count(cmd) > 0) {
      throw std::runtime_error("already in use: " + cmd);
    }

    // TODO(unno): use smart pointer
    cmdline::parser* p = new cmdline::parser;
    try {
      sub_command subcmd = {help, p};
      commands_.insert(std::make_pair(cmd, subcmd));
      return *p;
    } catch(...) {
      delete p;
      throw;
    }
  }

  void set_program_name(const std::string &name) {
    prog_name_ = name;
  }

  bool parse(int argc, const char * const argv[]) {
    command_.clear();
    errors_.clear();

    if (prog_name_ == "") {
      prog_name_ = argv[0];
    }

    if (argc < 2) {
      errors_.push_back("sub-command name is required");
      return false;
    }

    command_map_type::iterator it = commands_.find(argv[1]);
    if (it == commands_.end()) {
      errors_.push_back(std::string("Unknown sub-command: ") + argv[1]);
      return false;
    }

    command_ = argv[1];
    parser_ = it->second.parser;
    parser_->set_program_name(prog_name_ + " " + it->first);
    return parser_->parse(argc - 1, argv + 1);
  }

  void parse_check(int argc, char *argv[]) {
    check(argc, parse(argc, argv));
  }

  std::string error() const {
    if (parser_) {
      return parser_->error();
    }
    return errors_.size() > 0 ? errors_[0] : "";
  }

  std::string error_full() const {
    if (parser_) {
      return parser_->error_full();
    }

    std::ostringstream oss;
    for (std::size_t i = 0; i < errors_.size(); i++) {
      oss << errors_[i] << std::endl;
    }
    return oss.str();
  }

  std::string usage() const {
    if (parser_) {
      return parser_->usage();
    }

    std::ostringstream oss;
    oss << "usage: " << prog_name_ << " COMMAND [OPTIONS]" << std::endl
        << std::endl
        << "Commands:" << std::endl;

    std::size_t longest = 0;
    for (command_map_type::const_iterator it = commands_.begin();
         it != commands_.end(); ++it) {
      longest = std::max(longest, it->first.size());
    }

    for (command_map_type::const_iterator it = commands_.begin();
         it != commands_.end(); ++it) {
      oss << "  " << it->first;
      for (std::size_t i = it->first.size(); i < longest; ++i) {
        oss << ' ';
      }
      oss << "  " << it->second.help << std::endl;
    }
    return oss.str();
  }

  const std::string& get_command() const {
    return command_;
  }

  const cmdline::parser& get_parser() const {
    if (!parser_) {
      throw std::runtime_error("not parsed yet");
    }

    return *parser_;
  }

 private:
  void check(int argc, bool ok) {
    if (!ok) {
      std::cerr << error() << std::endl
                << usage();
      exit(1);
    }
  }

  struct sub_command {
    std::string help;
    cmdline::parser* parser;
  };

  typedef std::map<std::string, sub_command> command_map_type;

  std::string prog_name_;
  command_map_type commands_;
  std::vector<std::string> errors_;

  std::string command_;
  cmdline::parser* parser_;
};

}  // namespace subcmd

#endif  // SUBCMD_HPP_5642C751_7D39_481F_8F21_18493526E02C_
