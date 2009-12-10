
/// @file CL_Parser.cpp
/// Implementation of the vb::CL_Parser class

#include <vb/CL_Parser.h>

namespace vb {
  CL_Value::CL_Value (const std::string &s) : value(s) { }

  CL_Value::operator std::string() const { return value; }
  CL_Value::operator bool()        const { return atoi(value.c_str()); }
  CL_Value::operator int()         const { return atoi(value.c_str()); }
  CL_Value::operator long()        const { return atoi(value.c_str()); }
  CL_Value::operator double()      const { return atof(value.c_str()); }

  CL_Parser::CL_Parser (int argc, char **argv, std::string syntax,
                                               const std::string &help) :
    getopt_arg("h"), _help(help) {

    if (help.length()==0) {
      std::ostringstream os;
      os << "CL_Parser argument : " << syntax;
      _help = os.str();
    }

    int cut;
    while ((cut=syntax.find_first_of(",")) != (int) syntax.npos) {
      newparam (syntax.substr(0,cut));
      syntax = syntax.substr(cut+1);
    }
    if (syntax.length())
      newparam (syntax);
    
    parse(argc,argv);
  }
  
  void CL_Parser::newparam (const std::string &s) {
    if (s.length() == 1) {
      getopt_arg.append(1,s[0]);
      params[s[0]] = "0";
      has_arg[s[0]] = false;
    } else {
      getopt_arg.append(1,s[0]).append(1,':');
      params[s[0]] = s.substr(2);
      has_arg[s[0]] = true;
    }
  }
  
  void CL_Parser::parse (int argc, char **argv) {
    int ch;
    
    while ((ch = getopt(argc,argv,getopt_arg.c_str())) != -1) {
      if (ch == 'h') {
        std::cerr << _help << std::endl;
        exit(0);
      } else if (has_arg[ch]) {
	params[ch] = optarg;
      } else {
	params[ch] = "1";
      }
    }
  }
  
  CL_Value CL_Parser::operator() (char c) {
    return CL_Value (params[c]);
  }
}
