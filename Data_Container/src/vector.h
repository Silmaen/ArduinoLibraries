/**
 * \author argawaen
 * \date 18/01/2021
 *
 */

#pragma once
#include "iterator.h"

namespace astd {

    template<class T>
    struct vectorIterator;
    template<class T>
    struct vectorReverseIterator;

    template<class T>
    class vector {
    public:
        using value_type             = T;
        using size_type              = a_size_t;
        using difference_type        = a_diff_t;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        using pointer                = value_type *;
        using const_pointer          = const value_type *;
        using iterator               = vectorIterator<T>;
        using const_iterator         = const vectorIterator<T>;
        using reverse_iterator       = vectorReverseIterator<T>;
        using const_reverse_iterator = const vectorReverseIterator<T>;

        vector()                   = default;
        vector(const vector &)     = default;
        vector(vector &&) noexcept = default;
        inline auto operator=(const vector &) -> vector & = default;
        inline auto operator=(vector &&) noexcept -> vector & = default;
        virtual ~vector();

        // Element access
        // at
        reference       at(size_type __n) {
            if (empty())
                return value_type();
            if (__n >= _size)
                return (*this)[_size-1];
            return (*this)[__n];
        }
        const_reference at(size_type __n) const {
            if (empty())
                return value_type();
            if (__n >= _size)
                return (*this)[_size-1];
            return (*this)[__n];
        }
        // operator[]
        reference       operator[](size_type __n) ASTD_NOEXCEPT {return data[__n];}
        const_reference operator[](size_type __n) const ASTD_NOEXCEPT {return data[__n];}
        // front
        reference       front() {
            if (empty())
                return value_type();
            return (*this)[0];
        }
        const_reference front() const {
            if (empty())
                return value_type();
            return (*this)[0];
        }
        // back
        reference       back() {
            if (empty())
                return value_type();
            return (*this)[_size-1];
        }
        const_reference back() const {
            if (empty())
                return value_type();
            return (*this)[_size-1];
        }
        // data c++11

        // Iterators
        // begin
        iterator       begin() ASTD_NOEXCEPT { return iterator{this}; }
        const_iterator begin() const ASTD_NOEXCEPT { return const_iterator{this}; }
        // end
        iterator       end() ASTD_NOEXCEPT { return iterator{this, _size}; }
        const_iterator end() const ASTD_NOEXCEPT { return const_iterator{this, _size}; }
        // rbegin
        reverse_iterator       rbegin() ASTD_NOEXCEPT { return reverse_iterator{this}; }
        const_reverse_iterator rbegin() const ASTD_NOEXCEPT { return const_reverse_iterator{this}; }
        // rend
        reverse_iterator       rend() ASTD_NOEXCEPT { return reverse_iterator{this, _size}; }
        const_reverse_iterator rend() const ASTD_NOEXCEPT { return const_reverse_iterator{this, _size}; }
#if __cplusplus >= 201103L
        // cbegin c++11
        const_iterator cbegin() const ASTD_NOEXCEPT { return const_iterator{this}; }
        // cend c++11
        const_iterator cend() const ASTD_NOEXCEPT { return const_iterator{this, _size}; }
        // crbegin c++11
        const_reverse_iterator crbegin() const ASTD_NOEXCEPT { return const_reverse_iterator{this}; }
        // crend c++11
#endif

        // Capacity
        // empty
        ASTD_NODISCARD bool empty() const ASTD_NOEXCEPT { return _size == 0; }
        // size
        size_type size() const ASTD_NOEXCEPT { return _size; }
        // max_size
        // reserve
        // capacity
        size_type capacity() const ASTD_NOEXCEPT { return _allocated; }
        // shrink_to_fit
#if __cplusplus >= 201103L
        void shrink_to_fit();
#endif

        // Comparison
        ASTD_NODISCARD inline bool operator==(const vector &other) const {
            if (other._size != _size)
                return (false);
            for (size_type i = 0; i < _size; ++i)
                if (data[i] != other.data[i])
                    return (false);
            return (true);
        }
        ASTD_NODISCARD inline bool operator!=(const vector &other) const {
            if (other._size != _size)
                return (true);
            for (size_type i = 0; i < _size; ++i)
                if (data[i] != other.data[i])
                    return (true);
            return (false);
        }

    private:
        size_type _size{0};
        size_type _allocated{0};
        pointer   data{nullptr};
    };

    template<class T>
    struct vectorIterator {
        using value_type    = T;
        using pointer       = value_type *;
        using reference     = value_type &;
        using size_type     = a_size_t;
        using contener_type = vector<T>;
        friend class vector<T>;
        explicit vectorIterator(contener_type *related, size_type pos = 0) : position{pos}, _ref{related} {
            if (pos > related->size())
                position = related->size();
        }

    private:
        size_type      position;
        contener_type *_ref;
    };

    template<class T>
    struct vectorReverseIterator {
        using value_type    = T;
        using pointer       = value_type *;
        using reference     = value_type &;
        using size_type     = a_size_t;
        using contener_type = vector<T>;
        friend class vector<T>;
        explicit vectorReverseIterator(contener_type *related, size_type pos = 0) : position{pos}, _ref{related} {
            if (pos > related->size())
                position = related->size();
        }

    private:
        size_type      position;
        contener_type *_ref;
    };

} // namespace astd
