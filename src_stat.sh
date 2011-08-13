#!/bin/bash

# TODO: Number of files, number of lines and bytes per file.

print_d1000() {
	WHOLE=$(($1 / 1000))
	FRACTION=$(($1 - $WHOLE * 1000))
	printf "%d.%03d" $WHOLE $FRACTION
}

print_d1024() {
	WHOLE=$(($1 / 1024))
	FRACTION=$(($1 % 1024 * 1000 / 1024))
	printf "%d.%03d" $WHOLE $FRACTION
}

stat_init() {
	echo "0 0 0"
}

stat_get() {
	FILES=$(find "$1" -name '*.?pp')
	STAT=$(echo "$FILES" | xargs wc -lc | tail -n1 | sed 's/total//')
	FCOUNT=$(echo "$FILES" | wc -l)
	echo $STAT $FCOUNT
}

stat_add() {
	L1=$1; B1=$2; F1=$3
	L2=$4; B2=$5; F2=$6
	echo $(($L1 + $L2)) $(($B1 + $B2)) $(($F1 + $F2))
}

stat_print() {
	L=$1; B=$2; F=$3
	printf "%10s KiB   (%d B)\n" $(print_d1024 $B) $B
	printf "%10s kLOC  (%d B/LOC)\n" $(print_d1000 $L) $(($B / $L))
	printf "%10d files (%d B/file, %d LOC/file)\n" $F $(($B / $F)) $(($L / $F))
}

src_stat() {
	TOTAL=$(stat_init)
	for ENTRY in "$@"; do
		[[ -d "$ENTRY" ]] || continue
		STAT=$(stat_get "$ENTRY")
		echo $(readlink -mn "$ENTRY")
		stat_print $STAT
		TOTAL=$(stat_add $TOTAL $STAT)
	done
	echo "Total:"
	stat_print $TOTAL
}

src_stat include/ src/
