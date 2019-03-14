#pragma once

#include <utility>
#include <algorithm>
#include <cstdio>
#include <memory>
#include <exception>
#include <type_traits>
#include <limits>
#include <algorithm>

template<typename F, typename ... Others>
struct variant;

struct monostate{};

constexpr bool operator<(monostate, monostate) noexcept { return false; }

constexpr bool operator>(monostate, monostate) noexcept { return false; }

constexpr bool operator<=(monostate, monostate) noexcept { return true; }

constexpr bool operator>=(monostate, monostate) noexcept { return true; }

constexpr bool operator==(monostate, monostate) noexcept { return true; }

constexpr bool operator!=(monostate, monostate) noexcept { return false; }

auto comparer = [](auto&& action) constexpr {
    return ([action](auto&& a, auto&& b) constexpr -> bool  {
        if constexpr(std::is_same_v<decltype(a), decltype(b)>){
            return action(a, b);
        }
        else
        {
            return 0;
        }
    });
};

auto less = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x < y; });
auto greater = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x > y; });
auto equal = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x == y; });
auto neq = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x != y; });
auto leq = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x <= y; });
auto geq = comparer([](auto&& x, auto&& y) constexpr -> bool  {return x >= y; });


struct bad_variant_access : public std::exception {

    bad_variant_access() noexcept : exception(), reason("la-la-la") {};

    bad_variant_access(const char *why) noexcept : exception(), reason(why) {};

    const char *what() const noexcept override {
        return reason;
    }

    ~bad_variant_access() override = default;

private:
    char const *reason;
};

template<size_t CNT>
using variant_index_t = std::conditional_t<
        (CNT < static_cast<size_t>(std::numeric_limits<short>::max())),
        short ,
        std::conditional_t<
                (CNT < static_cast<size_t>(std::numeric_limits<int>::max())),
                int,
                long>>;



template<typename T>
struct variant_size;

template<typename T>
struct variant_size<const T> : variant_size<T> {};

template<typename ... Ts>
struct variant_size<variant<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

template<typename T>
inline constexpr size_t variant_size_v = variant_size<T>::value;


template<size_t I, typename T, bool fl = false>
struct variant_alternative;

template<size_t I, typename T>
struct variant_alternative<I, const T> {
    typedef std::add_const_t<typename variant_alternative<I, T>::type> type;
};

template<size_t I, typename ... Ts>
struct variant_alternative<I, variant<Ts...>, false> {
    static_assert(I < sizeof...(Ts), "Variant index out of bounds");
    typedef typename variant_alternative<I, variant<Ts...>, true>::type type;
};


template<size_t I, typename T0, typename ... Ts>
struct variant_alternative<I, variant<T0, Ts...>, true> {
    typedef typename variant_alternative<I - 1, variant<Ts...>, true>::type type;
};

template<typename T0, typename ... Ts>
struct variant_alternative<0, variant<T0, Ts...>, true> {
    typedef T0 type;
};

template<size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;


inline constexpr size_t variant_npos = -1;


template<bool td, typename ... Ts>
struct storage {
    void reset(size_t ind) {};

    template<typename STORAGE>
    void construct_from_storage(size_t ind, STORAGE &&other) {}

    template<typename STORAGE>
    void assign_from_storage(size_t ind, STORAGE &&other) {}

    template<typename T>
    void assign_from(size_t ind, T &&other) {}
};

template<typename ... Ts>
using storage_t = storage<std::conjunction_v<std::is_trivially_destructible<Ts>...>, Ts...>;

template<typename T0, typename ... Ts>
struct storage<true, T0, Ts...> {
    union {
        T0 head;
        storage_t<Ts...> tail;
    };

    storage() noexcept {}

    storage(storage const &) = default;

    template<typename ... Args>
    constexpr explicit storage(std::integral_constant<size_t, 0>, Args &&... args) noexcept(std::is_nothrow_constructible_v<T0, Args...>)
            : head(std::forward<Args>(args)...) {}

