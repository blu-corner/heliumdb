Introduction
============

heliumdb is a Python binding for Levyx Helium™, the ultra-high performance key
value storage solution. It offers developers a fully persisted object store
capable of scaling to many terabytes and handles sharing between python
processes easily. 

Using heliumdb is just like using a native Python dictionary making it easy for
developers to quickly access the power of Levyx Helium™.

.. code-block:: python

    from heliumdb import Heliumdb

    store = Heliumdb(url="he://.//tmp/storage", datastore="helium")
    store['key'] = 'value'
    store.commit()
