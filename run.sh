#!/bin/bash
DIR=$(dirname "$BASH_SOURCE")
cd "$DIR"
gcc -c -pipe -O3 -Wall -Wextra -fPIC  -I. -o binutils.o binutils.c
gcc -c -pipe -O3 -Wall -Wextra -fPIC  -I. -o definitions.o definitions.c
gcc -c -pipe -O3 -Wall -Wextra -fPIC  -I. -o main.o main.c
gcc -Wl,-O1 -o lru_sim binutils.o definitions.o main.o
rm *.o
./lru_sim $1
