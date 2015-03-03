//******************************************************************************
// Copyright (c) 2014 Tippett Studio. All rights reserved.
// $Id$ 
//******************************************************************************

#ifdef SWIGPYTHON

// Not useful in python
%ignore operator<<;
%ignore *::PodNode(const std::string& podName, const std::string& podType, PodValue* value);


// *****************************************************************************
%{ // Begin verbatim code (utility functions for internal use only)
    
#include <sstream>
#include <fstream>
#include "TipPodNode.h"
#include "TipPodValue.h"
#include "TipPodBlockPodValue.h"


// *****************************************************************************
//
// Return a valid index into a PodNodeDeque given a python-style 
// array index (which can be negative).
//
static size_t nodeIndex(const TipPod::PodNode* node, int index)
{
    const int blockSize = int(node->asBlock().size());
    if (index >= 0 && index < blockSize)
    {
        return index;
    }
    else if (index < 0 && index >= -blockSize)
    {
        return blockSize + index;
    }
    else
    {
        throw std::out_of_range("list index out of range");
    }
}


// *****************************************************************************
//
// Given a PyObject*, return the TipPod::PodNode* it contains, if any.
// Returns NULL if PyObject* is some other type.
//
static TipPod::PodNode* podNodeFromPyObject(PyObject* pyObj)
{
    static swig_type_info* PodNode_swigType = NULL;
    if (!PodNode_swigType) 
    {
        // Get the SWIG type descriptor for TipPod::PodNode*
        PodNode_swigType = SWIG_TypeQuery("TipPod::PodNode *");
        assert(PodNode_swigType);
    }
    TipPod::PodNode* node = NULL;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, (void**)&node, PodNode_swigType, 0)))
    {
        return node;
    }
    return NULL;
}


// *****************************************************************************
//
// Utility function to convert a PyObject* to a new PodValue* of the closest 
// compatible type.
//
static TipPod::PodValue* podValueFromPyObject(PyObject* pyObj)
{
    if (!pyObj || pyObj == Py_None)
    {
        return NULL;
    }
    else if (TipPod::PodNode* otherNode = podNodeFromPyObject(pyObj))
    {
        if (otherNode->podName().empty())
        {
            const TipPod::PodValue* otherValue = otherNode->value();
            return otherValue ? otherValue->copy() : NULL;
        }
        else
        {
            return new TipPod::IdentifierPodValue(otherNode->podName());
        }
    }
    else if (PyBool_Check(pyObj))
    {
        return new TipPod::BoolPodValue(pyObj == Py_True);
    }
    else if (PyInt_Check(pyObj))
    {
#warning TODO: overflow check
        return new TipPod::IntPodValue(PyInt_AsLong(pyObj));
    }
    else if (PyFloat_Check(pyObj))
    {
#warning TODO: overflow check
        return new TipPod::FloatPodValue(PyFloat_AsDouble(pyObj));
    }
    else if (PyString_Check(pyObj) || PyUnicode_Check(pyObj))
    {
        return new TipPod::StringPodValue(std::string(PyString_AsString(pyObj)));
    }
    else if (PySequence_Check(pyObj))
    {
        TipPod::PodNodeDeque block;
        for (int i = 0; i < PySequence_Size(pyObj); ++i)
        {
            PyObject* value = PySequence_GetItem(pyObj, i);
            if (TipPod::PodNode* otherNode = podNodeFromPyObject(value))
            {
                block.push_back(otherNode);
            }
            else
            {
                block.push_back(new TipPod::PodNode("", "", podValueFromPyObject(value)));
            }
        }
        return new TipPod::BlockPodValue(block);
    }
    else if (PyMapping_Check(pyObj))
    {
        PyObject* keys = PyMapping_Keys(pyObj);
        PyObject* values = PyMapping_Values(pyObj);
        assert (keys && values);

        TipPod::PodNodeDeque block;
        for (int i = 0; i < PyMapping_Size(pyObj); ++i)
        {
            std::string name = PyString_AS_STRING(PyObject_Str(PySequence_GetItem(keys, i)));
            PyObject* value = PySequence_GetItem(values, i);
            if (TipPod::PodNode* otherNode = podNodeFromPyObject(value))
            {
                block.push_back(otherNode);
            }
            else
            {
                block.push_back(new TipPod::PodNode(name, "", podValueFromPyObject(value)));
            }
        }
        return new TipPod::BlockPodValue(block);
    }
    else
    {
        std::ostringstream err;
        err << "Can't convert " << PyString_AS_STRING(PyObject_Repr(pyObj))
            << " to any TipPod value type.";
        throw std::runtime_error(err.str());
    }
    return NULL;
}




