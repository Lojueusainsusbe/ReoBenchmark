#!/bin/bash

cd /home/rijeun/bench/treo

make clean

rewrite=false

if [[ $1 == "seq" ]]; then
    proto="Nsequencer"
    rewrite=true
fi
if [[ $1 == "alt" ]]; then
    proto="Nalternator"
fi
if [[ $1 == "ear" ]]; then
    proto="EarlyAsyncReplicator"
fi
if [[ $1 == "eao" ]]; then
    proto="EarlyAsyncOutSequencer"
    rewrite=true
fi

java -jar /home/rijeun/reo/bin/reo-1.0.jar /home/rijeun/bench/treo/treo/$proto.treo -p P:int=$2 C:int=$3 -t Rust

if [[ "$rewrite" = true ]]; then
  python3 /home/rijeun/bench/treo/nseqscript.py $proto
fi

/home/rijeun/bench/rstosh $proto.rs

make

cd /home/rijeun/bench/tests
