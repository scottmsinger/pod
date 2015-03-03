//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodBlockPodValue.cpp 40759 2014-07-31 00:26:29Z miker $ 
//******************************************************************************

#include "TipPodBlockPodValue.h"

namespace TipPod {


// *****************************************************************************
BlockPodValue::~BlockPodValue()
{
    for (PodNodeDeque::iterator iter = m_value.begin();
            iter != m_value.end(); ++iter)
    {
        PodNode* node = *iter;
        delete node; 
        node = NULL;
    }
    m_value.clear();
}


// *****************************************************************************
void BlockPodValue::write(std::ostream& output, int indent) const
{
    if (!m_scopeType.empty()) output << m_scopeType << " ";
    output << "{" << std::endl;
    for (PodNodeDeque::const_iterator iter = m_value.begin();
            iter != m_value.end(); ++iter)
    {
        (*iter)->write(output, indent+1);
    }
    for (int i = 0; i < indent; ++i) output << "    ";
    output << "}";
}




}  //  End namespace TipPod
