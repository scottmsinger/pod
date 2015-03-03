//******************************************************************************
// Copyright (c) 2013 Tippett Studio. All rights reserved.
// $Id: TipPod.cpp 41879 2014-11-15 01:17:57Z miker $ 
//******************************************************************************

#include <cassert>
#include <cstring>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <limits>
#include <iostream>
#include <sstream>

#include "lexer.h"
#include "parser.h"

#include "TipPod.h"
#include "TipPodNode.h"
#include "TipPodBlockPodValue.h"
#include "TipPodUtils.h"
#include "LexerContext.h"

extern const char* TipPod_VERSIONTAG;

namespace TipPod {


#warning TODO: Move these to static factory methods on the TipPod::PodNode class

// *****************************************************************************
PodNode* parseFile(const std::string& filename)
{ 
    if (filename.empty())
    {
        return NULL;
    }

#if YYDEBUG
    extern int yydebug;
    const int yydebug_prev = ::yydebug;
#endif

    FILE *pod_in = fopen(filename.c_str(), "r");
    if (!pod_in)
    {
        throw std::runtime_error(strerror(errno));
    }
    try
    {
#if YYDEBUG
        ::yydebug = bool(getenv("TIP_POD_VERBOSE_DEBUG"));
#endif

        // Build and init scanner (i.e. lexer, i.e. tokenizer)
        yyscan_t scanner;
        yylex_init(&scanner);

        // Initialize the context struct
        LexerContext ctx;
        ctx.sourcefile = filename;
        yylex_init_extra(&ctx, &scanner);

        // Tell the lexer to get its input from a FILE* other than stdin
        yyset_in(pod_in, scanner);


        // Start the parser
        const int result = yyparse(scanner, &ctx);

        // These should always be true--if we encountered a parse error,
        // we would have thrown past this.
        assert(result == 0);
        assert(ctx.stack.size() == 0);

        //
        // Parsing has already occurred, and the results are stored in ctx.
        // So we contruct a thin wrapper around what we found in the file to 
        // return it in.
        //
        BlockPodValue* rootBlock = new BlockPodValue(ctx.current.second, ctx.current.first);
        PodNode* rootNode = new PodNode("", "", rootBlock);
        rootNode->setSource(filename, 0);

        // Clean up
        yylex_destroy(scanner);
#if YYDEBUG
        ::yydebug = yydebug_prev;
#endif
        return rootNode;
    }
    catch (...)
    {
#if YYDEBUG
        ::yydebug = yydebug_prev;
#endif
        fclose(pod_in);
        throw;
    }
}


// *****************************************************************************
PodNode* parseText(const std::string& text, const std::string& source)
{ 
    // NOTE: This is currently a hack--it writes the given text to
    // a tempfile and parses that.  Getting flex to read directly 
    // from a string is a bit more work...

    if (text.empty())
    {
        return NULL;
    }

#if YYDEBUG
    extern int yydebug;
    const int yydebug_prev = ::yydebug;
#endif

    FILE *pod_in = tmpfile();
    if (!pod_in)
    {
        throw std::runtime_error(strerror(errno));
    }
    try
    {
#if YYDEBUG
        ::yydebug = bool(getenv("TIP_POD_VERBOSE_DEBUG"));
#endif
        
        if (fwrite(text.c_str(), text.size(), 1, pod_in) != 1)
        {
            throw std::runtime_error("Failed to write temp file");
        }
        fflush(pod_in);
        fseek(pod_in, 0L, SEEK_SET);

        // Build and init scanner (i.e. lexer, i.e. tokenizer)
        yyscan_t scanner;
        yylex_init(&scanner);

        // Initialize the context struct
        LexerContext ctx;
        ctx.sourcefile = source;
        yylex_init_extra(&ctx, &scanner);

        // Tell the lexer to get its input from a FILE* other than stdin
        yyset_in(pod_in, scanner);

        // Start the parser
        const int result = yyparse(scanner, &ctx);

        // These should always be true--if we encountered a parse error,
        // we would have thrown past this.
        assert(result == 0);
        assert(ctx.stack.size() == 0);

        //
        // Parsing has already occurred, and the results are stored in ctx.
        // So we contruct a thin wrapper around what we found in the file to 
        // return it in.
        //
        BlockPodValue* rootBlock = new BlockPodValue(ctx.current.second, ctx.current.first);
        PodNode* rootNode = new PodNode("", "", rootBlock);
        rootNode->setSource(source, 0);

        // Clean up
        yylex_destroy(scanner);
#if YYDEBUG
        ::yydebug = yydebug_prev;
#endif
        return rootNode;
    }
    catch (...)
    {
#if YYDEBUG
        ::yydebug = yydebug_prev;
#endif
        fclose(pod_in);
        throw;
    }
}



// *****************************************************************************
PodNode* parseEnviron(char** env, bool inferDataTypes)
{
    PodNodeDeque envNodeVec;

    for (size_t i = 0; env[i] != 0; ++i)
    {
        std::string nameValuePair = env[i];
        size_t splitLoc = nameValuePair.find('=');
        std::string name = nameValuePair.substr(0, splitLoc);
        std::string value = (splitLoc < nameValuePair.length() - 1) ?
                                nameValuePair.substr(splitLoc + 1, nameValuePair.length() - splitLoc - 1) :
                                std::string();
        int tmpInt;
        float tmpFloat;
        if (inferDataTypes && stringToInt(value, tmpInt))
        {
            PodNode* node = new PodNode(name.c_str(), "", new IntPodValue(tmpInt));
            envNodeVec.push_back(node);
        }
        else if (inferDataTypes && stringToFloat(value, tmpFloat))
        {
            PodNode* node = new PodNode(name.c_str(), "", new FloatPodValue(tmpFloat));
            envNodeVec.push_back(node);
        }
        else
        {
            PodNode* node = new PodNode(name.c_str(), "", new StringPodValue(value));
            envNodeVec.push_back(node);
        }
    }

    BlockPodValue* envBlock = new BlockPodValue(envNodeVec);
    PodNode* envNode = new PodNode("", "", envBlock);
    envNode->setSource("environ", 0);

    return envNode;
}


// *****************************************************************************
const char* svnVersion()
{
    return ::strstr(::TipPod_VERSIONTAG, "SVN");
}


}  //  End namespace TipPod

