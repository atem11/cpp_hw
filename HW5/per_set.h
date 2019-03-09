#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include "custom_shared_ptr.h"
#include "linked_ptr.h"

using namespace std;

template<typename T, template<typename> typename Ptr = custom_shared_ptr>
struct per_set {
    struct iterator;

    friend struct iterator;

    per_set() noexcept : root(Ptr<base_node>(nullptr), Ptr<base_node>(nullptr)), _size(0) {};

    per_set(per_set const &other) noexcept : root(other.root), _size(other._size) {};

    per_set(per_set &&other) noexcept : root(move(other.root)), _size(move(other._size)) {};

    ~per_set() = default;

    per_set &operator=(per_set const &other) noexcept {
        per_set tmp(other);
        swap(tmp);
        return *this;
    }

    per_set &operator=(per_set &&other) noexcept {
        swap(other);
        return *this;
    }

    iterator find(T const &v) noexcept {
        base_node *tmp = root.left.get();

        while (tmp != nullptr) {
            if (static_cast<node const *>(tmp)->data > v) {
                tmp = tmp->left.get();
            } else if (static_cast<node const *>(tmp)->data < v) {
                tmp = tmp->right.get();
            } else {
                break;
            }
        }
        if (tmp == nullptr) {
            return end();
        }
        return iterator(&root, tmp);
    }

    std::pair<iterator, bool> insert(T v) {
        iterator it = find(v);
        if (it != end()) {
            return {it, false};
        }

        vector<base_node *> prevs;
        base_node *tmp = root.left.get();

        while (tmp != nullptr) {
            prevs.push_back(tmp);
            if (static_cast<node const *>(tmp)->data > v) {
                tmp = tmp->left.get();
            } else if (static_cast<node const *>(tmp)->data < v) {
                tmp = tmp->right.get();
            }
        }

        base_node *_node = new node(v);
        it.ptr = _node;
        prevs.push_back(_node);
        reverse(prevs.begin(), prevs.end());
        base_node *new_node = _node;
        for (size_t i = 0; i < prevs.size() - 1; i++) {
            T pr = static_cast<node const *>(prevs[i + 1])->data;
            T tm = static_cast<node const *>(prevs[i])->data;
            if (pr > tm) { //left son
                new_node = new node(Ptr<base_node>(new_node), prevs[i + 1]->right, pr);
            } else {
                new_node = new node(prevs[i + 1]->left, Ptr<base_node>(new_node), pr);
            }
        }
        root.left = Ptr<base_node>(new_node);
        return {it, true};
    }

    void erase(iterator const &it) {
        if (it == end()) {
            return;
        }
        root.left = erase_impl(root.left.get(), (*it));
    }

    iterator begin() const noexcept {
        base_node const *tmp = &root;
        while (tmp->left.get() != nullptr) {
            tmp = tmp->left.get();
        }
        return iterator(&root, tmp);
    }

    iterator end() const noexcept {
        return iterator(&root, &root);
    }

    void swap(per_set &other) noexcept {
        root.left.swap(other.root.left);
        std::swap(_size, other._size);
    }

    size_t size() const noexcept {
        return _size;
    }

private:
    struct node;

    struct base_node {

        friend struct per_set;

        base_node(base_node const &other) noexcept :
                left(other.left), right(other.right) {};

        base_node(base_node &&other) noexcept :
                left(std::move(other.left)), right(std::move(other.right)) {};

        base_node(Ptr<base_node> _left, Ptr<base_node> _right) noexcept :
                left(std::move(_left)), right(std::move(_right)) {};

        base_node &operator=(base_node const &other) noexcept {
            swap(other);
            return *this;
        }

        void swap(base_node &other) noexcept {
            left.swap(other.left);
            right.swap(other.right);
        }

        friend void swap(base_node &x, base_node &y) noexcept;

    private:
        Ptr<base_node> left;
        Ptr<base_node> right;
    };

    struct node : base_node {

        friend struct per_set;

        explicit node(T _data) noexcept :
                base_node(Ptr<base_node>(nullptr), Ptr<base_node>(nullptr)), data(std::move(_data)) {};

        node(node const &other) noexcept :
                base_node(other), data(other.data) {};

        node(node &&other) noexcept : base_node(move(other)),
                                      data(move(other.data)) {}

        node(Ptr<base_node> _left, Ptr<base_node> _right, T _data) noexcept : base_node(move(_left), move(_right)),
                                                                              data(move(_data)) {}

        node &operator=(node const &other) noexcept {
            node tmp(other);
            swap(other);
            return *this;
        }

        node &operator=(node &&other) noexcept {
            swap(other);
            return *this;
        }

        void swap(node &other) noexcept {
            base_node::swap(other);
            std::swap(data, other.data);
        }

        friend void swap(node &x, node &y) noexcept;

    private:
        T data;
    };

