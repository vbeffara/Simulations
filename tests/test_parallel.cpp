#include <vb/Stream.h>
#include <vb/util.h>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#endif
#include <numeric>

#ifdef __cpp_coroutines
#include <experimental/coroutine>

namespace vb {
	template <typename T> class generator;

	namespace detail {
		template<typename T> class generator_promise { public:
			using value_type = std::remove_reference_t<T>;
			using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
			using pointer_type = value_type*;

			generator_promise() = default;

			generator<T> get_return_object() noexcept;

			constexpr std::experimental::suspend_always initial_suspend() const { return {}; }
			constexpr std::experimental::suspend_always final_suspend() const { return {}; }

			std::experimental::suspend_always yield_value (T& value) noexcept { m_value = std::addressof(value); return {}; }
			std::experimental::suspend_always yield_value (T&& value) noexcept { m_value = std::addressof(value); return {}; }

			void unhandled_exception() { std::rethrow_exception(std::current_exception()); }

			void return_void() { }

			reference_type value() const noexcept { return *m_value; }

		private:
			pointer_type m_value;
		};

		template<typename T> class generator_iterator {
			using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;
		public:
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::size_t;
			using value_type = std::remove_reference_t<T>;
			using reference = value_type&;
			using pointer = value_type*;

			explicit generator_iterator (std::nullptr_t) noexcept : m_coroutine(nullptr) {}
			explicit generator_iterator (coroutine_handle coroutine) noexcept : m_coroutine(coroutine) {}

			bool operator== (const generator_iterator& other) const noexcept { return m_coroutine == other.m_coroutine; }
			bool operator!= (const generator_iterator& other) const noexcept { return !(*this == other); }

			generator_iterator& operator++() {
				m_coroutine.resume();
				if (m_coroutine.done()) m_coroutine = nullptr;
				return *this;
			}

			generator_iterator operator++(int) = delete;

			reference operator*() const noexcept { return m_coroutine.promise().value(); }
			pointer operator->() const noexcept { return std::addressof(operator*()); }
		private:
			coroutine_handle m_coroutine;
		};
	}

	template<typename T> class generator { public:
		using promise_type = detail::generator_promise<T>;
		using iterator = detail::generator_iterator<T>;

		generator ()                  noexcept : m_coroutine(nullptr)           {}
		generator (generator&& other) noexcept : m_coroutine(other.m_coroutine) { other.m_coroutine = nullptr; }
		generator (const generator&) = delete;

		~generator() { if (m_coroutine) { m_coroutine.destroy(); } }

		generator& operator= (generator other) noexcept { swap(other); return *this; }
		generator& operator= (generator && other) noexcept { swap(other); return *this; }

		iterator begin() {
			if (m_coroutine) {
				m_coroutine.resume();
				if (!m_coroutine.done()) { return iterator{ m_coroutine }; }
			}
			return iterator{ nullptr };
		}

		iterator end() noexcept { return iterator{ nullptr }; }

		void swap (generator& other) noexcept { std::swap(m_coroutine, other.m_coroutine); }

	private:
		friend class detail::generator_promise<T>;
		explicit generator (std::experimental::coroutine_handle<promise_type> coroutine) noexcept : m_coroutine(coroutine) {}
		std::experimental::coroutine_handle<promise_type> m_coroutine;
	};

	template<typename T> void swap(generator<T>& a, generator<T>& b) { a.swap(b); }

	namespace detail {
		template<typename T> generator<T> generator_promise<T>::get_return_object() noexcept {
			using coroutine_handle = std::experimental::coroutine_handle<generator_promise<T>>;
			return generator<T>{ coroutine_handle::from_promise(*this) };
		}
	}
}
#endif

using namespace vb; using namespace std;

int fib (int n) { return n<2 ? n : fib(n-1) + fib(n-2); }

double cost (double x) { for (int i=0; i<10; ++i) x = cos(x); return x; }

double cum (int n) {
    vector<double> X(n); for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum2 (int n) { double s=0; for (int i=0; i<n; ++i) s += cost(i); return s - long(s); }

double cum3 (int n) {
    vector<double> X(n);
    std::iota (X.begin(), X.end(), 0);
    std::transform (X.begin(), X.end(), X.begin(), cost);
    double s = std::accumulate (X.begin(), X.end(), 0.0);
    return s - long(s);
}

double cum4 (int n) {
    auto costs = take (n, fmap (cost, ints()));
    double s=0; for (auto x : costs) s+=x; return s - long(s);
}

#ifdef __cpp_coroutines
template <typename T> generator<T> take (int n, generator<T> g) { for (auto x : g) { if (!n) break; --n; co_yield x; } }
template <typename F, typename T> auto fmap (F f, generator<T> g) -> generator<decltype(f(T()))> { for (auto x : g) co_yield f(x); }

generator <int> new_ints () { int i=0; while (true) { co_yield i++; } }

double cum5 (int n) { double s=0; for (auto x : take(n,fmap(cost,new_ints()))) s += x; return s - long(s); }
#endif

#ifdef CILK
int fib_cilk (int n) {
    if (n < 20) return fib(n);
    int x = cilk_spawn fib_cilk (n-1);
    int y = fib_cilk (n-2);
    cilk_sync;
    return x + y;
}

double cum_cilk (int n) {
    vector<double> X(n);
    cilk_for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_cilk2 (int n) {
    cilk::reducer_opadd <double> s (0);
    cilk_for (int i=0; i<n; ++i) *s += cost(i);
    return s.get_value() - long(s.get_value());
}
#endif

#ifdef OPENMP
int fib_omp (int n) {
    if (n < 30) return fib(n);
    int x, y;
    #pragma omp parallel sections
    {
        x = fib_omp (n-1);
        #pragma omp section
        y = fib_omp (n-2);
    }
    return x + y;
}

double cum_omp (int n) {
    vector<double> X(n);
    #pragma omp parallel for
    for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_omp2 (int n) {
    double s=0;
    #pragma omp parallel for reduction(+:s)
    for (int i=0; i<n; ++i) s += cost(i);
    return s - long(s);
}
#endif

int main (int argc, char ** argv) {
    H.init ("Test of various parallel frameworks",argc,argv,"n=42,l=10000000");

    timing ("Fibonacci  | Single (recursive)", []{ return fib(H['n']); });
#ifdef CILK
    timing ("Fibonacci  | CILK", []{ return fib_cilk(H['n']); });
#endif
#ifdef OPENMP
    timing ("Fibonacci  | OpenMP (parallel sections)", []{ return fib_omp(H['n']); });
#endif

    timing ("Map+reduce | Single 1 (fill then sum)", []{ return cum(H['l']); });
    timing ("Map+reduce | Single 2 (direct sum)", []{ return cum2(H['l']); });
    timing ("Map+reduce | Single 3 (STL algorithms)", []{ return cum3(H['l']); });
    timing ("Map+reduce | Single 4 (Boost coroutine)", []{ return cum4(H['l']); });
#ifdef __cpp_coroutines
    timing ("Map+reduce | Single 5 (native coroutine)", []{ return cum5(H['l']); });
#endif
#ifdef CILK
    timing ("Map+reduce | CILK", []{ return cum_cilk(H['l']); });
    timing ("Map+reduce | CILK 2", []{ return cum_cilk2(H['l']); });
#endif
#ifdef OPENMP
    timing ("Map+reduce | OpenMP 1 (fill then sum)", []{ return cum_omp(H['l']); });
    timing ("Map+reduce | OpenMP 2 (direct reduction)", []{ return cum_omp2(H['l']); });
#endif
}
