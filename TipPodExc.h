//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodExc.h 39715 2014-03-17 21:54:39Z miker $ 
//******************************************************************************

#ifndef __TIPPODEXC_H__
#define __TIPPODEXC_H__

#include <sstream>
#include <stdexcept>

#include "TipPodNode.h"

namespace TipPod {

//
// Docs TBD
//

// *****************************************************************************
class ValueTypeError : public std::exception 
{
public:
    ValueTypeError(std::string func, const PodNode* node)
        : std::exception(), m_func(func), m_valueType(node ? node->valueTypeName() : "NULL") {}
    virtual ~ValueTypeError() throw() {}

    virtual const char* what() const throw()
    {
        std::ostringstream err;
        err << m_func << "() called on PodNode of incompatible type " << m_valueType;
        return err.str().c_str();
    }
private:
    const std::string m_func;
    const std::string m_valueType;
};


// *****************************************************************************
class PodIntegrityError : public std::exception 
{
public:
    PodIntegrityError(const PodNode* node, std::string msg)
        : std::exception(), m_node(node ? node->repr() : "NULL"), m_msg(msg) {}
    virtual ~PodIntegrityError() throw() {}

    virtual const char* what() const throw()
    {
        std::ostringstream err;
        err << "TipPod integrity violation(s) detected in " << m_node << ":" << std::endl;
        err << m_msg;
        return err.str().c_str();
    }
private:
    const std::string m_node;
    const std::string m_msg;
};

}  //  End namespace TipPod


#endif    // End #ifndef __TIPPODEXC_H__


