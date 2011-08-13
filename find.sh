#!/bin/sh

find . -name '*.?pp' | xargs grep --color=always -n "$1"