// *****************************************************************************
//
// Utility function to convert a PodNode's value to a PyObject* of the closest 
// compatible Python type.
//
static PyObject* PyObjectFromPodNodeValue(TipPod::PodNode* node)
{
    PyObject* result = NULL;
    switch (node->valueType())
    {
        case TipPod::PodNode::INT:
            result = PyInt_FromLong(node->asInt());
            break;
        case TipPod::PodNode::FLOAT:
            result = PyFloat_FromDouble(node->asFloat());
            break;
        case TipPod::PodNode::BOOL:
            if (node->asBool())
            {
                 result = Py_True;  Py_INCREF(Py_True);
            }
            else
            {
                 result = Py_False;  Py_INCREF(Py_False);
            }
            break;
        case TipPod::PodNode::IDENTIFIER: // TODO: Convert back to (PodNode*)?  Would require knowledge of scope, etc...
        case TipPod::PodNode::EMBED:
        case TipPod::PodNode::STRING:
            result = PyString_FromString(node->asString().c_str());
            break;
        case TipPod::PodNode::BLOCK:
            {
                static swig_type_info* PodNodeDeque_swigType = NULL;
                if (!PodNodeDeque_swigType) 
                {
                    // Get the SWIG type descriptor for TipPod::PodNodeDeque
                    PodNodeDeque_swigType = SWIG_TypeQuery("std::deque<TipPod::PodNode*>*");
                    assert(PodNodeDeque_swigType);
                }

                // TODO: use %newobject ?
                TipPod::PodNodeDeque* block = new TipPod::PodNodeDeque(node->asBlock().begin(),
                                                                       node->asBlock().end());
                result = SWIG_NewPointerObj(SWIG_as_voidptr(block), 
                                            PodNodeDeque_swigType, 
                                            0);
            }
            break;
        case TipPod::PodNode::UNDEFINED:
            result = Py_None; Py_INCREF(Py_None);
            break;
        default:
            throw std::runtime_error("Pod values of this type not supported yet");
    }
    return result;
}


%} // End verbatim code
// *****************************************************************************





