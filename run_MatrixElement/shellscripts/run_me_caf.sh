#!/bin/bash
if [[ $# -lt 8 ]] ;
  then echo "Usage: $0 <section> <input.root> <treename> <nevent> <user> <machine> <output directory> <label>"; exit
fi

#--------------------------------------------------------
# Personal settings here!
#--------------------------------------------------------
export MYUSER=$5
export MYMACHINE=$6
export MYOUTPUTDIR=$7
#--------------------------------------------------------

#--------------------------------------------------------
# Operational Settings here
#--------------------------------------------------------
export ROOTSYS=./root
export LD_LIBRARY_PATH=${ROOTSYS}/lib:./:${LD_LIBRARY_PATH}

SECTION=$1
INPUT=$2
TREENAME=$3
NEVENT=$4
TIMESTAMP=$9 #optional timestamp
MYLABEL=$8

LABEL=${MYLABEL}_${TIMESTAMP}
FNAME=${LABEL}_${SECTION}_${INPUT}
LOG=${FNAME}.log
OFFSET=$(( $(( ${SECTION} - 1 )) * ${NEVENT} ))
#--------------------------------------------------------

#---------------------------------------------
# Do the execution
#---------------------------------------------
# Start the log file first
echo "./run_MatrixElement.exe ${INPUT} ${FNAME} ${TREENAME} ${NEVENT} ${OFFSET} ${PRESCALE} >> ${LOG} 2>&1" >> ${LOG}

cat run_MatrixElement.cc >> ${LOG}
PRESCALE=1
./run_MatrixElement.exe ${INPUT} ${FNAME} ${TREENAME} ${NEVENT} ${OFFSET} ${PRESCALE} >> ${LOG} 2>&1
#---------------------------------------------

#---------------------------------------------
# Copy and Clean up
#---------------------------------------------
#source ${CDFSOFT}/cdf2.shrc 
rcp -N ${FNAME} ${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}
# Test for LCGCAF
#$GLOBUS_PATH/bin/globus-url-copy file://`pwd`/${FNAME} gsiftp://grid007g.cnaf.infn.it/flatfiles/SE00/cdf/pdong/${FNAME}
RCP_OK=$?
if [[ ${RCP_OK} -eq 0 ]] ;
  then rm -rf *.root
fi
rcp -N ${LOG} ${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}
# Test for LCGCAF
#$GLOBUS_PATH/bin/globus-url-copy file://`pwd`/${LOG} gsiftp://grid007g.cnaf.infn.it/flatfiles/SE00/cdf/pdong/${LOG}
RCP_OK=$?
if [[ ${RCP_OK} -eq 0 ]] ;
  then rm -rf ${LOG}
fi
#--------------------------------------------------------

#---------------------------------------------
# Clean up
#---------------------------------------------
rm -rf ./root
rm -rf ./lib*
rm -rf ./*.tbl
rm -rf ./*.exe
rm -rf ${INPUT}

#---------------------------------------------
# Return code check
#---------------------------------------------
X=$?
exit $X
#--------------------------------------------------------