    template<size_t I, typename ... Args>
    constexpr explicit storage(std::integral_constant<size_t, I>, Args &&... args)
        noexcept(std::is_nothrow_constructible_v<storage_t<Ts...>, std::integral_constant<size_t, I - 1>, Args...>)
            : tail(std::integral_constant<size_t, I - 1>{}, std::forward<Args>(args)...) {}

    void reset(size_t ind) noexcept {
        if (ind == 0) {
            head.~T0();
        } else {
            tail.reset(ind - 1);
        }
    }

    template<typename ... Args>
    void construct_from_args(size_t ind, Args &&... args) {
        if (ind == 0) {
            new(&head) T0(std::forward<Args>(args)...);
        } else {
            tail.construct_from_args(ind - 1, std::forward<Args>(args)...);
        }
    }

    template<typename STORAGE>
    void construct_from_storage(size_t ind, STORAGE &&other) {
        if (ind == 0) {
            new(&head) T0(std::forward<STORAGE>(other).head);
        } else {
            tail.construct_from_storage(ind - 1, std::forward<STORAGE>(other).tail);
        }
    }

    template<typename STORAGE>
    void assign_from_storage(size_t ind, STORAGE &&other) {
        if (ind == 0) {
            head = std::forward<STORAGE>(other).head;
        } else {
            tail.assign_from_storage(ind - 1, std::forward<STORAGE>(other).tail);
        }
    }

    template<typename T>
    void assign_from(size_t ind, T &&other) {
        if (ind == 0) {
            head = std::forward<T>(other);
        } else {
            tail.assign_from(ind - 1, std::forward<T>(other));
        }
    }

    ~storage() noexcept = default;
};


template<typename T0, typename ... Ts>
struct storage<false, T0, Ts...> {
    union {
        T0 head;
        storage_t<Ts...> tail;
    };

    storage() noexcept {}

    storage(storage const &) = default;

    template<typename ... Args>
    constexpr explicit storage(std::integral_constant<size_t, 0>, Args &&... args) noexcept(std::is_nothrow_constructible_v<T0, Args...>)
            : head(std::forward<Args>(args)...) {}

    template<size_t I, typename ... Args>
    constexpr explicit storage(std::integral_constant<size_t, I>, Args &&... args)
        noexcept(std::is_nothrow_constructible_v<storage_t<Ts...>, std::integral_constant<size_t, I - 1>, Args...>)
            : tail(std::integral_constant<size_t, I - 1>{}, std::forward<Args>(args)...) {}

    void reset(size_t ind) noexcept {
        if (ind == 0) {
            head.~T0();
        } else {
            tail.reset(ind - 1);
        }
    }

    template<typename ... Args>
    void construct_from_args(size_t ind, Args &&... args) {
        if (ind == 0) {
            new(&head) T0(std::forward<Args>(args)...);
        } else {
            tail.construct_from_args(ind - 1, std::forward<Args>(args)...);
        }
    }

    template<typename STORAGE>
    void construct_from_storage(size_t ind, STORAGE &&other) {
        if (ind == 0) {
            new(&head) T0(std::forward<STORAGE>(other).head);
        } else {
            tail.construct_from_storage(ind - 1, std::forward<STORAGE>(other).tail);
        }
    }

    template<typename STORAGE>
    void assign_from_storage(size_t ind, STORAGE &&other) {
        if (ind == 0) {
            head = std::forward<STORAGE>(other).head;
        } else {
            tail.assign_from_storage(ind - 1, std::forward<STORAGE>(other).tail);
        }
    }

    template<typename T>
    void assign_from(size_t ind, T &&other) {
        if (ind == 0) {
            head = std::forward<T>(other);
        } else {
            tail.assign_from(ind - 1, std::forward<T>(other));
        }
    }

    ~storage() noexcept {};
};


