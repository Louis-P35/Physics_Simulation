#pragma once
#include <string>


class StringUtils
{
    public:
    StringUtils() {};
    ~StringUtils() {};

    static bool match(const std::string& str, const std::string& pattern);
};