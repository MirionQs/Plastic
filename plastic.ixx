module;

#include <assert.h>

export module plastic;

import std;

export namespace plastic {

    struct uninitialized_t {
        explicit uninitialized_t() noexcept = default;
    };

    plastic::uninitialized_t uninitialized{};

    template<class T>
    class vector {
        T* _begin{};
        T* _last{};
        T* _end{};

        void _extend(std::size_t size) noexcept {
            reserve(capacity() + std::max(capacity() >> 1, size));
        }

    public:
        class iterator {
            friend class vector;

            T* _ptr{};

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::contiguous_iterator_tag;

            iterator() noexcept = default;

            iterator(T* ptr) noexcept :
                _ptr{ ptr } {}

            reference operator*() const noexcept {
                return *_ptr;
            }

            pointer operator->() const noexcept {
                return _ptr;
            }

            reference operator[](std::size_t index) const noexcept {
                return _ptr[index];
            }

            friend bool operator==(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr == iter2._ptr;
            }

            friend auto operator<=>(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr <=> iter2._ptr;
            }

            iterator& operator+=(difference_type diff) noexcept {
                _ptr += diff;
                return *this;
            }

            iterator& operator-=(difference_type diff) noexcept {
                _ptr -= diff;
                return *this;
            }

            friend iterator operator+(iterator iter, difference_type diff) noexcept {
                return iter += diff;
            }

            friend iterator operator+(difference_type diff, iterator iter) noexcept {
                return iter += diff;
            }

            friend iterator operator-(iterator iter, difference_type diff) noexcept {
                return iter -= diff;
            }

            friend difference_type operator-(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr - iter2._ptr;
            }

            iterator& operator++() noexcept {
                ++_ptr;
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator temp{ *this };
                ++*this;
                return temp;
            }

            iterator& operator--() noexcept {
                --_ptr;
                return *this;
            }

            iterator operator--(int) noexcept {
                iterator temp{ *this };
                --*this;
                return temp;
            }
        };

        using const_iterator = std::const_iterator<iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::const_iterator<reverse_iterator>;

        explicit vector() noexcept = default;

        explicit vector(plastic::uninitialized_t, std::size_t size) noexcept :
            _begin{ new T[size] },
            _last{ _begin + size },
            _end{ _last } {}

        explicit vector(std::size_t size, const T& value = {}) noexcept :
            vector(plastic::uninitialized, size) {

            std::uninitialized_fill(_begin, _end, value);
        }

        template<std::input_iterator It>
        explicit vector(It first, It last) noexcept :
            vector(plastic::uninitialized, std::distance(first, last)) {

            std::uninitialized_copy(first, last, _begin);
        }

        explicit vector(std::initializer_list<T> list) noexcept :
            vector(list.begin(), list.end()) {}

        explicit vector(const vector& other) noexcept :
            vector(other._begin, other._last) {}

        ~vector() noexcept {
            delete[] _begin;
        }

        vector& operator=(const vector& other) noexcept {
            if (this == &other) {
                return *this;
            }

            vector temp{ other };
            std::swap(_begin, temp._begin);
            std::swap(_last, temp._last);
            std::swap(_end, temp._end);

            return *this;
        }

        bool empty() const noexcept {
            return _begin == _last;
        }

        std::size_t size() const noexcept {
            return _last - _begin;
        }

        void clear() noexcept {
            std::destroy(_begin, _last);
            _last = _begin;
        }

        void resize(std::size_t new_size, const T& value = {}) noexcept {
            if (new_size == size()) {
                return;
            }

            if (new_size < size()) {
                T* new_last{ _begin + new_size };
                std::destroy(new_last, _last);
                _last = new_last;
                return;
            }

            if (new_size <= capacity()) {
                T* new_last{ _begin + new_size };
                std::uninitialized_fill(_last, new_last, value);
                _last = new_last;
                return;
            }

            reserve(new_size);
            std::uninitialized_fill(_last, _end, value);
            _last = _end;
        }

        std::size_t capacity() const noexcept {
            return _end - _begin;
        }

        void reserve(std::size_t new_capacity) noexcept {
            if (new_capacity <= capacity()) {
                return;
            }

            T* new_begin{ new T[new_capacity] };
            T* new_last{ std::uninitialized_move(_begin, _last, new_begin) };
            T* new_end{ new_begin + new_capacity };
            delete[] _begin;

            _begin = new_begin;
            _last = new_last;
            _end = new_end;
        }

        iterator begin() noexcept {
            return _begin;
        }

        const_iterator begin() const noexcept {
            return _begin;
        }

        const_iterator cbegin() const noexcept {
            return _begin;
        }

        iterator end() noexcept {
            return _last;
        }

        const_iterator end() const noexcept {
            return _last;
        }

        const_iterator cend() const noexcept {
            return _last;
        }

        reverse_iterator rbegin() noexcept {
            return reverse_iterator{ _last };
        }

