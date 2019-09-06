#pragma once

#include <Python.h>
#include <iostream>
#include <frameobject.h>
#include <string>
#include <vector>
#include <algorithm>
#include "bytesobject.h"

#include "utils.h"
#include "exception.h"

extern PyTypeObject heliumdbPyType;

extern PyTypeObject heliumdbIterKeyType;

extern PyTypeObject heliumdbIterItemType;

extern PyTypeObject heliumdbIterValuesType;

typedef bool (*serializer) (PyObject*, void*&, size_t&, PyObject*&);

typedef PyObject* (*deserializer) (void*, size_t);

typedef struct 
{
    PyObject_HEAD
        he_t         mDatastore;
        serializer   mKeySerializer;
        deserializer mKeyDeserializer;
        serializer   mValSerializer;
        deserializer  mValDeserializer;
} heliumdbPy;

typedef struct 
{
    PyObject_HEAD
        heliumdbPy* mHe;
        he_iter_t   mItr;
} heliumdbiter;

PyObject* heliumdb_contains (heliumdbPy* self,
                             PyObject* k);

Py_ssize_t heliumdb_len (heliumdbPy* self);

PyObject* heliumdb_sizeof (heliumdbPy* self);

PyObject* heliumdb_iter (heliumdbPy* h);

PyObject* heliumdb_iteritems (heliumdbPy* h);

PyObject* heliumdb_itervalues (heliumdbPy* h);

int heliumdbPy_init (heliumdbPy* self,
                     PyObject* args,
                     PyObject* kwargs);

PyObject* heliumdb_subscript (heliumdbPy* self, PyObject* k);

int heliumdb_ass_sub (heliumdbPy* self, PyObject* k, PyObject* v);

PyObject* heliumdb_keys (heliumdbPy* self);
