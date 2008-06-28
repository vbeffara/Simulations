/// @file CL_Parser.h
/// Definitions for the vb::CL_Parser class

#ifndef __VB_CL_PARSER_H
#define __VB_CL_PARSER_H

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace vb {

  /** A very useful and simple wrapper around getopt().
   *
   * Defines a syntax for the program, with default values. The string
   * passed to the constructor is a comma-separated list of argument
   * specifications. Each of these can have one of two forms (where 'a'
   * can of course be any letter, except 'h'):
   *
   * - "a" : then -a is a standard flag, 1 if present in the command
   *   line, 0 if not;
   * - "a=..." : then -a expects an argument, and ... gives the default
   * value for the parameter.
   *
   * The '-h' flag has a special meaning, it outputs the provided help
   * message and exits. If no help message is provided, it gives the
   * syntax string, which is better than nothing.
   *
   * The values of the parameters are stored as strings, and accessed
   * using the as_* methods.
   */

  class CL_Parser {
  public:
    std::map<char,std::string> params; ///< The current values of the parameters
    std::map<char,bool> has_arg;       ///< The list of known flags 
    
    /** The standard constructor of the vb::CL_Parser class.
     *
     * @param argc As passed to main().
     * @param argv As passed to main().
     * @param syntax A syntax specification (see class description).
     * @param help A help message for the '-h' flag.
     */

    CL_Parser (int argc, char** argv, std::string syntax,
               std::string help = "");
    
    /** Return the value of a parameter as an integer. */

    int as_int(char);

    /** Return the value of a parameter as a double. */

    double as_double(char);

    /** Return the value of a parameter as a boolean. */

    bool as_bool(char);

    /** Return the value of a parameter as a string. */

    std::string as_string(char);
  private:
    std::string getopt_arg;
    std::string _help;

    void newparam (std::string);
    void parse (int, char**);
  };
}

#endif
