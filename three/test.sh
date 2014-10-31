#!/usr/bin/env bash

TIMEFORMAT=$'wall=%e user=%U system=%S CPU=%P i-switched=%c v-switched=%w'

PROGRAM=( pi-sched rw mixed )
THREADS=( 7 70 700 )
SCHEDULER=( SCHED_OTHER SCHED_FIFO SCHED_RR )
for p in ${PROGRAM[@]}; do
    for s in ${SCHEDULER[@]}; do
        for t in ${THREADS[@]}; do
            sudo /usr/bin/time -f "$TIMEFORMAT" ./$p $s $t > out
        done
    done
done