template<size_t I, typename STORAGE, std::enable_if_t<(I == 0), int> = 0>
constexpr decltype(auto) raw_get(STORAGE &&st) {
    return (std::forward<STORAGE>(st).head);
}

template<size_t I, typename STORAGE, std::enable_if_t<(I != 0), int> = 0>
constexpr decltype(auto) raw_get(STORAGE &&st) {
    return raw_get<I - 1>(std::forward<STORAGE>(st).tail);
}

template<bool td, typename ... Ts>
struct destroyable_storage : storage_t<Ts...> {

protected:
    using index_t = variant_index_t<sizeof...(Ts)>;
    using base = storage_t<Ts...>;

    index_t cur_type;

    constexpr void set_index(size_t ind) noexcept {
        cur_type = static_cast<index_t>(ind);
    }

public:
    destroyable_storage() = default;

    destroyable_storage(destroyable_storage const &) = default;

    destroyable_storage(destroyable_storage &&) noexcept = default;

    template<size_t I, typename ... Args>
    constexpr explicit destroyable_storage(std::integral_constant<size_t, I>, Args &&... args)
        noexcept(std::is_nothrow_constructible_v<base, std::integral_constant<size_t, I>, Args...>)
            : base(std::integral_constant<size_t, I>{}, std::forward<Args>(args)...),
              cur_type(static_cast<index_t>(I)) {}

    constexpr bool valueless_by_exception() const noexcept {
        return index() == variant_npos;
    }

    constexpr size_t index() const noexcept {
        return static_cast<size_t>(cur_type);
    }

    constexpr base &get_storage() & noexcept {
        return *this;
    }

    constexpr const base &get_storage() const & noexcept {
        return *this;
    }

    constexpr base &&get_storage() && noexcept {
        return std::move(*this);
    }

    constexpr const base &&get_storage() const && noexcept {
        return std::move(*this);
    }

    ~destroyable_storage() noexcept = default;
};

template<typename ... Ts>
struct destroyable_storage<false, Ts...> : destroyable_storage<1, Ts...> {
    using base = destroyable_storage<true, Ts...>;

    using base::base;

    destroyable_storage() = default;

    destroyable_storage(destroyable_storage const &) = default;

    destroyable_storage(destroyable_storage &&) noexcept = default;

    ~destroyable_storage() noexcept {
        if (!base::valueless_by_exception()) {
            base::reset(base::index());
        }
    }
};

template<typename ... Ts>
using destroyable_storage_t = destroyable_storage<std::conjunction_v<std::is_trivially_destructible<Ts>...>, Ts...>;

template<bool copyable, typename ... Ts>
struct copy_storage : destroyable_storage_t<Ts...> {

    using copy_base = destroyable_storage_t<Ts...>;
    using copy_base::copy_base;

    copy_storage() = default;

    copy_storage(copy_storage const &other) {
        copy_base::cur_type = other.cur_type;
        if (!copy_base::valueless_by_exception()) {
            copy_base::base::construct_from_storage(other.index(), other);
        }
    }

    copy_storage(copy_storage &&) noexcept = default;
};


template<typename ... Ts>
struct copy_storage<false, Ts...> : copy_storage<true, Ts...> {

    using copy_base = copy_storage<true, Ts...>;
    using copy_base::copy_base;

    copy_storage() = default;

    copy_storage(copy_storage const &) = delete;

    copy_storage(copy_storage &&) noexcept = default;
};

template<typename ... Ts>
using copy_storage_t = copy_storage<std::conjunction_v<std::is_copy_constructible<Ts>...>, Ts...>;

template<bool movable, typename ... Ts>
struct move_storage : copy_storage_t<Ts...> {

    using move_base = copy_storage_t<Ts...>;
    using move_base::move_base;

    move_storage() = default;

    move_storage(move_storage const &) = default;

    move_storage(move_storage &&other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>...>) {
        move_base::cur_type = other.cur_type;
        if (!move_base::valueless_by_exception()) {
            move_base::construct_from_storage(other.index(), std::move(other));
        }
    }
};

