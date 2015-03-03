//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodValue.h 40764 2014-07-31 19:07:31Z miker $ 
//******************************************************************************

#ifndef __TIPPODVALUE_H__
#define __TIPPODVALUE_H__

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include "TipPodNode.h"

namespace TipPod {


// *****************************************************************************
//
// Instances of this class represent a general, untyped, rvalue in a Pod file.
//
// NOTES:
//
// * Values are 'dumb'.  They know nothing except the info they hold.
// * Values (except blocks) are invariant once created.
// * End-users are NOT allowed to create their own instances of PodValue.  All
//   end-user interaction should be through the PodNode class.
// * An instance of PodValue is allowed to be owned by exactly one PodNode.
//
class PodValue
{
public:
    friend class PodNode;

    std::string repr() const; // Convenience debug method.  Casts all types to a string 
                              // which may or may not exactly represent the real value.

    // Syntactic Pod data type, NOT user-specified semantic type
    virtual PodNode::ValueType type() const { return m_type; }

    //
    // Serialization
    //
    virtual void write(std::ostream& output, int indent=0) const = 0;

    //
    //
    // Internal and/or junk to be moved and/or removed
    //
    friend std::ostream& operator<<(std::ostream&, const PodValue&);

    virtual PodValue* copy() const = 0;  // 'virtual' copy constructor

protected:
    PodValue(PodNode::ValueType t=PodNode::UNDEFINED) 
        : m_type(t) {}
    virtual ~PodValue() {}

protected:
    PodNode::ValueType m_type;   // Syntactic Pod data type, NOT user-specified semantic type
};


// *****************************************************************************
//
// Instances of this class represent a specific, typed, value in a Pod file.
//
// ValueType    => C++ data type
// SemanticType => Pod file data type
//
template<typename ValueType, PodNode::ValueType SemanticType>
class TypedPodValue : public PodValue
{
public:
    TypedPodValue() : PodValue(SemanticType), m_value() {}
    TypedPodValue(const ValueType& v) 
            : PodValue(SemanticType), m_value(v) {}
    virtual ~TypedPodValue() {}

    virtual const ValueType& value() const { return m_value; }

    virtual void write(std::ostream& output, int indent=0) const;

    virtual PodValue* copy() const
        { 
            TypedPodValue<ValueType, SemanticType>* v = new TypedPodValue<ValueType, SemanticType>();
            v->m_value = this->m_value;
            return v;
        }

protected:
    ValueType m_value;
};


// *****************************************************************************
//
// Specialization for 'EMBED' Pod type.  Also holds language info.
//
template <>
class TypedPodValue<std::string, PodNode::EMBED> : public PodValue
{
public:
    TypedPodValue() : PodValue(PodNode::EMBED), m_value(), m_language() {}
    TypedPodValue(const std::string& text, const std::string& language)
        : PodValue(PodNode::EMBED), m_value(text), m_language(language) {}
    virtual ~TypedPodValue() {}

    virtual void write(std::ostream& output, int indent=0) const
    {
        // TODO: Deal with indents properly
        output << "<" << m_language << ">" 
               << m_value
               << "</" << m_language << ">";
    }

    virtual const std::string& value() const { return m_value; }
    virtual void setValue(const std::string& v) { m_value = v; } // TODO: remove this
    virtual const std::string& language() const { return m_language; }
    virtual void setLanguage(const std::string& l) { m_language = l; }

    virtual PodValue* copy() const
        { 
            TypedPodValue<std::string, PodNode::EMBED>* v = new TypedPodValue<std::string, PodNode::EMBED>();
            v->m_value = this->m_value;
            v->m_language = this->m_language;
            return v;
        }

    void dedent(); // Remove any common leading whitespace from every line in text.

protected:
    std::string m_value;    // Text between <xxx> and </xxx>
    std::string m_language; // Text in the leading <xxx>, sans angle brackets
};


//
// Typedefs for all possible value types (except BlockPodValue)
//
typedef TypedPodValue<std::string, PodNode::STRING>     StringPodValue;
typedef TypedPodValue<int, PodNode::INT>                IntPodValue;
typedef TypedPodValue<float, PodNode::FLOAT>            FloatPodValue;
typedef TypedPodValue<bool, PodNode::BOOL>              BoolPodValue;
typedef TypedPodValue<std::string, PodNode::IDENTIFIER> IdentifierPodValue;
typedef TypedPodValue<std::string, PodNode::EMBED>      EmbedPodValue;


}  //  End namespace TipPod


#endif    // End #ifndef __TIPPODVALUE_H__
