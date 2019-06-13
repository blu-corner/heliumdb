import unittest
import sys
from test_basic import TestBasic
from test_int_types import TestInt

if len(sys.argv) > 1:
    sys.path.insert(0, sys.argv[1])

if __name__ == '__main__':
    if len(sys.argv) > 1:
        del sys.argv[1:]
    unittest.main()
