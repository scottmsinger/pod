//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id$ 
//******************************************************************************

#if SWIG_VERSION < 0x020012
#error Requires swig >= 2.0.12
#endif

%define MODULE_DOCSTRING
"This module provides low-level read/write access to POD files."
%enddef

%module(docstring=MODULE_DOCSTRING) TipPod

%include "stl.i"
%include "std_vector.i"
%include "std_deque.i"
%include "std_string.i"
%include "std_wstring.i"
%include "std_except.i"

%{
#include <stdexcept>
#include "TipPodExc.h"
class key_error : public std::runtime_error
{
public:
    key_error(std::string key) : std::runtime_error(key) {}
};
%}

// Set up error handling
%exception
{
    try { $action }
    catch (const key_error& err)
    {
        PyErr_SetString(PyExc_KeyError, err.what());
        SWIG_fail;
    }
    catch (const TipPod::ValueTypeError& err)
    {
        PyErr_SetString(PyExc_TypeError, err.what());
        SWIG_fail;
    }
    catch (const TipPod::PodIntegrityError& err)
    {
        PyErr_SetString(PyExc_ValueError, err.what());
        SWIG_fail;
    }
    SWIG_CATCH_STDEXCEPT
}

%include "TipPodNode.i"
%include "TipPod.i"

