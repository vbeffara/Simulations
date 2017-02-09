#pragma once

// Various tweaks and fixes to make compilation smoother.

// Copy of <boost/context/detail/apply.hpp> with one additional test to
// prevent defining apply again when compiling using -std=c++1z. Should be
// written more carefully because at this points it breaks compilation with
// -std=c++14, but that is not a big problem for the moment as GCC 6 works.

#ifndef BOOST_CONTEXT_DETAIL_APPLY_H
#define BOOST_CONTEXT_DETAIL_APPLY_H
#define BOOST_CONTEXT_DETAIL_INVOKE_H

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/config.hpp>

#include <boost/context/detail/config.hpp>
#include <boost/context/detail/invoke.hpp>
#include <boost/context/detail/index_sequence.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
# include BOOST_ABI_PREFIX
#endif

namespace boost {
namespace context {
namespace detail {

template< typename Fn, typename Tpl, std::size_t ... I >
auto
apply_impl( Fn && fn, Tpl && tpl, index_sequence< I ... >)
    -> decltype( std::invoke( std::forward< Fn >( fn), std::get< I >( std::forward< Tpl >( tpl) ) ... ) )
{
    return std::invoke( std::forward< Fn >( fn), std::get< I >( std::forward< Tpl >( tpl) ) ... );
}

#if _LIBCPP_STD_VER <= 14
template< typename Fn, typename Tpl >
auto
apply( Fn && fn, Tpl && tpl)
    -> decltype( apply_impl( std::forward< Fn >( fn),
                 std::forward< Tpl >( tpl),
                 make_index_sequence< std::tuple_size< typename std::decay< Tpl >::type >::value >{}) )
{
    return apply_impl( std::forward< Fn >( fn),
                       std::forward< Tpl >( tpl),
                       make_index_sequence< std::tuple_size< typename std::decay< Tpl >::type >::value >{});
}
#endif

}}}

#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_CONTEXT_DETAIL_APPLY_H