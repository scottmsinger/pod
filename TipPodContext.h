//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodContext.h 39191 2014-01-23 18:01:37Z miker $ 
//******************************************************************************

#ifndef __TIPPODCONTEXT_H__
#define __TIPPODCONTEXT_H__

#include <map>
#include "TipPodNode.h"

namespace TipPod {

// *****************************************************************************
//
// Instances of this class represent a context within which to evaluate
// the final value of a TipPodNode.
//
class TipPodContext
{
public:
    TipPodContext(const TipPodNode* node);
    virtual ~TipPodContext();

protected:
    const TipPodNode* m_node;
};


}  //  End namespace TipPod

#endif    // End #ifndef __TIPPODCONTEXT_H__
