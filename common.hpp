#ifndef KP19PP_COMMON_HPP_
#define KP19PP_COMMON_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <utility>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <boost/range/iterator_range.hpp>

namespace kp19pp{
    namespace utf8{
        template<class Iter>
        struct iterator{
            using value_type = typename Iter::value_type;

            class utf8_exception : public std::runtime_error{
            public:
                utf8_exception() = delete;

                utf8_exception(const std::string &str) :
                    std::runtime_error(str)
                {}

                utf8_exception(const char *what_arg) :
                    std::runtime_error(what_arg)
                {}
            };

            iterator() = delete;

            iterator(const iterator &other) :
                place(other.place), first(other.first), last(other.last),
                char_count(other.char_count), line_count(other.line_count)
            {}

            iterator(const Iter &plase, const Iter &first, const Iter &last) :
                place(plase), first(first), last(last),
                char_count(0), line_count(0)
            {}

            ~iterator() = default;

            iterator &operator =(const iterator &other){
                place = other.place;
                char_count = other.char_count, line_count = other.line_count;
                return *this;
            }

            iterator &operator ++(){
                unsigned char c = static_cast<unsigned char>(*place);
                if(c >= 0x00 && c <= 0x7F){
                    if(c == '\n'){
                        char_count = 0;
                        ++line_count;
                    } else{
                        ++char_count;
                    }
                    ++place;
                } else{
                    std::size_t n;
                    if(c >= 0xC0 && c <= 0xDF){
                        n = 2;
                    } else if(c >= 0xE0 && c <= 0xEF){
                        n = 3;
                    } else if(c >= 0xF0 && c <= 0xF7){
                        n = 4;
                    } else if(c >= 0xF8 && c <= 0xFB){
                        n = 5;
                    } else if(c >= 0xFC && c <= 0xFD){
                        n = 6;
                    } else{
                        n = 0;
                        std::stringstream ss;
                        ss
                            << "(UTF-8 iterator) "
                            << static_cast<const void*>(&*place)
                            << " -> "
                            << static_cast<unsigned int>(c);
                        throw utf8_exception(ss.str());
                    }
                    place += n;
                    ++char_count;
                }
                return *this;
            }

            iterator &operator --(){
                {
                    bool first_flag = place == first;
                    --place;
                    if(first_flag){
                        --char_count;
                        return *this;
                    }
                }
                unsigned char c = static_cast<unsigned char>(*place);
                if(c >= 0x00 && c <= 0x7F){
                    if(c == '\n'){
                        char_count = 0;
                        --line_count;
                    } else{
                        --char_count;
                    }
                    return *this;
                } else{
                    c = static_cast<unsigned char>(*place);
                    while(true){
                        if(c >= 0x80 && c <= 0xBF){
                            --place;
                            c = static_cast<unsigned char>(*place);
                        } else if(c >= 0xC0 && c <= 0xFF){
                            break;
                        }
                    }
                    --char_count;
                    return *this;
                }
            }

            std::string operator *() const{
                std::string str;
                unsigned char c = static_cast<unsigned char>(*place);
                if(c >= 0x00 && c <= 0x7F){
                    str += c;
                } else{
                    std::size_t n;
                    if(c >= 0xC0 && c <= 0xDF){
                        n = 1;
                    } else if(c >= 0xE0 && c <= 0xEF){
                        n = 2;
                    } else if(c >= 0xF0 && c <= 0xF7){
                        n = 3;
                    } else if(c >= 0xF8 && c <= 0xFB){
                        n = 4;
                    } else if(c >= 0xFC && c <= 0xFD){
                        n = 5;
                    } else{
                        n = 0;
                        std::stringstream ss;
                        ss
                            << "(UTF-8 iterator) "
                            << static_cast<const void*>(&*place)
                            << " -> "
                            << static_cast<unsigned int>(c);
                        throw utf8_exception(ss.str());
                    }
                    Iter q = place;
                    str += c, ++q;
                    for(std::size_t i = 0; i < n; ++i, ++q){
                        c = *q;
                        str += c;
                    }
                }
                return std::move(str);
            }

            bool operator ==(const iterator &other) const{
                return place == other.place;
            }

            template<class Other>
            bool operator !=(const Other &other) const{
                return !(*this == other);
            }

            Iter base() const{
                return place;
            }

        private:
            Iter place, first, last;
            std::size_t char_count, line_count;

        public:
            const std::size_t &ref_char_count = char_count, &ref_line_count = line_count;
        };
    }

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

    inline std::string str_to_upper(const char *str){
        std::locale l;
        std::string ret;
        for(std::size_t i = 0; str[i]; ++i){
            ret += std::toupper(str[i], l);
        }
        return ret;
    }
}

#endif // KP19PP_COMMON_HPP_
