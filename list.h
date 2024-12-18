#pragma once

#include "macro.h"

#include <iterator>

namespace plastic {

	template<class T>
	class list {
		struct node {
			T value;
			node* prev;
			node* next;
		};

		node* _sentinel;
		size_t _size;

	public:
		class iterator {
			friend class list;

			node* _ptr;

		public:
			using difference_type = ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator(node* node = {}) {
				_ptr = node;
			}

			reference operator*() const {
				return _ptr->value;
			}

			pointer operator->() const {
				return &_ptr->value;
			}

			bool operator==(iterator it) const {
				return _ptr == it._ptr;
			}

			iterator& operator++() {
				_ptr = _ptr->next;
				return *this;
			}

			iterator operator++(int) {
				iterator temp{ *this };
				++*this;
				return temp;
			}

			iterator& operator--() {
				_ptr = _ptr->prev;
				return *this;
			}

			iterator operator--(int) {
				iterator temp{ *this };
				--*this;
				return temp;
			}
		};

		using const_iterator = std::const_iterator<iterator>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::const_iterator<reverse_iterator>;

		explicit list(size_t size = {}, const T& value = {}) {
			_sentinel->next = _sentinel->prev = _sentinel = new node;
			_size = 0;
			insert(end(), size, value);
		}

		template<std::input_iterator It>
		explicit list(It first, It last) {
			_sentinel->next = _sentinel->prev = _sentinel = new node;
			_size = 0;
			insert(end(), first, last);
		}

		explicit list(std::initializer_list<T> list) : list(list.begin(), list.end()) {}

		~list() {
			clear();
			delete _sentinel;
		}

		bool empty() const {
			return _size == 0;
		}

		size_t size() const {
			return _size;
		}

		void clear() {
			resize(0);
		}

		void resize(size_t size, const T& value = {}) {
			if (size == _size) {
				return;
			}
			if (size < _size) {
				while (_size != size) {
					pop_back();
				}
				return;
			}
			insert(end(), size - _size, value);
		}

		iterator begin() {
			return _sentinel->next;
		}

		const_iterator begin() const {
			return iterator{ _sentinel->next };
		}

		const_iterator cbegin() const {
			return iterator{ _sentinel->next };
		}

		iterator end() {
			return _sentinel;
		}

		const_iterator end() const {
			return iterator{ _sentinel };
		}

		const_iterator cend() const {
			return iterator{ _sentinel };
		}

		reverse_iterator rbegin() {
			return reverse_iterator{ { _sentinel->next } };
		}

		const_reverse_iterator rbegin() const {
			return reverse_iterator{ { _sentinel->next } };
		}

		const_reverse_iterator crbegin() const {
			return reverse_iterator{ { _sentinel->next } };
		}

		reverse_iterator rend() {
			return reverse_iterator{ { _sentinel } };
		}

		const_reverse_iterator rend() const {
			return reverse_iterator{ { _sentinel } };
		}

		const_reverse_iterator crend() const {
			return reverse_iterator{ { _sentinel } };
		}

		T& front() {
			PLASTIC_VERIFY(!empty());
			return _sentinel->next->value;
		}

		const T& front() const {
			PLASTIC_VERIFY(!empty());
			return _sentinel->next->value;
		}

		T& back() {
			PLASTIC_VERIFY(!empty());
			return _sentinel->prev->value;
		}

		const T& back() const {
			PLASTIC_VERIFY(!empty());
			return _sentinel->prev->value;
		}

		void push_front(const T& value) {
			_sentinel->next->next->prev = _sentinel->next = new node{ value, _sentinel, _sentinel->next };
			++_size;
		}

		void pop_front() {
			PLASTIC_VERIFY(!empty());
			erase(begin());
		}

		void push_back(const T& value) {
			_sentinel->prev->prev->next = _sentinel->prev = new node{ value, _sentinel->prev, _sentinel };
			++_size;
		}

		void pop_back() {
			PLASTIC_VERIFY(!empty());
			erase(--end());
		}

		iterator insert(iterator pos, const T& value) {
			return insert(pos, 1, value);
		}

		iterator insert(iterator pos, size_t count, const T& value) {
			node* i{ (--pos)._ptr };
			for (size_t j{}; j != count; ++j) {
				i = i->next = new node{ value, i, i->next };
			}
			i->next->prev = i;
			_size += count;
			return ++pos;
		}

		template<std::input_iterator It>
		iterator insert(iterator pos, It first, It last) {
			node* i{ (--pos)._ptr };
			while (first != last) {
				i = i->next = new node{ *first++, i, i->next };
				++_size;
			}
			i->next->prev = i;
			return ++pos;
		}

		iterator insert(iterator pos, std::initializer_list<T> list) {
			return insert(pos, list.begin(), list.end());
		}

		iterator erase(iterator pos) {
			node* i{ (++pos)._ptr };
			PLASTIC_VERIFY(i->prev != _sentinel);
			delete std::exchange(i->prev, i->prev->prev);
			--_size;
			i->prev->next = i;
			return pos;
		}

		iterator erase(iterator first, iterator last) {
			node* i{ first._ptr }, * j{ last._ptr };
			i->prev->next = j;
			j->prev = i->prev;
			while (i != j) {
				delete std::exchange(i, i->next);
				--_size;
			}
			return last;
		}

		bool operator==(const list& cont) const {
			return std::equal(begin(), end(), cont.begin(), cont.end());
		}

		auto operator<=>(const list& cont) const {
			return std::lexicographical_compare_three_way(begin(), end(), cont.begin(), cont.end());
		}

	};

	template<class It>
	explicit list(It, It)->list<std::iter_value_t<It>>;

}