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


def string_read_benchmark(x, iterations):
    for i in range(iterations):
        y = x[str(i)]


def read_benchmark(x, iterations):
    for i in range(iterations):
        y = x[i]


def dictionary_baseline(iterations, string=False):
    x = {}

    start = time.time()
    if string:
        string_iters(x, iterations)
    else:
        int_iters(x, iterations)
    end = time.time()

    write = (end - start)

    start = time.time()
    if string:
        string_read_benchmark(x, iterations)
    else:
        read_benchmark(x, iterations)
    end = time.time()

    read = (end - start)

    return (write, read)


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

    write = end - start

    rstart = time.time()
    if string:
        string_read_benchmark(hdb, iterations)
    else:
        read_benchmark(hdb, iterations)
    rend = time.time()


    cleanup(hdb, '/tmp/test-obj')

    x = (write, rend - rstart)
    return x


if len(sys.argv) != 2:
    print('require iters')
    sys.exit(1)

iterations = int(sys.argv[1])

baseline_w, baseline_r= dictionary_baseline(iterations)
pickle_w, pickle_r = helium_benchmark(iterations)
typed_w, typed_r = helium_benchmark(iterations, key_type='i', val_type='i')
batched_w, batched_r = helium_benchmark(iterations, key_type='i', val_type='i', batch=1024)

print('{0} {1:^20} {2}'.format('-' * 10, 'integer results (write/read)', '-' * 10))
print('dictionary baseline       -> {0:.09f}/{1:.09f}'.format(baseline_w, baseline_r))
print('helium typed object       -> {0:.09f}/{1:.09f}'.format(typed_w, typed_r))
print('helium pickled object     -> {0:.09f}/{1:.09f}'.format(pickle_w, pickle_r))
print('helium typed 1024 batch   -> {0:.09f}/{1:.09f}'.format(batched_w, batched_r))

baseline_w, baseline_r = dictionary_baseline(iterations, string=True)
pickle_w, pickle_r = helium_benchmark(iterations, string=True)
typed_w, typed_r = helium_benchmark(iterations, key_type='s', val_type='s',
                                    string=True)
batched_w, batched_r = helium_benchmark(iterations, key_type='s', val_type='s',
                                        string=True, batch=1024)
print('{0} {1:^20} {2}'.format('-' * 10, 'string results (write/read)', '-' * 10))
print('dictionary baseline       -> {0:.09f}/{1:.09f}'.format(baseline_w, baseline_r))
print('helium typed object       -> {0:.09f}/{1:.09f}'.format(typed_w, typed_r))
print('helium pickled object     -> {0:.09f}/{1:.09f}'.format(pickle_w, pickle_r))
print('helium typed 1024 batch   -> {0:.09f}/{1:.09f}'.format(batched_w, batched_r))
