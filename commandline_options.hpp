#ifndef KP19PP_COMMANDLINE_OPTIONS
#define KP19PP_COMMANDLINE_OPTIONS
#include <string>

namespace kp19pp{
    class commandline_options_type{
    public:
        enum language_enum{
            language_cpp,
            language_csharp,
            language_d,
            language_java,
            language_javascript
        };

        enum indent_enum{
            indent_space,
            indent_space4,
            indent_space8,
            indent_tab
        };

        commandline_options_type();
        bool get(int argc, char **argv);
        const std::string &ifile_path() const;
        std::string ifile_name() const;
        const std::string &ofile_path() const;
        std::string ofile_name() const;
        language_enum language() const;
        indent_enum indent() const;
        bool time() const;
        bool alltime() const;
        bool log() const;

    private:
        std::string file_name(const std::string &str) const;
        std::string ifile_path_, ofile_path_;
        language_enum language_;
        indent_enum indent_;
        bool time_, alltime_, log_;
    };
}

#endif // KP19PP_COMMANDLINE_OPTIONS
