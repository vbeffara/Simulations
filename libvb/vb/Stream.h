#pragma once
#include <boost/coroutine/all.hpp>

template <typename T> using Stream = boost::coroutines::pull_coroutine<T>;
template <typename T> using Sink = boost::coroutines::push_coroutine<T>;

template <typename T> auto filter (auto && f, Stream<T> & S) {
	return Stream<T> ([&](Sink<T> & yield) { for (auto x : S) if (f(x)) yield(x); });
}

template <typename U, typename V> auto fmap (auto f, Stream<U> & S) {
	return Stream<V> ([&](Sink<V> & yield) { for (auto x : S) yield(f(x)); });
}
