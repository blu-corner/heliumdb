#include "module.h"

using namespace std;

struct module_state 
{
    PyObject* error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#define DEFERRED_ADDRESS(ADDR) 0

template <class T>
static void
setEnvVariable (PyObject* kwargs, const char* param, T val, T& o)
{
    PyObject* k = PyUnicode_FromString (param);
    if (PyDict_Contains (kwargs, k) == 1)
        o = val;
}

PyObject*
heliumdb_contains (heliumdbPy* self, PyObject* k)
{
    he_item item;
    if (!self->mKeySerializer (k, item.key, item.key_len))
    {
        PyErr_SetString (HeliumDbException, "could not serialize key object");
        return NULL;
    }

    return PyBool_FromLong (he_exists (self->mDatastore, &item) == 0);
}

int
heliumdbPy_init (heliumdbPy* self, PyObject* args, PyObject* kwargs)
{
    char* url = NULL;
    char* datastore = NULL;
    char* key_type = NULL;
    char* val_type = NULL;
    
    uint64_t fanout;
    int32_t flags = 0;
    uint64_t gc_fanout;
    uint64_t write_cache;
    uint64_t read_cache;
    uint64_t auto_commit_period;
    uint64_t auto_clean_period;
    uint64_t clean_util_pct;
    uint64_t clean_dirty_pct;
    uint64_t retry_count;
    uint64_t retry_delay;
    uint64_t compress_threshold;

    char *kwlist[] = {(char*)"url",
                      (char*)"datastore",
                      (char*)"key_type",
                      (char*)"val_type",
                      (char*)"fanout",
                      (char*)"flags",
                      (char*)"gc_fanout",
                      (char*)"write_cache",
                      (char*)"read_cache",
                      (char*)"auto_commit_period",
                      (char*)"auto_clean_period",
                      (char*)"clean_util_pct",
                      (char*)"clean_dirty_pct",
                      (char*)"retry_count",
                      (char*)"retry_delay",
                      (char*)"compress_threshold",
                      NULL};

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "|ssssKKKKKKKKKKK",
                                     kwlist,
                                     &url,
                                     &datastore,
                                     &key_type,
                                     &val_type,
                                     &fanout,
                                     &flags,
                                     &gc_fanout,
                                     &write_cache,
                                     &read_cache,
                                     &auto_commit_period,
                                     &auto_clean_period,
                                     &clean_util_pct,
                                     &clean_dirty_pct,
                                     &retry_count,
                                     &retry_delay,
                                     &compress_threshold))
        return -1;

    if (url == NULL)
    {
        PyErr_SetString (HeliumDbException, "missing required argument url");
        return -1;
    }

    if (datastore == NULL)
    {
        PyErr_SetString (HeliumDbException, "missing required argument datastore");
        return -1;
    }

    /* env setup */
    he_env env;
    memset (&env, 0, sizeof (env));
    setEnvVariable (kwargs, "fanout", fanout, env.fanout);
    setEnvVariable (kwargs, "gc_fanout", gc_fanout, env.gc_fanout);
    setEnvVariable (kwargs, "write_cache", write_cache, env.write_cache);
    setEnvVariable (kwargs, "read_cache", read_cache, env.read_cache);
    setEnvVariable (kwargs, "auto_commit_period", auto_commit_period, env.auto_commit_period);
    setEnvVariable (kwargs, "auto_clean_period", auto_clean_period, env.auto_clean_period);
    setEnvVariable (kwargs, "clean_util_pct", clean_util_pct, env.clean_util_pct);
    setEnvVariable (kwargs, "clean_dirty_pct", clean_dirty_pct, env.clean_dirty_pct);
    setEnvVariable (kwargs, "retry_count", retry_count, env.retry_count);
    setEnvVariable (kwargs, "retry_delay", retry_delay, env.retry_delay);
    setEnvVariable (kwargs, "compress_threshold", compress_threshold, env.compress_threshold);

    if (self->mDatastore == NULL)
    {
        self->mDatastore = he_open (url, datastore, flags, &env);
        if (!self->mDatastore)
        {
            PyErr_SetString (HeliumDbException, he_strerror (errno));
            return -1;
        }
    }

    if (key_type == NULL || strcmp (key_type, "O") == 0)
    {
        self->mKeySerializer = &serializeObject;
        self->mKeyDeserializer = &deserializeObject;
    }
    else if (strcmp (key_type, "b") == 0)
    {
        self->mKeySerializer = &serializeBytes;
        self->mKeyDeserializer = &deserializeBytes;
    }
    else if (strcmp (key_type, "i") == 0)
    {
        self->mKeySerializer = &serializeIntKey;
        self->mKeyDeserializer = &deserializeInt;
    }
    else if (strcmp (key_type, "s") == 0)
    {
        self->mKeySerializer = &serializeString;
        self->mKeyDeserializer = &deserializeString;
    }
    else if (strcmp (key_type, "f") == 0)
    {
        self->mKeySerializer = &serializeFloatKey;
        self->mKeyDeserializer = &deserializeFloat;
    }
    else
    {
        PyErr_SetString (HeliumDbException, "unsupported key_type");
        return -1;
    }

    if (val_type == NULL || strcmp (val_type, "O") == 0)
    {
        self->mValSerializer = &serializeObject;
        self->mValDeserializer = &deserializeObject;
    }
    else if (strcmp (val_type, "b") == 0)
    {
        self->mValSerializer = &serializeBytes;
        self->mValDeserializer = &deserializeBytes;
    }
    else if (strcmp (val_type, "i") == 0)
    {
        self->mValSerializer = &serializeIntVal;
        self->mValDeserializer = &deserializeInt;
    }
    else if (strcmp (val_type, "s") == 0)
    {
        self->mValSerializer = &serializeString;
        self->mValDeserializer = &deserializeString;
    }
    else if (strcmp (val_type, "f") == 0)
    {
        self->mValSerializer = &serializeFloatVal;
        self->mValDeserializer = &deserializeFloat;
    }
    else
    {
        PyErr_SetString (HeliumDbException, "unsupported val_type");
        return -1;
    }

    return 0;
}

