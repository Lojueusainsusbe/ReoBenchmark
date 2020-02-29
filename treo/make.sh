#!/bin/bash

echo compiling the main program
gcc ./main.c ./$1.so -o $1