    Ptr<base_node> erase_impl(base_node *vert, T const &v) {
        if (v < static_cast<node const *>(vert)->data) {
            return Ptr<base_node>(
                    new node(erase_impl(vert->left.get(), v), vert->right, static_cast<node const *>(vert)->data));
        } else if (v > static_cast<node const *>(vert)->data) {
            return Ptr<base_node>(
                    new node(vert->left, erase_impl(vert->right.get(), v), static_cast<node const *>(vert)->data));
        } else {
            if (!vert->left && !vert->right) {
                return Ptr<base_node>(nullptr);
            }
            if (vert->left && !vert->right) {
                return vert->left;
            }
            if (vert->right && !vert->left) {
                return vert->right;
            }
            vector<base_node *> path;
            base_node *tmp = vert->right.get();
            while (tmp != nullptr) {
                path.push_back(tmp);
                tmp = tmp->left.get();
            }
            auto last_val = static_cast<node const *>(path.back())->data;
            if (path.size() == 1) {
                return Ptr<base_node>(new node(vert->left, path.back()->right, last_val));
            }
            base_node *tmp_node = new node(path.back()->right, path[path.size() - 2]->right,
                                           static_cast<node const *>(path[path.size() - 2])->data);
            path.pop_back();
            path.pop_back();
            path.push_back(tmp_node);

            reverse(path.begin(), path.end());
            base_node *new_node = path[0];
            for (size_t i = 0; i < path.size() - 1; i++) {
                T pr = static_cast<node const *>(path[i + 1])->data;
                T tm = static_cast<node const *>(path[i])->data;
                new_node = new node(Ptr<base_node>(new_node), path[i + 1]->right, pr);
            }
            return Ptr<base_node>(new node(vert->left, Ptr<base_node>(new_node), last_val));
        }
    }

    base_node root;
    size_t _size;


};

template<typename T, template<typename> typename Ptr>
void swap(typename per_set<T, Ptr>::base_node &x, typename per_set<T, Ptr>::base_node &y) noexcept {
    x.swap(y);
}

template<typename T, template<typename> typename Ptr>
void swap(typename per_set<T, Ptr>::node &x, typename per_set<T, Ptr>::node &y) noexcept {
    x.swap(y);
}


template<typename T, template<typename> typename Ptr>
struct per_set<T, Ptr>::iterator {
    friend struct per_set<T, Ptr>;

    iterator() noexcept : root(nullptr), ptr(nullptr) {};

    T const &operator*() const noexcept {
        return (static_cast<node const *> (ptr))->data;
    }

    iterator &operator++() noexcept;

    iterator operator++(int) noexcept {
        iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    iterator &operator--() noexcept;

    iterator operator--(int) noexcept {
        iterator tmp(*this);
        --(*this);
        return tmp;
    }

    bool operator==(iterator const &other) const noexcept {
        return (root == other.root && ptr == other.ptr);
    }

    bool operator!=(iterator const &other) const noexcept {
        return !(other == (*this));
    }

private:
    iterator(base_node const *x, base_node const *y) : root(x), ptr(y) {};

    base_node const *_left(base_node *root) const noexcept;

    base_node const *_right(base_node *root) const noexcept;

    base_node const *root;
    base_node const *ptr;

};

template<typename T, template<typename> typename Ptr>
typename per_set<T, Ptr>::iterator &per_set<T, Ptr>::iterator::operator++() noexcept {
    if (root == ptr) {
        cerr << "Has no next element";
    }

    T x = static_cast<node const *>(ptr)->data;
    vector<base_node *> prevs;
    prevs.push_back(const_cast<base_node *>(root));
    base_node *tmp = root->left.get();

    while (tmp != nullptr) {
        prevs.push_back(tmp);
        if (static_cast<node const *>(tmp)->data > x) {
            tmp = tmp->left.get();
        } else if (static_cast<node const *>(tmp)->data < x) {
            tmp = tmp->right.get();
        } else {
            break;
        }
    }

    if (ptr->right.get() != nullptr) {
        ptr = _left(ptr->right.get());
    } else {
        prevs.pop_back();
        tmp = prevs.back();
        prevs.pop_back();
        while (tmp->left.get() != ptr) {
            ptr = tmp;
            tmp = prevs.back();
            prevs.pop_back();
        }
        ptr = tmp;
    }

    return *this;
}

template<typename T, template<typename> typename Ptr>
typename per_set<T, Ptr>::iterator &per_set<T, Ptr>::iterator::operator--() noexcept {
    T x = static_cast<node const *>(ptr)->data;
    vector<base_node *> prevs;
    base_node *tmp = root->left.get();

    while (tmp != nullptr) {
        prevs.push_back(tmp);
        if (static_cast<node const *>(tmp)->data > x) {
            tmp = tmp->left.get();
        } else if (static_cast<node const *>(tmp)->data < x) {
            tmp = tmp->right.get();
        } else {
            break;
        }
    }

    if (ptr->left.get() != nullptr) {
        ptr = _right(ptr->left.get());
    } else {
        prevs.pop_back();
        tmp = prevs.back();
        prevs.pop_back();
        while (tmp->right.get() != ptr) {
            ptr = tmp;
            tmp = prevs.back();
            prevs.pop_back();
        }
        ptr = tmp;
    }

    return *this;
}

template<typename T, template<typename> class Ptr>
typename per_set<T, Ptr>::base_node const *per_set<T, Ptr>::iterator::_left(base_node *root) const noexcept {
    base_node *tmp = root;
    while (tmp->left.get() != nullptr) {
        tmp = tmp->left.get();
    }
    return tmp;
}

template<typename T, template<typename> class Ptr>
typename per_set<T, Ptr>::base_node const *per_set<T, Ptr>::iterator::_right(base_node *root) const noexcept {
    base_node *tmp = root;
    while (tmp->right.get() != nullptr) {
        tmp = tmp->right.get();
    }
    return tmp;
}
