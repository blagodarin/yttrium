#!/bin/bash

git archive --format=tar HEAD | bzip2 --best > ../yttrium_$(date -u +%Y-%m-%d).tar.bz2
