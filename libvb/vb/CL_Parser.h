/// @file
/// Definitions for the vb::CL_Parser class

#pragma once
#include <vb/common.h>

/** The namespace where libvb puts everything it defines. */

namespace vb {

  /** A helper class for CL_Parser, that holds an unparsed parameter.
   *
   * It can be casted to an int, a double or a string, and it will do
   * what needs to be done accordingly.
   */

  class CL_Value {
    public:
      CL_Value (const std::string &s); ///< Creator from a string.

      operator std::string() const; ///< Convert to a string.
      operator bool()        const; ///< Convert to a bool.
      operator int()         const; ///< Convert to an int.
      operator long()        const; ///< Convert to a long int.
      operator double()      const; ///< Convert to a double.

    private:
      std::string value;
  };

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
   * by casting the returned vb::CL_Value object to the wanted type.
   */

  class CL_Parser {
  public:
    std::map<char,std::string> params;  ///< The current values of the parameters
    std::map<char,bool>        has_arg; ///< The list of known flags

    /** The standard constructor of the vb::CL_Parser class.
     *
     * @param argc As passed to main().
     * @param argv As passed to main().
     * @param syntax A syntax specification (see class description).
     * @param help A help message for the '-h' flag.
     */

    CL_Parser (int argc, char** argv,
               std::string syntax,
               const std::string &help = "");

    /** Return a generic CL_Value holder. */

    CL_Value operator() (char);

  private:
    std::string getopt_arg;
    std::string _help;

    void newparam (const std::string &);
    void parse (int, char**);
  };
}
