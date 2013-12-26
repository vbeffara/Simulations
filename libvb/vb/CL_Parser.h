#pragma once /// \file
#include <string>
#include <map>

namespace vb {
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

	class CL_Parser {
	public:
		std::map<char,std::string> params;  ///< The current values of the parameters
		std::map<char,bool>        has_arg; ///< The list of known flags

		CL_Parser (int argc, char** argv, std::string syntax);

		CL_Value operator() (char);

	private:
		std::string getopt_arg;
		std::string _help;

		void newparam (const std::string &);
		void parse (int, char**);
	};
}
