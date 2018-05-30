#!/bin/sh
#$ -cwd
#$ -l f_node=1
#$ -l h_rt=00:10:00

base=10
index=7
core=28
limit=1000

while getopts ":c:i:l:" OPT
do
  case $OPT in
    i) index=$OPTARG;;
    c) core=$OPTARG;;
    l) limit=$OPTARG;;
    :) echo "option argument need.-c=core.-i=index"
  esac
done

size=$((base ** index))

export OMP_NUM_THREADS=$core

#./sort_seq 10000000
./sort_seq -n $size
./sort -n $size -l $limit
