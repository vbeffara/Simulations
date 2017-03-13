#include <vb/Hub.h>
#include <utility>
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
	void meow (int n) const { H.L->info ("Cat | Meowing {} times", n); }
	void walk (int n) const { H.L->info ("Cat | Walking {} meters", n); }
};

class Dog { public:
	void bark (int n) const { H.L->info ("Dog | Barking {} times", n); }
	void walk (int n) const { H.L->info ("Dog | Walking {} meters", n); }
};

using Animal_ = variant <Cat,Dog>;

class Animal : public Animal_ { public:
	using Animal_::Animal_;

	struct printer {
		printer (int n) : n(n) {}
		void operator() (const Cat &c) const { c.meow(n); }
		void operator() (const Dog &c) const { c.bark(n); }
		int n;
	};
	void print (int n) { visit (printer(n), *this); }

	void walk (int n) { visit ([n](const auto &t){ t.walk(n); }, *this); }
};

int main (int argc, char ** argv) {
	H.init ("Variants", argc, argv, "");
	std::vector<Animal> zoo = { Dog{}, Cat{}, Dog{}, Dog{} };
	for (auto & a : zoo) a.print(5);
	for (auto & a : zoo) a.walk(10);
}
