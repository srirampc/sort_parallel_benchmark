//----------------------------------------------------------------------------
/// @file merge_vector.hpp
/// @brief In this file have the functions for to do a stable merge of
//         ranges, in a vector
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_UTIL_MERGE_VECTOR_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_UTIL_MERGE_VECTOR_HPP

#include <boost/sort/parallel/detail/util/merge_four.hpp>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

namespace boost    {
namespace sort     {
namespace parallel {
namespace detail   {
namespace util     {
//
//
//############################################################################
//                                                                          ##
//                       F U S I O N     O F                                ##
//                                                                          ##
//              F O U R     E L E M E N T S    R A N G E                    ##
//                                                                          ##
//############################################################################
using std::iterator_traits;
//
//-----------------------------------------------------------------------------
//  function : merge_level4
/// @brief merge the ranges in the vector v_input with the full_merge4 function.
///        The v_output vector is used as auxiliary memory in the internal
///        process. The final results is in the dest range.
///        All the ranges of v_output are inside the range dest
/// @param dest : range where move the elements merged
/// @param v_input : vector of ranges to merge
/// @param v_output : vector of ranges obtained
/// @param comp : comparison object
/// @return range with all the elements moved
//-----------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Compare>
void merge_level4(range<Iter1_t> dest, 
                  std::vector<range<Iter2_t>> &v_input,
                  std::vector<range<Iter1_t>> &v_output, Compare comp)
{
    //------------------------------------------------------------------------
    typedef range<Iter1_t> range1_t;
    typedef typename iterator_traits<Iter1_t>::value_type type1;
    typedef typename iterator_traits<Iter2_t>::value_type type2;
    static_assert(std::is_same<type1, type2>::value,
                  "Incompatible iterators\n");

    //------------------------------- begin ----------------------------------
    v_output.clear();
    if (v_input.size() == 0) return;
    if (v_input.size() == 1) {
        v_output.emplace_back(init_move(dest, v_input[0]));
        return;
    };

    uint32_t nrange = v_input.size();
    uint32_t pos_ini = 0;
    while (pos_ini < v_input.size()) {
        uint32_t nmerge = (nrange + 3) >> 2;
        uint32_t nelem = (nrange + nmerge - 1) / nmerge;
        range1_t rz = full_merge4(dest, &v_input[pos_ini], nelem, comp);
        v_output.emplace_back(rz);
        dest.first = rz.last;
        pos_ini += nelem;
        nrange -= nelem;
    };
    return;
};
//
//-----------------------------------------------------------------------------
//  function : uninit_merge_level4
/// @brief merge the ranges moving the objects and constructing them in
///        uninitialized memory, in the vector v_input
///        using full_merge4. The v_output vector is used as auxiliary memory
///        in the internal process. The final results is in the dest range.
///        All the ranges of v_output are inside the range dest
///
/// @param dest : range where move the elements merged
/// @param v_input : vector of ranges to merge
/// @param v_output : vector of ranges obtained
/// @param comp : comparison object
/// @return range with all the elements moved and constructed
//-----------------------------------------------------------------------------
template <class Value_t, class Iter_t, class Compare>
void uninit_merge_level4(range<Value_t *> dest,
                         std::vector<range<Iter_t>> &v_input,
                         std::vector<range<Value_t *>> &v_output, Compare comp)
{
    //-------------------------------------------------------------------------
    typedef range<Value_t *> range1_t;
    typedef typename iterator_traits<Iter_t>::value_type type1;
    static_assert(std::is_same<type1, Value_t>::value,
                  "Incompatible iterators\n");

    //------------------------------- begin ----------------------------------
    v_output.clear();
    if (v_input.size() == 0) return;
    if (v_input.size() == 1) {
        v_output.emplace_back(uninit_move(dest, v_input[0]));
        return;
    };

    uint32_t nrange = v_input.size();
    uint32_t pos_ini = 0;
    while (pos_ini < v_input.size()) {
        uint32_t nmerge = (nrange + 3) >> 2;
        uint32_t nelem = (nrange + nmerge - 1) / nmerge;
        range1_t rz = uninit_full_merge4(dest, &v_input[pos_ini], nelem, comp);
        v_output.emplace_back(rz);
        dest.first = rz.last;
        pos_ini += nelem;
        nrange -= nelem;
    };
    return;
};
//
//-----------------------------------------------------------------------------
//  function : merge_vector4
/// @brief merge the ranges in the vector v_input using the merge_level4
///        function. The v_output vector is used as auxiliary memory in the
///        internal process
///        The final results is in the range_output range.
///        All the ranges of v_output are inside the range range_output
///        All the ranges of v_input are inside the range range_input
/// @param range_input : range including all the ranges of v_input
/// @param ange_output : range including all the elements of v_output
/// @param v_input : vector of ranges to merge
/// @param v_output : vector of ranges obtained
/// @param comp : comparison object
/// @return range with all the elements moved
//-----------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Compare>
range<Iter2_t>
    merge_vector4(range<Iter1_t> range_input, 
                  range<Iter2_t> range_output,
                  std::vector<range<Iter1_t>> &v_input,
                  std::vector<range<Iter2_t>> &v_output, Compare comp)
{
    //--------------------------------------------------------------------------
    typedef range<Iter2_t> range2_t;
    typedef typename iterator_traits<Iter1_t>::value_type type1;
    typedef typename iterator_traits<Iter2_t>::value_type type2;
    static_assert(std::is_same<type1, type2>::value,
                  "Incompatible iterators\n");

    //--------------------- code -------------------------------------------
    v_output.clear();
    if (v_input.size() == 0) {
        return range2_t(range_output.first, range_output.first);
    };
    if (v_input.size() == 1) {
        return init_move(range_output, v_input[0]);
    };
    bool sw = false;
    uint32_t nrange = v_input.size();

    while (nrange > 1) {
        if (sw) {
            merge_level4(range_input, v_output, v_input, comp);
            sw = false;
            nrange = v_input.size();
        }
        else
        {
            merge_level4(range_output, v_input, v_output, comp);
            sw = true;
            nrange = v_output.size();
        };
    };
    return (sw) ? v_output[0] : init_move(range_output, v_input[0]);
};

//****************************************************************************
}; //    End namespace util
}; //    End namespace detail
}; //    End namespace parallel
}; //    End namespace sort
}; //    End namespace boost
//****************************************************************************
//
#endif