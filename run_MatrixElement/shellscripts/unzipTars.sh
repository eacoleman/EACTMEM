#!/bin/bash 
if [[ $# -lt 1 ]] ; then
	echo "Usage: $0 <output directory>"
  exit;
fi

MYOUTPUTDIR=$1

for file in `ls -1 *.tgz`
do

  tar -zxf ${file}

  echo "Moving file" `ls *ME*.root`
  mv *ME*.root* ${MYOUTPUTDIR}

done
