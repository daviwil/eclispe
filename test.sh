#!/bin/sh

gcc -g -o bin/eclispe-test test/test-main.c src/types.c src/parser.c && ./bin/eclispe-test
