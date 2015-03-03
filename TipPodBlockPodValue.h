//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodBlockPodValue.h 40759 2014-07-31 00:26:29Z miker $ 
//******************************************************************************

#ifndef __TIPPODBLOCKPODVALUE_H__
#define __TIPPODBLOCKPODVALUE_H__

#include <fstream>
#include <deque>
#include <string>

#include "TipPodValue.h"
#include "TipPodNode.h"

namespace TipPod {


// *****************************************************************************
//
// Class to hold a block of PodNode*s
//
class BlockPodValue : public PodValue 
{
public:
    typedef std::deque<PodNode*> PodNodeDeque;

    // Default constructor
    BlockPodValue() : PodValue(PodNode::BLOCK), m_value(), m_scopeType() {}

    // Copy constructor
    BlockPodValue(const BlockPodValue& other, const std::string& scopeType="") 
        : PodValue(PodNode::BLOCK), m_value(other.m_value), m_scopeType() {}

    // TEMPORARY copy constructor
    BlockPodValue(const PodNodeDeque& other, const std::string& scopeType="") 
        : PodValue(PodNode::BLOCK), m_value(other), m_scopeType() {}

    // Destructor
    virtual ~BlockPodValue();

    virtual void write(std::ostream& output, int indent=0) const;

    virtual const PodNodeDeque& value() const { return m_value; }
    virtual PodNodeDeque& value() { return m_value; }
    virtual const std::string& scopeType() const { return m_scopeType; }
    virtual void setScopeType(const std::string& s) { m_scopeType = s; }

    virtual PodValue* copy() const
        {
            throw std::runtime_error("Blocks cannot be copied");
            return NULL;
        }


protected:  

    PodNodeDeque m_value;     // Nodes in block
    std::string m_scopeType;  // Optional type_name preceeding the 
                              // opening brace in a .pod file
};




}  //  End namespace TipPod


#endif    // End #ifndef __TIPPODBLOCKPODVALUE_H__


