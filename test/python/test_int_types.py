#
# Copyright 2014-2018 Neueda Ltd.
#
from heliumdb import Heliumdb, HE_O_CREATE, HE_O_VOLUME_CREATE
import unittest
import os


class TestInt(unittest.TestCase):
    def setUp(self):
        os.system('truncate -s 2g /tmp/test-int')
        flags = HE_O_CREATE | HE_O_VOLUME_CREATE
        self.hdb = Heliumdb(url="he://.//tmp/test-int",
                            datastore='helium',
                            key_type='i',
                            val_type='i',
                            flags=flags)

    def tearDown(self):
        self.hdb.cleanup()
        if os.path.exists('/tmp/test-int'):
            os.remove('/tmp/test-int')

    def test_subscript(self):
        self.hdb[1] = 163
        self.hdb[2] = 164

        self.assertEqual(self.hdb[1], 163)
        self.assertEqual(self.hdb[2], 164)

    def test_keys(self):
        exp = {}
        exp[1] = 163
        exp[2] = 164

        self.hdb[1] = 163
        self.hdb[2] = 164

        keys = []
        for k in self.hdb.keys():
            keys.append(k)
            self.assertEqual(self.hdb[k], exp[k])

        self.assertEqual(len(keys), len(exp.keys()))

    def test_iter_keys(self):
        exp = {}
        exp[1] = 163
        exp[2] = 164

        self.hdb[1] = 163
        self.hdb[2] = 164

        keys = []
        for k in self.hdb:
            keys.append(k)
            self.assertEqual(self.hdb[k], exp[k])

        self.assertEqual(len(keys), len(exp.keys()))

    def test_iter_items(self):
        exp = {}
        exp[1] = 163
        exp[2] = 164

        self.hdb[1] = 163
        self.hdb[2] = 164

        keys = []
        for k, v in self.hdb.items():
            keys.append(k)
            self.assertEqual(v, exp[k])

        self.assertEqual(len(keys), len(exp.keys()))

    def test_get(self):
        self.hdb[1] = 123
        self.assertEqual(self.hdb.get(1), 123)

        self.assertEqual(self.hdb.get(2, 100), 100)

    def test_del(self):
        #put key 'x'
        self.hdb[1] = 60
        self.assertEqual(self.hdb.get(1), 60)

        #deleting key 'x'
        del self.hdb[1]
        self.assertEqual(self.hdb.get(1, None), None)
