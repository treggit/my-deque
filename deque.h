//
// Created by Anton Shelepov on 23.06.18.
//

#ifndef MY_DEQUE_DEQUE_H
#define MY_DEQUE_DEQUE_H

#include <iterator>
#include <cassert>
#include <algorithm>

namespace finalizer {

    template<typename T>
    typename std::enable_if<std::is_trivially_destructible<T>::value, void>::type destruct(T* data, size_t len) {}

    template<typename T>
    typename std::enable_if<!std::is_trivially_destructible<T>::value, void>::type destruct(T* data, size_t len) {
        for (size_t i = 0; i < len; i++) {
            data[i].~T();
        }
    }


    template <typename T>
    void finalize(T* data, size_t len) {
        if (data == nullptr || len == 0) {
            return;
        }
        destruct<T>(data, len);
        operator delete(data);
    }
}

template <typename T>
struct deque {
private:

    template <typename V>
    struct deque_iterator;

    mutable T* _head;
    mutable T*_tail;

    T* _data;
    size_t _cap;

    static constexpr double EXPAND_FACTOR = 1.5;
    const size_t MIN_SIZE = 4;

    T* inc(T* pos);
    T* dec(T* pos);

public:
    using value_type = T;
    using iterator = deque_iterator<T>;
    using const_iterator = deque_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    deque();
    deque(deque const& other);
    deque<T>& operator=(deque other);
    ~deque();

    void clear();
    bool empty();
    size_t size() const;

    void push_back(T const& val);
    void pop_back();
    T& back();
    T const& back() const;

    void push_front(T const& val);
    void pop_front();
    T& front();
    T const& front() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    iterator insert(const_iterator pos, T const& val);
    iterator erase(const_iterator pos);
    //iterator erase(const_iterator first, const_iterator last);

    void swap(deque<T>& other);

    T& operator[](size_t pos);
    T const& operator[](size_t pos) const;

private:
    void expand(size_t n);
    ptrdiff_t dist(const_iterator a, const_iterator b) const;
    void my_copy(T* first, T* last, T* dst);
};


template <typename T>
template <typename V>
struct deque<T>::deque_iterator : std::iterator<std::random_access_iterator_tag, V> {

    friend struct deque<T>;

    explicit deque_iterator(V* ptr = nullptr, V* dbegin = nullptr, V* dend = nullptr, V* dhead = nullptr)
            : _ptr(ptr), _dbegin(dbegin), _dend(dend), _dhead(dhead) {}
    deque_iterator(deque_iterator const& other)
            : _ptr(other._ptr), _dbegin(other._dbegin), _dend(other._dend), _dhead(other._dhead) {}

    template <typename U>
    deque_iterator(deque_iterator<U> const& other, typename std::enable_if<std::is_same<V, const U>::value
                                                                  && std::is_const<V>::value, void>::type* = nullptr)
            : _ptr(other._ptr), _dbegin(other._dbegin), _dend(other._dend), _dhead(other._dhead) {}

    deque_iterator& operator=(deque_iterator<V> const& other) = default;
    ~deque_iterator() = default;

    friend deque_iterator operator+(deque_iterator const& cur, size_t shift) {
        if (cur.size()) {
            shift %= cur.size();
        }
        V* next;
        if (cur._ptr + shift >= cur._dend) {
            next = cur._dbegin + shift - (cur._dend - cur._ptr);
        } else {
            next = cur._ptr + shift;
        }
        return deque_iterator(next, cur._dbegin, cur._dend, cur._dhead);
    }

    friend deque_iterator operator-(deque_iterator const& cur, size_t shift) {
        if (cur.size()) {
            shift %= cur.size();
        }
        V* next;
        if (cur._dbegin + shift > cur._ptr) {
            next = cur._dend - (shift - (cur._ptr - cur._dbegin));
        } else {
            next = cur._ptr - shift;
        }
        return deque_iterator(next, cur._dbegin, cur._dend, cur._dhead);
    }

    friend deque_iterator operator+(size_t shift, deque_iterator const& cur) {
        return cur + shift;
    }

    deque_iterator& operator+=(size_t shift) {
        *this = *this + shift;
        return *this;
    }

    deque_iterator& operator-=(size_t shift) {
        *this = *this - shift;
        return *this;
    }

    deque_iterator& operator++() {
        return (*this += 1);
    }

    deque_iterator operator++(int) {
        deque_iterator old = *this;
        *this += 1;
        return old;
    }

    deque_iterator& operator--() {
        return (*this -= 1);
    }