// *****************************************************************************
//
// Python-specific class extensions
//
%extend TipPod::PodNode
{
    %feature("autodoc", "0");
    %feature("kwargs") PodNode;
    PodNode(const std::string& name="", const std::string& type="", PyObject* value=NULL)
    {
        TipPod::PodNode* node = new TipPod::PodNode(name, type, podValueFromPyObject(value));
        return node;
    }


    std::string __repr__() const
    {
        std::ostringstream repr;
        repr << $self->repr(); 
        return repr.str();
    }


    std::string __str__()
    {
        std::ostringstream str;
        $self->write(str);
        return str.str();
    }


    int __len__() const
    {
        if ($self->isBlock())
        {
            return $self->asBlock().size();
        }
        else if ($self->isString())
        {
            return $self->asString().size();
        }
        else
        {
            throw TipPod::ValueTypeError("len", $self);
        }
    }


    bool __eq__(PyObject* other) const
    {
        if (TipPod::PodNode* otherNode = podNodeFromPyObject(other))
        {
            return $self == otherNode;
        }
        return false;
    }


    TipPod::PodNode* __getitem__(PyObject* key)
    {
        if (PyInt_Check(key))
        {
            return $self->asBlock()[nodeIndex($self, PyInt_AsLong(key))];
        }
        else if (PyString_Check(key) || PyUnicode_Check(key))
        {
            const std::string name(PyString_AsString(key));
            TipPod::PodNode* result = $self->childByName(name);
            if (!result)
            {
                throw key_error(name.c_str());
            }
            return result;
        }
        else
        {
            throw key_error(PyString_AsString(PyObject_Repr(key)));
        }
    }


    void __setitem__(PyObject* key, PyObject* value)
    {
        TipPod::PodNode* node = NULL;
        if (PyInt_Check(key))
        {
            node = $self->asBlock()[nodeIndex($self, PyInt_AsLong(key))];
        }
        else if (PyString_Check(key) || PyUnicode_Check(key))
        {
            const std::string name(PyString_AsString(key));
            node = $self->childByName(name);
            if (!node)
            {
                node = new TipPod::PodNode(name);
                $self->asBlock().push_back(node);
            }
        }
        else
        {
            throw key_error(PyString_AsString(PyObject_Repr(key)));
        }

        assert(node);
        if ($self->valueType() == TipPod::PodNode::UNDEFINED)
        {
            // This node is NULL, so just start a new block
            TipPod::PodNodeDeque block;
            $self->setValue(block);
        }
        node->setValue(podValueFromPyObject(value));
    }


    void __delitem__(PyObject* key)
    {
        TipPod::PodNode* node = NULL;
        if (PyInt_Check(key))
        {
            node = $self->asBlock()[nodeIndex($self, PyInt_AsLong(key))];
        }
        else if (PyString_Check(key) || PyUnicode_Check(key))
        {
            const std::string name(PyString_AsString(key));
            node = $self->childByName(name);
        }
        if (!node)
        {
            throw key_error(PyString_AsString(PyObject_Repr(key)));
        }

        assert(node);
        TipPod::PodNodeDeque& block = $self->asBlock();
        for (TipPod::PodNodeDeque::iterator iter = block.begin();
                iter != block.end(); ++iter)
        {
            if ((*iter) == node)
            {
                block.erase(iter);
                return;
            }
        }
    }


    std::vector<std::string> keys() const
    {
        std::vector<std::string> result;
        const TipPod::PodNodeDeque& block = $self->asBlock();
        for (TipPod::PodNodeDeque::const_iterator iter = block.begin();
                iter != block.end(); ++iter)
        {
            result.push_back((*iter)->podName());
        }
        return result;
    }


    PyObject* values()
    {
        TipPod::PodNodeDeque& block = $self->asBlock();
        PyObject* result = PyTuple_New(block.size());

        size_t tupleIndex = 0;
        for (TipPod::PodNodeDeque::const_iterator iter = block.begin();
                iter != block.end(); ++iter)
        {
            PyTuple_SetItem(result, tupleIndex++, PyObjectFromPodNodeValue((*iter)));
        }
        return result;
    }


    PyObject* items()
    {
        TipPod::PodNodeDeque& block = $self->asBlock();
        PyObject* result = PyTuple_New(block.size());

        size_t tupleIndex = 0;
        for (TipPod::PodNodeDeque::const_iterator iter = block.begin();
                iter != block.end(); ++iter)
        {
            PyObject* item = PyTuple_New(2);
            PyTuple_SetItem(item, 0, PyString_FromString((*iter)->podName().c_str()));
            PyTuple_SetItem(item, 1, PyObjectFromPodNodeValue((*iter)));
            PyTuple_SetItem(result, tupleIndex++, item);
        }
        return result;
    }


    bool __contains__(const std::string& key) const
    {
        const TipPod::PodNodeDeque& block = $self->asBlock();
        for (size_t i = 0; i < block.size(); ++i)
        {
            if (block[i]->podName() == key)
            {
                return true;
            }
        }
        return false;
    }


    PyObject* value()
    {
        return PyObjectFromPodNodeValue($self);
    }


    void write(const std::string& filename)
    {
        std::ofstream file;
        file.open(filename.c_str());
        $self->write(file);
        file.close();
    }


    void append(PyObject* value)
    {
        if ($self->valueType() == TipPod::PodNode::UNDEFINED)
        {
            // This node is NULL, so just start a new block
            TipPod::PodNodeDeque block;
            $self->setValue(block);
        }
        if ($self->valueType() != TipPod::PodNode::BLOCK)
        {
            throw TipPod::ValueTypeError("append", $self);
        }
        TipPod::PodNodeDeque& block = $self->asBlock();

        TipPod::PodNode* node = podNodeFromPyObject(value);
        if (node)
        {
            // Are we appending another PodNode*?  Make sure Python knows
            // we're using it.
            Py_INCREF(value);
        }
        else
        {
            // Do we have some other supported value?  Make an anonymous
            // PodNode* to hold the value
            node = new TipPod::PodNode();
            try
            {
                node->setValue(podValueFromPyObject(value));
            }
            catch (...)
            {
                delete node;
                throw;
            }
        }

        // Do the actual append
        block.push_back(node);
        $self->syncBlock();
    }



    void extend(PyObject* value)
    {
        if (!PySequence_Check(value))
        {
            throw TipPod::ValueTypeError("extend", $self);
        }
        
        TipPod::PodNodeDeque& block = $self->asBlock();
        for (int i = 0; i < PySequence_Size(value); ++i)
        {
            PyObject* item = PySequence_GetItem(value, i);
            if (TipPod::PodNode* otherNode = podNodeFromPyObject(item))
            {
                block.push_back(otherNode);
            }
            else
            {
                block.push_back(new TipPod::PodNode("", "", podValueFromPyObject(item)));
            }
        }
    }


    void insert(int index, PyObject* value)
    {
        if ($self->valueType() == TipPod::PodNode::UNDEFINED)
        {
            // This node is NULL, so just start a new block
            TipPod::PodNodeDeque block;
            $self->setValue(block);
        }
        if ($self->valueType() != TipPod::PodNode::BLOCK)
        {
            throw TipPod::ValueTypeError("insert", $self);
        }
        TipPod::PodNodeDeque& block = $self->asBlock();

        TipPod::PodNode* node = podNodeFromPyObject(value);
        if (node)
        {
            // Are we appending another PodNode*?  Make sure Python knows
            // we're using it.
            Py_INCREF(value);
        }
        else
        {
            // Do we have some other supported value?  Make an anonymous
            // PodNode* to hold the value
            node = new TipPod::PodNode();
            try
            {
                node->setValue(podValueFromPyObject(value));
            }
            catch (...)
            {
                delete node;
                throw;
            }
        }
        block.insert(block.begin() + nodeIndex($self, index), node);
        $self->syncBlock();
    }


}; // End %extend TipPod::PodNode


#endif // End #ifdef SWIGPYTHON

%template(PodNodeDeque) std::deque<TipPod::PodNode*>;
%template(StringVector) std::vector<std::string>;

%include "TipPodNode.h"

