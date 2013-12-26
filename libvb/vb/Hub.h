#pragma once /// \file

#include <cstdlib>
#include <map>
#include <string>

namespace vb {
	class Value : public std::string {
		public:
			Value (const std::string &s) : std::string (s) {}

			operator bool()  	const { return atoi(c_str()); }
			operator int()   	const { return atoi(c_str()); }
			operator long()  	const { return atoi(c_str()); }
			operator double()	const { return atof(c_str()); }
	};

	class Hub : public std::map <char,std::string> {
	public:
		Hub (std::string t, int argc, char ** argv, std::string c = "");

		std::string title,help;
		std::map<char,bool>        has_arg;

		Value operator() (char c) { return at(c); }

	private:
		void newparam (const std::string &);
		void parse (int, char**);

		std::string getopt_arg;
	};
}