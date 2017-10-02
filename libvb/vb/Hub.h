#pragma once /// \file
#undef False
#include <boost/chrono.hpp>
#include <cstdlib>
#include <map>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>

namespace vb {
	using Duration = boost::chrono::duration<double>;
	using TimePoint = boost::chrono::time_point<boost::chrono::process_real_cpu_clock,Duration>;

	class Value : public std::string {
	public:
		Value (std::string s = "") : std::string (std::move(s)) {}
		Value & operator= (const std::string & s) { std::string::operator=(s); return *this; }

		operator bool()    	const { return atoi(c_str()); }
		operator int()     	const { return atoi(c_str()); }
		operator int64_t() 	const { return atoi(c_str()); }
		operator double()  	const { return atof(c_str()); }
	};

	class Hub : public std::map <char,Value> {
	public:
		Hub ();
		Hub (const Hub &) =delete;
		Hub (Hub &&) =delete;

		~Hub ();

		Hub & operator= (const Hub &) =delete;
		Hub & operator= (Hub &&) =delete;

		void init (std::string t, int argc, char ** argv, std::string c = "");

		std::string title="undefined", cmd="undefined", prog="undefined", version="undefined", dir="output/", help="undefined";
		std::map <char,bool> has_arg;

		std::shared_ptr <spdlog::logger> L;

		boost::chrono::time_point<boost::chrono::process_real_cpu_clock,Duration> start;
		boost::chrono::time_point<boost::chrono::process_user_cpu_clock,Duration> start_u;
		boost::chrono::time_point<boost::chrono::process_system_cpu_clock,Duration> start_s;
		bool initialized = false;
	};

	extern Hub H;
} // namespace vb