template<typename ... Ts>
struct move_storage<false, Ts...> : move_storage<true, Ts...> {
    using move_base = move_storage<true, Ts...>;

    using move_base::move_base;

    move_storage() = default;

    move_storage(move_storage const &) = default;

    move_storage(move_storage &&) = delete;
};

template<typename ... Ts>
using move_storage_t = move_storage<std::conjunction_v<std::is_move_constructible<Ts>...>, Ts...>;


template<bool move_assign, typename ... Ts>
struct move_assign_storage : move_storage_t<Ts...> {

    using move_assign_base = move_storage_t<Ts...>;
    using move_assign_base::move_assign_base;
    using move_assign_base::valueless_by_exception;
    using move_assign_base::reset;
    using move_assign_base::index;
    using move_assign_base::set_index;
    using move_assign_base::construct_from_storage;
    using move_assign_base::assign_from_storage;

    void smart_reset() noexcept {
        if (!valueless_by_exception()) {
            reset(index());
        }
        set_index(variant_npos);
    }


    move_assign_storage() = default;

    move_assign_storage(move_assign_storage const &) = default;

    move_assign_storage(move_assign_storage &&other) noexcept = default;

    move_assign_storage &operator=(move_assign_storage &&other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_move_assignable<Ts>...>) {
        if (other.valueless_by_exception()) {
            smart_reset();
        } else {
            if (index() == other.index()) {
                assign_from_storage(index(), std::move(other));
            } else {
                smart_reset();
                set_index(other.index());
                try {
                    construct_from_storage(index(), std::move(other));
                } catch (...) {
                    set_index(variant_npos);
                }
            }
        }
        return *this;
    }
};

template<typename ... Ts>
struct move_assign_storage<false, Ts...> : move_assign_storage<true, Ts...> {
    using move_assign_base = move_assign_storage<true, Ts...>;

    using move_assign_base::move_assign_base;

    move_assign_storage() = default;

    move_assign_storage(move_assign_storage const &) = default;

    move_assign_storage(move_assign_storage &&other) noexcept = default;

    move_assign_storage &operator=(move_assign_storage &&other) = delete;
};

template<typename ... Ts>
using move_assign_storage_t = move_assign_storage<std::conjunction_v<std::is_move_constructible<Ts>..., std::is_move_assignable<Ts>...>, Ts...>;

template<bool copy_assign, typename ... Ts>
struct copy_assign_storage : move_assign_storage_t<Ts...> {

    using copy_assign_base = move_assign_storage_t<Ts...>;
    using copy_assign_base::copy_assign_base;
    using copy_assign_base::index;
    using copy_assign_base::set_index;
    using copy_assign_base::smart_reset;
    using copy_assign_base::assign_from_storage;
    using copy_assign_base::construct_from_storage;

    copy_assign_storage() = default;

    copy_assign_storage(copy_assign_storage const &) = default;

    copy_assign_storage(copy_assign_storage &&other) noexcept = default;

    copy_assign_storage &operator=(copy_assign_storage &&other) noexcept = default;

    copy_assign_storage &operator=(copy_assign_storage const &other) {
        if (other.valueless_by_exception()) {
            smart_reset();
        } else {
            if (index() == other.index()) {
                assign_from_storage(index(), other);
            } else {
                smart_reset();
                set_index(other.index());
                try {
                    construct_from_storage(index(), other);
                } catch (...) {
                    set_index(variant_npos);
                    this->operator=(copy_assign_storage(other));
                }
            }
        }
        return *this;
    }
};

template<typename ... Ts>
struct copy_assign_storage<false, Ts...> : copy_assign_storage<true, Ts...> {
    using copy_assign_base = copy_assign_storage<true, Ts...>;

    using copy_assign_base::copy_assign_base;

    copy_assign_storage() = default;

    copy_assign_storage(copy_assign_storage const &) = default;

