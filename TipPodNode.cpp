//******************************************************************************
// Copyright (c) 2013 Tippett Studio. All rights reserved.
// $Id: TipPodNode.cpp 41077 2014-08-25 19:05:53Z miker $ 
//******************************************************************************

#include <cassert>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <iostream>
#include <sstream>

#include "TipPod.h"
#include "TipPodValue.h"
#include "TipPodBlockPodValue.h"
#include "TipPodExc.h"
#include "TipPodUtils.h"

namespace TipPod {


// *****************************************************************************
PodNode::PodNode(const std::string& podName, 
                 const std::string& podType,
                 PodValue* value)
        : m_podName(podName),
          m_podType(podType),
          m_value(value),
          m_parent(NULL),
          m_sourcefile(),
          m_sourceline(-1)
{
    if (m_value)
    {
        syncBlock();
    }
}


// *****************************************************************************
PodNode::~PodNode()
{
    delete m_value;
    m_value = NULL;
}


// *****************************************************************************
bool PodNode::isValid() const
{
    return (m_value && m_value->type() != UNDEFINED) 
           || (!m_podName.empty());
}


// *****************************************************************************
PodNode::ValueType PodNode::valueType() const
{
    return m_value ? m_value->type() : UNDEFINED; 
}


// *****************************************************************************
bool PodNode::isValueType(int type) const
{
    return bool(m_value && ((type & m_value->type()) != 0));
}


// *****************************************************************************
std::string PodNode::valueTypeName() const
{
    switch (m_value ? m_value->type() : UNDEFINED)
    {
        case STRING:     return "STRING";
        case INT:        return "INT";
        case FLOAT:      return "FLOAT";
        case BOOL:       return "BOOL";
        case IDENTIFIER: return "IDENTIFIER";
        case BLOCK:      return "BLOCK";
        case EMBED:      return "EMBED";
    }
    return "UNDEFINED";
}


// *****************************************************************************
void PodNode::setValue(const PodNode& other)
{
    delete m_value; 
    if (other.valueType() != UNDEFINED)
    {
        m_value = other.m_value->copy();
    }
    else
    {
        m_value = NULL;
    }
    syncBlock();
}


// *****************************************************************************
void PodNode::setValue(PodValue* value)
{
    delete m_value;
    m_value = value;
    syncBlock();
}


// *****************************************************************************
bool PodNode::isString() const
{
    return m_value && m_value->type() == STRING;
}


// *****************************************************************************
std::string PodNode::asString() const
{
    std::ostringstream result;
    if (const StringPodValue* v = dynamic_cast<const StringPodValue*>(m_value))
    {
        result << v->value();
    }
    else if (const IntPodValue* v = dynamic_cast<const IntPodValue*>(m_value))
    {
        result << v->value();
    }
    else if (const FloatPodValue* v = dynamic_cast<const FloatPodValue*>(m_value))
    {
        result << v->value();
    }
    else if (const IdentifierPodValue* v = dynamic_cast<const IdentifierPodValue*>(m_value))
    {
        result << v->value();
    }
    else if (const EmbedPodValue* v = dynamic_cast<const EmbedPodValue*>(m_value))
    {
        result << v->value();
    }
    else if (const BlockPodValue* v = dynamic_cast<const BlockPodValue*>(m_value))
    {
        write(result);
    }
    else
    {
        throw ValueTypeError(__FUNCTION__, this);
    }
    return result.str();
}


// *****************************************************************************
void PodNode::setValue(const std::string& value)
{
    delete m_value;
    m_value = new StringPodValue(value);
}


// *****************************************************************************
bool PodNode::isNumeric() const
{
    return isValueType(INT | FLOAT);
}


// *****************************************************************************
bool PodNode::isBool() const
{
    return m_value && m_value->type() == BOOL;
}


// *****************************************************************************
bool PodNode::asBool() const
{
    if (const BoolPodValue* v = dynamic_cast<const BoolPodValue*>(m_value))
    {
        return v->value();
    }
    else if (const IntPodValue* v = dynamic_cast<const IntPodValue*>(m_value))
    {
        return static_cast<bool>(v->value());
    }
    else if (const FloatPodValue* v = dynamic_cast<const FloatPodValue*>(m_value))
    {
        return static_cast<bool>(v->value());
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setValue(bool value)
{
    delete m_value;
    m_value = new BoolPodValue(value);
}


// *****************************************************************************
bool PodNode::isInt() const
{
    return m_value && m_value->type() == INT;
}


// *****************************************************************************
int PodNode::asInt() const
{
    if (const IntPodValue* v = dynamic_cast<const IntPodValue*>(m_value))
    {
        return v->value();
    }
    else if (const FloatPodValue* v = dynamic_cast<const FloatPodValue*>(m_value))
    {
        return static_cast<int>(v->value());
    }
    else if (const BoolPodValue* v = dynamic_cast<const BoolPodValue*>(m_value))
    {
        return static_cast<int>(v->value());
    }
    else if (const StringPodValue* v = dynamic_cast<const StringPodValue*>(m_value))
    {
        int result;
        if (stringToInt(v->value(), result))
        {
            return result;
        }
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setValue(int value)
{
    delete m_value;
    m_value = new IntPodValue(value);
}


// *****************************************************************************
bool PodNode::isFloat() const
{
    return m_value && m_value->type() == FLOAT;
}


// *****************************************************************************
float PodNode::asFloat() const
{
    if (const FloatPodValue* v = dynamic_cast<const FloatPodValue*>(m_value))
    {
        return v->value();
    }
    else if (const IntPodValue* v = dynamic_cast<const IntPodValue*>(m_value))
    {
        return static_cast<float>(v->value());
    }
    else if (const StringPodValue* v = dynamic_cast<const StringPodValue*>(m_value))
    {
        float result;
        if (stringToFloat(v->value(), result))
        {
            return result;
        }
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setValue(float value)
{
    delete m_value;
    m_value = new FloatPodValue(value);
}


// *****************************************************************************
bool PodNode::isBlock() const
{
    return m_value && m_value->type() == BLOCK;
}


// *****************************************************************************
const PodNodeDeque& PodNode::asBlock() const
{
    if (const BlockPodValue* v = dynamic_cast<const BlockPodValue*>(m_value))
    {
        return v->value();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
PodNodeDeque& PodNode::asBlock()
{
    if (BlockPodValue* v = dynamic_cast<BlockPodValue*>(m_value))
    {
        return v->value();
    }
    else if (!m_value)
    {
        // Special case: User asked for value as a block, but this node has no
        // current value, so set the value to be an empty block and return it.
        m_value = new BlockPodValue;
        return asBlock();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
const std::string& PodNode::blockScopeType() const
{
    if (const BlockPodValue* v = dynamic_cast<const BlockPodValue*>(m_value))
    {
        return v->scopeType();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setBlockScopeType(const std::string& blockScopeType)
{
    if (BlockPodValue* v = dynamic_cast<BlockPodValue*>(m_value))
    {
        v->setScopeType(blockScopeType);
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setValue(PodNodeDeque& value, const std::string& blockScopeType)
{
    for (PodNodeDeque::iterator iter = value.begin();
            iter != value.end(); ++iter)
    {
        PodNode* child = (*iter);
        if (child->parent())
        {
            throw PodIntegrityError(child, "Node cannot be a member of more than one block");
        }
    }

    delete m_value;
    m_value = new BlockPodValue(value, blockScopeType);
    syncBlock();
}


// *****************************************************************************
PodNode* PodNode::childByName(const std::string& name)
{
    // TODO: Use a better data structure/algorithm.
    for (PodNodeDeque::const_iterator iter = asBlock().begin();
            iter != asBlock().end(); ++iter)
    {
        assert(*iter);
        if ((*iter)->podName() == name)
        {
            return (*iter);
        }
    }
    return NULL;
}


// *****************************************************************************
const PodNode* PodNode::childByName(const std::string& name) const 
{
    // TODO: Use a better data structure/algorithm.
    for (PodNodeDeque::const_iterator iter = asBlock().begin();
            iter != asBlock().end(); ++iter)
    {
        assert(*iter);
        if ((*iter)->podName() == name)
        {
            return (*iter);
        }
    }
    return NULL;
}


// *****************************************************************************
PodNode* PodNode::firstChildOfType(const std::string& podType)
{
    // TODO: Use a better data structure/algorithm.
    for (PodNodeDeque::const_iterator iter = asBlock().begin();
            iter != asBlock().end(); ++iter)
    {
        assert(*iter);
        if ((*iter)->podType() == podType)
        {
            return (*iter);
        }
    }
    return NULL;
}


// *****************************************************************************
const PodNode* PodNode::firstChildOfType(const std::string& podType) const
{
    // TODO: Use a better data structure/algorithm.
    for (PodNodeDeque::const_iterator iter = asBlock().begin();
            iter != asBlock().end(); ++iter)
    {
        assert(*iter);
        if ((*iter)->podType() == podType)
        {
            return (*iter);
        }
    }
    return NULL;
}


// *****************************************************************************
void PodNode::syncBlock()
{
    if (!isBlock()) return;

    //
    // We need to remove any PodNode*s from this block if they are
    // already parented to some other node.  If we don't they can 
    // get double-deleted if this node is deleted during the thrown
    // exeception, which will cause a segfault.
    //
    // Because of this, we collect all the errors found and throw
    // them at once after cleaning things up.
    //
    std::ostringstream err;
    PodNodeDeque& block = asBlock();
    PodNodeDeque::iterator iter = block.begin();
    while (iter != block.end())
    {
        PodNode* child = (*iter);
        if (!child)
        {
            err << "Invalid NULL pointer in block." << std::endl;
            block.erase(iter);
            iter = block.begin();  // erase() invalidates iter, so start over
        }
        else if (child->m_parent && child->m_parent != this)
        {
            err << child->repr() << " is already a child of " 
                << child->m_parent->repr() << std::endl;
            block.erase(iter);
            iter = block.begin();  // erase() invalidates iter, so start over
        }
        else
        {
            child->m_parent = this;
            child->syncBlock();
            ++iter;
        }
    }
    if (!err.str().empty())
    {
        throw PodIntegrityError(this, err.str());
    }
}


// *****************************************************************************
bool PodNode::isIdentifier() const
{
    return m_value && m_value->type() == IDENTIFIER;
}


// *****************************************************************************
std::string PodNode::asIdentifier() const
{
    if (const IdentifierPodValue* v = dynamic_cast<const IdentifierPodValue*>(m_value))
    {
        return v->value();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
PodNode& PodNode::setIdentifierValue(const std::string& value)
{
    delete m_value;
    m_value = new IdentifierPodValue(value);
}


// *****************************************************************************
bool PodNode::isEmbedScript() const
{
    return m_value && m_value->type() == EMBED;
}


// *****************************************************************************
std::string PodNode::asEmbedScript() const
{
    if (const EmbedPodValue* v = dynamic_cast<const EmbedPodValue*>(m_value))
    {
        return v->value();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
const std::string& PodNode::embedScriptLanguage() const
{
    if (const EmbedPodValue* v = dynamic_cast<const EmbedPodValue*>(m_value))
    {
        return v->language();
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
void PodNode::setEmbedScriptLanguage(const std::string& language)
{
    if (EmbedPodValue* v = dynamic_cast<EmbedPodValue*>(m_value))
    {
        v->setLanguage(language);
    }
    throw ValueTypeError(__FUNCTION__, this);
}


// *****************************************************************************
PodNode& PodNode::setEmbedScriptValue(const std::string& value, const std::string& language)
{
    delete m_value;
    m_value = new EmbedPodValue(value, language);
}


// *****************************************************************************
void PodNode::write(std::ostream& output, int indent) const
{
    if (!isValid()) return;

    if (valueType() == BLOCK 
        && (!parent() && m_podName.empty() && m_podType.empty()))
    {
        
        // At the top-level, don't output enclosing braces, just the
        // nodes in the block.
        for (PodNodeDeque::const_iterator iter = asBlock().begin();
                iter != asBlock().end(); ++iter)
        {
            assert(*iter);
            (*iter)->write(output, indent);
        }
    }
    else
    {
        for (int i = 0; i < indent; ++i) output << "    ";

        if (!m_podType.empty())
        {
            output << m_podType << " ";
        }
        if (!m_podName.empty())
        {
            output << m_podName;
        }
        if (m_value)
        {
            if (!m_podName.empty()) output << " = ";
            m_value->write(output, indent);
        }
        output << ";" << std::endl;
    }
}



// *****************************************************************************
std::string PodNode::repr() const
{
    std::ostringstream result;
    result << "<PodNode '" << podName() << "' at " << this << ">"; 
    return result.str();
}


// *****************************************************************************
std::ostream& operator<<(std::ostream& output, const PodNode& pv)
{
    output << pv.repr(); 
}


// *****************************************************************************
void PodNode::dump(std::ostream& output, int indent)
{
    for (int i = 0; i < indent; ++i) output << "\t";

    output << "PodNode('" << m_podName << "'";
    if (!m_podType.empty())
    {
        output << ", type='" << m_podType << "'";
    }

    if (m_value)
    {
        output << ", value=" << *m_value;
    }
    else
    {
        output << ", value=NULL";
    }

    output << ", parent='" << (m_parent ? m_parent->podName() : "NULL") << "'";


    if (const EmbedPodValue* embed = dynamic_cast<const EmbedPodValue*>(m_value))
    {
        output << ", lang='" << embed->language() << "'";
    }
    else if (isBlock())
    {
        const PodNodeDeque& children = asBlock();

        output << std::endl;
        for (PodNodeDeque::const_iterator iter = asBlock().begin();
                iter != asBlock().end(); ++iter)
        {
            assert(*iter);
            (*iter)->dump(output, indent+1);
        }
        for (int i = 0; i < indent; ++i) output << "\t";
    }
    output << " [defined in '" << m_sourcefile << "', line " << m_sourceline << "]";
    output << ")" << std::endl;
}

}  //  End namespace TipPod
