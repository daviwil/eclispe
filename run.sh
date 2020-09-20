#!/bin/sh

mkdir -p bin
gcc -o bin/eclispe src/main.c src/types.c src/parser.c && ./bin/eclispe
