#ifndef KP19PP_COMMON_HPP_
#define KP19PP_COMMON_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <boost/range/iterator_range.hpp>

namespace kp19pp{
    typedef std::vector<char> scanner_string_type;
    class string_iter_pair_type : public boost::iterator_range<scanner_string_type::const_iterator>{
    private:
        typedef boost::iterator_range<scanner_string_type::const_iterator> base_type;

    public:
        string_iter_pair_type();
        string_iter_pair_type(const string_iter_pair_type &other);
        string_iter_pair_type(string_iter_pair_type &&other);
        string_iter_pair_type(scanner_string_type::const_iterator first, scanner_string_type::const_iterator last);
        bool is_epsilon() const;
        std::string to_string() const;
        string_iter_pair_type &operator =(const string_iter_pair_type &other);
        string_iter_pair_type &operator =(std::pair<scanner_string_type::const_iterator, scanner_string_type::const_iterator>);
        bool operator ==(const string_iter_pair_type &other) const;
        bool operator <(const string_iter_pair_type &other) const;

    private:
        static scanner_string_type &dummy_strage();

    public:
        struct hash{
            std::size_t operator ()(const string_iter_pair_type &item) const;
        };
    };

    std::ostream &operator <<(std::ostream&, const string_iter_pair_type&);

    std::size_t lexical_cast(std::string value);
    std::string lexical_cast(std::size_t);
}

#endif // KP19PP_COMMON_HPP_
