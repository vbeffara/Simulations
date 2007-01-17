
#include <vb/CL_Parser.h>

#include <iostream>
#include <sstream>

namespace vb {

  CL_Parser::CL_Parser (int argc, char **argv, std::string syntax,
                                               std::string help) :
    getopt_arg("h"), _help(help) {

    if (help.length()==0) {
      std::ostringstream os;
      os << "CL_Parser argument : " << syntax;
      _help = os.str();
    }

    int cut;
    while ((cut=syntax.find_first_of(",")) != (int) syntax.npos) {
      this->newparam (syntax.substr(0,cut));
      syntax = syntax.substr(cut+1);
    }
    if (syntax.length())
      newparam (syntax);
    
    this->parse(argc,argv);
  }
  
  void CL_Parser::newparam (std::string s) {
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
    char ch;
    
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
  
  int CL_Parser::as_int (char c) {
    return atoi(params[c].c_str());
  }
  
  double CL_Parser::as_double (char c) {
    return atof(params[c].c_str());
  }
  
  bool CL_Parser::as_bool (char c) {
    return atoi(params[c].c_str());
  }
  
  std::string CL_Parser::as_string (char c) {
    return params[c];
  }
}