    deque_iterator operator--(int) {
        deque_iterator old = *this;
        *this -= 1;
        return old;
    }

    V& operator*() const {
        return *_ptr;
    }

    V* operator->() const {
        return _ptr;
    }

    friend bool operator==(deque_iterator const& a, deque_iterator const& b) {
        return a._ptr == b._ptr;
    }

    friend bool operator!=(deque_iterator const& a, deque_iterator const& b) {
        return a._ptr != b._ptr;
    }

    friend bool operator<(deque_iterator const& a, deque_iterator const& b) {
        if ((a._ptr >= a._dhead && b._ptr >= b._dhead) || (a._ptr <= a._dhead && b._ptr <= b._dhead)) {
            return a._ptr < b._ptr;
        }
        return a._ptr >= a._dhead;
    }

    friend bool operator>(deque_iterator const& a, deque_iterator const& b) {
        return (!(a < b) && a != b);
    }

    friend bool operator<=(deque_iterator const& a, deque_iterator const& b) {
        return (a < b || a == b);
    }

    friend bool operator>=(deque_iterator const& a, deque_iterator const& b) {
        return (a > b || a == b);
    }

    friend ptrdiff_t operator-(deque_iterator const& a, deque_iterator const& b) {
        if (a._ptr == a._dhead && b._ptr == b._dhead) {
            return 0;
        }
        if (a._ptr == a._dhead) {
            if (b._ptr > b._dhead) {
                return b._ptr - a._ptr;
            }
            return b._ptr - b._dbegin + b._dend - a._dhead;
        }
        if (b._ptr == b._dhead) {
            if (a._ptr > a._dhead) {
                return a._ptr - b._ptr;
            }
            return a._ptr - a._dbegin + a._dend - b._dhead;
        }
        if ((a._ptr > a._dhead && b._ptr > b._dhead) || (a._ptr < a._dhead && b._ptr < b._dhead)) {
            return std::max(a._ptr, b._ptr) - std::min(a._ptr, b._ptr);
        }
        if (a._ptr >= a._dhead) {
            return b._ptr - b._dbegin + a._dend - a._ptr;
        }
        return a._ptr - a._dbegin + b._dend - b._ptr;
    }

private:

    size_t size() const {
        return _dend - _dbegin;
    }

    V* _ptr;
    V* _dbegin;
    V* _dend;
    V* _dhead;
};


template <typename T>
T* deque<T>::inc(T* pos) {
    pos++;
    if (pos == _data + _cap) {
        pos = _data;
    }
    return pos;
}

template <typename T>
T* deque<T>::dec(T* pos) {
    if (pos == _data) {
        pos = _data + _cap;
    }
    pos--;
    return pos;
}

template<typename T>
deque<T>::deque()
        : _head(nullptr), _tail(nullptr), _data(nullptr), _cap(0) {}

template<typename T>
deque<T>::deque(deque<T> const& other) : deque() {
    expand(other.size());
    for (auto it = other.begin(); it != other.end(); it++) {
        push_back(*it);
    }
}

template<typename T>
deque<T>& deque<T>::operator=(deque<T> other) {
    swap(other);

    return *this;
}

template<typename T>
deque<T>::~deque() {
    clear();
}

template<typename T>
void deque<T>::clear() {
    finalizer::finalize(_data, _cap);
    _data = nullptr;
    _head = nullptr;
    _tail = nullptr;
    _cap = 0;
}

template<typename T>
bool deque<T>::empty() {
    return size() == 0;
}

template<typename T>
size_t deque<T>::size() const {
    if (_tail >= _head) {
        return _tail - _head;
    }
    return _tail + _cap - _head;
}

template<typename T>
void deque<T>::push_back(T const& val) {
    insert(end(), val);
}

template<typename T>
void deque<T>::pop_back() {
    assert(!empty());
    _tail = dec(_tail);
    (end())._ptr->~T();
}

template<typename T>
T& deque<T>::back() {
    return *(std::prev(end()));
}

template<typename T>
T const& deque<T>::back() const {
    return *(std::prev(end()));
}

template<typename T>
void deque<T>::push_front(const T& val) {
    insert(begin(), val);
}

template<typename T>
void deque<T>::pop_front() {
    assert(!empty());
    _head = inc(_head);
    dec(_head)->~T();
}

template<typename T>
T& deque<T>::front() {
    return *begin();
}

template<typename T>
T const& deque<T>::front() const {
    return *begin();
}

template<typename T>
typename deque<T>::iterator deque<T>::begin() {
    return iterator(_head, _data, _data + _cap, _head);
}

