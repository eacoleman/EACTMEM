#!/bin/bash
if [[ $# -lt 8 ]] ;
  then echo "Usage: $0 <section> <treename> <queue> <time per event (in seconds)> <user> <machine> <output directory> <label>"; exit
fi

MYSECTION=$1
MYTREE=$2
MYQUEUE=$3
MYTIME=$4
MYUSER=$5
MYMACHINE=$6
MYOUTPUTDIR=$7
MYLABEL=$8
MYTIMESTAMP=$9 #Optional timestamp

if [[ "$MYQUEUE" == "short" ]] ;
	then
	QUEUELENGTH=21600
elif [[ "$MYQUEUE" == "lcg" ]] ;
	then
	QUEUELENGTH=72000
	MYQUEUE=medium
elif [[ "$MYQUEUE" == "medium" ]] ;
	then
	QUEUELENGTH=108000
elif [[ "$MYQUEUE" == "long" ]] ;
	then
	QUEUELENGTH=259200
elif [[ "$MYQUEUE" == "test" ]] ;
	then
	QUEUELENGTH=7200
else
	echo "Invalid queue name!"
	exit;
fi

SECTIONNUM=0

export ROOTSYS=./root
export LD_LIBRARY_PATH=${ROOTSYS}/lib:./:${LD_LIBRARY_PATH}

for file in `ls -1 *.root`
do
	LOCALFILE=`basename ${file}`
	FINALFILE="temp_${LOCALFILE}"
	NEVENT=`./getEntriesAndSkim.exe ${MYTREE} ${file} ${FINALFILE} nocut`
	#We don't need the output
	rm $FINALFILE

	TOTALTIME=$((${MYTIME} * ${NEVENT}))
	NJOBS=$(($TOTALTIME / $QUEUELENGTH))
	MODULO=$(($TOTALTIME % $QUEUELENGTH))
	if [[ $MODULO -gt 0 ]] ;
		then NJOBS=$(($NJOBS + 1))
  	fi

	SECTIONNUM=$(($SECTIONNUM + $NJOBS))

#	echo section $MYSECTION sectionnum $SECTIONNUM

	if [[ $MYSECTION -le $SECTIONNUM ]] ;
		then
		FINALSECTION=$(($NJOBS - $(($SECTIONNUM - $MYSECTION))))
		EVENTSPERSECTION=$(($NEVENT / $NJOBS))
		EVENTSREMAINDER=$(($NEVENT % $NJOBS))
		if [[ $EVENTSREMAINDER -gt 0 ]] ;
			then EVENTSPERSECTION=$(($EVENTSPERSECTION + 1))
		fi
		./run_me_caf.sh $FINALSECTION $file $MYTREE $EVENTSPERSECTION $MYUSER \
			$MYMACHINE $MYOUTPUTDIR $MYLABEL $MYTIMESTAMP
		X=$?
		exit $X
	fi
done


