#!/bin/sh

nm -C -D --defined-only -f sysv lib/libyttrium.so | grep '^Yttrium::' | cut -d '|' -f 1 | uniq | sort
