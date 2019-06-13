import time
import sys
from heliumdb import Heliumdb
import os


def setup_trunc_file(f):
    os.system('truncate -s 2g {0}'.format(f))


def cleanup(hdb, f):
    hdb.cleanup()
    if os.path.exists(f):
        os.remove(f)


def string_batch_iters(x, iterations, batch=None):
    if not batch:
        batch = iterations + 1

    for i in range(0, iterations):
        x[str(i)] = str(i)
        if i % batch == 0:
            x.commit()


def int_batch_iters(x, iterations, batch=None):
    if not batch:
        batch = iterations + 1

    for i in range(0, iterations):
        x[i] = i
        if i % batch == 0:
            x.commit()


def string_iters(x, iterations):
    for i in range(0, iterations):
        x[str(i)] = str(i)


def int_iters(x, iterations, batch=None):
    for i in range(0, iterations):
        x[i] = i


def dictionary_baseline(iterations, string=False):
    x = {}

    start = time.time()
    if string:
        string_iters(x, iterations)
    else:
        int_iters(x, iterations)
    end = time.time()

    return (end - start)


def helium_benchmark(iterations, key_type='O',
                     val_type='O', batch=None, string=False):
    setup_trunc_file('/tmp/test-obj')
    hdb = Heliumdb(url="he://.//tmp/test-obj",
                   datastore='helium',
                   key_type=key_type,
                   val_type=val_type)

    start = time.time()

    if string and batch:
        string_batch_iters(hdb, iterations, batch=batch)
    elif string:
        string_iters(hdb, iterations)
    elif not string and batch:
        int_batch_iters(hdb, iterations, batch=batch)
    else:
        int_iters(hdb, iterations)

    hdb.commit()
    end = time.time()

    cleanup(hdb, '/tmp/test-obj')

    return (end - start)


if len(sys.argv) != 2:
    print('require iters')
    sys.exit(1)

iterations = int(sys.argv[1])

baseline = dictionary_baseline(iterations)
pickle = helium_benchmark(iterations)
typed = helium_benchmark(iterations, key_type='i', val_type='i')
batched = helium_benchmark(iterations, key_type='i', val_type='i', batch=1024)

print('{0} {1:^20} {2}'.format('-' * 10, 'integer results', '-' * 10))
print('dictionary baseline       -> {0:.09f}'.format(baseline))
print('helium typed object       -> {0:.09f}'.format(typed))
print('helium pickled object     -> {0:.09f}'.format(pickle))
print('helium typed 1024 batch   -> {0:.09f}'.format(batched))

baseline = dictionary_baseline(iterations, string=True)
pickle = helium_benchmark(iterations, string=True)
typed = helium_benchmark(iterations, key_type='s', val_type='s',
                         string=True)
batched = helium_benchmark(iterations, key_type='s', val_type='s',
                           string=True, batch=1024)

print('{0} {1:^20} {2}'.format('-' * 10, 'string results', '-' * 10))
print('dictionary baseline       -> {0:.09f}'.format(baseline))
print('helium typed object       -> {0:.09f}'.format(typed))
print('helium pickled object     -> {0:.09f}'.format(pickle))
print('helium typed 1024 batch   -> {0:.09f}'.format(batched))
