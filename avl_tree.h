#pragma once

#include <functional>

namespace plastic {

	template<class T, ::std::strict_weak_order<T, T> auto _compare = ::std::less<>{} >
	class avl_tree {
		struct node {
			T _value;
			::std::size_t _count;
			::std::size_t _balanceFactor;
			node* _parent;
			node* _left;
			node* _right;
		};

		node* _root;
		::std::size_t _size;

		void _freeNode(node* nd) {
			if (nd == nullptr) {
				return;
			}
			_freeNode(nd->_left);
			_freeNode(nd->_right);
			delete nd;
		}

		::std::pair<node*&, node*&> _findNodeWithParent(const T& value) {
			node** pLast{&_root};
			node** p{&_root};
			while (true) {
				node*& parent{*pLast};
				node*& current{*p};
				if (current == nullptr || current->_value == value) {
					return {current, parent};
				}
				pLast = p;
				p = _compare(value, current->_value) ? &current->_left : &current->_right;
			}
		}

		node*& _findNode(const T& node) {
			return _findNodeWithParent(node).first;
		}

		node*& _minChild(node*& nd) {
			if (nd == nullptr) {
				::std::abort();
			}
			node** p{&nd};
			while (true) {
				node*& current{*p};
				if (current->_left == nullptr) {
					return current;
				}
				p = &current->_left;
			}
		}

		node*& _maxChild(node*& nd) {
			if (nd == nullptr) {
				::std::abort();
			}
			node** p{&nd};
			while (true) {
				node*& current{*p};
				if (current->_right == nullptr) {
					return current;
				}
				p = &current->_right;
			}
		}

	public:
		explicit avl_tree() {
			_root = nullptr;
			_size = 0;
		}

		~avl_tree() {
			_freeNode(_root);
		}

		bool empty() {
			return _size == 0;
		}

		::std::size_t size() {
			return _size;
		}

		::std::size_t count(const T& value) {
			node* nd{_findNode(value)};
			return nd == nullptr ? 0 : nd->_count;
		}

		bool contains(const T& value) {
			return _findNode(value) != nullptr;
		}

		void insert(const T& value, ::std::size_t count = 1) {

		}

		void erase(const T& value, ::std::size_t count = 1) {

		}

		T min() {
			return _minChild(_root)->_value;
		}

		T max() {
			return _maxChild(_root)->_value;
		}
	};

}