#!/bin/bash

cd ~/bench/treo

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

java -jar ~/reo/bin/reo-1.0.jar ~/bench/treo/treo/$proto.treo -p P:int=$2 C:int=$3 -t Rust

if [[ "$rewrite" = true ]]; then
  python3 ~/bench/treo/nseqscript.py $proto
fi

~/bench/rstosh $proto.rs

make

cd ~/bench/tests
