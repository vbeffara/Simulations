#include <vb/cpx.h>
#include <vb/util.h>
#include <vector>

#if __has_include(<variant>)
	#include <variant>
	template <typename... Ts> std::ostream & operator<< (std::ostream &os, const std::variant<Ts...> & e) {
		visit ([&](const auto &x) { os << x; }, e); return os;
	}
#else
	#pragma message("Inserting boost::variant into std")
	#define FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
	#include <boost/variant.hpp>
	namespace std {
		template <typename... Ts> using variant = ::boost::variant<Ts...>;
		template <typename... Ts> decltype(auto) visit(Ts&&... xs) { return ::boost::apply_visitor(FWD(xs)...); }
		template <typename T, typename... Ts>       T* get_if (      variant<Ts...> *pv) { return ::boost::get<T>(pv); }
		template <typename T, typename... Ts> const T* get_if (const variant<Ts...> *pv) { return ::boost::get<T>(pv); }
	}
#endif

using namespace vb; using namespace std;

template <typename U, typename V> U convert (const V &v) { if (auto p = get_if<U> (&v)) return *p; else return U{v}; }

struct Expression;
struct Number : public real_t { using real_t::real_t; };
struct Symbol : public string { using string::string; };
struct Plus : public vector<Expression> { using vector<Expression>::vector; };

using Expression_ = variant <Number,Symbol,Plus>;
struct Expression : public Expression_ { using Expression_::Expression_; };

bool operator== (const Expression &e1, const Expression &e2) {
	return static_cast <const Expression_> (e1) == static_cast <const Expression_> (e2);
}

bool operator< (const Expression &e1, const Expression &e2) {
	return static_cast <const Expression_> (e1) < static_cast <const Expression_> (e2);
}

Expression operator"" _e (const char *s, unsigned long) { return Symbol(s); }
Expression operator"" _e (unsigned long long x) { return Number(x); }

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
			if (auto n = get_if<Number>(&e)) acc += *n;
			else out.push_back(visit(*this,e));
		if (acc != 0) out.push_back(acc);
		sort (out.begin(), out.end());
		return out;
	};
};

struct recurser {
	template <typename V> recurser (V v) : v(std::move(v)) {}
	const function<Expression(const Expression)> v;
	Expression operator() (const Number &n) const { return v(n); }
	Expression operator() (const Symbol &s) const { return v(s); }
	Expression operator() (const Plus &p) const {
		Plus out;
		for (const auto &e : p) out.push_back(visit(*this,e));
		return v(out);
	}
};

ostream & operator<< (ostream &os, const Plus &p) { return vprint (os,p,"+"); }

Expression flatten (const Expression &e) { return visit (flattener(), e); }
Expression normalize (const Expression &e) { return visit (normalizer(), e); }
Expression recurse (function<Expression(const Expression)> f, const Expression &e) { return visit(recurser(std::move(f)),e); }
Expression replace (const Expression &from, const Expression &to, const Expression &e) {
	return recurse ([&](const Expression &ee){ return ee==from ? to : ee; }, e);
}

Expression operator+ (const Expression &e1, const Expression &e2) {
	if (auto p = get_if<Plus>(&e1)) { Plus out = *p; out.push_back(e2); return out; }
	else return Plus {e1,e2};
}

int main (int argc, char ** argv) {
	H.init ("Variants", argc, argv, "");
	Expression e = 1 + "x"_e + (2 + ("y"_e + "z"_e));
	H.L->info ("Initial expression | {}", e);
	H.L->info (" -> Replaced       | {}", e=replace("y"_e + "z"_e, "t"_e + 4, e));
	H.L->info (" -> Flattened      | {}", e=flatten(e));
	H.L->info (" -> Normalized     | {}", e=normalize(e));
}