    copy_assign_storage(copy_assign_storage &&other) noexcept = default;

    copy_assign_storage &operator=(copy_assign_storage &&other) noexcept = default;

    copy_assign_storage &operator=(copy_assign_storage const &other) = delete;
};

template<typename ... Ts>
using copy_assign_storage_t = copy_assign_storage<std::conjunction_v<std::is_copy_constructible<Ts>..., std::is_copy_assignable<Ts>...>, Ts...>;


template<typename T, template<typename...> typename TT>
struct is_specialization : std::false_type {};

template<template<typename...> typename TT, typename ... Ts>
struct is_specialization<TT<Ts...>, TT> : std::true_type {};


template<typename T>
struct is_in_place_index_specialization {
    static constexpr bool value = false;
};

template<size_t I>
struct is_in_place_index_specialization<std::in_place_index_t<I>> {
    static constexpr bool value = true;
};


template<typename T>
struct single_type {
    static T f(T);
};

template<typename T, typename ... Ts>
struct type_chooser : single_type<T>, type_chooser<Ts...> {
    using type_chooser<Ts...>::f;
    using single_type<T>::f;
};

template<typename T>
struct type_chooser<T> : single_type<T> {
    using single_type<T>::f;
};

template<typename U, typename T, typename ... Ts>
using type_chooser_t = decltype(type_chooser<T, Ts...>::f(std::declval<U>()));

template<typename U, typename T, typename ... Ts>
struct is_unique {
    static constexpr size_t value = std::is_same_v<U, T> + is_unique<U, Ts...>::value;
};

template<typename U, typename T>
struct is_unique<U, T> {
    static constexpr size_t value = static_cast<const size_t>(std::is_same_v<U, T>);
};

template<typename U, typename ... Ts>
inline constexpr bool is_unique_v = is_unique<U, Ts...>::value == 1;

template<typename T, typename U, typename ... Us>
constexpr size_t get_type_ind() noexcept {
    if constexpr(std::is_same_v<T, U>) {
        return 0;
    } else {
        return 1 + get_type_ind<T, Us...>();
    }
}

template<typename T>
constexpr size_t get_type_ind() noexcept {
    return 0;
}

template<size_t I, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> &v) {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    if (v.index() != I) {
        throw bad_variant_access("Wrong variant index");
    }
    return raw_get<I>(v.get_storage());
}

template<size_t I, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const &v) {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    if (v.index() != I) {
        throw bad_variant_access("Wrong variant index");
    }
    return raw_get<I>(v.get_storage());
}

template<size_t I, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> &&v) {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    if (v.index() != I) {
        throw bad_variant_access("Wrong variant index");
    }
    return raw_get<I>(std::move(v).get_storage());
}

template<size_t I, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const &&v) {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    if (v.index() != I) {
        throw bad_variant_access("Wrong variant index");
    }
    return raw_get<I>(std::move(v).get_storage());
}


template<typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> &v) {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique variant alternative");
    return get<I>(v);
}

template<typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const &v) {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique variant alternative");
    return get<I>(v);
}

template<typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> &&v) {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique variant alternative");
    return get<I>(std::move(v));
}

template<typename T, typename ... Ts>
constexpr decltype(auto) get(variant<Ts...> const &&v) {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique variant alternative");
    return get<I>(std::move(v));
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...>* pv) noexcept {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    return ((!pv || (pv->index() != I)) ? nullptr : &get<I>(*pv));
}

template <size_t I, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...> const* pv) noexcept {
    static_assert(I < sizeof...(Ts), "Variant Index out of range");
    return ((!pv || (pv->index() != I)) ? nullptr : &get<I>(*pv));
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...>* pv) noexcept {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique varriant alternative");
    return get_if<I>(pv);
}

