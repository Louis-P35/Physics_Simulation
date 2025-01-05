#include "StringUtils.hpp"

#include "iostream"

bool StringUtils::match(const std::string& str, const std::string& pattern)
{
    if (str.size() == 0 && pattern.size() == 0)
    {
        return true;
    }

    if ((str.size() > 0 && pattern.size() > 0) && str[0] == pattern[0])
    {
        return match(str.substr(1), pattern.substr(1));
    }
    else if (pattern.size() > 0 && pattern[0] == '*')
    {
        for (int i = 0; i <= str.size(); ++i)
        {
            if (match(str.substr(i), pattern.substr(1)))
            {
                return true;
            }
        }

        return false;
    }

    return false;
}