template<typename T>
typename deque<T>::const_iterator deque<T>::begin() const {
    return const_iterator(_head, _data, _data + _cap, _head);
}

template<typename T>
typename deque<T>::iterator deque<T>::end() {
    return iterator(_tail, _data, _data + _cap, _head);
}

template<typename T>
typename deque<T>::const_iterator deque<T>::end() const {
    return const_iterator(_tail, _data, _data + _cap, _head);
}

template<typename T>
typename deque<T>::reverse_iterator deque<T>::rbegin() {
    return reverse_iterator(end());
}

template<typename T>
typename deque<T>::const_reverse_iterator deque<T>::rbegin() const {
    return const_reverse_iterator(end());
}

template<typename T>
typename deque<T>::reverse_iterator deque<T>::rend() {
    return reverse_iterator(begin());
}

template<typename T>
typename deque<T>::const_reverse_iterator deque<T>::rend() const {
    return const_reverse_iterator(begin());
}

template<typename T>
typename deque<T>::iterator deque<T>::insert(const_iterator pos, const T& val) {
//    if (_data == nullptr) {
//        expand(MIN_SIZE);
//    }
    size_t ind = dist(pos, begin());
    expand(size() + 1);
    pos = const_iterator((begin() + ind)._ptr, _data, _data + _cap, _head);
    iterator ptr;
    if (dist(pos, begin()) <= dist(pos, end())) {
        _head = dec(_head);
        for (ptr = begin(); ptr != pos - 1; ptr++) {
            *ptr = *(ptr + 1);
        }
    } else {
        _tail = inc(_tail);
        for (ptr = end() - 1; ptr != pos; ptr--) {
            *ptr = *(ptr - 1);
        }
    }
    *ptr = val;
    //expand(size() + 1);
    return ptr;
}

template<typename T>
typename deque<T>::iterator deque<T>::erase(const_iterator pos) {
    assert(!empty());
    if (pos == begin()) {
        pop_front();
        return begin();
    }
    if (pos == end()) {
        pop_back();
        return end();
    }
    size_t ind = dist(pos, begin());
    iterator ptr = iterator((begin() + ind)._ptr, _data, _data + _cap, _head);
    (*this)[ind].~T();

    if (dist(pos, begin()) <= dist(pos, end())) {
        for (; ptr != begin(); ptr--) {
            *ptr = *(ptr - 1);
        }
        _head = inc(_head);
    } else {
        for (; ptr != end() - 1; ptr++) {
            *ptr = *(ptr + 1);
        }
        _tail = dec(_tail);
    }
    (*ptr).~T();
    return iterator((begin() + ind)._ptr, _data, _data + _cap, _head);
}

//template<typename T>
//deque_iterator <T> deque<T>::erase(deque_iterator<const T> first, deque_iterator<const T> last) {
//    return nullptr;
//}

template<typename T>
void deque<T>::swap(deque<T>& other) {
    std::swap(_head, other._head);
    std::swap(_tail, other._tail);
    T* tmp = std::move(_data);
    _data = std::move(other._data);
    other._data = std::move(tmp);
    std::swap(_cap, other._cap);
}

template<typename T>
T& deque<T>::operator[](size_t pos) {
    T* res;
    if (_head + pos >= _data + _cap) {
        res = pos - _cap + _head;
    } else {
        res = _head + pos;
    }
    return *res;
}

template<typename T>
T const& deque<T>::operator[](size_t pos) const {
    T* res;
    if (_head + pos >= _data + _cap) {
        res = pos - _cap + _head;
    } else {
        res = _head + pos;
    }
    return *res;
}

template <typename T>
void deque<T>::my_copy(T* first, T* last, T* dst) {
    size_t i = 0;
    while (first != last) {
        dst[i++] = *first;
        first = inc(first);
    }
}

template <typename T>
void deque<T>::expand(size_t n) {
    if (n < _cap) {
        return;
    }
    size_t new_cap = std::max(n, static_cast<size_t>(EXPAND_FACTOR * _cap)) + 1;

    auto new_storage = (T*) operator new(sizeof(T) * new_cap);
    my_copy(_head, _tail, new_storage);
    finalizer::finalize(_data, _cap);
    _data = new_storage;
    _tail = _data + size();
    _head = _data;
    _cap = new_cap;
}

template<typename T>
ptrdiff_t deque<T>::dist(const_iterator a, const_iterator b) const {
    if (a > b) {
        return a - b;
    }
    return b - a;
}

template <typename T>
void swap(deque<T>& a, deque<T>& b) {
    a.swap(b);
}

#endif //MY_DEQUE_DEQUE_H