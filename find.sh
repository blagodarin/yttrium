#!/bin/sh

find . -name '*.?pp' | sort | xargs grep --color=always -n "$1"