template <typename T, typename ... Ts>
constexpr decltype(auto) get_if(variant<Ts...> const* pv) noexcept {
    constexpr size_t I = get_type_ind<T, Ts...>();
    static_assert(is_unique_v<T, Ts...>, "T has to be unique varriant alternative");
    return get_if<I>(pv);
}

template<typename T0, typename ... Ts>
struct variant : copy_assign_storage_t<T0, Ts...> {
    using variant_base = copy_assign_storage_t<T0, Ts...>;

    using variant_base::valueless_by_exception;
    using variant_base::set_index;
    using variant_base::index;
    using variant_base::reset;
    using variant_base::construct_from_storage;
    using variant_base::assign_from;

    template<size_t I>
    using type_ind = std::integral_constant<size_t, I>;

    template<size_t I>
    using get_type = variant_alternative_t<I, variant>;

    static_assert(std::conjunction_v<std::is_object<T0>, std::is_object<Ts>...,
                          std::negation<std::is_array<T0>>, std::negation<std::is_array<Ts>>...>,
                  "Arrays, references or void are not permitted");

    template<std::enable_if_t<std::is_default_constructible_v<T0>, int> = 0>
    constexpr variant() noexcept(std::is_nothrow_default_constructible_v<T0>)
            : variant_base(type_ind<0>{}) {}

    template<typename T, typename TT = type_chooser_t<T, T0, Ts...>,
            std::enable_if_t<!std::is_same_v<std::decay_t<T>, variant>
                             && is_unique_v<TT, T0, Ts...>
                             && std::is_constructible_v<TT, T>, int> = 0>
    constexpr variant(T &&t) noexcept(std::is_nothrow_constructible_v<TT, T>)
            : variant_base(type_ind<get_type_ind<TT, T0, Ts...>()>{}, std::forward<T>(t)) {}

    template<size_t I, typename ... Args,
            std::enable_if_t<(I < sizeof...(Ts) + 1), int> = 0,
            typename TT = variant_alternative_t<I, variant>,
            std::enable_if_t<(std::is_constructible_v<TT, Args...>), int> = 0>
    constexpr explicit variant(std::in_place_index_t<I>, Args &&... args)
            : variant_base(type_ind<I>{}, std::forward<Args>(args)...) {}

    template<typename T, typename ... Args, size_t I = get_type_ind<T, T0, Ts...>(),
            std::enable_if_t<(I < sizeof...(Ts) + 1)
            && (std::is_constructible_v<T, Args...>)
            && is_unique_v<T, T0, Ts...>, int> = 0>
    constexpr explicit variant(std::in_place_type_t<T>, Args &&... args)
            : variant_base(type_ind<get_type_ind<T, T0, Ts...>()>{}, std::forward<Args>(args)...) {}

    variant(const variant &) = default;

    variant(variant &&) noexcept = default;

    variant &operator=(variant const &) = default;

    variant &operator=(variant &&) noexcept = default;

    template<size_t I, typename ... Args,
            std::enable_if_t<(I < (sizeof...(Ts) + 1)) && std::is_constructible_v<get_type<I>, Args...>, int> = 0,
            typename TT = get_type<I>>
    TT &emplace(Args &&... args) {
        if (valueless_by_exception()) {
            reset(index());
        }
        try {
            new(this) variant(std::in_place_index_t<I>{}, std::forward<Args>(args)...);
        } catch (...) {
            set_index(variant_npos);
        }
        return raw_get<I>(*this);
    }

    template<typename T, typename ... Args,
            std::enable_if_t<is_unique_v<T, T0, Ts...> && std::is_constructible_v<T, Args...>, int> = 0>
    T &emplace(Args &&... args) {
        constexpr size_t I = get_type_ind<T, T0, Ts...>();
        return emplace<I>(std::forward<Args>(args)...);
    }

