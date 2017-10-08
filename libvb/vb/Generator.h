#pragma once

#ifdef __cpp_coroutines
#include <experimental/coroutine>

namespace vb {
	template <typename T> class Generator;

	namespace detail {
		template<typename T> class Generator_promise { public:
			using value_type = std::remove_reference_t<T>;
			using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
			using pointer_type = value_type*;

			Generator_promise() = default;

			Generator<T> get_return_object() noexcept;

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

		template<typename T> class Generator_iterator {
			using coroutine_handle = std::experimental::coroutine_handle<Generator_promise<T>>;
		public:
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::size_t;
			using value_type = std::remove_reference_t<T>;
			using reference = value_type&;
			using pointer = value_type*;

			explicit Generator_iterator (std::nullptr_t) noexcept : m_coroutine(nullptr) {}
			explicit Generator_iterator (coroutine_handle coroutine) noexcept : m_coroutine(coroutine) {}

			bool operator== (const Generator_iterator& other) const noexcept { return m_coroutine == other.m_coroutine; }
			bool operator!= (const Generator_iterator& other) const noexcept { return !(*this == other); }

			Generator_iterator& operator++() {
				m_coroutine.resume();
				if (m_coroutine.done()) m_coroutine = nullptr;
				return *this;
			}

			Generator_iterator operator++(int) = delete;

			reference operator*() const noexcept { return m_coroutine.promise().value(); }
			pointer operator->() const noexcept { return std::addressof(operator*()); }
		private:
			coroutine_handle m_coroutine;
		};
	}

	template<typename T> class Generator { public:
		using promise_type = detail::Generator_promise<T>;
		using iterator = detail::Generator_iterator<T>;

		Generator ()                  noexcept : m_coroutine(nullptr)           {}
		Generator (Generator&& other) noexcept : m_coroutine(other.m_coroutine) { other.m_coroutine = nullptr; }
		Generator (const Generator&) = delete;

		~Generator() { if (m_coroutine) { m_coroutine.destroy(); } }

		Generator& operator= (Generator other) noexcept { swap(other); return *this; }
		Generator& operator= (Generator && other) noexcept { swap(other); return *this; }

		iterator begin() {
			if (m_coroutine) {
				m_coroutine.resume();
				if (!m_coroutine.done()) { return iterator{ m_coroutine }; }
			}
			return iterator{ nullptr };
		}

		iterator end() noexcept { return iterator{ nullptr }; }

		void swap (Generator& other) noexcept { std::swap(m_coroutine, other.m_coroutine); }

	private:
		friend class detail::Generator_promise<T>;
		explicit Generator (std::experimental::coroutine_handle<promise_type> coroutine) noexcept : m_coroutine(coroutine) {}
		std::experimental::coroutine_handle<promise_type> m_coroutine;
	};

	template<typename T> void swap(Generator<T>& a, Generator<T>& b) { a.swap(b); }

	namespace detail {
		template<typename T> Generator<T> Generator_promise<T>::get_return_object() noexcept {
			using coroutine_handle = std::experimental::coroutine_handle<Generator_promise<T>>;
			return Generator<T>{ coroutine_handle::from_promise(*this) };
		}
	}

    template <typename T> Generator<T> take (int n, Generator<T> g) {
        for (auto x : g) { if (n<=0) break; --n; co_yield x; }
    }

    template <typename F, typename T> auto fmap (F f, Generator<T> g) -> Generator<decltype(f(T()))> {
        for (auto x : g) co_yield f(x);
    }

    Generator <int> new_ints () { int i=0; while (true) { co_yield i++; } }
}
#endif
