#!/bin/sh

find . -name '*.h' -o -name '*.cpp' | sort | xargs grep --color=always -n 'TODO:'
