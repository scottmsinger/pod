//******************************************************************************
// Copyright (c) 2013 Tippett Studio. All rights reserved.
// $Id: main.cpp 39710 2014-03-14 23:45:53Z miker $ 
//******************************************************************************

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "TipPod.h"
#include "parser.h"
#include "lexer.h"

using namespace TipPod;

// *****************************************************************************
int main(int argc, char **argv)
{
    PodNode* rootNode = NULL;
    try
    {
#if YYDEBUG
        extern int yydebug;
        yydebug = 0;
#endif

        for (int i = 1; i < argc; ++i)
        {
            rootNode = parseFile(argv[i]);
            std::cerr << "---------------------------------" << std::endl;
            rootNode->dump(std::cout);
            std::cerr << "---------------------------------" << std::endl;
            std::cerr << rootNode->asString() << std::endl;
            delete rootNode;
            rootNode = NULL;
            std::cerr << "---------------------------------" << std::endl;
        }

//         if (argc < 2 || argc > 3)
//         {
//             printf("Usage: %s in_filename [out_filename]\n", argv[0]);
//             exit(0);
//         }
// 
//         PodNode* rootNode = parseFile(argv[1]);
// 
//         rootNode->dump(std::cout);
// 
//         if (argc == 3)
//         {
//             std::ofstream file;
//             file.open(argv[2]);
//             rootNode->write(file);
//             file.close();
//         }
//         
//         delete rootNode;
    }
    catch (const std::exception& e)
    {
        delete rootNode;
        rootNode = NULL;
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
