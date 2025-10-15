#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include "types.hpp"

class ArgParser
{
public:
    static bool parse_args(int argc, char *argv[], Settings &settings, Image &image);
};

#endif