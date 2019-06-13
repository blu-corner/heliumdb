#
# Copyright 2014-2018 Neueda Ltd.
#
from heliumdb import Heliumdb
import unittest
import os


class TestBasic(unittest.TestCase):
    def setUp(self):
        os.system('truncate -s 2g /tmp/file')
        self.hdb = Heliumdb(url="he://.//tmp/file", datastore='helium')

    def tearDown(self):
        self.hdb.cleanup()
        if os.path.exists('/tmp/file'):
            os.remove('/tmp/file')

    def test_subscript(self):
        self.hdb[1] = 'a'
        self.assertEqual(self.hdb[1], 'a')

    def test_get(self):
        self.hdb[1] = 'a'

        self.assertEqual(self.hdb.get(1), 'a')

    def test_pop(self):
        #put key 'x'
        self.hdb['x'] = 'y'
        self.assertEqual(self.hdb.get('x'), 'y')

        #deleting key 'x'
        self.assertEqual(self.hdb.pop('x'), 'y')
        self.assertEqual(self.hdb.get('x', None), None)

    def test_del(self):
        #put key 'x'
        self.hdb['x'] = 'y'
        self.assertEqual(self.hdb['x'], 'y')

        #deleting key 'x'
        del self.hdb['x']
        self.assertEqual(self.hdb.get('x', None), None)

    def test_keys(self):
        self.hdb[1] = 'a'
        self.hdb[2] = 'b'
        self.hdb['345'] = 'c'
        keys = Heliumdb.keys(self.hdb)
        self.assertEqual(keys, [1, 2, '345'])
