#pragma once

#include <cstdio>
#include <utility>

template<typename T>
struct custom_shared_ptr {

private:
    T *ptr;
    size_t *cnt;

public:
    custom_shared_ptr() noexcept : ptr(nullptr), cnt(nullptr) {};

    explicit custom_shared_ptr(T *_ptr) : ptr(_ptr), cnt(nullptr) {
        if (ptr != nullptr) {
            cnt = new size_t(1);
        }
    }

    custom_shared_ptr(custom_shared_ptr const &other) noexcept : ptr(other.ptr), cnt(other.cnt) {
        if (ptr != nullptr) {
            ++(*cnt);
        }
    }

    custom_shared_ptr(custom_shared_ptr &&other) noexcept : ptr(other.ptr), cnt(other.cnt) {
        other.ptr = nullptr;
        other.cnt = nullptr;
    }

    ~custom_shared_ptr() {
        if (cnt != nullptr) {
            --(*cnt);
            if (*cnt == 0) {
                delete (ptr);
                delete (cnt);
            }
        }
    }

    custom_shared_ptr &operator=(custom_shared_ptr &&other) noexcept {
        custom_shared_ptr tmp(other);
        swap(tmp);
        return *this;
    }


    custom_shared_ptr &operator=(custom_shared_ptr const &other) noexcept {
        custom_shared_ptr tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(custom_shared_ptr &other) noexcept {
        std::swap(cnt, other.cnt);
        std::swap(ptr, other.ptr);
    }

    friend void swap(custom_shared_ptr &x, custom_shared_ptr &y) noexcept;

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
void swap(custom_shared_ptr<T> &x, custom_shared_ptr<T> &y) noexcept {
    x.swap(y);
}
