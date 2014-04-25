#include <iostream>
#include <algorithm>
#include <map>
#include <functional>
#include "common.hpp"
#include "commandline_options.hpp"

namespace kp19pp{
    namespace{
        std::map<std::string, std::function<void()>> command_map;
    }

    commandline_options_type::commandline_options_type() :
        ifile_path_(), ofile_path_(), language_(language_cpp), indent_(indent_space4), time_(false), totaltime_(false), log_(false)
    {
        command_map["-C++"] =
            command_map["-CPP"] =
            [&](){ language_ = language_cpp; };
        command_map["-HS"] =
            command_map["-HASKELL"] =
            [&](){ language_ = language_haskell; };
        command_map["-VIM"] =
            command_map["-VIMSCRIPT"] =
            [&](){ language_ = language_vimscript; };
        command_map["-INDENT=SPACE"] =
            [&](){ indent_ = indent_space; };
        command_map["-INDENT=SPACE2"] =
            [&](){ indent_ = indent_space2; };
        command_map["-INDENT=SPACE4"] =
            [&](){ indent_ = indent_space4; };
        command_map["-INDENT=SPACE8"] =
            [&](){ indent_ = indent_space8; };
        command_map["-TAB"] =
            [&](){ indent_ = indent_tab; };
        command_map["-TIME"] =
            [&](){ time_ = true; };
        command_map["-ALLTIME"] =
            [&](){ totaltime_ = true; };
        command_map["-LOG"] =
            [&](){ log_ = true; };
    }

    bool commandline_options_type::get(int argc, char *argv[]){
        int state = 0;
        for(int index = 1; index < argc; ++index){
            if(argv[index][0] == '-'){
                auto iter = command_map.find(str_to_upper(argv[index]));
                if(iter != command_map.end()){
                    (iter->second)();
                    continue;
                }
                std::cerr << "unknown options" << argv[index] << "\n";
                return false;
            }
            switch(state){
            case 0: ifile_path_ = argv[index]; ++state; break;
            case 1: ofile_path_ = argv[index]; ++state; break;
            default:
                std::cerr << "too many arguments\n";
                return false;
            }
        }
        if(state < 2){
            std::cout << "kp19pp usage: kp19pp [ -c++ | -vim | -haskell | -indent=space | -indent=tab ] ifile_name ofile_name\n";
            return false;
        }
        return true;
    }

    const std::string &commandline_options_type::ifile_path() const{
        return ifile_path_;
    }

    std::string commandline_options_type::ifile_name() const{
        return file_name(ifile_path_);
    }

    const std::string &commandline_options_type::ofile_path() const{
        return ofile_path_;
    }

    std::string commandline_options_type::ofile_name() const{
        return file_name(ofile_path_);
    }

    std::string commandline_options_type::file_name(const std::string &str) const{
        std::string ret;
        for(std::size_t i = 0, length = str.size(); i < length; ++i){
            char c = str[length - i - 1];
            if(c == '/' || c == '\\'){ break; }
            ret += c;
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }

    commandline_options_type::language_enum commandline_options_type::language() const{
        return language_;
    }

    commandline_options_type::indent_enum commandline_options_type::indent() const{
        return indent_;
    }

    bool commandline_options_type::time() const{
        return time_;
    }

    bool commandline_options_type::totaltime() const{
        return totaltime_;
    }

    bool commandline_options_type::log() const{
        return log_;
    }
}
