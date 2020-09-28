#!/bin/sh

mkdir -p bin
gcc -g -o bin/eclispe-test test/test-main.c src/types.c src/log.c src/error.c src/list.c src/parser.c src/eval.c && ./bin/eclispe-test
