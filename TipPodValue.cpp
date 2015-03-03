//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id: TipPodValue.cpp 41465 2014-10-10 19:14:35Z miker $ 
//******************************************************************************

#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <vector>

#include "TipPodValue.h"
#include "TipPodUtils.h"

namespace TipPod {


// *****************************************************************************
//
// Specializations for TypedPodValue::write()
//
template <> 
void TypedPodValue<std::string, PodNode::STRING>::write(std::ostream& output, int indent) const
{
    // Replace '"' with '\"' during serialization.  Backslashes are removed from strings
    // by the parser during reading.

    std::string outstr(m_value);

    static const std::string doubleQuote("\"");
    static const std::string escapedDoubleQuote("\\\"");
    size_t start_pos = 0;
    while ((start_pos = outstr.find(doubleQuote, start_pos)) != std::string::npos) 
    {
        outstr.replace(start_pos, doubleQuote.length(), escapedDoubleQuote);
        start_pos += escapedDoubleQuote.length();
    }

    output << "\"" << outstr << "\"";
}


template <> 
void TypedPodValue<int, PodNode::INT>::write(std::ostream& output, int indent) const
{
    output << m_value;
}


template <> 
void TypedPodValue<float, PodNode::FLOAT>::write(std::ostream& output, int indent) const
{
    char tmp[32];
    snprintf(tmp, 32, "%0.8g", m_value);
    if (strchr(tmp, '.') == NULL)
    {
        output << tmp << ".0";
    }
    else
    {
        output << tmp;
    }
}


template <> 
void TypedPodValue<std::string, PodNode::IDENTIFIER>::write(std::ostream& output, int indent) const
{
    output << m_value;
}


template <> 
void TypedPodValue<bool, PodNode::BOOL>::write(std::ostream& output, int indent) const
{
    output << (m_value ? "true" : "false");
}


// *****************************************************************************
void TypedPodValue<std::string, PodNode::EMBED>::dedent()
{
    //
    // Split text into lines
    //
    std::vector<std::string> lines = splitlines(m_value);

    //
    // Find common leading whitespace
    //
    std::string::size_type common_whitespace = std::string::npos;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];
        const std::string::size_type firstchar = line.find_first_not_of(" \t");
        if (firstchar != std::string::npos)
        {
            common_whitespace = std::min(common_whitespace, firstchar);
        }
    }

    //
    // Rebuild text, chopping off common leading whitespace
    //
    m_value.clear();
    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string& line = lines[i];
        if (line.size() > common_whitespace)
        {
            line = line.substr(common_whitespace, line.size() - common_whitespace);
        }
        m_value.append(line);
        m_value.push_back('\n');
    }
}


// *****************************************************************************
std::string PodValue::repr() const
{
    switch (m_type)
    {
        case PodNode::STRING:
            if (const StringPodValue* v = dynamic_cast<const StringPodValue*>(this))
            {
                return v->value();
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::IDENTIFIER:
            if (const IdentifierPodValue* v = dynamic_cast<const IdentifierPodValue*>(this))
            {
                return v->value();
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::EMBED:
            if (const EmbedPodValue* v = dynamic_cast<const EmbedPodValue*>(this))
            {
                return v->value();
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::INT:
            if (const IntPodValue* v = dynamic_cast<const IntPodValue*>(this))
            {
                std::ostringstream result;
                result << v->value();
                return result.str();
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::FLOAT:
            if (const FloatPodValue* v = dynamic_cast<const FloatPodValue*>(this))
            {
                std::ostringstream result;
                result << v->value();
                return result.str();
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::BOOL:
            if (const BoolPodValue* v = dynamic_cast<const BoolPodValue*>(this))
            {
                return std::string(v->value() ? "True" : "False");
            }
            else
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
            }
            break;

        case PodNode::UNDEFINED:
            return std::string("<UNDEFINED>");
            break;

        case PodNode::BLOCK:
            return std::string("<BLOCK>");
            break;

        default:
            {
                std::ostringstream err;
                err << "Internal POD integrity problem at line " << __LINE__;
                throw std::runtime_error(err.str().c_str());
                break;
            }
    }
}


// *****************************************************************************
std::ostream& operator<<(std::ostream& output, const PodValue& pv)
{
    output << "PodValue(";
    switch (pv.type())
    {
        case PodNode::STRING : output << "'" << pv.repr() << "', STRING"; break;
        case PodNode::IDENTIFIER : output << "'" << pv.repr() << "', IDENTIFIER"; break;
        case PodNode::EMBED : output << "'" << pv.repr() << "', EMBED"; break;
        case PodNode::INT : output << pv.repr() << ", INT"; break;
        case PodNode::FLOAT : output << pv.repr() << ", FLOAT"; break;
        case PodNode::BOOL : output << pv.repr() << ", BOOL"; break;
        case PodNode::BLOCK : output << pv.repr() << ", BLOCK"; break;
        default: output << "UNDEFINED"; break;
    }
    output << ")";
}



}  //  End namespace TipPod
