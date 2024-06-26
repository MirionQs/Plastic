#pragma once

#include <iterator>

namespace plastic {

	template<class T>
	class forward_list {
		struct _node {
			T _value;
			_node* _next;
		};

		_node* _sentinel;

	public:
		class iterator {
			friend forward_list;

			_node* _ptr;

		public:
			using difference_type = ::std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = ::std::input_iterator_tag;

			explicit iterator(_node* node = nullptr) noexcept {
				_ptr = node;
			}

			reference operator*() const noexcept {
				return _ptr->_value;
			}

			pointer operator->() const noexcept {
				return &_ptr->_value;
			}

			bool operator==(iterator it) const noexcept {
				return _ptr == it._ptr;
			}

			iterator& operator++() noexcept {
				_ptr = _ptr->_next;
				return *this;
			}

			iterator operator++(int) noexcept {
				iterator temp{*this};
				++*this;
				return temp;
			}
		};

		explicit forward_list(::std::size_t count = 0, const T& value = {}) noexcept {
			_sentinel = new _node;
			_sentinel->_next = _sentinel;
			resize(count, value);
		}

		template<::std::input_iterator iter>
		explicit forward_list(iter first, iter last) noexcept {
			_sentinel = new _node;
			_sentinel->_next = _sentinel;
			while (first != last) {
				push_front(*--last);
			}
		}

		~forward_list() noexcept {
			clear();
			delete _sentinel;
		}

		bool empty() const noexcept {
			return _sentinel->_next == _sentinel;
		}

		void clear() const noexcept {
			_node* i{_sentinel->_next};
			while (i != _sentinel) {
				_node* next{i->_next};
				delete i;
				i = next;
			}
		}

		void resize(::std::size_t count, const T& value = {}) const noexcept {
			for (::std::size_t i{0}; i < count; ++i) {
				push_front(value);
			}
		}

		iterator begin() const noexcept {
			return iterator{_sentinel->_next};
		}

		iterator end() const noexcept {
			return iterator{_sentinel};
		}

		T& front() const noexcept {
#ifdef PLASTIC_VERIFY
			if (empty()) {
				::std::abort();
			}
#endif
			return _sentinel->_next->_value;
		}

		void push_front(const T& value) const noexcept {
			_sentinel->_next = new _node{value, _sentinel->_next};
		}

		void pop_front() const noexcept {
#ifdef PLASTIC_VERIFY
			if (empty()) {
				::std::abort();
			}
#endif
			_node* temp{_sentinel->_next};
			_sentinel->_next = temp->_next;
			delete temp;
		}

		iterator insert_after(iterator pos, const T& value, std::size_t count = 1) const noexcept {
			_node* i{pos._ptr};
			for (::std::size_t j{0}; j < count; ++j) {
				i = i->_next = new _node{value, i->_next};
			}
			return iterator{i};
		}

		template<::std::input_iterator iter>
		iterator insert_after(iterator pos, iter first, iter last) const noexcept {
			_node* i{pos._ptr};
			while (first != last) {
				i = i->_next = new _node{*first++, i->_next};
			}
			return iterator{i};
		}

		iterator erase_after(iterator pos) const noexcept {
			_node* i{pos._ptr};
			_node* next{i->_next};
#ifdef PLASTIC_VERIFY
			if (next == _sentinel) {
				::std::abort();
			}
#endif
			i->_next = next->_next;
			delete next;
			return ++pos;
		}

		iterator erase_after(iterator first, iterator last) const noexcept {
			if (first == last) {
				return last;
			}
			_node* i{first._ptr}, * j{last._ptr};
			_node* next{i->_next};
#ifdef PLASTIC_VERIFY
			if (next == _sentinel) {
				::std::abort();
			}
#endif
			i->_next = j;
			i = next;
			while (i != j) {
				next = i->_next;
				delete i;
				i = next;
			}
			return last;
		}

		::std::compare_three_way_result<T> operator<=>(const forward_list& container) const noexcept {
			iterator i{_sentinel->_next}, j{container._sentinel->_next};
			while (i != _sentinel && j != container._sentinel) {
				::std::compare_three_way_result<T> cmp{*i++ <=> *j++};
				if (cmp != 0) {
					return cmp;
				}
			}
			return i == _sentinel && j != container._sentinel ? ::std::strong_ordering::less
				: i != _sentinel && j == container._sentinel ? ::std::strong_ordering::greater
				: ::std::strong_ordering::equal;
		}
	};

}