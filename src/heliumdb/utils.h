#pragma once

#include "Python.h"
#include "he.h"

PyObject* pickleDumps (PyObject* obj);

PyObject* pickleLoads (const char* buf, size_t len);

bool serializeKeyObject (PyObject* k, he_item& item);

bool serializeValueObject (PyObject* k, he_item& item);

bool serializeObject (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeIntKey (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeIntVal (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeString (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeFloatKey (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeFloatVal (PyObject* o, void*& v, size_t& l, PyObject*& obj);
bool serializeBytes (PyObject* o, void*& v, size_t& l, PyObject*& obj);

PyObject* deserializeObject (void* v, size_t l);
PyObject* deserializeInt (void* v, size_t l);
PyObject* deserializeString (void* v, size_t l);
PyObject* deserializeFloat (void* v, size_t l);
PyObject* deserializeBytes (void* v, size_t l);
