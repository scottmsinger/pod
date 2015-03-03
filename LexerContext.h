//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: LexerContext.h 41025 2014-08-21 16:23:34Z miker $ 
//******************************************************************************

#ifndef __LEXERCONTEXT_H__
#define __LEXERCONTEXT_H__

#include <string>
#include <stack>

#include "TipPodNode.h"

namespace TipPod {

// 
// This will be available as global 'state' during parsing.  It is created
// and passed to yyparse() in TipPod::parseFile().
// 
class LexerContext
{
public:
    LexerContext() : current(), stack(), parent(NULL), sourcefile() {}

    // <user-specified block "scope type", nodes in block>
    typedef std::pair<std::string, TipPod::PodNodeDeque> BlockScope;

    BlockScope             current; // Current block context
    std::stack<BlockScope> stack;   // Stack of enclosing block contexts

    TipPod::PodNode* parent;

    std::string sourcefile;
};


}  //  End namespace TipPod

#endif    // End #ifndef __LEXERCONTEXT_H__
