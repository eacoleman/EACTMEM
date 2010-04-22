#!/bin/bash 
if [[ $# -lt 7 ]] ; then
	echo "Usage: $0 <input pattern> <output directory> <label> <caf> <queue> <tree> <time per event (in seconds)> [option]"
	echo Make sure to overestimate the time per event!
  exit;
fi

MYUSER=`whoami`
MYMACHINE=`hostname`

MYDIR=$1
MYOUTPUTDIR=$2
MYLABEL=$3
MYFARM=$4
MYQUEUE=$5
MYTREE=$6
MYTIME=$7
MYOPTION=$8

if [[ "$MYFARM" == "fermigrid" ]] ;
	then
	MYGROUP="common"
else
	MYGROUP="common"
fi

TEMPDIR=tempStripFiles

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
	echo "Invalid queue name! ${MYQUEUE}"
	echo "Did you remember to backwhack your asterisks?"
	exit;
fi

mkdir -p $MYOUTPUTDIR

if [[ -d $TEMPDIR ]] ;
	then
	rm ${TEMPDIR}/*.root
fi
mkdir -p ${TEMPDIR}

SECTIONNUM=0
TOTALFILES=0

for file in `ls -1 ${MYDIR}`
do
	LOCALFILE=`basename ${file}`
	FINALFILE="${TEMPDIR}/stripped_${LOCALFILE}"
	NEVENT=`../macros/getEntriesAndSkim.exe ${MYTREE} ${file} ${FINALFILE} ${MYOPTION}`

	TOTALTIME=$((${MYTIME} * ${NEVENT}))
	NJOBS=$(($TOTALTIME / $QUEUELENGTH))
	MODULO=$(($TOTALTIME % $QUEUELENGTH))
	if [[ $MODULO -gt 0 ]] ;
		then NJOBS=$(($NJOBS + 1))
  	fi

	echo "File ${file} has ${NEVENT} events, needs ${NJOBS} sections, `expr $SECTIONNUM + 1` to `expr $SECTIONNUM + $NJOBS`"

	SECTIONNUM=$(($SECTIONNUM + $NJOBS))
	TOTALFILES=$(($TOTALFILES + 1))
done

echo Total files: ${TOTALFILES}  Total sections: ${SECTIONNUM}
echo Is this okay?  Are we ready to submit?
read DECISION
if [[ "$DECISION" != "yes" ]] ; then
	echo Aborting...
	rm ${TEMPDIR}/*.root
	rmdir $TEMPDIR
	exit;
fi

echo Making tarball
./makeTarball.sh ${TEMPDIR}/*.root

echo Unzipping tarball...
gunzip caf_me.tar.gz

ISTRUE=0
for file in `ls -1 ${TEMPDIR}/*.root`
do
	if [[ $ISTRUE -eq 0 ]];
	then
		ISTRUE=1
	else
		echo Adding $file to tarball...
		BASEFILE=`basename $file`
		cp $file .
		tar -rhf caf_me.tar $BASEFILE
		rm $BASEFILE
	fi
done

echo Zipping tarball again...
gzip caf_me.tar

rm ${TEMPDIR}/*.root
rmdir $TEMPDIR

TIMESTAMP=`date +%b%d_%Hh%Mm`

. ~cdfsoft/cdf2.shrc
setup cdfsoft2 development

WORKER_SCRIPT=run_me_caf_multi.sh

CafSubmit --tarFile=./caf_me.tar.gz \
            --outLocation=${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}/out\$.tgz \
            --dhaccess=None --farm=${MYFARM} --dataset=None \
        	--group=${MYGROUP} --procType=${MYQUEUE} \
	        --email=${MYUSER}@fnal.gov \
            --start=1 --end=${SECTIONNUM} ./${WORKER_SCRIPT} \$ ${MYTREE} ${MYQUEUE} ${MYTIME} ${MYUSER} ${MYMACHINE} ${MYOUTPUTDIR} ${MYLABEL} ${TIMESTAMP}

#CafSubmit --tarFile=./caf_me.tar.gz \
#            --outLocation=${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}/out\$.tgz \
#            --dhaccess=None --farm=${MYFARM} --dataset=None \
#            --procType=${MYQUEUE} \
#	        --email=${MYUSER}@fnal.gov \
#            --start=1 --end=${SECTIONNUM} ./${WORKER_SCRIPT} \$ ${MYTREE} ${MYQUEUE} ${MYTIME} ${MYUSER} ${MYMACHINE} ${MYOUTPUTDIR} ${MYLABEL} ${TIMESTAMP}
