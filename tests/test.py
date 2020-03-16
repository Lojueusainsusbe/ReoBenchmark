import sys
import subprocess
from argparse import ArgumentParser

parser = ArgumentParser ()
parser.add_argument("file", type=str, help="name of file to write results to")
args = parser.parse_args()
filename = args.file

runs=25
actions=1 #0000
sizes=[4] #,16,64,256,512,1024]

protocols=["alt", "eao", "ear", "seq"]

def det_producer (prot, size):
    if prot == "alt" or prot == "seq":
        return size
    return 1

def det_consumer (prot, size):
    if prot == "seq":
        return 0
    if prot == "alt":
        return 1
    return size

def det_productions (prot, size, actions):
    if prot == "eao":
        return actions*size
    return actions

def det_consumptions (prot, size, actions):
    if prot == "seq":
        return 0
    if prot == "alt":
        return actions*size
    return actions

def parse_output (time):
    time = time[2:-4]
    return float(time)

results = open(filename + ".txt", "w")

for prot in protocols:
    for size in sizes:
        ctime = 0
        ttime = 0
        numproducers = str(det_producer(prot, size))
        numconsumers = str(det_consumer(prot, size))
        productions = str(det_productions(prot, size, actions))
        consumptions = str(det_consumptions(prot, size, actions))

        results.write(prot + " " + numproducers + " " + productions + " " + numconsumers + " " + consumptions + "\n")
        results.write("size: " +  str(size) + ", actions: " + str(actions) + "\n")

	# Make Treo executable
        subprocess.run(['./../treo/make.sh', prot, numproducers, numconsumers])

        for run in range(runs):
            # C
            time = subprocess.check_output(['./../c/test', prot, numproducers, productions, numconsumers, consumptions])

            ctime += parse_output(time)
            # Treo
            time = subprocess.check_output(['./../treo/test', numproducers, productions, numconsumers, consumptions])

            ttime += parse_output(time)

        ctime = ctime / runs
        ttime = ttime / runs

        results.write("average C time over " + str(runs) + " runs: " + str(ctime) + "\n")
        results.write("average treo time over " + str(runs) + " runs: " + str(ttime) + "\n")

results.close()
