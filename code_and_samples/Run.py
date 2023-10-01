#!/usr/bin/python3

import os
import time
import platform


times = {}
roots = {}
runs = 0

pre_time = time.perf_counter()

for root, subdir, files in os.walk("./samples"):
    root = root.split("/")[-1]
    if root == "samples":
        continue
    for file in files:
        if not file.endswith(".bmp"):
            continue
        runs += 1
        exe = "./main.out" if platform.system() != "Windows" else "./main.exe"
        cmd = exe + " ./samples/" + root + "/" + file + " output.bmp"
        print(cmd)
        sub_p = os.popen(cmd)
        stdout = sub_p.read()
        try:
            found = int(stdout.split(" ")[-1].strip())
            if root in times:
                roots[root] += 1
                times[root] += found
            else:
                times[root] = found
                roots[root] = 1

        except:
            print("Failed " + root + "/" + file)
            print(stdout)


for key, item in times.items():
    print(key + ": ", item / roots[key])
print()

total_time = time.perf_counter() - pre_time
print("total time:", total_time, "s")
print("avg time:", total_time / runs, "s")
