#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
	template <class T> class Automaton;

	template <class T> class Rule { public:
		typedef std::vector < std::pair<int,T> > Pattern;

		Rule (Pattern in, Pattern out) : cond(in), move(out) {}

		bool check   (Automaton<T> &A, int i) { for (auto & c : cond) if (A.atp(i+c.first) != c.second) return false; return true; }
		void trigger (Automaton<T> &A, int i) { if (check(A,i)) for (auto & m : move) A.atp(i+m.first) = m.second; }

	private:
		Pattern cond, move;
	};

	template <class T> class Automaton : public std::vector<T> { public:
		Automaton (int n_, T s, std::string title, bool p = true) : std::vector<T>(n_,s), n(n_), y(0), lt(0), pause(p), I(n,500,title) {}

		T & atp (int i) { int j = i%n; if (j<0) j+=n; return (*this)[j]; }

		void show () { I.show(); }

		void step () {
			double U = prng.uniform_real(0,lt); int j=0; while (U>rates[j]) U -= rates[j++];
			rules[j].trigger (*this, prng.uniform_int(n));
		}

		void swipe () {
			for (int i=0; i<n*lt; ++i) step();
			for (int i=0; i<n; ++i) I.put (coo(i,y), (*this)[i]);
			++y; if (y==500) { y=0; if (pause) I.pause(); }
		}

		void add_rule (double r, Rule<T> R) { lt += r; rates.push_back(r); rules.push_back(R); }

	private:
		std::vector<double> rates;
		std::vector<Rule<T>> rules;
		int n,y;
		double lt;
		bool pause;
		Bitmap<T> I;
	};
}
