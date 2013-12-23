#pragma once /// @file
#include <string>
#include <boost/format.hpp>

namespace vb {
	class fmt : public boost::format {
	public:
		fmt (const std::string &s) : boost::format(s) { }
		operator std::string() const { return boost::format::str(); }
	};
};
