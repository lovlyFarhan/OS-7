#!/usr/bin/env bash

TIMEFMT=$'wall=%e user=%U system=%S CPU=%P i-switched=%c v-switched=%w'

run () {
    for i in {1..$2}; do
        $1
    done
}

PROGRAM=( pi-sched rw mixed )
THREADS=( 7 70 700 )
SCHEDULER=( SCHED_OTHER SCHED_FIFO SCHED_RR )
for p in ${PROGRAM[@]}; do
    for s in ${SCHEDULER[@]}; do
        for t in ${THREADS[@]}; do
            time run "./$p $s" $t
        done
    done
done
