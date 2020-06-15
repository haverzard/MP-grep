import os
import sys
import subprocess
import time

if len(sys.argv) == 4:
    process_count = int(sys.argv[1])
    thread_count = sys.argv[2]
    pattern = sys.argv[3]
    pids = {}
    res = {}

    start = time.time()
    for file in [os.path.join(dp, f) for dp, dn, fn in os.walk(os.path.expanduser(".")) for f in fn]:
        while len(pids) == process_count:
            dumps = []
            for i in pids:
                if (pids[i].poll() != None):
                    dumps.append(i)
            for i in dumps:
                res[i] = pids[i].returncode
                pids.pop(i, None)
            del dumps
        pids[file] = subprocess.Popen(["./sf", thread_count, pattern, file])

    while len(pids):
        dumps = []
        for i in pids:
            if (pids[i].poll() != None):
                dumps.append(i)
        for i in dumps:
            res[i] = pids[i].returncode
            pids.pop(i, None)
        del dumps
    time_elapsed = time.time()-start
    counts = 0
    for i in res:
        if (res[i] == 1):
            print(i)
            counts += 1
        elif (res[i] != 0):
            print("Error has occured, the results you see are not fully correct")
    
    print('Found: {} file(s)'.format(counts))
    print('Time elapsed: {} s'.format(time_elapsed))