        const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ _last };
        }

        const_reverse_iterator crbegin() const noexcept {
            return reverse_iterator{ _last };
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator{ _begin };
        }

        const_reverse_iterator rend() const noexcept {
            return reverse_iterator{ _begin };
        }

        const_reverse_iterator crend() const noexcept {
            return reverse_iterator{ _begin };
        }

        auto&& operator[](this auto&& self, std::size_t index) noexcept {
            assert(index < self.size());
            return std::forward_like<decltype(self)>(self._begin[index]);
        }

        auto&& front(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(*self._begin);
        }

        auto&& back(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(self._last[-1]);
        }

        T* data() noexcept {
            return _begin;
        }

        const T* data() const noexcept {
            return _begin;
        }

        void push_back(const T& value) noexcept {
            if (_last == _end) {
                _extend(1);
            }
            *_last++ = value;
        }

        void pop_back() noexcept {
            assert(!empty());
            std::destroy_at(--_last);
        }

        iterator insert(iterator pos, const T& value) noexcept {
            return insert(pos, 1, value);
        }

        iterator insert(iterator pos, std::size_t count, const T& value) noexcept {
            if (count == 0) {
                return pos;
            }

            if (_end - _last < static_cast<std::ptrdiff_t>(count)) {
                std::ptrdiff_t offset{ pos - begin() };
                _extend(count);
                pos = begin() + offset;
            }

            std::fill(pos, std::move_backward(pos, end(), end() + count), value);
            _last += count;

            return pos;
        }

        template<std::input_iterator It>
        iterator insert(iterator pos, It first, It last) noexcept {
            if (first == last) {
                return pos;
            }

            std::ptrdiff_t count{ std::distance(first, last) };
            if (_end - _last < count) {
                std::ptrdiff_t offset{ pos - begin() };
                _extend(count);
                pos = begin() + offset;
            }

            std::copy_backward(first, last, std::move_backward(pos, end(), end() + count));
            _last += count;

            return pos;
        }

        iterator insert(iterator pos, std::initializer_list<T> list) noexcept {
            return insert(pos, list.begin(), list.end());
        }

        iterator erase(iterator pos) noexcept {
            _last = std::move(++iterator{ pos }, end(), pos)._ptr;
            std::destroy_at(_last);
            return pos;
        }

        iterator erase(iterator first, iterator last) noexcept {
            if (first == last) {
                return first;
            }

            T* new_last{ std::move(last, end(), first)._ptr };
            std::destroy(new_last, _last);
            _last = new_last;

            return first;
        }

        friend bool operator==(const vector& cont1, const vector& cont2) noexcept {
            return std::equal(cont1._begin, cont1._end, cont2._begin, cont2._end);
        }

        friend auto operator<=>(const vector& cont1, const vector& cont2) noexcept {
            return std::lexicographical_compare_three_way(cont1._begin, cont1._end, cont2._begin, cont2._end);
        }
    };

    template<class It>
    explicit plastic::vector(It, It)->plastic::vector<std::iter_value_t<It>>;

    template<class T>
    class deque {
        T* _begin;
        T* _end;
        T* _first;
        T* _last;

        bool _is_contiguous() const noexcept {
            return _first <= _last;
        }

        bool _is_full() const noexcept {
            return _last + 1 == _first || _last + 1 == _end && _first == _begin;
        }

        std::size_t _allocated_capacity() const noexcept {
            return _end - _begin;
        }

        std::size_t _residual_capacity() const noexcept {
            if (_is_contiguous()) {
                return _allocated_capacity() - (_last - _first);
            }
            return _first - _last;
        }

        void _extend(std::size_t size) noexcept {
            reserve(capacity() + std::max(capacity() >> 1, size));
        }

    public:
        class iterator {
            friend class deque;

            T* _ptr{};
            const deque* _cont{};

            std::ptrdiff_t _offset() const noexcept {
                std::ptrdiff_t offset{ _ptr - _cont->_first };
                if (offset < 0) {
                    offset += _cont->_allocated_capacity();
                }
                return offset;
            }

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::random_access_iterator_tag;

            iterator() noexcept = default;

            iterator(T* ptr, const deque* cont) noexcept :
                _ptr{ ptr },
                _cont{ cont } {}

            reference operator*() const noexcept {
                return *_ptr;
            }

            pointer operator->() const noexcept {
                return _ptr;
            }

            reference operator[](std::size_t index) const noexcept {
                return *(*this + index);
            }

            friend bool operator==(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr == iter2._ptr;
            }

            friend auto operator<=>(iterator iter1, iterator iter2) noexcept {
                return iter1._offset() <=> iter2._offset();
            }

            iterator& operator+=(difference_type diff) noexcept {
                if (_cont->_end - _ptr <= diff) {
                    diff -= _cont->_allocated_capacity();
                }
                else if (_ptr - _cont->_begin <= -diff) {
                    diff += _cont->_allocated_capacity();
                }
                _ptr += diff;
                return *this;
            }

            iterator& operator-=(difference_type diff) noexcept {
                return *this += -diff;
            }

            friend iterator operator+(iterator iter, difference_type diff) noexcept {
                return iter += diff;
            }

            friend iterator operator+(difference_type diff, iterator iter) noexcept {
                return iter += diff;
            }

            friend iterator operator-(iterator iter, difference_type diff) noexcept {
                return iter -= diff;
            }

            friend difference_type operator-(iterator iter1, iterator iter2) noexcept {
                return iter1._offset() - iter2._offset();
            }

            iterator& operator++() noexcept {
                ++_ptr;
                if (_ptr == _cont->_end) {
                    _ptr = _cont->_begin;
                }
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator temp{ *this };
                ++*this;
                return temp;
            }

            iterator& operator--() noexcept {
                if (_ptr == _cont->_begin) {
                    _ptr = _cont->_end;
                }
                --_ptr;
                return *this;
            }

            iterator operator--(int) noexcept {
                iterator temp{ *this };
                --*this;
                return temp;
            }
        };

        using const_iterator = std::const_iterator<iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::const_iterator<reverse_iterator>;

        explicit deque() noexcept :
            deque(plastic::uninitialized, 0) {}

        explicit deque(plastic::uninitialized_t, std::size_t size) noexcept :
            _begin{ new T[size + 1] },
            _end{ _begin + size + 1 },
            _first{ _begin },
            _last{ _begin + size } {}

        explicit deque(std::size_t size, const T& value = {}) noexcept :
            deque(plastic::uninitialized, size) {

            std::uninitialized_fill(_first, _last, value);
        }

        template<std::input_iterator It>
        explicit deque(It first, It last) noexcept :
            deque(plastic::uninitialized, std::distance(first, last)) {

            std::uninitialized_copy(first, last, _begin);
        }

        explicit deque(std::initializer_list<T> list) noexcept :
            deque(list.begin(), list.end()) {}

        explicit deque(const deque& other) noexcept :
            deque(plastic::uninitialized, other.size()) {

            if (other._is_contiguous()) {
                std::uninitialized_copy(other._first, other._last, _first);
            }
            else {
                std::uninitialized_copy(other._begin, other._last, std::uninitialized_copy(other._first, other._end, _first));
            }
        }

        ~deque() noexcept {
            delete[] _begin;
        }

        deque& operator=(const deque& other) noexcept {
            if (this == &other) {
                return *this;
            }

            deque temp{ other };
            std::swap(_begin, temp._begin);
            std::swap(_end, temp._end);
            std::swap(_first, temp._first);
            std::swap(_last, temp._last);

            return *this;
        }

        bool empty() const noexcept {
            return _first == _last;
        }

        std::size_t size() const noexcept {
            if (_is_contiguous()) {
                return _last - _first;
        }
            return _allocated_capacity() - (_first - _last);
        }

        void clear() noexcept {
            if (_is_contiguous()) {
                std::destroy(_first, _last);
            }
            else {
                std::destroy(_first, _end);
                std::destroy(_begin, _first);
            }
            _first = _last = _begin;
        }

        void resize(std::size_t new_size, const T& value = {}) noexcept {
            if (new_size == size()) {
                return;
            }

            if (new_size < size()) {
                T* new_last{ (begin() + new_size)._ptr };
                if (new_last <= _last) {
                    std::destroy(new_last, _last);
                }
                else {
                    std::destroy(new_last, _end);
                    std::destroy(_begin, _last);
                }
                _last = new_last;
                return;
            }

            if (new_size <= capacity()) {
                T* new_last{ (begin() + new_size)._ptr };
                if (_last <= new_last) {
                    std::uninitialized_fill(_last, new_last, value);
                }
                else {
                    std::uninitialized_fill(_last, _end, value);
                    std::uninitialized_fill(_begin, new_last, value);
                }
                _last = new_last;
                return;
            }

            reserve(new_size);
            std::uninitialized_fill(_last, _end - 1, value);
            _last = _end - 1;
        }

        std::size_t capacity() const noexcept {
            return _allocated_capacity() - 1;
        }

        void reserve(std::size_t new_capacity) noexcept {
            if (new_capacity <= capacity()) {
                return;
            }

            T* new_begin{ new T[new_capacity + 1] };
            T* new_end{ new_begin + new_capacity + 1 };
            T* new_last;
            if (_is_contiguous()) {
                new_last = std::uninitialized_move(_first, _last, new_begin);
            }
            else {
                new_last = std::uninitialized_move(_begin, _last, std::uninitialized_move(_first, _end, new_begin));
            }
            delete[] _begin;

            _begin = new_begin;
            _end = new_end;
            _first = new_begin;
            _last = new_last;
        }

        iterator begin() noexcept {
            return { _first, this };
        }

        const_iterator begin() const noexcept {
            return iterator{ _first, this };
        }

        const_iterator cbegin() const noexcept {
            return iterator{ _first, this };
        }

        iterator end() noexcept {
            return { _last, this };
        }

        const_iterator end() const noexcept {
            return iterator{ _last, this };
        }

        const_iterator cend() const noexcept {
            return iterator{ _last, this };
        }

        reverse_iterator rbegin() noexcept {
            return reverse_iterator{ { _last, this } };
        }

        const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ { _last, this } };
        }

        const_reverse_iterator crbegin() const noexcept {
            return reverse_iterator{ { _last, this } };
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator{ { _first, this } };
        }

        const_reverse_iterator rend() const noexcept {
            return reverse_iterator{ { _first, this } };
        }

        const_reverse_iterator crend() const noexcept {
            return reverse_iterator{ { _first, this } };
        }

        auto&& operator[](this auto&& self, std::size_t index) noexcept {
            assert(index < self.size());
            return std::forward_like<decltype(self)>(self.begin()[index]);
        }

        auto&& front(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(*self._first);
        }

        auto&& back(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(*--self.end());
        }

        void push_front(const T& value) noexcept {
            if (_is_full()) {
                _extend(1);
            }
            if (_first == _begin) {
                _first = _end;
            }
            *--_first = value;
        }

        void pop_front() noexcept {
            assert(!empty());
            std::destroy_at(_first++);
            if (_first == _end) {
                _first = _begin;
            }
        }

        void push_back(const T& value) noexcept {
            if (_is_full()) {
                _extend(1);
            }
            *_last++ = value;
            if (_last == _end) {
                _last = _begin;
            }
        }

        void pop_back() noexcept {
            assert(!empty());
            if (_last == _begin) {
                _last = _end;
            }
            std::destroy_at(--_last);
        }

        iterator insert(iterator pos, const T& value) noexcept {
            return insert(pos, 1, value);
        }

        iterator insert(iterator pos, std::size_t count, const T& value) noexcept {
            if (count == 0) {
                return pos;
            }

            if (_residual_capacity() < count) {
                std::ptrdiff_t offset{ pos - begin() };
                _extend(count);
                pos = begin() + offset;
            }

            std::fill(pos, std::move_backward(pos, end(), end() + count), value);
            _last = (end() + count)._ptr;

            return pos;
        }

        template<std::input_iterator It>
        iterator insert(iterator pos, It first, It last) noexcept {
            if (first == last) {
                return pos;
            }

            std::ptrdiff_t count{ std::distance(first, last) };
            if (_residual_capacity() < static_cast<std::size_t>(count)) {
                std::ptrdiff_t offset{ pos - begin() };
                _extend(count);
                pos = begin() + offset;
            }

            std::copy_backward(first, last, std::move_backward(pos, end(), end() + count));
            _last = (end() + count)._ptr;

            return pos;
        }

        iterator insert(iterator pos, std::initializer_list<T> list) noexcept {
            return insert(pos, list.begin(), list.end());
        }

        iterator erase(iterator pos) noexcept {
            _last = std::move(++iterator{ pos }, end(), pos)._ptr;
            std::destroy_at(_last);
            return pos;
        }

        iterator erase(iterator first, iterator last) noexcept {
            if (first == last) {
                return first;
            }

            T* new_last{ std::move(last, end(), first)._ptr };
            std::destroy(new_last, _last);
            _last = new_last;

            return first;
        }

        friend bool operator==(const deque& cont1, const deque& cont2) noexcept {
            return std::equal(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }

        friend auto operator<=>(const deque& cont1, const deque& cont2) noexcept {
            return std::lexicographical_compare_three_way(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }
    };

    template<class It>
    explicit plastic::deque(It, It)->plastic::deque<std::iter_value_t<It>>;

    template<class T>
    class forward_list {
        struct node {
            T value;
            node* next;
        };

        node* _sentinel;
        std::size_t _size;

    public:
        class iterator {
            friend class forward_list;

            node* _ptr{};

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::forward_iterator_tag;

            iterator() noexcept = default;

            iterator(node* ptr) noexcept :
                _ptr{ ptr } {}

            reference operator*() const noexcept {
                return _ptr->value;
            }

            pointer operator->() const noexcept {
                return &_ptr->value;
            }

            friend bool operator==(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr == iter2._ptr;
            }

            iterator& operator++() noexcept {
                _ptr = _ptr->next;
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator temp{ *this };
                ++*this;
                return temp;
            }
        };

        using const_iterator = std::const_iterator<iterator>;

        explicit forward_list() noexcept :
            _sentinel{ new node },
            _size{} {

            _sentinel->next = _sentinel;
        }

        explicit forward_list(std::size_t size, const T& value = {}) noexcept :
            forward_list() {

            insert_after(end(), size, value);
        }

        template<std::input_iterator It>
        explicit forward_list(It first, It last) noexcept :
            forward_list() {

            insert_after(end(), first, last);
        }

        explicit forward_list(std::initializer_list<T> list) noexcept :
            forward_list(list.begin(), list.end()) {}

        explicit forward_list(const forward_list& other) noexcept :
            forward_list(other.begin(), other.end()) {}

        ~forward_list() noexcept {
            clear();
            delete _sentinel;
        }

        forward_list& operator=(const forward_list& other) noexcept {
            if (this == &other) {
                return *this;
            }

            forward_list temp{ other };
            std::swap(_sentinel, temp._sentinel);
            std::swap(_size, temp._size);

            return *this;
        }

        bool empty() const noexcept {
            return _size == 0;
        }

        std::size_t size() const noexcept {
            return _size;
        }

        void clear() noexcept {
            erase_after(end(), end());
        }

        void resize(std::size_t new_size, const T& value = {}) noexcept {
            if (new_size == _size) {
                return;
            }
            if (new_size < _size) {
                erase_after(std::next(end(), new_size), end());
                return;
            }
            insert_after(std::next(end(), _size), new_size - _size, value);
        }

        iterator begin() noexcept {
            return _sentinel->next;
        }

        const_iterator begin() const noexcept {
            return iterator{ _sentinel->next };
        }

        const_iterator cbegin() const noexcept {
            return iterator{ _sentinel->next };
        }

        iterator end() noexcept {
            return _sentinel;
        }

        const_iterator end() const noexcept {
            return iterator{ _sentinel };
        }

        const_iterator cend() const noexcept {
            return iterator{ _sentinel };
        }

        auto&& front(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(self._sentinel->next->value);
        }

        void push_front(const T& value) noexcept {
            _sentinel->next = new node{ value, _sentinel->next };
            ++_size;
        }

        void pop_front() noexcept {
            assert(!empty());
            erase_after(end());
        }

        iterator insert_after(iterator pos, const T& value) noexcept {
            return insert_after(pos, 1, value);
        }

        iterator insert_after(iterator pos, std::size_t count, const T& value) noexcept {
            node* i{ pos._ptr };
            _size += count;
            while (count-- != 0) {
                i = i->next = new node{ value, i->next };
            }
            return i;
        }

        template<std::input_iterator It>
        iterator insert_after(iterator pos, It first, It last) noexcept {
            node* i{ pos._ptr };
            while (first != last) {
                i = i->next = new node{ *first++, i->next };
                ++_size;
            }
            return i;
        }

        iterator insert_after(iterator pos, std::initializer_list<T> list) noexcept {
            return insert_after(pos, list.begin(), list.end());
        }

        iterator erase_after(iterator pos) noexcept {
            node* i{ pos._ptr };
            delete std::exchange(i->next, i->next->next);
            --_size;
            return ++pos;
        }

        iterator erase_after(iterator first, iterator last) noexcept {
            node* i{ first._ptr }, * j{ last._ptr };
            i = std::exchange(i->next, j);
            while (i != j) {
                delete std::exchange(i, i->next);
                --_size;
            }
            return last;
        }

        friend bool operator==(const forward_list& cont1, const forward_list& cont2) noexcept {
            return std::equal(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }

        friend auto operator<=>(const forward_list& cont1, const forward_list& cont2) noexcept {
            return std::lexicographical_compare_three_way(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }
    };

    template<class It>
    explicit plastic::forward_list(It, It)->plastic::forward_list<std::iter_value_t<It>>;

    template<class T>
    class list {
        struct node {
            T value;
            node* prev;
            node* next;
        };

        node* _sentinel;
        std::size_t _size;

    public:
        class iterator {
            friend class list;

            node* _ptr{};

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::bidirectional_iterator_tag;

            iterator() noexcept = default;

            iterator(node* ptr) noexcept :
                _ptr{ ptr } {}

            reference operator*() const noexcept {
                return _ptr->value;
            }

            pointer operator->() const noexcept {
                return &_ptr->value;
            }

            friend bool operator==(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr == iter2._ptr;
            }

            iterator& operator++() noexcept {
                _ptr = _ptr->next;
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator temp{ *this };
                ++*this;
                return temp;
            }

            iterator& operator--() noexcept {
                _ptr = _ptr->prev;
                return *this;
            }

            iterator operator--(int) noexcept {
                iterator temp{ *this };
                --*this;
                return temp;
            }
        };

        using const_iterator = std::const_iterator<iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::const_iterator<reverse_iterator>;

        explicit list() noexcept :
            _sentinel{ new node },
            _size{} {

            _sentinel->next = _sentinel->prev = _sentinel;
        }

        explicit list(std::size_t size, const T& value = {}) noexcept :
            list() {

            insert(end(), size, value);
        }

        template<std::input_iterator It>
        explicit list(It first, It last) noexcept :
            list() {

            insert(end(), first, last);
        }

        explicit list(std::initializer_list<T> list) noexcept :
            list(list.begin(), list.end()) {}

        explicit list(const list& other) noexcept :
            list(other.begin(), other.end()) {}

        ~list() noexcept {
            clear();
            delete _sentinel;
        }

        list& operator=(const list& other) noexcept {
            if (this == &other) {
                return *this;
            }

            list temp{ other };
            std::swap(_sentinel, temp._sentinel);
            std::swap(_size, temp._size);

            return *this;
        }

        bool empty() const noexcept {
            return _size == 0;
        }

        std::size_t size() const noexcept {
            return _size;
        }

        void clear() noexcept {
            erase(begin(), end());
        }

        void resize(std::size_t new_size, const T& value = {}) noexcept {
            if (new_size == _size) {
                return;
            }
            if (new_size < _size) {
                while (_size != new_size) {
                    pop_back();
                }
                return;
            }
            insert(end(), new_size - _size, value);
        }

        iterator begin() noexcept {
            return _sentinel->next;
        }

        const_iterator begin() const noexcept {
            return iterator{ _sentinel->next };
        }

        const_iterator cbegin() const noexcept {
            return iterator{ _sentinel->next };
        }

        iterator end() noexcept {
            return _sentinel;
        }

        const_iterator end() const noexcept {
            return iterator{ _sentinel };
        }

        const_iterator cend() const noexcept {
            return iterator{ _sentinel };
        }

        reverse_iterator rbegin() noexcept {
            return reverse_iterator{ { _sentinel } };
        }

        const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ { _sentinel } };
        }

        const_reverse_iterator crbegin() const noexcept {
            return reverse_iterator{ { _sentinel } };
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator{ { _sentinel->next } };
        }

        const_reverse_iterator rend() const noexcept {
            return reverse_iterator{ { _sentinel->next } };
        }

        const_reverse_iterator crend() const noexcept {
            return reverse_iterator{ { _sentinel->next } };
        }

        auto&& front(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(self._sentinel->next->value);
        }

        auto&& back(this auto&& self) noexcept {
            assert(!self.empty());
            return std::forward_like<decltype(self)>(self._sentinel->prev->value);
        }

        void push_front(const T& value) noexcept {
            _sentinel->next->next->prev = _sentinel->next = new node{ value, _sentinel, _sentinel->next };
            ++_size;
        }

        void pop_front() noexcept {
            assert(!empty());
            erase(begin());
        }

        void push_back(const T& value) noexcept {
            _sentinel->prev->prev->next = _sentinel->prev = new node{ value, _sentinel->prev, _sentinel };
            ++_size;
        }

        void pop_back() noexcept {
            assert(!empty());
            erase(--end());
        }

        iterator insert(iterator pos, const T& value) noexcept {
            return insert(pos, 1, value);
        }

        iterator insert(iterator pos, std::size_t count, const T& value) noexcept {
            node* i{ pos._ptr->prev };
            _size += count;
            while (count-- != 0) {
                i = i->next = new node{ value, i, i->next };
            }
            i->next->prev = i;
            return pos;
        }

        template<std::input_iterator It>
        iterator insert(iterator pos, It first, It last) noexcept {
            node* i{ pos._ptr->prev };
            while (first != last) {
                i = i->next = new node{ *first++, i, i->next };
                ++_size;
            }
            i->next->prev = i;
            return pos;
        }

        iterator insert(iterator pos, std::initializer_list<T> list) noexcept {
            return insert(pos, list.begin(), list.end());
        }

        iterator erase(iterator pos) noexcept {
            node* i{ (++pos)._ptr };
            delete std::exchange(i->prev, i->prev->prev);
            --_size;
            i->prev->next = i;
            return pos;
        }

        iterator erase(iterator first, iterator last) noexcept {
            node* i{ first._ptr }, * j{ last._ptr };
            i->prev->next = j;
            j->prev = i->prev;
            while (i != j) {
                delete std::exchange(i, i->next);
                --_size;
            }
            return last;
        }

        friend bool operator==(const list& cont1, const list& cont2) noexcept {
            return std::equal(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }

        friend auto operator<=>(const list& cont1, const list& cont2) noexcept {
            return std::lexicographical_compare_three_way(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }
    };

    template<class It>
    explicit plastic::list(It, It)->plastic::list<std::iter_value_t<It>>;

    template<class T, class Cmp = std::less<T>>
    class binary_search_tree {
        static constexpr Cmp _cmp{};

        struct node {
            node* parent;
            node* left;
            node* right;
            T value;
            bool is_head;

            node* leftmost() noexcept {
                node* i{ this };
                while (!i->left->is_head) {
                    i = i->left;
                }
                return i;
            }

            node* rightmost() noexcept {
                node* i{ this };
                while (!i->right->is_head) {
                    i = i->right;
                }
                return i;
            }

            void free() noexcept {
                if (is_head) {
                    return;
                }
                left->free();
                right->free();
                delete this;
            }

            node* clone(node* head) noexcept {
                if (is_head) {
                    return head;
                }
                return new node{ this, left->clone(), right->clone(), value, false };
            }
        };

        node* _head;
        std::size_t _size;

    public:
        class iterator {
            friend class binary_search_tree;

            node* _ptr;

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = const T*;
            using reference = const T&;
            using iterator_category = std::bidirectional_iterator_tag;

            iterator(node* ptr = {}) noexcept :
                _ptr{ ptr } {}

            reference operator*() const noexcept {
                return _ptr->value;
            }

            pointer operator->() const noexcept {
                return &_ptr->value;
            }

            friend bool operator==(iterator iter1, iterator iter2) noexcept {
                return iter1._ptr == iter2._ptr;
            }

            iterator& operator++() noexcept {
                if (_ptr->right->is_head) {
                    node* old_ptr;
                    do {
                        old_ptr = _ptr;
                        _ptr = _ptr->parent;
                    } while (!_ptr->is_head && _ptr->right == old_ptr);
                }
                else {
                    _ptr = _ptr->right->leftmost();
                }
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator temp{ *this };
                ++*this;
                return temp;
            }

            iterator& operator--() noexcept {
                if (_ptr->left->is_head) {
                    node* old_ptr;
                    do {
                        old_ptr = _ptr;
                        _ptr = _ptr->parent;
                    } while (!_ptr->is_head && _ptr->left == old_ptr);
                }
                else {
                    _ptr = _ptr->left->rightmost();
                }
                return *this;
            }

            iterator operator--(int) noexcept {
                iterator temp{ *this };
                --*this;
                return temp;
            }
        };

        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        explicit binary_search_tree() noexcept :
            _head{ new node },
            _size{} {

            _head->parent = _head->left = _head->right = _head;
            _head->is_head = true;
        }

        template<std::input_iterator It>
        explicit binary_search_tree(It first, It last) noexcept :
            binary_search_tree() {

            insert(first, last);
        }

        explicit binary_search_tree(std::initializer_list<T> list) noexcept :
            binary_search_tree(list.begin(), list.end()) {}

        explicit binary_search_tree(const binary_search_tree& other) noexcept :
            binary_search_tree() {

            if (other._size != 0) {
                _head->parent = other._head->clone(_head);
                _head->left = _head->parent->leftmost();
                _head->right = _head->parent->rightmost();
            }
        }

        ~binary_search_tree() noexcept {
            clear();
            delete _head;
        }

        binary_search_tree& operator=(const binary_search_tree& other) noexcept {
            if (this == &other) {
                return *this;
            }

            binary_search_tree temp{ other };
            std::swap(_head, other._head);
            std::swap(_size, other._size);

            return *this;
        }

        bool empty() const noexcept {
            return _size == 0;
        }

        std::size_t size() const noexcept {
            return _size;
        }

        void clear() noexcept {
            _head->parent->free();
        }

        const_iterator begin() const noexcept {
            return _head->right;
        }

        const_iterator end() const noexcept {
            return _head;
        }

        const_reverse_iterator rbegin() const noexcept {
            return _head;
        }

        const_reverse_iterator rend() const noexcept {
            return _head->right;
        }

        const T& front() const noexcept {
            assert(!empty());
            return _head->right->value;
        }

        const T& back() const noexcept {
            assert(!empty());
            return _head->left->value;
        }

        // only for test purposes
        node* data() noexcept {
            return _head;
        }

        // only for test purposes
        const node* data() const noexcept {
            return _head;
        }

        const_iterator lower_bound(const T& value) const noexcept {
            node* bound{ _head }, * i{ _head->parent };
            while (!i->is_head) {
                if (!_cmp(i->value, value)) {
                    bound = i;
                    i = i->left;
        }
                else {
                    i = i->right;
                }
            }
            return bound;
        }

        const_iterator upper_bound(const T& value) const noexcept {
            node* bound{ _head }, * i{ _head->parent };
            while (!i->is_head) {
                if (_cmp(value, i->value)) {
                    bound = i;
                    i = i->left;
        }
                else {
                    i = i->right;
                }
            }
            return bound;
        }

        std::pair<const_iterator, const_iterator> equal_range(const T& value) const noexcept {
            node* first{ _head }, * last{ _head }, * i{ _head->parent };

            while (!i->is_head) {
                if (!_cmp(i->value, value)) {
                    if (last->is_head && _cmp(value, i->value)) {
                        last = i;
                    }
                    first = i;
                    i = i->left;
                }
                else {
                    i = i->right;
                }
            }

            i = last->is_head ? _head->parent : last->left;
            while (!i->is_head) {
                if (_cmp(value, i->value)) {
                    last = i;
                    i = i->left;
                }
                else {
                    i = i->right;
                }
            }

            return { first, last };
        }

        const_iterator find(const T& value) const noexcept {
            node* bound{ lower_bound(value)._ptr };
            return !bound->is_head && !_cmp(*bound, value) ? bound : _head;
        }

        bool contains(const T& value) const noexcept {
            node* bound{ lower_bound(value)._ptr };
            return !bound->is_head && !_cmp(*bound, value);
        }

        std::size_t count(const T& value) const noexcept {
            auto [first, last] { equal_range(value) };
            return std::distance(first, last);
        }

        iterator insert(const T& value) noexcept {
            node* parent{ _head }, * i{ _head->parent };
            bool is_left{};
            while (!i->is_head) {
                parent = i;
                is_left = _cmp(value, i->value);
                i = is_left ? i->left : i->right;
            }

            node* new_node{ new node{ parent, _head, _head, value, false } };

            if (parent == _head) {
                _head->parent = new_node;
            }

            if (is_left) {
                parent->left = new_node;
                if (parent == _head->right) {
                    _head->right = new_node;
                }
            }
            else {
                parent->right = new_node;
                if (parent == _head->left) {
                    _head->left = new_node;
                }
            }
            ++_size;

            return new_node;
        }

        template<std::input_iterator It>
        void insert(It first, It last) noexcept {
            while (first != last) {
                insert(*first++);
            }
        }

        void insert(std::initializer_list<T> list) noexcept {
            insert(list.begin(), list.end());
        }

        iterator erase(iterator pos) noexcept {
            node* ptr{ (pos++)._ptr };

            if (ptr->left->is_head || ptr->right->is_head) {
                node* parent{ ptr->parent };
                node* replaced{ ptr->left->is_head ? ptr->right : ptr->left };

                replaced->parent = parent;
                if (ptr == parent->left) {
                    parent->left = replaced;
                }
                else {
                    parent->right = replaced;
                }

                if (ptr == _head->right) {
                    _head->right = replaced->is_head ? parent : replaced->rightmost();
                }
                if (ptr == _head->left) {
                    _head->left = replaced->is_head ? parent : replaced->leftmost();
                }

                delete ptr;
                --_size;
            }
            else {

            }
                }
                else if (parent->left == ptr) {
                    parent->left = erased;
                }
                else {
                    parent->right = erased;
                }

                if (_head->left == ptr) {
                    _head->left = erased->is_head ? parent : erased->leftmost();
                }
                if (_head->right == ptr) {
                    _head->right = erased->is_head ? parent : erased->rightmost();
                }

                --_size;
                return ++pos;
            }

            return pos;
        }

        iterator erase(iterator first, iterator last) noexcept {
            while (first != last) {
                erase(first++);
            }
            return last;
        }

        std::size_t erase(const T& value) noexcept {
            std::size_t count{};
            auto [first, last] { equal_range(value) };
            while (first != last) {
                erase(first++);
                ++count;
            }
            return count;
        }

        void merge(const binary_search_tree& other) noexcept {

        }

        friend bool operator==(const binary_search_tree& cont1, const binary_search_tree& cont2) noexcept {
            return std::equal(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }

        friend auto operator<=>(const binary_search_tree& cont1, const binary_search_tree& cont2) noexcept {
            return std::lexicographical_compare_three_way(cont1.begin(), cont1.end(), cont2.begin(), cont2.end());
        }
    };

    // incompleted
    template<class T, class Cmp = std::less<T>>
    class red_black_tree {
        static constexpr Cmp _cmp{};

        struct node {
            enum class color {
                red,
                black
            };

            T value;
            std::size_t count;
            color color;
            node* parent;
            node* left;
            node* right;
        };

        node* _root;
        std::size_t _size;

        void _free(node* nd) noexcept {
            if (nd == nullptr) {
                return;
            }
            _free(nd->left);
            _free(nd->right);
            delete nd;
        }

        std::pair<node*&, node*&> _find_with_parent(const T& value) noexcept {
            node** last{ &_root };
            node** p{ &_root };
            while (true) {
                node*& parent{ *last };
                node*& current{ *p };
                if (current == nullptr || current->value == value) {
                    return { current, parent };
                }
                last = p;
                p = _cmp(value, current->value) ? &current->left : &current->right;
            }
        }

        node*& _find(const T& node) noexcept {
            return _find_with_parent(node).first;
        }

        node*& _min(node*& nd) noexcept {
            assert(nd != nullptr);
            node** p{ &nd };
            while (true) {
                node*& current{ *p };
                if (current->left == nullptr) {
                    return current;
                }
                p = &current->left;
            }
        }

        node*& _max(node*& nd) noexcept {
            assert(nd != nullptr);
            node** p{ &nd };
            while (true) {
                node*& current{ *p };
                if (current->right == nullptr) {
                    return current;
                }
                p = &current->right;
            }
        }

        void _left_rotate(node* nd) noexcept {
            node* left{ nd->left };
            node* parent{ nd->parent };
            node* grandparent{ parent->parent };
            nd->left = parent;
            nd->parent = grandparent;
            parent->right = left;
            parent->parent = nd;
            if (left != nullptr) {
                left->parent = parent;
            }
            if (grandparent != nullptr) {
                (parent == grandparent->left ? grandparent->left : grandparent->right) = nd;
            }
            else {
                _root = nd;
            }
        }

        void _right_rotate(node* nd) noexcept {
            node* right{ nd->right };
            node* parent{ nd->parent };
            node* grandparent{ parent->parent };
            nd->right = parent;
            nd->parent = grandparent;
            parent->left = right;
            parent->parent = nd;
            if (right != nullptr) {
                right->parent = parent;
            }
            if (grandparent != nullptr) {
                (parent == grandparent->left ? grandparent->left : grandparent->right) = nd;
            }
            else {
                _root = nd;
            }
        }

    public:
        explicit red_black_tree() noexcept :
            _root{},
            _size{} {}

        ~red_black_tree() noexcept {
            _free(_root);
        }

        bool empty() const noexcept {
            return _size == 0;
        }

        std::size_t size() const noexcept {
            return _size;
        }

        std::size_t count(const T& value) const noexcept {
            node* nd{ _find(value) };
            return nd == nullptr ? 0 : nd->count;
        }

        bool contains(const T& value) const noexcept {
            return _find(value) != nullptr;
        }

        T min() const noexcept {
            return _min(_root)->value;
        }

        T max() const noexcept {
            return _max(_root)->value;
        }

        void insert(const T& value, std::size_t count = 1) noexcept {
            _size += count;
            if (_root == nullptr) {
                _root = new node{ value, count, node::color::black, nullptr, nullptr, nullptr };
                return;
            }
            auto [nd, pr] {_find_with_parent(value)};
            if (nd != nullptr) {
                nd->count += count;
                return;
            }
            nd = new node{ value, count, node::color::red, pr, nullptr, nullptr };
            node* current{ nd };
            while (true) {
                if (current == _root) {
                    current->color = node::color::black;
                    break;
                }
                node* parent{ current->parent };
                if (parent->color == node::color::black) {
                    break;
                }
                node* grandparent{ parent->parent };
                bool is_xl{ current == parent->left };
                bool is_lx{ parent == grandparent->left };
                node* uncle{ is_lx ? grandparent->right : grandparent->left };
                if (uncle == nullptr || uncle->color == node::color::black) {
                    if (is_lx && is_xl) {
                        _right_rotate(parent);
                        parent->color = node::color::black;
                    }
                    else if (is_lx) {
                        _left_rotate(current);
                        _right_rotate(current);
                        current->color = node::color::black;
                    }
                    else if (is_xl) {
                        _right_rotate(current);
                        _left_rotate(current);
                        current->color = node::color::black;
                    }
                    else {
                        _left_rotate(parent);
                        parent->color = node::color::black;
                    }
                    grandparent->color = node::color::red;
                    break;
                }
                parent->color = node::color::black;
                uncle->color = node::color::black;
                grandparent->color = node::color::red;
                current = grandparent;
            }
        }

        void erase(const T& value, std::size_t count = 1) noexcept {
            node*& nd{ _find(value) };
            if (nd == nullptr) {
                return;
            }
            if (nd->count > count) {
                _size -= count;
                nd->count -= count;
                return;
            }
            _size -= nd->count;
            node** erased_ptr{ &nd };
            if (nd->left != nullptr && nd->right != nullptr) {
                node*& successor{ _min(nd->right) };
                nd->value = successor->value;
                nd->count = successor->count;
                erased_ptr = &successor;
            }
            node*& erased{ *erased_ptr };
            if (erased->left != nullptr) {
                node* temp{ erased };
                erased = erased->left;
                erased->color = node::color::black;
                erased->parent = temp->parent;
                delete temp;
                return;
            }
            if (erased->right != nullptr) {
                node* temp{ erased };
                erased = erased->right;
                erased->color = node::color::black;
                erased->parent = temp->parent;
                delete temp;
                return;
            }
            node* current{ erased };
            while (true) {
                if (current == _root || current->color == node::color::red) {
                    break;
                }
                node* parent{ current->parent };
                bool is_lx{ current == parent->right };
                bool is_rx{ !is_lx };
                node* sibling{ is_lx ? parent->left : parent->right };
                if (sibling->color == node::color::red) {
                    sibling->color = node::color::black;
                    parent->color = node::color::red;
                    if (is_lx) {
                        _right_rotate(sibling);
                        sibling = parent->left;
                    }
                    else {
                        _left_rotate(sibling);
                        sibling = parent->right;
                    }
                }
                node* left{ sibling->left };
                node* right{ sibling->right };
                bool is_xl{ left != nullptr && left->color == node::color::red };
                bool is_xr{ right != nullptr && right->color == node::color::red };
                if (is_xl || is_xr) {
                    if (is_lx && is_xl) {
                        _right_rotate(sibling);
                        sibling->color = parent->color;
                        left->color = node::color::black;
                    }
                    else if (is_rx && is_xr) {
                        _left_rotate(sibling);
                        sibling->color = parent->color;
                        right->color = node::color::black;
                    }
                    else if (is_lx) {
                        _left_rotate(right);
                        _right_rotate(right);
                        right->color = parent->color;
                    }
                    else {
                        _right_rotate(left);
                        _left_rotate(left);
                        left->color = parent->color;
                    }
                    parent->color = node::color::black;
                    break;
                }
                bool is_red{ parent->color == node::color::red };
                parent->color = node::color::black;
                sibling->color = node::color::red;
                if (is_red) {
                    break;
                }
                current = parent;
            }
            node* temp{ erased };
            erased = nullptr;
            delete temp;
        }
    };

    // incompleted
    template<class T, class Cmp = std::less<T>>
    class avl_tree {
        static constexpr Cmp _cmp{};

        struct node {
            T value;
            std::size_t count;
            int factor;
            node* parent;
            node* left;
            node* right;
        };

        node* _root;
        std::size_t _size;

        void _free(node* nd) noexcept {
            if (nd == nullptr) {
                return;
            }
            _free(nd->left);
            _free(nd->right);
            delete nd;
        }

        std::pair<node*&, node*&> _find_with_parent(const T& value) noexcept {
            node** last{ &_root };
            node** p{ &_root };
            while (true) {
                node*& parent{ *last };
                node*& current{ *p };
                if (current == nullptr || current->value == value) {
                    return { current, parent };
                }
                last = p;
                p = _cmp(value, current->value) ? &current->left : &current->right;
            }
        }

        node*& _find(const T& value) noexcept {
            return _find_with_parent(value).first;
        }

        node*& _min(node*& nd) noexcept {
            assert(nd != nullptr);
            node** p{ &nd };
            while (true) {
                node*& current{ *p };
                if (current->left == nullptr) {
                    return current;
                }
                p = &current->left;
            }
        }

        node*& _max(node*& nd) noexcept {
            assert(nd != nullptr);
            node** p{ &nd };
            while (true) {
                node*& current{ *p };
                if (current->right == nullptr) {
                    return current;
                }
                p = &current->right;
            }
        }

        void _left_rotate(node* nd) noexcept {
            node* left{ nd->left };
            node* parent{ nd->parent };
            node* grandparent{ parent->parent };
            nd->left = parent;
            nd->parent = grandparent;
            parent->right = left;
            parent->parent = nd;
            if (left != nullptr) {
                left->parent = parent;
            }
            if (grandparent != nullptr) {
                (parent == grandparent->left ? grandparent->left : grandparent->right) = nd;
            }
            else {
                _root = nd;
            }
        }

        void _right_rotate(node* nd) noexcept {
            node* right{ nd->right };
            node* parent{ nd->parent };
            node* grandparent{ parent->parent };
            nd->right = parent;
            nd->parent = grandparent;
            parent->left = right;
            parent->parent = nd;
            if (right != nullptr) {
                right->parent = parent;
            }
            if (grandparent != nullptr) {
                (parent == grandparent->left ? grandparent->left : grandparent->right) = nd;
            }
            else {
                _root = nd;
            }
        }

    public:
        explicit avl_tree() noexcept :
            _root{},
            _size{} {}

        ~avl_tree() noexcept {
            _free(_root);
        }

        bool empty() const noexcept {
            return _size == 0;
        }

        std::size_t size() const noexcept {
            return _size;
        }

        std::size_t count(const T& value) const noexcept {
            node* nd{ _find(value) };
            return nd == nullptr ? 0 : nd->count;
        }

        bool contains(const T& value) const noexcept {
            return _find(value) != nullptr;
        }

        T min() const noexcept {
            return _min(_root)->value;
        }

        T max() const noexcept {
            return _max(_root)->value;
        }

        void insert(const T& value, std::size_t count = 1) noexcept {
            _size += count;
            if (_root == nullptr) {
                _root = new node{ value, count, 0, nullptr, nullptr, nullptr };
                return;
            }
            auto [nd, pr] {_find_with_parent(value)};
            if (nd != nullptr) {
                nd->count += count;
                return;
            }
            nd = new node{ value, count, 0, pr, nullptr, nullptr };
            node* current{ nd };
            while (true) {
                if (current == _root) {
                    break;
                }
                node* parent{ current->parent };
                if (current == parent->left) {
                    ++parent->factor;
                }
                else {
                    --parent->factor;
                }
                if (parent->factor == 0) {
                    break;
                }
                bool is_lx{ parent->factor == 2 };
                bool is_rx{ parent->factor == -2 };
                bool is_xl{ current->factor == 1 };
                if (is_lx || is_rx) {
                    if (is_lx && is_xl) {
                        _right_rotate(current);
                        current->factor = 0;
                        parent->factor = 0;
                    }
                    else if (is_lx) {
                        node* right{ current->right };
                        _left_rotate(right);
                        _right_rotate(right);
                        current->factor = right->factor == -1 ? 1 : 0;
                        parent->factor = right->factor == 1 ? -1 : 0;
                        right->factor = 0;

                    }
                    else if (is_xl) {
                        node* left{ current->left };
                        _right_rotate(left);
                        _left_rotate(left);
                        current->factor = left->factor == -1 ? 1 : 0;
                        parent->factor = left->factor == 1 ? -1 : 0;
                        left->factor = 0;
                    }
                    else {
                        _left_rotate(current);
                        current->factor = 0;
                        parent->factor = 0;
                    }
                    break;
                }
                current = parent;
            }
        }

        void erase(const T& value, std::size_t count = 1) noexcept {
            node*& nd{ _find(value) };
            if (nd == nullptr) {
                return;
            }
            if (nd->count > count) {
                _size -= count;
                nd->count -= count;
                return;
            }
            _size -= nd->count;
            node** erased_ptr{ &nd };
            if (nd->left != nullptr && nd->right != nullptr) {
                node*& successor{ _min(nd->right) };
                nd->value = successor->value;
                nd->count = successor->count;
                erased_ptr = &successor;
            }
            if (node*& erased{ *erased_ptr }, *& left{ erased->left }, *& right{ erased->right };
                left != nullptr) {
                erased->value = left->value;
                erased->count = left->count;
                erased->factor = 0;
                erased_ptr = &left;
            }
            else if (right != nullptr) {
                erased->value = right->value;
                erased->count = right->count;
                erased->factor = 0;
                erased_ptr = &right;
            }
            node*& erased{ *erased_ptr };
            node* current{ erased };
            while (true) {
                if (current == _root) {
                    break;
                }
                node* parent{ current->parent };
                if (current == parent->left) {
                    --parent->factor;
                }
                else {
                    ++parent->factor;
                }
                if (parent->factor == 1 || parent->factor == -1) {
                    break;
                }
                bool is_lx{ parent->factor == 2 };
                bool is_rx{ parent->factor == -2 };
                if (is_lx || is_rx) {
                    node* sibling{ is_lx ? parent->left : parent->right };
                    bool is_xl{ is_lx && sibling->factor != -1 || is_rx && sibling->factor == 1 };
                    if (is_lx && is_xl) {
                        _right_rotate(sibling);
                        if (sibling->factor == 0) {
                            sibling->factor = -1;
                            parent->factor = 1;
                            break;
                        }
                        sibling->factor = 0;
                        parent->factor = 0;
                    }
                    else if (is_lx) {
                        node* right{ sibling->right };
                        _left_rotate(right);
                        _right_rotate(right);
                        sibling->factor = right->factor == -1 ? 1 : 0;
                        parent->factor = right->factor == 1 ? -1 : 0;
                        right->factor = 0;
                    }
                    else if (is_xl) {
                        node* left{ sibling->left };
                        _right_rotate(left);
                        _left_rotate(left);
                        sibling->factor = left->factor == -1 ? 1 : 0;
                        parent->factor = left->factor == 1 ? -1 : 0;
                        left->factor = 0;
                    }
                    else {
                        _left_rotate(sibling);
                        if (sibling->factor == 0) {
                            sibling->factor = 1;
                            parent->factor = -1;
                            break;
                        }
                        sibling->factor = 0;
                        parent->factor = 0;
                    }
                }
                current = parent;

            }
            node* temp{ erased };
            erased = nullptr;
            delete temp;
        }
    };

    // incompleted
    template<class T, class Cmp = std::less<T>, std::size_t order = 5>
    class b_tree {
        static constexpr Cmp _cmp{};
        static constexpr std::size_t _min_size{ order - 1 };
        static constexpr std::size_t _max_size{ 2 * order - 1 };

        struct node {
            T value[_max_size];
            node* children[_max_size + 1];
            std::size_t size;
        };

        node* _root;
        std::size_t _size;

    public:
        b_tree() noexcept :
            _root{},
            _size{} {}
    };

    template<class It>
    explicit plastic::binary_search_tree(It, It)->plastic::binary_search_tree<std::iter_value_t<It>>;

    template<class T, class Cmp = std::less<T>>
    class binary_heap {
        static constexpr Cmp _cmp{};

        plastic::vector<T> _data{};

        void _sift_up(std::size_t index) noexcept {
            T value{ std::move(_data[index]) };
            while (index != 0) {
                std::size_t parent{ (index - 1) / 2 };
                if (!_cmp(_data[parent], value)) {
                    break;
                }
                _data[index] = std::move(_data[parent]);
                index = parent;
            }
            _data[index] = std::move(value);
        }

        void _sift_down(std::size_t index) noexcept {
            T value{ std::move(_data[index]) };
            while (true) {
                std::size_t child{ index * 2 + 1 };
                if (child >= _data.size()) {
                    break;
                }
                if (child + 1 < _data.size() && _cmp(_data[child], _data[child + 1])) {
                    ++child;
                }
                if (!_cmp(value, _data[child])) {
                    break;
                }
                _data[index] = std::move(_data[child]);
                index = child;
            }
            _data[index] = std::move(value);
        }

        void _sift_up_down(std::size_t index) noexcept {
            _sift_up(index);
            _sift_down(index);
        }

        void _make_heap() noexcept {
            std::size_t i{ _data.size() / 2 };
            while (i-- != 0) {
                _sift_down(i);
            }
        }

    public:
        explicit binary_heap() noexcept = default;

        template<std::input_iterator It>
        explicit binary_heap(It first, It last) noexcept :
            _data(first, last) {

            _make_heap();
        }

        explicit binary_heap(std::initializer_list<T> list) noexcept :
            binary_heap(list.begin(), list.end()) {}

        bool empty() const noexcept {
            return _data.empty();
        }

        std::size_t size() const noexcept {
            return _data.size();
        }

        void clear() noexcept {
            return _data.clear();
        }

        const T& top() const noexcept {
            assert(!_data.empty());
            return _data.front();
        }

        // only for test purposes
        auto data(this auto&& self) noexcept {
            return self._data.data();
        }

        void push(const T& value) noexcept {
            _data.push_back(value);
            _sift_up(_data.size() - 1);
        }

        void pop() noexcept {
            assert(!_data.empty());
            _data.front() = std::move(_data.back());
            _data.pop_back();
            _sift_down(0);
        }

        void merge(const binary_heap& other) noexcept {
            _data.insert(_data.end(), other._data.begin(), other._data.end());
            _make_heap();
        }

        void assign(T* ptr, const T& value) noexcept {
            *ptr = value;
            _sift_up_down(ptr - _data.data());
        }

        void erase(T* ptr) noexcept {
            *ptr = std::move(_data.back());
            _data.pop_back();
            if (!_data.empty()) {
                _sift_up_down(ptr - _data.data());
            }
        }
    };

    template<class It>
    explicit plastic::binary_heap(It, It)->plastic::binary_heap<std::iter_value_t<It>>;

}