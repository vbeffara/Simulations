#include <vb/Hub.h>
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

class Cat { public:
	void walk (int n, const string &p = "") const { H.L->info ("{}Cat named {} | Walking {} meters", p, name, n); }
	string name;
};

class Dog { public:
	void walk (int n, const string &p = "") const { H.L->info ("{}Dog | Walking {} meters", p, n); }
};

class Animal;
class Family : public vector<Animal> { public:
	using vector<Animal>::vector;
	void walk (int n, const string &p = "") const;
};

using Animal_ = variant <Cat,Dog,Family>;
class Animal : public Animal_ { public:
	using Animal_::Animal_;

	struct printer {
		printer (int n, string p) : n(n), p(move(p)) {}
		void operator() (const Cat &c)    const { H.L->info ("{}Cat named {} | Meowing {} times", p, c.name, n); }
		void operator() (const Dog &)     const { H.L->info ("{}Dog | Barking {} times", p, n); }
		void operator() (const Family &f) const { H.L->info ("{}Family talking together:", p); for (auto & i : f) i.print(n, p + "  "); }
		int n; string p;
	};

	void print (int n, string prefix = "") const { visit (printer(n,move(prefix)),                 *this); }
	void walk  (int n, string prefix = "") const { visit ([=](const auto &t){ t.walk(n,prefix); }, *this); }
};

void Family::walk (int n, const string &p) const {
	H.L->info ("{}Family walking {} meters together:", p, n);
	for (auto & i : *this) i.walk (n, p + "  ");
}

int main (int argc, char ** argv) {
	H.init ("Variants", argc, argv, "");
	Animal z = Family { Dog{}, Cat{"Sylvester"}, Dog{}, Dog{}, Family {Dog{}, Family {Dog{}}, Cat{"Garfield"}} };
	z.print(5); z.walk(10);
}