static void
heliumdbPy_dealloc (heliumdbPy* self)
{
    Py_BEGIN_ALLOW_THREADS
    he_close (self->mDatastore);
    Py_END_ALLOW_THREADS
    Py_TYPE (self)->tp_free((PyObject*)self);
	
}

static PyObject*
heliumdb_cleanup (heliumdbPy* self)
{
    int rc;
    Py_BEGIN_ALLOW_THREADS
    rc = he_remove (self->mDatastore);
    Py_END_ALLOW_THREADS
    if (rc)
    {
        PyErr_SetString (HeliumDbException, he_strerror (errno));
        return NULL;
    }

    Py_INCREF (Py_None);
    return Py_None;
}

static PyObject*
heliumdb_get (heliumdbPy *self, PyObject *args)
{
    PyObject *k = NULL;
    PyObject *failobj = NULL;

    if (!PyArg_UnpackTuple (args, "get", 1, 2, &k, &failobj))
        return NULL; 

    he_item item;
    if (!self->mKeySerializer (k, item.key, item.key_len))
        return NULL;

    int rc;
    Py_BEGIN_ALLOW_THREADS
    rc = he_exists (self->mDatastore, &item);
    Py_END_ALLOW_THREADS

    if (rc != 0)
    {
        if (failobj == NULL)
        {
            PyErr_SetString (HeliumDbException, "key not found");
            return NULL;
        }
        Py_INCREF (failobj);
        return failobj;
    }

    return heliumdb_subscript (self, k);
}

