#ifndef KP19PP_ITERATOR_RANGE_HPP_
#define KP19PP_ITERATOR_RANGE_HPP_

#include <boost/assert.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/type_traits/is_abstract.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/detail/safe_bool.hpp>
#include <boost/utility/enable_if.hpp>
#include <iterator>
#include <algorithm>
#include <cstddef>

namespace kp19pp{
    namespace iterator_range_detail{
        template<class IteratorT>
        struct iterator_range_impl {
            template< class ForwardRange >
            static IteratorT adl_begin( ForwardRange& r )
            {
                return static_cast<IteratorT>( boost::begin( r ) );
            }

            template< class ForwardRange >
            static IteratorT adl_end( ForwardRange& r )
            {
                return static_cast<IteratorT>( boost::end( r ) );
            }
        };

        template< class Left, class Right >
        inline bool less_than( const Left& l, const Right& r )
        {
            return std::lexicographical_compare( boost::begin(l),
                                                 boost::end(l),
                                                 boost::begin(r),
                                                 boost::end(r) );
        }

        template< class Left, class Right >
        inline bool greater_than( const Left& l, const Right& r )
        {
            return less_than(r,l);
        }
        
        template< class Left, class Right >
        inline bool less_or_equal_than( const Left& l, const Right& r )
        {
            return !iterator_range_detail::less_than(r,l);
        }
        
        template< class Left, class Right >
        inline bool greater_or_equal_than( const Left& l, const Right& r )
        {
            return !iterator_range_detail::less_than(l,r);
        }

        // This version is maintained since it is used in other boost libraries
        // such as Boost.Assign
        template< class Left, class Right >
        inline bool equal(const Left& l, const Right& r)
        {
            return boost::equal(l, r);
        }

        struct range_tag { };
        struct const_range_tag { };
    }

    template<class IteratorT>
    class iterator_range{
    protected:
        typedef iterator_range_detail::iterator_range_impl<IteratorT> impl;

    public:
        typedef iterator_range<IteratorT> type;
        typedef typename std::iterator_traits<IteratorT>::type ;

    private:
        iterator begin_, end_;
    };
}

#endif // KP19PP_ITERATOR_RANGE_HPP_
