Quick Start
===========

Once Helium™ has been installed and configured setup is straightforward.
First initialise a file of the required size:

.. code-block:: console
    
    $ truncate -s 2g /tmp/storage

Or from Python:

.. code-block:: python

    import os
    os.system('truncate -s 2g /tmp/storage')

Once successfully created, this can be used from Python:

.. code-block:: python

    from heliumdb import Heliumdb

    store = Heliumdb(url="he://.//tmp/storage", datastore="helium")

To assign data into `store`, use it as you would a Python dictionary:

.. code-block:: python

    store['key'] = 'value'

`store` can now be persisted to the helium data store using commit:

.. code-block:: python
    
    store.commit()

Values can be accessed from within the helium datastore using the same methods
as the native dictionary:

.. code-block:: python

    assert store['key'] == 'value'
    assert store.get('key') == 'value'
    assert store.get('not-present', None) == None

So the full example is:

.. code-block:: python

    from heliumdb import Heliumdb
    import os

    # create the file
    os.system('truncate -s 2g /tmp/storage')

    # initialise the helium store
    store = Heliumdb(url="he://.//tmp/storage", datastore="helium")

    # assign a value
    store['key'] = 'value'

    # persist the data
    store.commit()

    # access value from the store
    assert store['key'] == 'value'
    assert store.get('key') == 'value'
    assert store.get('not-present', None) == None
