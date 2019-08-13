#include "utils.h"
#include "exception.h"
#include <string>
#include "bytesobject.h"

static PyObject* PICKLE_MODULE = NULL;

PyObject*
pickleDumps (PyObject* obj)
{
    if (PICKLE_MODULE == NULL &&
        (PICKLE_MODULE = PyImport_ImportModuleNoBlock ("pickle")) == NULL)
        return NULL;

    return PyObject_CallMethodObjArgs (PICKLE_MODULE,
                                       PyUnicode_FromString ("dumps"),
                                       obj,
                                       NULL);
}


PyObject*
pickleLoads (const char* buf, size_t len)
{
#if PY_MAJOR_VERSION >= 3
    PyObject* pickedByteObj = PyBytes_FromStringAndSize (buf, len);
#else
    PyObject* pickedByteObj = PyString_FromStringAndSize (buf, len);
#endif

    return PyObject_CallMethodObjArgs (PICKLE_MODULE,
                                       PyUnicode_FromString("loads"),
                                       pickedByteObj,
                                       NULL);
}

bool
serializeObject (PyObject* o, void*& v, size_t& l)
{
    PyObject* pickledObj = pickleDumps (o);

    char* obj;
    Py_ssize_t objLen;
#if PY_MAJOR_VERSION >= 3
    if (PyBytes_AsStringAndSize (pickledObj, &obj, &objLen) == -1)
#else
    if (PyString_AsStringAndSize (pickledObj, &obj, &objLen) == -1)
#endif
    {
        printf ("failed to serialize object");
        return false;
    }
    
    v = (void*)obj;
    l = objLen;

    return true;
}

bool
serializeIntKey (PyObject* o, void*& v, size_t& l)
{
    static __thread int64_t res;

    if (!PyLong_Check (o))
    {
        PyErr_SetString (HeliumDbException, "value not an int");
        return false;
    }
    
    res = PyLong_AsLongLong (o);

    v = &res;
    l = sizeof (res);

    return true;
}

bool
serializeIntVal (PyObject* o, void*& v, size_t& l)
{
    static __thread int64_t res;

    if (!PyLong_Check (o))
    {
        PyErr_SetString (HeliumDbException, "value not an int");
        return false;
    }
    
    res = PyLong_AsLongLong (o);

    v = &res;
    l = sizeof (res);

    return true;
}

bool
serializeFloatKey (PyObject* o, void*& v, size_t& l)
{
    static __thread double res;
    if (!PyFloat_Check (o))
        return false;
    
    res = PyFloat_AsDouble (o);

    v = &res;
    l = sizeof (double);

    return true;
}

bool
serializeFloatVal (PyObject* o, void*& v, size_t& l)
{
    static __thread double res;
    if (!PyFloat_Check (o))
        return false;
    
    res = PyFloat_AsDouble (o);

    v = &res;
    l = sizeof (double);

    return true;
}

bool
serializeString (PyObject* o, void*& v, size_t& l)
{
    if (!PyUnicode_Check (o))
        return false;

    Py_ssize_t objLen;
#if PY_MAJOR_VERSION >= 3
    const char* res = PyUnicode_AsUTF8AndSize (o, &objLen);
#else
    // the py2 unicode api is quite restricted so reverting to
    // string object for py2
    char* res;
    if (PyString_AsStringAndSize (o, &res, &objLen) == -1)
        return false;
#endif

    v = (void*)res;
    l = objLen;

    return true;
}

bool
serializeBytes (PyObject* o, void*& v, size_t& l)
{
    char* res;
    Py_ssize_t objLen;

#if PY_MAJOR_VERSION >= 3
    if (!PyBytes_Check (o))
        return false;

    if (PyBytes_AsStringAndSize (o, &res, &objLen) == -1)
#else
    if (!PyString_Check (o))
        return false;

    if (PyString_AsStringAndSize (o, &res, &objLen) == -1)
#endif
    {
        printf ("failed to serialize bytes object");
        return false;
    }

    v = (void*)res;
    l = objLen;

    return true;
}

PyObject*
deserializeObject (void* buf, size_t len)
{
    const char* d = reinterpret_cast <const char*> (buf);
#if PY_MAJOR_VERSION >= 3
    PyObject* pickedByteObj = PyBytes_FromStringAndSize (d, len);
#else
    PyObject* pickedByteObj = PyString_FromStringAndSize (d, len);
#endif

    return PyObject_CallMethodObjArgs (PICKLE_MODULE,
                                       PyUnicode_FromString("loads"),
                                       pickedByteObj,
                                       NULL);
}

PyObject*
deserializeInt (void* buf, size_t len)
{
    int64_t* v = reinterpret_cast<int64_t*> (buf);

    PyObject* res = PyLong_FromLongLong (*v);
    return res;
}

PyObject*
deserializeString (void* buf, size_t len)
{
    const char* v = reinterpret_cast<const char*> (buf);

    PyObject* res = PyUnicode_FromStringAndSize (v, len);
    return res;
}

PyObject*
deserializeFloat (void* buf, size_t len)
{
    double* v = reinterpret_cast<double*> (buf);

    PyObject* res = PyFloat_FromDouble (*v);
    return res;
}

PyObject*
deserializeBytes (void* buf, size_t len)
{
    const char* d = reinterpret_cast<const char*> (buf);

#if PY_MAJOR_VERSION >= 3
    PyObject* res = PyBytes_FromStringAndSize (d, len);
#else
    PyObject* res = PyString_FromStringAndSize (d, len);
#endif

    return res;
}