    template<typename T, typename TT = type_chooser_t<T, T0, Ts...>,
            std::enable_if_t<is_unique_v<TT, T0, Ts...>
                             && std::is_constructible_v<TT, T>
                             && std::is_assignable_v<TT &, T>, int> = 0>
    variant & operator=(T &&t) noexcept(std::is_nothrow_assignable_v<TT &, T> && std::is_nothrow_constructible_v<TT, T>) {
        constexpr size_t I = get_type_ind<TT, T0, Ts...>();
        if (index() == I) {
            get<I>(*this) = std::forward<T>(t);
        } else {
            if constexpr(std::is_nothrow_constructible_v<TT, T> || !std::is_nothrow_move_constructible_v<TT>) {
                this->emplace<I>(std::forward<T>(t));
            } else {
                this->operator=(variant(std::forward<T>(t)));
            }
        }
        return *this;
    }

    void swap(variant &other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<Ts>..., std::is_nothrow_move_constructible<T0>,
                                       std::is_nothrow_swappable<Ts>..., std::is_nothrow_swappable<T0>>) {
        static_assert(std::conjunction_v<std::is_move_constructible<Ts>..., std::is_move_constructible<T0>>,
                      "All types have to be move constructible");
        static_assert(std::conjunction_v<std::is_swappable<Ts>..., std::is_swappable<T0>>,
                      "All types have to be swappable");
        swap_impl(other);
    }

    ~variant() noexcept = default;

private:
    void swap_impl(variant &other) {
        if (index() != other.index()) {
            variant tmp(std::move(*this));
            emplace_from(std::move(other));
            other.emplace_from(std::move(tmp));
        } else {
            if (!valueless_by_exception()) {
                auto swapper = [](auto &&a, auto &&b) -> void {
                    if constexpr(std::is_same_v<decltype(a), decltype(b)>) {
                        std::swap(a, b);
                    } else {
                        return;
                    }
                };
                visit(swapper, *this, other);
            }
        }
    }

    void emplace_from(variant &&other) {
        if (!valueless_by_exception()) {
            reset(index());
        }
        set_index(other.index());
        if (!other.valueless_by_exception()) {
            construct_from_storage(index(), std::move(other));
        }
    }
};


template<typename ... Ts,
        std::enable_if_t<std::conjunction_v<std::is_move_constructible<Ts>...>
                         && std::conjunction_v<std::is_move_assignable<Ts>...>, int> = 0>
void swap(variant<Ts...> &a, variant<Ts...> &b) noexcept(noexcept(a.swap(b))) {
    a.swap(b);
}

template <typename X, typename ... Us>
constexpr bool holds_alternative(const variant<Us...>& v) noexcept {
    if (v.valueless_by_exception()) {
        return false;
    }
    else {
        return v.index() == get_type_ind<X, Us...>();
    }
}

template<typename T, size_t ... dimens>
struct multi_array {
    constexpr const T& access() const {
        return data;
    }
    T data;
};


template<typename T, size_t first_dim, size_t ... rest_dims>
struct multi_array<T, first_dim, rest_dims...> {
    template<typename ... size_ts>
    constexpr const T& access(size_t first_ind, size_ts ... other_inds) const {
        return data_arr[first_ind].access(other_inds...);
    }

    multi_array<T, rest_dims...> data_arr[first_dim];
};

template<typename ... Ts>
struct many_vars;

template<size_t I, typename T, typename ... Ts>
struct get_ith_type {
    typedef typename get_ith_type<I - 1, Ts...>::type type;
};

template<typename T, typename ... Ts>
struct get_ith_type<0, T, Ts...> {
    typedef T type;
};

template<size_t I, typename ... Ts>
using get_ith_type_t = typename get_ith_type<I, Ts...>::type;

template<typename arr_type, typename varians, typename idex_seq>
struct table_impl;

template<typename ret, typename vis, typename ... vars, size_t dim_first, size_t ... dim_rest, size_t ... inds>
struct table_impl<multi_array<ret (*)(vis, vars...), dim_first, dim_rest...>, many_vars<vars...>, std::index_sequence<inds...>> {
    using cur_var = get_ith_type_t<sizeof...(inds), std::decay_t<vars>...>;

