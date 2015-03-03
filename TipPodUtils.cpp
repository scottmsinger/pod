//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodUtils.cpp 40764 2014-07-31 19:07:31Z miker $ 
//******************************************************************************

#include <errno.h>
#include <limits>
#include <sstream>
#include <cstdlib>

#include "TipPodUtils.h"

namespace TipPod {


// *****************************************************************************
bool stringToFloat(const std::string& str, float& result)
{
    if (str.empty()) return false;

    char* endPtr = 0;
    errno = 0;

    result = std::strtof(str.c_str(), &endPtr);
    if (errno == ERANGE)
    {
        return false;
    }

    return endPtr == str.c_str() + str.size(); // ensure the whole string was parsed
}


// *****************************************************************************
bool stringToInt(const std::string& str, int& result)
{
    if (str.empty()) return false;

    char* endPtr = 0;
    errno = 0;

    const int base = 0; // "auto"; 0xNNN=16, 0NNN=8, NNN=10
    const long longval = std::strtol(str.c_str(), &endPtr, base);
    if (errno == ERANGE)
    {
        return false;
    }
    if (longval <= std::numeric_limits<int>::min()
        || longval >= std::numeric_limits<int>::max())
    {
        return false;
    }

    result = static_cast<int>(longval);
    return endPtr == str.c_str() + str.size(); // ensure the whole string was parsed
}


// *****************************************************************************
std::vector<std::string> splitlines(const std::string &s)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item))
    {
        elems.push_back(item);
    }
    return elems;
}




}  //  End namespace TipPod