static PyObject*
heliumdb_del (heliumdbPy* self, PyObject *args)
{
    PyObject *k;
    PyObject *failobj = Py_None;

    char* buffer[8096] = {0};
    size_t rdSize = sizeof (buffer);
    void* buf = NULL;

    if (!PyArg_UnpackTuple (args, "del", 1, 2, &k, &failobj))
    {
        PyErr_SetString (HeliumDbException, "could not unpack tuple args");
        return NULL;
    }
 
    he_item item;
    if (!self->mKeySerializer (k, item.key, item.key_len))
    {
        PyErr_SetString (HeliumDbException, "could not serialize key object");
        return NULL;
    }
    item.val = (void*)buffer;

    int rc;
    for (;;)
    {
        Py_BEGIN_ALLOW_THREADS
        rc = he_delete_lookup (self->mDatastore, &item, 0, rdSize);
        Py_END_ALLOW_THREADS

        if (rc != 0)
        {
            PyErr_SetString (HeliumDbException, he_strerror (errno));
            Py_INCREF (failobj);
            return failobj;
        }
        if (item.val_len > rdSize)
        {
            rdSize = item.val_len;
            buf = malloc (rdSize);
            item.val = buf;
        }

        else
        {
            break;
        }
    }

    PyObject* obj = self->mKeyDeserializer (item.val, item.val_len);
    if (obj == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize key object");
        return NULL;
    }

    if (buf)
        free (buf);

    return obj;
}

int
heliumdb_ass_sub (heliumdbPy* self, PyObject* k, PyObject* v)
{
    char err[128];
    he_item item;
    int rc;

    if (!self->mKeySerializer (k, item.key, item.key_len))
    {
        PyErr_SetString (HeliumDbException, "could not serialize key object");
        return -1;
    }

    if (v == NULL)
    {
        // delete
        char buffer[2] = {0};
        item.val = (void*)buffer;
        item.val_len = sizeof (buffer);

        Py_BEGIN_ALLOW_THREADS
        rc = he_delete_lookup (self->mDatastore, &item, 0, 1024);
        Py_END_ALLOW_THREADS

        if (rc != 0)
        {
            snprintf (err, 128, "he_delete_lookup failed: %s", he_strerror (errno));
            PyErr_SetString (HeliumDbException, err);
            return -1;
        }
        return 0;
    }
    if (!self->mValSerializer (v, item.val, item.val_len))
    {
        PyErr_SetString (HeliumDbException, "could not serialize value object");
	}
    Py_BEGIN_ALLOW_THREADS
    rc = he_update (self->mDatastore, &item);
    Py_END_ALLOW_THREADS

    if (rc)
    {
        snprintf (err, 128, "he_update failed: %s", he_strerror (errno));
        PyErr_SetString (HeliumDbException, err);
        return -1;
    }

    return 0;
}


static char   buffer[700000];

PyObject*
heliumdb_subscript (heliumdbPy* self, PyObject* k)
{
    size_t  rdSize = sizeof (buffer);
    void*   buf = NULL;

    he_item getItem;
    getItem.val = (void*)buffer;

    if (!self->mKeySerializer (k, getItem.key, getItem.key_len))
    {
        PyErr_SetString (HeliumDbException, "could not serialize key object");
        return NULL;
    }

    int rc;
    for (;;)
    {
		Py_BEGIN_ALLOW_THREADS
        rc = he_lookup (self->mDatastore, &getItem, 0, rdSize);
        Py_END_ALLOW_THREADS

        if (rc != 0)
        {
            PyErr_SetString (HeliumDbException, "he_lookup failed");
            return NULL;
        }

        if (getItem.val_len > rdSize)
        {
            rdSize = getItem.val_len;
            buf = malloc (rdSize);
            getItem.val = buf;
        }
        else
        {
            break;
        }
    }

    PyObject* obj = self->mValDeserializer (getItem.val, getItem.val_len);
    if (obj == NULL)
    {
        PyErr_SetString (HeliumDbException, "failed to deserialize value object");
        return NULL;
    }


    if (buf)
        free (buf);

    return obj;
}

