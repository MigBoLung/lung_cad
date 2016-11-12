#!/bin/bash

#simple script that obtains models for different weight parameters
#$1 is the

prefix="0.0028"
prefix_written="0_0028"
outprefix="fold1_al3_w0_"
outdir="models"
for i in   1 2 3 4 5 6 7 8 9
do
    w_val="${prefix}$i"
    w_val_written="${prefix_written}$i"
    outfile="$outdir/${outprefix}${w_val_written}.model"
    echo "creating model for val $w_val"
    command="./svm-train -m 512 -s 0 -c 0.1 -t 2 -w1 1 -w0 $w_val $1 $outfile"
    echo $command
    $command
done




