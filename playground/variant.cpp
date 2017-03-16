#include <vb/Hub.h>
#include <iostream>
#include <string>
#include <vector>

#if __has_include(<variant>)
#include <variant>
#else
	#pragma message("Inserting boost::variant into std")
	#define FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
	#include <boost/variant.hpp>
	namespace std {
	    template <typename... Ts> using variant = ::boost::variant<Ts...>;
	    template <typename... Ts> decltype(auto) visit(Ts&&... xs) { return ::boost::apply_visitor(FWD(xs)...); }
	}
#endif

using namespace vb; using namespace std;

class Expression;
ostream & operator<< (ostream & os, const Expression & e);

struct Number { int value; };
struct Symbol { string name; };
struct Plus : public vector<Expression> { using vector<Expression>::vector; };

using Expression_ = variant <Number,Symbol,Plus>;
class Expression : public Expression_ { public:
	using Expression_::Expression_;

	struct printer {
		printer (ostream & os) : os(os) {}
		ostream & os;
		void operator() (const Number &n) const { os << n.value; }
		void operator() (const Symbol &s) const { os << s.name; }
		void operator() (const Plus &p) const {
			string sep = "";
			os << "("; for (const auto &x : p) os << exchange(sep,"+") << x; os << ")";
		}
	};

	template <typename U> U convert () const { return visit ([](const auto &e){ return U{e}; }, *this); }

	struct flattener {
		template <typename T> Expression operator() (const T &e) const { return e; }
		Expression operator() (const Plus &p) const {
			Plus sum;
			for (const auto &e : p) for (const auto &ee : e.flatten().convert<Plus>()) sum.push_back(ee);
			return sum;
		}
	};
	Expression flatten() const { return visit (flattener(), *this); }
};

ostream & operator<< (ostream & os, const Expression & e) { visit(Expression::printer(os),e); return os; }

int main (int argc, char ** argv) {
	H.init ("Variants", argc, argv, "");
	Expression e = Plus { Number{1}, Symbol{"x"}, Plus { Number{2}, Symbol{"y"}, Plus { Symbol{"z"} } } };
	cout << e << "\n";
	cout << e.flatten() << "\n";
}
