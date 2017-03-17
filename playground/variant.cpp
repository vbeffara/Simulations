#include <vb/Hub.h>
#include <iostream>
#include <sstream>
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

template <typename U, typename V> U convert (const V &v) { return visit ([](const auto &e){ return U{e}; }, v); }

class Expression;
struct Number { int value; };
struct Symbol { string name; };
struct Plus : public vector<Expression> { using vector<Expression>::vector; };

using Expression_ = variant <Number,Symbol,Plus>;
class Expression : public Expression_ { public:
	using Expression_::Expression_;
};

ostream & operator<< (ostream & os, const Expression & e);
bool operator< (const Expression &e1, const Expression &e2) {
	ostringstream os1, os2; os1 << e1; os2 << e2;
	return os1.str() < os2.str();
}

struct flattener {
	template <typename T> Expression operator() (const T &e) const { return e; }
	Expression operator() (const Plus &p) const {
		Plus sum;
		for (const auto &e : p) for (const auto &ee : convert<Plus>(visit(*this,e))) sum.push_back(ee);
		return sum;
	}
};

struct normalizer {
	template <typename T> Expression operator() (const T &e) const { return e; }
	Expression operator() (const Plus &p) const {
		Plus out; Number acc{0};
		for (const auto &e : p)
			if (auto n = get_if<Number>(&e)) acc.value += n->value;
			else out.push_back(visit(*this,e));
		sort (out.begin(), out.end());
		if (acc.value) out.push_back(acc);
		return out;
	};
};

struct printer {
	printer (ostream & os) : os(os) {}
	ostream & os;
	void operator() (const Number &n) const { os << n.value; }
	void operator() (const Symbol &s) const { os << s.name; }
	void operator() (const Plus &p) const {
		string sep = "";
		os << "("; for (const auto &x : p) { os << exchange(sep,"+"); visit(*this,x); } os << ")";
	}
};

Expression flatten (const Expression &e) { return visit (flattener(), e); }
Expression normalize (const Expression &e) { return visit (normalizer(), e); }

ostream & operator<< (ostream & os, const Expression & e) { visit(printer(os),e); return os; }

Expression operator+ (const Expression &e1, const Expression &e2) {
	if (auto p = get_if<Plus>(&e1)) { Plus out = *p; out.push_back(e2); return out; }
	else return Plus {e1,e2};
}

int main (int argc, char ** argv) {
	H.init ("Variants", argc, argv, "");
	Expression e = Number{1} + Symbol{"x"} + (Number{2} + (Symbol{"y"} + Symbol{"z"}));
	H.L->info ("Initial expression | {}", e);
	H.L->info (" -> Flattened      | {}", e=flatten(e));
	H.L->info (" -> Normalized     | {}", e=normalize(e));
}
