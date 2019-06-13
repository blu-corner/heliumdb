#pragma once

#include "Python.h"
#include "he.h"

PyObject* pickleDumps (PyObject* obj);

PyObject* pickleLoads (const char* buf, size_t len);

bool serializeKeyObject (PyObject* k, he_item& item);

bool serializeValueObject (PyObject* k, he_item& item);

bool serializeObject (PyObject* o, void*& v, size_t& l);
bool serializeIntKey (PyObject* o, void*& v, size_t& l);
bool serializeIntVal (PyObject* o, void*& v, size_t& l);
bool serializeString (PyObject* o, void*& v, size_t& l);
bool serializeFloatKey (PyObject* o, void*& v, size_t& l);
bool serializeFloatVal (PyObject* o, void*& v, size_t& l);
bool serializeBytes (PyObject* o, void*& v, size_t& l);

PyObject* deserializeObject (void* v, size_t l);
PyObject* deserializeInt (void* v, size_t l);
PyObject* deserializeString (void* v, size_t l);
PyObject* deserializeFloat (void* v, size_t l);
PyObject* deserializeBytes (void* v, size_t l);
