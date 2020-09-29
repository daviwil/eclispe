#!/bin/sh

mkdir -p bin
gcc -g -o bin/eclispe src/mem.c src/main.c src/repl.c src/types.c src/log.c src/error.c src/list.c src/parser.c src/eval.c && ./bin/eclispe