PyObject*
heliumdb_keys (heliumdbPy* self)
{
    vector<PyObject*>   keys;
    he_iter_t           itr;
    const he_item*      item;
    PyObject*           k;

    if (!(itr = he_iter_open (self->mDatastore, NULL, 0, 0, 0)))
    {
        PyErr_SetString (HeliumDbException, "failed to open iterator");
        return NULL;
    }

    while ((item = he_iter_next (itr)))
    {
        k = self->mKeyDeserializer (item->key, item->key_len);
        if (k == NULL)
        {
            PyErr_SetString (HeliumDbException, "failed to deserialize key");
            return NULL;
        }
        keys.push_back (k);
    }

    he_iter_close (itr);

    PyObject* keyList = PyList_New (keys.size ());

    vector<PyObject*>::iterator kitr = keys.begin ();

    int j = 0;
    for (; kitr != keys.end (); ++kitr)
        PyList_SetItem (keyList, j++, *kitr);

    return keyList;
}

PyObject*
heliumdb_stats (heliumdbPy* self)
{
    PyObject* res = PyDict_New ();

    if (res == NULL)
        return NULL;

    struct he_stats stats;

    int rc;
    Py_BEGIN_ALLOW_THREADS
    rc = he_stats (self->mDatastore, &stats);
    Py_END_ALLOW_THREADS

    if (rc != 0)
        return NULL;

    PyObject* name = PyUnicode_FromString (stats.name);
    if (PyDict_SetItemString (res, "name", name) < 0) 
    {
        Py_DECREF (name);
        return NULL;
    }

    PyObject* valid_items = PyLong_FromUnsignedLongLong (stats.valid_items);
    if (PyDict_SetItemString (res, "valid_items", valid_items) < 0) 
    {
        Py_DECREF (valid_items);
        return NULL;
    }

    PyObject* deleted_items = PyLong_FromUnsignedLongLong (stats.deleted_items);
    if (PyDict_SetItemString (res, "deleted_items", deleted_items) < 0) 
    {
        Py_DECREF (deleted_items);
        return NULL;
    }

    PyObject* utilized = PyLong_FromUnsignedLongLong (stats.utilized);
    if (PyDict_SetItemString (res, "utilized", utilized) < 0) 
    {
        Py_DECREF (utilized);
        return NULL;
    }

    PyObject* capacity = PyLong_FromUnsignedLongLong (stats.capacity);
    if (PyDict_SetItemString (res, "capacity", capacity) < 0) 
    {
        Py_DECREF (capacity);
        return NULL;
    }

    PyObject* buffered_writes = PyLong_FromUnsignedLongLong (stats.buffered_writes);
    if (PyDict_SetItemString (res, "buffered_writes", buffered_writes) < 0) 
    {
        Py_DECREF (buffered_writes);
        return NULL;
    }

    PyObject* buffered_reads = PyLong_FromUnsignedLongLong (stats.buffered_reads);
    if (PyDict_SetItemString (res, "buffered_reads", buffered_reads) < 0) 
    {
        Py_DECREF (buffered_reads);
        return NULL;
    }

    PyObject* dirty_writes = PyLong_FromUnsignedLongLong (stats.dirty_writes);
    if (PyDict_SetItemString (res, "dirty_writes", dirty_writes) < 0) 
    {
        Py_DECREF (dirty_writes);
        return NULL;
    }

    PyObject* device_writes = PyLong_FromUnsignedLongLong (stats.device_writes);
    if (PyDict_SetItemString (res, "device_writes", device_writes) < 0) 
    {
        Py_DECREF (device_writes);
        return NULL;
    }

    PyObject* device_reads = PyLong_FromUnsignedLongLong (stats.device_reads);
    if (PyDict_SetItemString (res, "device_reads", device_reads) < 0) 
    {
        Py_DECREF (device_reads);
        return NULL;
    }

    PyObject* auto_commits = PyLong_FromUnsignedLongLong (stats.auto_commits);
    if (PyDict_SetItemString (res, "auto_commits", auto_commits) < 0) 
    {
        Py_DECREF (auto_commits);
        return NULL;
    }

    PyObject* auto_cleans = PyLong_FromUnsignedLongLong (stats.auto_cleans);
    if (PyDict_SetItemString (res, "auto_cleans", auto_cleans) < 0) 
    {
        Py_DECREF (auto_cleans);
        return NULL;
    }

    PyObject* clean_bytes = PyLong_FromUnsignedLongLong (stats.clean_bytes);
    if (PyDict_SetItemString (res, "clean_bytes", clean_bytes) < 0) 
    {
        Py_DECREF (clean_bytes);
        return NULL;
    }

    PyObject* cache_hits = PyLong_FromUnsignedLongLong (stats.cache_hits);
    if (PyDict_SetItemString (res, "cache_hits", cache_hits) < 0) 
    {
        Py_DECREF (cache_hits);
        return NULL;
    }

    PyObject* cache_misses = PyLong_FromUnsignedLongLong (stats.cache_misses);
    if (PyDict_SetItemString (res, "cache_misses", cache_misses) < 0) 
    {
        Py_DECREF (cache_misses);
        return NULL;
    }

    Py_INCREF (res);
    return res;
}

