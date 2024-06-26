#pragma once

#include <memory>

namespace plastic {

	template<class T>
	class vector {
		T* _begin;
		T* _last;
		T* _end;

	public:
		explicit vector(::std::size_t count = 0, const T& value = {}) noexcept {
			_begin = new T[count];
			_last = _end = _begin + count;
			::std::uninitialized_fill(_begin, _end, value);
		}

		template<::std::input_iterator iter>
		explicit vector(iter first, iter last) noexcept {
			::std::ptrdiff_t d{::std::distance(first, last)};
			_begin = new T[d];
			_last = _end = _begin + d;
			::std::uninitialized_copy(first, last, _begin);
		}

		~vector() noexcept {
			delete[] _begin;
		}

		bool empty() const noexcept {
			return _begin == _last;
		}

		void clear() noexcept {
			resize(0);
		}

		::std::size_t size() const noexcept {
			return _last - _begin;
		}

		void resize(::std::size_t size, const T& value = {}) noexcept {
			::std::size_t oldSize{this->size()};

			if (size == oldSize) {
				return;
			}

			if (size < oldSize) {
				T* newLast{_begin + size};
				::std::destroy(newLast, _last);
				_last = newLast;
				return;
			}

			if (size <= capacity()) {
				T* newLast{_begin + size};
				::std::uninitialized_fill(_last, newLast, value);
				_last = newLast;
				return;
			}

			reserve(size);
			::std::uninitialized_fill(_last, _end, value);
			_last = _end;
		}

		::std::size_t capacity() const noexcept {
			return _end - _begin;
		}

		void reserve(::std::size_t capacity) noexcept {
			if (capacity <= this->capacity()) {
				return;
			}

			T* newBegin{new T[capacity]};
			T* newLast{::std::uninitialized_move(_begin, _last, newBegin)};
			T* newEnd{newBegin + capacity};
			delete[] _begin;

			_begin = newBegin;
			_last = newLast;
			_end = newEnd;
		}

		T* begin() const noexcept {
			return _begin;
		}

		T* end() const noexcept {
			return _last;
		}

		T& operator[](::std::size_t index) const noexcept {
#ifdef PLASTIC_VERIFY
			if (index >= size()) {
				::std::abort();
			}
#endif
			return _begin[index];
		}

		T& front() const noexcept {
#ifdef PLASTIC_VERIFY
			if (empty()) {
				::std::abort();
			}
#endif
			return *_begin;
		}

		T& back() const noexcept {
#ifdef PLASTIC_VERIFY
			if (empty()) {
				::std::abort();
			}
#endif
			return _last[-1];
		}

		void push_back(const T& value) noexcept {
			if (_last == _end) {
				::std::size_t capacity{this->capacity()};
				reserve(capacity + (capacity <= 1 ? 1 : capacity >> 1));
			}
			*_last++ = value;
		}

		void pop_back() noexcept {
#ifdef PLASTIC_VERIFY
			if (empty()) {
				::std::abort();
			}
#endif
			::std::destroy_at(--_last);
		}

		T* insert(T* pos, const T& value, ::std::size_t count = 1) noexcept {
			if (count == 0) {
				return pos;
			}
			if (static_cast<::std::size_t>(_end - _last) < count) {
				::std::size_t capacity{this->capacity()};
				::std::ptrdiff_t offset{pos - _begin};
				reserve(capacity + ((capacity >> 1) < count ? count : capacity >> 1));
				pos = _begin + offset;
			}
			T* i{_last + count}, * j{_last};
			while (j != pos) {
				*--i = ::std::move(*--j);
			}
			while (i != pos) {
				*--i = value;
			}
			_last += count;
			return pos;
		}

		template<::std::input_iterator iter>
		T* insert(T* pos, iter first, iter last) noexcept {
			if (first == last) {
				return pos;
			}
			::std::ptrdiff_t d{::std::distance(first, last)};
			if (_end - _last < d) {
				::std::size_t capacity{this->capacity()};
				::std::ptrdiff_t offset{pos - _begin};
				reserve(capacity + ((capacity >> 1) < d ? d : capacity >> 1));
				pos = _begin + offset;
			}
			T* i{_last + d}, * j{_last};
			while (j != pos) {
				*--i = ::std::move(*--j);
			}
			while (i != pos) {
				*--i = static_cast<T>(*--last);
			}
			_last += d;
			return pos;
		}

		T* erase(T* pos) noexcept {
#ifdef PLASTIC_VERIFY
			if (pos == _last) {
				::std::abort();
			}
#endif
			T* i{pos}, * j{pos + 1};
			while (j != _last) {
				*i++ = ::std::move(*j++);
			}
			::std::destroy_at(i);
			_last = i;
			return pos;
		}

		T* erase(T* first, T* last) noexcept {
			if (first == last) {
				return first;
			}
			T* i{first}, * j{last};
			while (j != _last) {
				*i++ = ::std::move(*j++);
			}
			::std::destroy(i, _last);
			_last = i;
			return first;
		}

		::std::compare_three_way_result<T> operator<=>(const vector& container) const noexcept {
			T* i{_begin}, j{container._begin};
			while (i != _last && j != container._last) {
				::std::compare_three_way_result<T> cmp{*i++ <=> *j++};
				if (cmp != 0) {
					return cmp;
				}
			}
			return i == _last && j != container._last ? ::std::strong_ordering::less
				: i != _last && j == container._last ? ::std::strong_ordering::greater
				: ::std::strong_ordering::equal;
		}
	};

}