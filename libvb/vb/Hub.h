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
		explicit Value (std::string s = "") : std::string (std::move(s)) {}
		Value & operator= (const std::string & s) { std::string::operator=(s); return *this; }

		operator bool()        const { return strtol(c_str(), (char **) nullptr, 10); } // NOLINT implicit conversion is the point
		operator int()         const { return strtol(c_str(), (char **) nullptr, 10); } // NOLINT implicit conversion is the point
		operator unsigned()    const { return strtol(c_str(), (char **) nullptr, 10); } // NOLINT implicit conversion is the point
		operator int64_t()     const { return strtol(c_str(), (char **) nullptr, 10); } // NOLINT implicit conversion is the point
		operator double()  	   const { return strtod(c_str(), (char **) nullptr); }     // NOLINT implicit conversion is the point
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

		const Value & operator[] (char c) const { return find(c)->second; }

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