    using arr_type = multi_array<ret(*)(vis, vars...), dim_first, dim_rest...>;

    static constexpr arr_type make_table() {
        return make_many(std::make_index_sequence<variant_size_v<cur_var>>{});
    }

    template<size_t ... var_inds>
    static constexpr arr_type make_many(std::index_sequence<var_inds...>) {
        using smaller_arr = multi_array<ret(*)(vis, vars...), dim_rest...>;
        return arr_type{ make_one<var_inds, smaller_arr>()... };
    }

    template<size_t ind, typename arr>
    static constexpr arr make_one() {
        return table_impl<arr, many_vars<vars...>, std::index_sequence<inds..., ind>>::make_table();
    }
};

template<typename ret, typename vis, typename ... vars, size_t ... inds>
struct table_impl<multi_array<ret(*)(vis, vars...)>, many_vars<vars...>, std::index_sequence<inds...>> {
    using arr_type = multi_array<ret(*)(vis, vars...)>;

    static constexpr decltype(auto) invoke(vis visitor, vars ... variants) {
        return std::forward<vis>(visitor)(get<inds>(std::forward<vars>(variants))...);
    }

    static constexpr arr_type make_table() {
        return arr_type{ & invoke };
    }
};

template<typename ret, typename vis, typename ... vars>
struct gen_table {
    using func_ptr = ret(*)(vis, vars...);

    using arr_type = multi_array<func_ptr, variant_size_v<std::remove_reference_t<vars>>...>;

    static constexpr arr_type table = table_impl<arr_type, many_vars<vars...>, std::index_sequence<>>::make_table();

};

template<typename Visitor, typename ... Variants>
constexpr decltype(auto) visit(Visitor&& vis, Variants&& ... vars) {
    if ((vars.valueless_by_exception() || ...)) {
        throw bad_variant_access();
    }

    using ret_type = decltype(std::forward<Visitor>(vis)(get<0>(std::forward<Variants>(vars))...));

    constexpr auto& v_table = gen_table<ret_type, Visitor&&, Variants&&...>::table;
    auto func_ptr = v_table.access(vars.index()...);

    return (*func_ptr)(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}

template <typename ... Ts>
constexpr bool operator==(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (v.index() != w.index()) {
        return false;
    }
    if (v.valueless_by_exception()) {
        return true;
    }
    return visit(equal, v, w);
}

template <typename ... Ts>
constexpr bool operator!=(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (v.index() != w.index()) {
        return true;
    }
    if (v.valueless_by_exception()) {
        return false;
    }
    return visit(neq, v, w);
}

template <typename ... Ts>
constexpr bool operator<(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (w.valueless_by_exception()) {
        return false;
    }
    if (v.valueless_by_exception()) {
        return true;
    }
    if (v.index() == w.index()) {
        return visit(less, v, w);
    }
    else {
        return v.index() < w.index();

    }
}

template <typename ... Ts>
constexpr bool operator>(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (v.valueless_by_exception()) {
        return false;
    }
    if (w.valueless_by_exception()) {
        return true;
    }
    if (v.index() == w.index()) {
        return visit(greater, v, w);;
    }
    else {
        return v.index() > w.index();

    }
}

template <typename ... Ts>
constexpr bool operator<=(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (v.valueless_by_exception()) {
        return true;
    }
    if (w.valueless_by_exception()) {
        return false;
    }
    if (v.index() == w.index()) {
        return  visit(leq, v, w);
    }
    else {
        return v.index() < w.index();

    }
}

template <typename ... Ts>
constexpr bool operator>=(const variant<Ts...>& v, const variant<Ts...>& w) {
    if (w.valueless_by_exception()) {
        return true;
    }
    if (v.valueless_by_exception()) {
        return false;
    }
    if (v.index() == w.index()) {
        return visit(geq, v, w);
    }
    else {
        return v.index() < w.index();

    }
}