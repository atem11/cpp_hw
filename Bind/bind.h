#pragma once

#include <iostream>
#include <tuple>

template<typename T, T ... values>
struct integer_sequence {
};

template<typename T, typename Seq, T arg>
struct append;

template<typename T, T ... values, T arg>
struct append<T, integer_sequence<T, values...>, arg> {
    typedef integer_sequence<T, values..., arg> type;
};

template<typename T, T N, typename Q = void>
struct make_integer_sequence {
    typedef typename append<T, typename make_integer_sequence<T, N - 1>::type, N - 1>::type type;
};

template<typename T, T N>
struct make_integer_sequence<T, N, typename std::enable_if<N == 0>::type> {
    typedef integer_sequence<T> type;
};

template<int N>
struct placeholder {
};

template<typename F, typename ... As>
struct bind_t;

constexpr placeholder<1> _1;
constexpr placeholder<2> _2;
constexpr placeholder<3> _3;

template<typename T>
struct wrapper {
    typedef T type;
};

template<int N>
struct wrapper<placeholder<N> &> {
    typedef placeholder<N> type;
};


template<int N>
struct wrapper<placeholder<N> &&> {
    typedef placeholder<N> type;
};

template<typename F, typename ... As>
struct wrapper<bind_t<F, As...> &> {
    typedef bind_t<F, As...> type;
};


template<typename F, typename ... As>
struct wrapper<bind_t<F, As...> &&> {
    typedef bind_t<F, As...> type;
};

template<typename T>
using wrapper_t = typename wrapper<T>::type;



template<typename T, typename Q>
struct merge;

template<int ... h, int ... t>
struct merge<integer_sequence<int, h...>, integer_sequence<int, t...> > {
    typedef integer_sequence<int, h..., t...> value;
};

template<typename ... Types>
struct indexes;

template<typename T>
struct type_index {
    typedef integer_sequence<int> value;
};

template<int N>
struct type_index<placeholder<N> > {
    typedef integer_sequence<int, N> value;
};

template<typename F, typename ... As>
struct type_index<bind_t<F, As...> > {
    typedef typename indexes<std::decay_t<As>...>::value value;
};

template<typename H, typename ... T>
struct indexes<H, T...> {
    typedef typename merge<typename type_index<H>::value, typename indexes<T...>::value>::value value;
};

template<>
struct indexes<> {
    typedef integer_sequence<int> value;
};

template<typename  ... T>
using indexes_t = typename indexes<std::decay_t<T>...>::value;



template<int Ind, typename S>
struct checker;

template<int Ind, int h, int ... t>
struct checker<Ind, integer_sequence<int, h, t ...> > {
    static constexpr int value = static_cast<int>(Ind == h) + checker<Ind, integer_sequence<int, t ...> >::value;
};

template<int Ind>
struct checker<Ind, integer_sequence<int> > {
    static const int value = 0;
};

template<int Ind, typename Seq>
constexpr bool checker_t = checker<Ind, Seq>::value == 1;

template<typename T, bool uniq>
struct convert {
    typedef T &type;
};

template<typename T>
struct convert<T, true> {
    typedef T &&type;
};

template<typename T, bool uniq>
using convert_t = typename convert<T, uniq>::type;



template<typename A>
struct G {

    template<typename Ar>
    G(Ar &&a) : a(std::forward<Ar>(a)) {}

    template<typename ... Bs>
    A operator()(Bs &&...) {
        return static_cast<A>(a);
    }

    std::decay_t<A> a;
};

template<>
struct G<placeholder<1> > {

    G(placeholder<1>) {}

    template<typename B1, typename ... Bs>
    decltype(auto) operator()(B1 &&b1, Bs &&...) {
        return std::forward<B1>(b1);
    }
};

template<int N>
struct G<placeholder<N> > {

    G(placeholder<N>) {}

    template<typename B, typename ... Bs>
    decltype(auto) operator()(B &&, Bs &&... bs) {
        G<placeholder<N - 1> > next{placeholder<N - 1>()};
        return next(std::forward<Bs>(bs)...);
    }
};

template<typename F, typename ... As>
struct G<bind_t<F, As...> > {

    G(bind_t<F, As...> &&fun1) : fun(std::move(fun1)) {}

    template<typename ... Bs>
    decltype(auto) operator()(Bs &&... bs) {
        return fun(std::forward<Bs>(bs)...);
    }

private:
    bind_t<F, As...> fun;
};

template<typename F, typename ... As>
struct bind_t {

    template<typename Fr, typename ... Asr>
    bind_t(Fr &&f1, Asr &&... as)
            : f(std::forward<Fr>(f1)), gs(std::forward<Asr>(as)...) {}

    template<typename ... Bs>
    decltype(auto) operator()(Bs &&... bs) {
        return call(typename make_integer_sequence<int, sizeof...(As)>::type(),
                    typename make_integer_sequence<int, sizeof...(Bs)>::type(), std::forward<Bs>(bs)...);
    }

private:
    template<int ... ks, int ... kss, typename ... Bs>
    decltype(auto) call(integer_sequence<int, ks ...>, integer_sequence<int, kss ...>, Bs &&... bs) {
        return f(std::get<ks>(gs)(
                static_cast<convert_t<Bs, checker_t<(kss + 1), indexes_t<As...> > > >(bs)...)...);
    }

private:
    F f;
    std::tuple<G<wrapper_t<As> >...> gs;
};

template<typename F, typename ... As>
decltype(auto) bind(F &&f, As &&... as) {
    return bind_t<std::decay_t<F>, wrapper_t<std::decay_t<As> &>...>(std::forward<F>(f), std::forward<As>(as)...);
}

template<typename F, typename ... As>
decltype(auto) call_once_bind(F &&f, As &&... as) {
    return bind_t<std::decay_t<F>, wrapper_t<std::decay_t<As> &&>...>(std::forward<F>(f), std::forward<As>(as)...);
}