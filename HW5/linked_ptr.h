#pragma once

#include <cstdio>
#include <utility>

using std::swap;

template<typename T>
struct linked_ptr {

private:
    T *ptr;
    linked_ptr *next;
    linked_ptr *prev;

    void change() noexcept {
        if (ptr == nullptr || (prev == nullptr && next == nullptr)) {
            return;
        }
        if (prev != nullptr) {
            prev->next = this;
        }
        if (next != nullptr) {
            next->prev = this;
        }
    }

public:
    linked_ptr() noexcept : ptr(nullptr), next(nullptr), prev(nullptr) {}

    explicit linked_ptr(T *_ptr) : ptr(_ptr), next(nullptr), prev(nullptr) {}

    linked_ptr(linked_ptr const &other) noexcept : ptr(other.ptr),
                                                   next(other.next), prev(const_cast<linked_ptr *>(&other)) {
        change();
    }

    linked_ptr(linked_ptr &&other) noexcept : ptr(other.ptr),
                                              next(other.next), prev(&other) {
        change();
    }

    ~linked_ptr() {
        if (ptr != nullptr) {
            if (prev == nullptr && next == nullptr) {
                delete ptr;
            } else {
                if (next != nullptr) {
                    next->prev = prev;
                }
                if (prev != nullptr) {
                    prev->next = next;
                }
            }
        }
    }

    linked_ptr &operator=(linked_ptr &&other) noexcept {
        linked_ptr tmp(other);
        swap(tmp);
        return *this;
    }


    linked_ptr &operator=(linked_ptr const &other) noexcept {
        linked_ptr tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(linked_ptr &other) noexcept {
        if (other.ptr == ptr) {
            return;
        }
        std::swap(ptr, other.ptr);
        std::swap(next, other.next);
        std::swap(prev, other.prev);
        change();
        other.change();
    }

    friend void swap(linked_ptr &x, linked_ptr &y) noexcept;

    T *get() const noexcept {
        return ptr;
    }

    T const &operator*() const noexcept {
        return *ptr;
    }

    T const &operator->() const noexcept {
        return &ptr;
    }

    explicit operator bool() const noexcept {
        return (ptr != nullptr);
    }
};

template<typename T>
void swap(linked_ptr<T> &x, linked_ptr<T> &y) noexcept {
    x.swap(y);
}
