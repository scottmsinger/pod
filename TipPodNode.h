//******************************************************************************
// Copyright (c) 2013 Tippett Studio. All rights reserved.
// $Id: TipPodNode.h 41077 2014-08-25 19:05:53Z miker $ 
//******************************************************************************

#ifndef __TIPPODNODE_H__
#define __TIPPODNODE_H__

#include <string>
#include <deque>

namespace TipPod {

class PodValue;
class PodNode;
typedef std::deque<PodNode*> PodNodeDeque;
typedef PodNodeDeque::iterator PodNodeIter;


// *****************************************************************************
//
// A PodNode is effectively a single 'thing' in a Pod file.
//
// IMPORTANT CONCEPT:  When both an lvalue and and rvalue are present (both are
//                     optional), a PodNode encapsulates both the lvalue AND the 
//                     rvalue of the assignment ("lvalue = rvalue;").
//
// NOTES:
//
// * Can have a name, or be nameless.
// * Can have a semantic type (specified explicitly in the Pod file), or be typeless.
// * Can have a value, or be NULL.  
// * Value can be a block of additional PodNodes.
//
// * PodNode is not polymorphic (and PodValue is a separate class that is polymorphic) because
//   we want to allow a node's value AND value type to be changeable, while preserving the
//   identity of the PodNode instance itself.
//
class PodNode 
{
public:
    // 
    // Data types a PodNode's value may have.  This enum is here
    // to enable its use by clients without having access to the 
    // PodValue class.
    // 
    enum ValueType { 
                     UNDEFINED  = 1<<0, // Effectively NULL
                     STRING     = 1<<1,
                     INT        = 1<<2,
                     FLOAT      = 1<<3,
                     BOOL       = 1<<4,
                     IDENTIFIER = 1<<5,
                     BLOCK      = 1<<6,
                     EMBED      = 1<<7,
                   };

    // 
    // Constructor / Destructor
    // 
    PodNode(const std::string& podName="", 
            const std::string& podType="", 
            PodValue* value=NULL);
    virtual ~PodNode();

    bool isValid() const;

    // Node's user-specified name.  May be empty.
    const std::string& podName() const { return m_podName; }
    void setPodName(const std::string& n) { m_podName = n; }

    // Node's user-specified semantic type.  May be empty.
    const std::string& podType() const { return m_podType; }
    void setPodType(const std::string& t) { m_podType = t; }

    // Node representing the block this node was defined in.
    // Returns NULL if node is at the top level.
    PodNode* parent() const { return m_parent; }

    //
    // Getting / Setting this node's value(s)
    //
    ValueType valueType() const;
    bool isValueType(int type) const;
    std::string valueTypeName() const;
    void setValue(const PodNode& other);  // Makes a copy of other PodNode's value
    void setValue(PodValue* value);       // Takes ownership of this pointer

    // Node's value as a string.
    bool isString() const;
    std::string asString() const;   // All types are cast to string as one would expect.
    void setValue(const std::string& value);

    // Node's value as a bool.
    bool isBool() const;
    bool asBool() const;        // Throws if not a bool, float or an int
    void setValue(bool value);

    bool isNumeric() const;  // True if value is a float OR an int

    // Node's value as an int.
    bool isInt() const;
    int asInt() const;        // Throws if value is not castable to an int
    void setValue(int value);

    // Node's value as a float.
    bool isFloat() const;
    float asFloat() const;      // Throws if value is not castable to a float
    void setValue(float value);

    // Node's value as a block of other PodNodes
    //
    bool isBlock() const;
    const PodNodeDeque& asBlock() const;                        // Throws if not a block or undefined
    const std::string& blockScopeType() const;                  // Throws if not a block
    void setBlockScopeType(const std::string& blockScopeType);  // Throws if not a block
    void setValue(PodNodeDeque& value, const std::string& blockScopeType="");
    PodNode* childByName(const std::string& name);             // Returns first match, NULL if not found
    const PodNode* childByName(const std::string& name) const; // Returns first match, NULL if not found
    PodNode* firstChildOfType(const std::string& podType);              // Returns first match, NULL if not found
    const PodNode* firstChildOfType(const std::string& podType) const;   // Returns first match, NULL if not found
    PodNodeDeque& asBlock();  // Throws if not a block, creates empty block if undefined
    //
    // IMPORTANT: If you modify the contents of the block, you MUST call
    //            syncBlock, or anything from subtle incorrectness 
    //            to segfaults may result.
    void syncBlock();         // No-op if not a block, throws if integrity errors are found


    // Node's value as an identifier
    bool isIdentifier() const;
    std::string asIdentifier() const;   // Throws if value is not an indentifier
    PodNode& setIdentifierValue(const std::string& value);

    // Node's value as an embedded script in another language.
    bool isEmbedScript() const;
    std::string asEmbedScript() const;              // Throws if value is not an embedded script
    const std::string& embedScriptLanguage() const; // Throws if not an embedded script
    void setEmbedScriptLanguage(const std::string& language); // Throws if not an embedded script
    PodNode& setEmbedScriptValue(const std::string& value, const std::string& language);

    //
    // Serialization
    //
    void write(std::ostream& output, int indent=0) const;
    std::string repr() const;

    //
    // Internal and/or junk to be moved and/or removed
    //
    void dump(std::ostream& output, int indent=0);
    friend std::ostream& operator<<(std::ostream&, const PodNode&);
    void setSource(const std::string& filename, int line) { m_sourcefile = filename; m_sourceline = line; }
    const PodValue* value() const { return m_value; }

protected:

protected:
    std::string     m_podName; // Name of this node, may be unspecified ("")
    std::string     m_podType; // Type of this node, may be unspecified ("")
    PodValue*       m_value;   // Value of this node, may be unspecified (NULL)
    PodNode*        m_parent;  // Pointer to PodNode whose m_value is the BlockPodValue that
                               // owns this node.  May be unspecified (NULL).

    std::string m_sourcefile;  // Where this node came from
    int         m_sourceline;
};


}  //  End namespace TipPod


#endif    // End #ifndef __TIPPODNODE_H__


