#include "kp19pp.hpp"
#include "common.hpp"

namespace kp19pp{
    string_iter_pair_type::string_iter_pair_type() :
        base_type(dummy_strage().begin(), dummy_strage().end())
    {}

    string_iter_pair_type::string_iter_pair_type(const string_iter_pair_type &other) :
        base_type(other)
    {}
        
    string_iter_pair_type::string_iter_pair_type(string_iter_pair_type &&other) :
        base_type(other)
    {}
        
    string_iter_pair_type::string_iter_pair_type(
        scanner_string_type::const_iterator first,
        scanner_string_type::const_iterator last
    ) :
        base_type(first, last)
    {}

    string_iter_pair_type &string_iter_pair_type::operator =(const string_iter_pair_type &other){
        base_type::operator =(other);
        return *this;
    }

    string_iter_pair_type &string_iter_pair_type::operator =(std::pair<scanner_string_type::const_iterator, scanner_string_type::const_iterator> pair){
        base_type::operator =(pair);
        return *this;
    }

    bool string_iter_pair_type::operator ==(const string_iter_pair_type &other) const{
        if(size() != other.size()){ return false; }
        for(auto iter = begin(), other_iter = other.begin(); iter != end(); ++iter, ++other_iter){
            if(*iter != *other_iter){ return false; }
        }
        return true;
    }

    scanner_string_type &string_iter_pair_type::dummy_strage(){
        static scanner_string_type dummy;
        return dummy;
    }

    std::size_t string_iter_pair_type::hash::operator ()(const string_iter_pair_type &item) const{
        std::size_t h = 0;
        for(auto iter = item.begin(), end = item.end(); iter != end; ++iter){
            hash_combine(h, *iter);
        }
        return h;
    }
}
