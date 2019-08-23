#include "module.h"

static void
heliumdbiter_dealloc (heliumdbiter* hitr)
{
    Py_BEGIN_ALLOW_THREADS
    he_iter_close (hitr->mItr);
    Py_END_ALLOW_THREADS

    Py_XDECREF (hitr->mHe);
    PyObject_GC_Del (hitr);
}

PyObject*
heliumdb_itervalues (heliumdbPy* h)
{
    heliumdbiter* hitr = PyObject_GC_New (heliumdbiter, &heliumdbIterValuesType);
    if (hitr == NULL)
        return NULL;

    Py_INCREF (h);

    hitr->mHe = h;

    Py_BEGIN_ALLOW_THREADS
    hitr->mItr = he_iter_open (h->mDatastore, NULL, 0, HE_MAX_VAL_LEN, 0);
    Py_END_ALLOW_THREADS

    if (!hitr->mItr)
    {
        PyErr_SetString (HeliumDbException, "failed to open iterator");
        return NULL;
    }

    return (PyObject*)hitr;
}

PyObject*
heliumdb_iteritems (heliumdbPy* h)
{
    heliumdbiter* hitr = PyObject_GC_New (heliumdbiter, &heliumdbIterItemType);
    if (hitr == NULL)
        return NULL;

    Py_INCREF (h);

    hitr->mHe = h;

    Py_BEGIN_ALLOW_THREADS
    hitr->mItr = he_iter_open (h->mDatastore, NULL, 0, HE_MAX_VAL_LEN, 0);
    Py_END_ALLOW_THREADS

    if (!hitr->mItr)
    {
        PyErr_SetString (HeliumDbException, "failed to open iterator");
        return NULL;
    }

    return (PyObject*)hitr;
}

PyObject*
heliumdb_iter (heliumdbPy* h)
{
    heliumdbiter* hitr = PyObject_GC_New (heliumdbiter, &heliumdbIterKeyType);
    if (hitr == NULL)
        return NULL;

    Py_INCREF (h);

    hitr->mHe = h;

    Py_BEGIN_ALLOW_THREADS
    hitr->mItr = he_iter_open (h->mDatastore, NULL, 0, 0, 0);
    Py_END_ALLOW_THREADS

    if (!hitr->mItr)
    {
        PyErr_SetString (HeliumDbException, "failed to open iterator");
        return NULL;
    }

    return (PyObject*)hitr;
}

PyObject*
heliumdbiter_iternextkey (heliumdbiter* hitr)
{
    const he_item* item;

    Py_BEGIN_ALLOW_THREADS
    item = he_iter_next (hitr->mItr);
    Py_END_ALLOW_THREADS

    if (!item)
        return NULL;

    PyObject* key = hitr->mHe->mKeyDeserializer (item->key, item->key_len);
    if (key == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize key object");
        return NULL;
    }

    return key;
}

PyObject*
heliumdbiter_iternextitem (heliumdbiter* hitr)
{
    const he_item* item;
    Py_BEGIN_ALLOW_THREADS
    item = he_iter_next (hitr->mItr);
    Py_END_ALLOW_THREADS

    if (!item)
        return NULL;

    PyObject* key = hitr->mHe->mKeyDeserializer (item->key, item->key_len);
    if (key == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize key object");
        return NULL;
    }

    PyObject* val = hitr->mHe->mValDeserializer (item->val, item->val_len);
    if (val == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize val object");
        return NULL;
    }

    PyObject* result = PyTuple_New (2);
    if (result == NULL)
        return NULL;

    PyTuple_SET_ITEM (result, 0, key);
    PyTuple_SET_ITEM (result, 1, val);

    return result;
}

PyObject*
heliumdbiter_iternextvalue (heliumdbiter* hitr)
{
    const he_item* item;

    Py_BEGIN_ALLOW_THREADS
    item = he_iter_next (hitr->mItr);
    Py_END_ALLOW_THREADS

    if (!item)
        return NULL;

    PyObject* val = hitr->mHe->mValDeserializer (item->val, item->val_len);
    if (val == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize val object");
        return NULL;
    }

    return val;
}

static PyMethodDef heliumdbiter_methods[] = {
    { NULL, NULL, 0, NULL }
};

PyTypeObject heliumdbIterKeyType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "heliumdb-keyiterator",                       /* tp_name */
    sizeof(heliumdbiter),                         /* tp_basicsize */
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)heliumdbiter_dealloc,             /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc)heliumdbiter_iternextkey,       /* tp_iternext */
    heliumdbiter_methods,                         /* tp_methods */
    0                                           /* tp_members */
};

PyTypeObject heliumdbIterItemType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "heliumdb-keyiterator",                       /* tp_name */
    sizeof(heliumdbiter),                         /* tp_basicsize */
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)heliumdbiter_dealloc,             /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc)heliumdbiter_iternextitem,      /* tp_iternext */
    heliumdbiter_methods,                         /* tp_methods */
    0                                           /* tp_members */
};

PyTypeObject heliumdbIterValuesType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "heliumdb-valuesiterator",                       /* tp_name */
    sizeof(heliumdbiter),                         /* tp_basicsize */
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)heliumdbiter_dealloc,             /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    0,                                          /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc)heliumdbiter_iternextvalue,     /* tp_iternext */
    heliumdbiter_methods,                         /* tp_methods */
    0                                           /* tp_members */
};
