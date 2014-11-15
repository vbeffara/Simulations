#pragma once
#include <vb/cpx.h>
#include <iostream>
#include <vector>

namespace vb {
	template <typename T> class Polynomial : public std::vector<T> {
	public:
		Polynomial (const std::vector<T> V = {{T(1)}}) : std::vector<T> (V) {};

		using std::vector<T>::at; using std::vector<T>::size; using std::vector<T>::back; using std::vector<T>::push_back;

		template <typename U> U operator() (U z) const;

		unsigned     	degree    	() const;
		Polynomial<T>	derivative	() const;
		void         	add_root  	(T l);

		std::string v = "z";
	};

	template <typename T> Polynomial<T> operator* (const T & c, const Polynomial<T> & Q);

	template <typename T> Polynomial<T> operator- (const Polynomial<T> & P, const Polynomial<T> & Q);
	template <typename T> Polynomial<T> operator* (const Polynomial<T> & P, const Polynomial<T> & Q);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Polynomial<T> & P);
}