PyObject*
heliumdb_commit (heliumdbPy* self)
{
    // TODO implement transaction handling
    int rc;
    Py_BEGIN_ALLOW_THREADS
    rc = he_commit (self->mDatastore);
    Py_END_ALLOW_THREADS

    if (rc != 0)
    {
        char buffer[128];
        snprintf (buffer, 128, "commit failed: %s", he_strerror (errno));
        PyErr_SetString (HeliumDbException, buffer);
        return NULL;
    }

    Py_INCREF (Py_None);
    return Py_None;
}

Py_ssize_t
heliumdb_len (heliumdbPy* self)
{
    struct he_stats stats;
    int rc;
    Py_BEGIN_ALLOW_THREADS
    rc = he_stats (self->mDatastore, &stats);
    Py_END_ALLOW_THREADS

    if (rc != 0)
        return 0;

    return PyLong_AsSsize_t (PyLong_FromUnsignedLongLong (stats.valid_items));
}

PyObject*
heliumdb_sizeof (heliumdbPy* self)
{
    struct he_stats stats;
    int rc;

    Py_BEGIN_ALLOW_THREADS
    rc = he_stats (self->mDatastore, &stats);
    Py_END_ALLOW_THREADS

    if (rc != 0)
        return NULL;

    return PyLong_FromUnsignedLongLong (stats.valid_items);
}

/* TODO update length and subscript method to be useful*/
static PyMappingMethods heliumdb_as_mapping = {
    (lenfunc)heliumdb_len,                  /*mp_length*/
    (binaryfunc)heliumdb_subscript,         /*mp_subscript*/
    (objobjargproc)heliumdb_ass_sub,        /*mp_ass_subscript*/
};

static PyMethodDef heliumdbPy_methods[] = {
    {"contains",  (PyCFunction)heliumdb_contains, METH_O | METH_COEXIST,
     "True if H has a key k, else False"},
    {"__contains__", (PyCFunction)heliumdb_contains, METH_O | METH_COEXIST, "True if H has a key K, else False"},
    {"__sizeof__", (PyCFunction)heliumdb_sizeof, METH_NOARGS, "returns number valid entries"},
    {"commit", (PyCFunction)heliumdb_commit, METH_NOARGS, "commits a transaction to datastore"},
    {"get",  (PyCFunction)heliumdb_get, METH_VARARGS, "get value by key"},
    {"cleanup",  (PyCFunction)heliumdb_cleanup, METH_NOARGS, "delete all entries in data store"},
    {"pop",  (PyCFunction)heliumdb_del, METH_VARARGS, "delete dict entry by key"},
    {"stats",  (PyCFunction)heliumdb_stats, METH_NOARGS, "retrieve datastore statistics"},

    {"keys",  (PyCFunction)heliumdb_keys, METH_NOARGS, "return list of all keys"},
    {"values",  (PyCFunction)heliumdb_itervalues, METH_NOARGS, "return list of all values"},
    {"items", (PyCFunction)heliumdb_iteritems,    METH_NOARGS, "iterates items"},
    // placeholders
    // __eq__
    // __ne__
    
    { NULL, NULL, 0, NULL }
};

