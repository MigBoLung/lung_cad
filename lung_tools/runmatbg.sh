#!/bin/bash -f
unset DISPLAY
nohup matlab -nojvm -nodisplay -logfile $2 < $1  &> /dev/null &
