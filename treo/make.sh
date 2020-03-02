#!/bin/bash

echo compiling the main program
g++ -pthread -g ./main.c ./$1.so -o $1