static PyMethodDef heliumdb_methods[] = {
    { NULL, NULL, 0, NULL }
};

PyTypeObject heliumdbPyType = {
#if PY_MAJOR_VERSION >= 3
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
#else
    PyObject_HEAD_INIT (NULL)
    0,                                          /*ob_size*/
#endif
    "heliumdb.Heliumdb",                        /*tp_name*/
    sizeof(heliumdbPy),                         /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)heliumdbPy_dealloc,             /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_as_sync*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    &heliumdb_as_mapping,                       /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,                         /*tp_flags*/
    "HeliumDb wrapper",                         /*tp_doc */
    0,                                          /*tp_traverse */
    0,                                          /*tp_clear */
    0,                                          /*tp_richcompare */
    0,                                          /*tp_weaklistoffset */
    (getiterfunc)heliumdb_iter,                   /*tp_iter */
    0,                                          /*tp_iternext */
    heliumdbPy_methods,                         /*tp_methods */
    0,                                          /*tp_members */
    0,                                          /*tp_getset */
    0,                                          /* tp_base */
    0,                                          /*tp_dict */
    0,                                          /*tp_descr_get */
    0,                                          /*tp_descr_set */
    0,                                          /*tp_dictoffset */
    (initproc)heliumdbPy_init,                  /*tp_init */
    0,                                          /*tp_alloc */
    0,                                          /*tp_new */
    0,                                          /*tp_free*/
    0,                                          /*tp_is_gc*/
    0,                                          /*tp_bases*/
    0,                                          /*tp_mro*/
    0,                                          /*tp_cache*/
    0,                                          /*tp_subclasses*/
    0,                                          /*tp_weaklist*/
    0,                                          /*tp_del*/
    0                                           /*tp_version_tag*/
};

#if PY_MAJOR_VERSION >= 3
static int heliumdb_traverse (PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int heliumdb_clear (PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = 
{
        PyModuleDef_HEAD_INIT,
        "heliumdb",
        NULL,
        sizeof(struct module_state),
        heliumdb_methods,
        NULL,
        heliumdb_traverse,
        heliumdb_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_heliumdb (void)
#else
#define INITERROR return

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initheliumdb (void)
#endif
{
    PyObject* m = NULL;

    PyEval_InitThreads ();

    heliumdbPyType.tp_new = PyType_GenericNew;
    if(PyType_Ready (&heliumdbPyType) < 0)
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create (&moduledef);
#else
    m = Py_InitModule ("heliumdb", heliumdb_methods);
#endif

    if (m == NULL)
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif

    Py_INCREF (&heliumdbPyType);
    PyModule_AddObject (m, "Heliumdb", (PyObject*)&heliumdbPyType);

    HeliumDbException = PyErr_NewException ("heliumdb.HeliumdbException", NULL, NULL);
    Py_INCREF (HeliumDbException);
    PyModule_AddObject (m, "HeliumdbException", HeliumDbException);

    PyModule_AddIntConstant (m, "HE_O_CREATE", 1);
    PyModule_AddIntConstant (m, "HE_O_TRUNCATE", 2);
    PyModule_AddIntConstant (m, "HE_O_VOLUME_CREATE", 4);
    PyModule_AddIntConstant (m, "HE_O_VOLUME_TRUNCATE", 8);
    PyModule_AddIntConstant (m, "HE_O_VOLUME_NOTRIM", 16);
    PyModule_AddIntConstant (m, "HE_O_NOSORT", 32);
    PyModule_AddIntConstant (m, "HE_O_SCAN", 64);
    PyModule_AddIntConstant (m, "HE_O_CLEAN", 128);
    PyModule_AddIntConstant (m, "HE_O_COMPRESS", 256);
    PyModule_AddIntConstant (m, "HE_O_READONLY", 512);
    PyModule_AddIntConstant (m, "HE_O_ERR_EXISTS", 1024);

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
