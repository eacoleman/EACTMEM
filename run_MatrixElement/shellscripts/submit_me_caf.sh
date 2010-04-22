#!/bin/bash 
if [[ $# -lt 7 ]] ; then
  echo "Usage (7 arg): $0 <queue> <caf> <input.root> <output directory> <tree> <label> <eventsPerSection>";
  echo "  Note: The number of sections is automatically determined from the number of events in <input.root>";
  echo "Usage (8 arg): $0 <queue> <caf> <input.root> <output directory> <tree> <label> <eventsPerSection> <nsec>";
  echo "  Note: The number of sections is explicitly given";
  exit;
fi
 
#--------------------------------------------------------
# Input parameters
#--------------------------------------------------------
MYQUEUE=$1
MYFARM=$2
MYINPUT=$3
MYOUTPUTDIR=$4
MYTREE=$5
MYLABEL=$6
NEVENTSEC=$7

MYINPUTLOCAL=`basename ${MYINPUT}`

MYUSER=`whoami`
MYMACHINE=`hostname`
# Just for now
#MYUSER=bernd
#MYMACHINE=bismuth.fnal.gov


WORKER_SCRIPT=run_me_caf.sh
TIMESTAMP=`date +%b%d_%Hh%Mm`

mkdir -p $MYOUTPUTDIR

#--------------------------------------------------------
# Personal settings are obtained from worker_script
#--------------------------------------------------------
#export MYUSER=`cat ${WORKER_SCRIPT}      | grep MYUSER=      | cut -c 15-`
#export MYMACHINE=`cat ${WORKER_SCRIPT}   | grep MYMACHINE=   | cut -c 18-`
#export MYOUTPUTDIR=`cat ${WORKER_SCRIPT} | grep MYOUTPUTDIR= | cut -c 20-`

#--------------------------------------------------------
# Break up the jobs
#--------------------------------------------------------
FINALFILENAME="stripped_${MYINPUTLOCAL}"
NEVENT=`../macros/getEntriesAndSkim.exe ${MYTREE} ${MYINPUT} ${FINALFILENAME}`

if [[ $# -eq 7 ]] ; then
  NSEC=`expr $NEVENT / $NEVENTSEC`
  MODULO=`expr $NEVENT % $NEVENTSEC`
  if [[ $MODULO -gt 0 ]] ;
    then NSEC=`expr $NSEC + 1`
  fi
  echo Note: ${MYINPUTLOCAL} has ${NEVENT} events, need ${NSEC} sections
else 
  NSEC=$8
fi

#--------------------------------------------------------
# Make the tarball
#--------------------------------------------------------
echo Making tarball
./makeTarball.sh ${FINALFILENAME}

. ~cdfsoft/cdf2.shrc
setup cdfsoft2 development

CafSubmit --tarFile=./caf_me.tar.gz \
            --outLocation=${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}/out\$.tgz \
            --dhaccess=None --farm=${MYFARM} --dataset=None --group=common\
            --procType=${MYQUEUE} --email=${MYUSER}@fnal.gov \
            --start=1 --end=${NSEC} ./${WORKER_SCRIPT} \$ ${FINALFILENAME} \
	${MYTREE} ${NEVENTSEC} ${MYUSER} ${MYMACHINE} ${MYOUTPUTDIR} ${MYLABEL} \
	${TIMESTAMP}

rm ${FINALFILENAME}
