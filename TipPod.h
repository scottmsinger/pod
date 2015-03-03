//******************************************************************************
// Copyright (c) 2013 Tippett Studio. All rights reserved.
// $Id: TipPod.h 41879 2014-11-15 01:17:57Z miker $ 
//******************************************************************************

#ifndef __TIPPOD_H__
#define __TIPPOD_H__

#include <string>
#include <unistd.h>

#include "TipPodNode.h"

#define TIPPOD_VERSION_MAJOR  0
#define TIPPOD_VERSION_MINOR  1
#define TIPPOD_VERSION_PATCH  0

#define TIPPOD_VERSION (10000 * TIPPOD_VERSION_MAJOR + \
                          100 * TIPPOD_VERSION_MINOR + \
                                TIPPOD_VERSION_PATCH)

// Magic macros to make TIPPOD_VERSION_STRING that looks like "1.2.3"
#define TIPPOD_MAKE_VERSION_STRING2(a,b,c) #a "." #b "." #c
#define TIPPOD_MAKE_VERSION_STRING(a,b,c) TIPPOD_MAKE_VERSION_STRING2(a,b,c)
#define TIPPOD_VERSION_STRING \
    TIPPOD_MAKE_VERSION_STRING(TIPPOD_VERSION_MAJOR, TIPPOD_VERSION_MINOR, TIPPOD_VERSION_PATCH)

#define POD_FLOAT_TYPE float
#define POD_INT_TYPE   int

// Needed on some platforms
extern char** environ;

namespace TipPod {

// Parse the given file.  Returns a PodNode whose name and semantic type are
// both "", and whose value is a BlockPodValue containing all the 
// nodes in the file.
// Throws on error.
PodNode* parseFile(const std::string& filename);


// Parse the given text.  Returns a PodNode whose name and semantic type are
// both "", and whose value is a BlockPodValue containing all the 
// nodes in the file.
// Throws on error.
PodNode* parseText(const std::string& text, const std::string& source="");


// Parse the given environment.  Returns a PodNode whose name and semantic 
// type are both "", and whose value is a BlockPodValue containing all the 
// variables in the environment.  If inferDataTypes is true, values will
// be converted to int or float if possible; otherwise, all values will
// be strings.
// Throws on error.
PodNode* parseEnviron(char** env=environ, bool inferDataTypes=false);


// Return the SVN version as a string.
const char* svnVersion();

}  //  End namespace TipPod


#endif    // End #ifndef __TIPPOD_H__


