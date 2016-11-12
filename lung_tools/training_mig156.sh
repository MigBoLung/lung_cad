#!/bin/sh
# script that holds parameter for training in
# mig156
# and calls training itself in background

RESIZE=0.6
#DIRLIST_FILE="/mnt/DATA/LUNG/DB/LIDC_NEW/dirlist_all_mig156.txt"
#DIRLIST_FILE="/mnt/DATA/LUNG/DB/LIDC_NEW/dirlist_31_mig156.txt"
DIRLIST_FILE="/mnt/DATA/LUNG/DB/LIDC_NEW/dirlist_no_faulty_mig156.txt"
GT_DIR="/mnt/DATA/LUNG/DB/LIDC_NEW/GT/GT_all/GT_min3/"
CAD_RESULTS_DIR="/mnt/WORK/lungcad_work/results_LIDC_NEW_det/"
OUTFILE_PREFIX="/mnt/WORK/lungcad_work/training_data/LIDC_NEW_all_al3_"

OUTPUT_LOG_FILE="/mnt/WORK/lungcad_work/log/training.log"

COMMAND="../build/training $RESIZE $DIRLIST_FILE $GT_DIR $CAD_RESULTS_DIR $OUTFILE_PREFIX" 
echo calling $COMMAND
nohup $COMMAND > $OUTPUT_LOG_FILE &



