#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
	template <class T> class Rule {
	public:
		typedef std::vector < std::pair<int,T> > Pattern;

		Rule (Pattern in, Pattern out) : cond(in), move(out) {}
		Pattern cond, move;
	};

	template <class T> class Automaton : public std::vector<T> { public:
		Automaton (int n_, T s, std::string title, bool p = true) : std::vector<T>(n_,s), n(n_), y(0), lt(0), pause(p), I(n,500,title) {}

		T      	& atp (int i)      	{ return std::vector<T>::at((i%n+n)%n); }
		T const	& atp (int i) const	{ return std::vector<T>::at((i%n+n)%n); }

		void show () { I.show(); }

		void step () {
			int i = prng.uniform_int(n); double U = prng.uniform_real(0,lt); int j=0;
			while (U>rates[j]) U -= rates[j++];
			for (auto & c : rules[j].cond) if (atp(i+c.first) != c.second) return;
			for (auto & m : rules[j].move) atp(i+m.first) = m.second;
		}

		void swipe () {
			for (int i=0; i<n*lt; ++i) step();
			for (int i=0; i<n; ++i) I.put (coo(i,y), (*this)[i]);
			++y; if (y==500) { y=0; if (pause) I.pause(); }
		}

		void add_rule (double r, typename Rule<T>::Pattern in, typename Rule<T>::Pattern out) {
			lt += r; rates.push_back(r); rules.push_back (Rule<T> (in,out));
		}

	private:
		std::vector<double> rates;
		std::vector<Rule<T>> rules;
		int n,y;
		double lt;
		bool pause;
		Bitmap<T> I;
	};
}
