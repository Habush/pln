/** Counter.h --- 
 *
 * Copyright (C) 2011 OpenCog Foundation
 *
 * Author: Nil Geisweiller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef _OPENCOG_COUNTER_H
#define _OPENCOG_COUNTER_H

#include <map>
#include <initializer_list>
#include <boost/operators.hpp>
#include <opencog/util/foreach.h>

namespace opencog {

/**
 * Class that mimics python Counter container
 */

template<typename T, typename CT>
struct Counter : public std::map<T, CT>,
    boost::addable<Counter<T, CT>>
{
    typedef std::map<T, CT> super;
    typedef typename super::value_type value_type;
    // this will be replaced by C++11 constructor delegation instead
    // of init
    template<typename IT>
    void init(IT from, IT to) {
        while(from != to) {
            operator[](*from) += 1;  // we don't use ++ to put the
                                     // least assumption on on CT
            ++from;
        }
    }
    Counter() {}
    template<typename IT>
    Counter(IT from, IT to) {
        init(from, to);
    }
    template<typename Container>
    Counter(const Container& c) {
        init(c.begin(), c.end());
    }
    Counter(const std::initializer_list<value_type>& il) {
        foreach(const auto& v, il)
            operator[](v.first) = v.second;
    }

    // add 2 counters, for example
    // c1 = {'a':1, 'b':1}
    // c2 = {'b':1, 'c':3}
    // after
    // c1 += c2
    // now
    // c1 = {'a':1, 'b':2, 'c':3}
    Counter& operator+=(const Counter& other) {
        foreach(const auto& v, other)
            operator[](v.first) += v.second;
        return *this;
    }
    /// @todo add method to subtract, multiply, etc Counters, or
    /// scalar and Counter, etc...
};

} // ~namespace opencog

#endif // _OPENCOG_COUNTER_H
