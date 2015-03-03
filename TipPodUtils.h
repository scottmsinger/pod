//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodUtils.h 40764 2014-07-31 19:07:31Z miker $ 
//******************************************************************************

#ifndef __TIPPODUTILS_H__
#define __TIPPODUTILS_H__

#include <string>
#include <vector>

namespace TipPod {


bool stringToFloat(const std::string& str, float& result);
bool stringToInt(const std::string& str, int& result);

std::vector<std::string> splitlines(const std::string &s);


}  //  End namespace TipPod


#endif    // End #ifndef __TIPPODUTILS_H__
