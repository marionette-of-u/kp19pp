#include <sstream>
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

    bool string_iter_pair_type::is_epsilon() const{
        return empty();
    }

    std::string string_iter_pair_type::to_string() const{
        return std::string(begin(), end());
    }

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

    bool string_iter_pair_type::operator <(const string_iter_pair_type &other) const{
        bool a = begin() == end(), b = other.begin() == other.end();
        if(a && !b){
            return true;
        }else if(!a && b || a && b){
            return false;
        }
        auto iter = begin(), other_iter = other.begin();
        auto last = end() - 1;
        for(; *iter == *other_iter && iter != last; ++iter, ++other_iter);
        return *reinterpret_cast<const unsigned char*>(&*iter) - *reinterpret_cast<const unsigned char*>(&*other_iter) < 0;
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

    std::ostream &operator <<(std::ostream &os, const string_iter_pair_type &range){
        for(auto iter = range.begin(), end = range.end(); iter != end; ++iter){
            os << *iter;
        }
        return os;
    }

    std::size_t lexical_cast(std::string value){
        std::size_t r;
        std::istringstream is(value);
        is >> r;
        return r;
    }

    std::string lexical_cast(std::size_t value){
        std::string r;
        std::ostringstream os;
        os << value;
        return os.str();
    }
}
