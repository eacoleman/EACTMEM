#!/bin/bash 
if [[ $# -lt 8 ]] ; then
	echo "Usage: $0 <input pattern> <output directory> <label> <caf> <queue> <tree> <time per event (in seconds)> <username> [option] [force]"
	echo Make sure to overestimate the time per event!
	echo 'If you do not specify "force" it will ask you for confirmation.'
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
MYUSERNAME=$8
MYOPTION=$9
MYFORCE=${10}

if [[ "$MYOPTION" == "force" ]] ;
	then
	MYFORCE=$9
	MYOPTION=""
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
	echo "MakeRecovery.sh NEVENT= ${NEVENT}"
	if [[ $NEVENT -eq -1 ]] ; then 
	    exit
	fi

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


if [[ "$MYFORCE" == "force" ]] ; then
    echo "Submitting directly because option force was specified"
else
    echo Is this okay?  Are we ready to submit?
    read DECISION
    if [[ "$DECISION" != "yes" ]] ; then
	echo Aborting...
	rm ${TEMPDIR}/*.root
	rmdir $TEMPDIR
	exit;
    fi
fi

TIMESTAMP=`date +%b%d_%Hh%Mm%Ss`
TARFILENAME=caf_me_${MYUSERNAME}_${TIMESTAMP}.tar

echo Making tarball
./makeTarball.sh ${TEMPDIR}/*.root
mv caf_me.tar.gz ${TARFILENAME}.gz

echo Unzipping tarball...
gunzip ${TARFILENAME}.gz

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
		tar -rhf $TARFILENAME $BASEFILE
		rm $BASEFILE
	fi
done

echo Zipping tarball again...
gzip $TARFILENAME

rm ${TEMPDIR}/*.root
rmdir $TEMPDIR

NEWDIRECTORY=/cdf/scratch/${USER}/for${MYUSERNAME}

mkdir -p ${NEWDIRECTORY}

WORKER_SCRIPT=run_me_caf_multi.sh

SCRIPTNAME=run_caf_${MYUSERNAME}_${TIMESTAMP}.sh

echo . ~cdfsoft/cdf2.shrc >> ${SCRIPTNAME}
echo setup cdfsoft2 development >> ${SCRIPTNAME}
echo cd ${NEWDIRECTORY} >> ${SCRIPTNAME}

MYGROUP=common
if [[ "$MYFARM" == "cdfgrid" ]] ;
	then
	#if [[ "$MYUSERNAME" == "blabla" ]] ;
	if [[ "$MYUSERNAME" == "eusebi" || "$MYUSERNAME" == "palencia" ]] ;
		then
		MYGROUP=fnal
	fi
fi

echo CafSubmit --tarFile=./${TARFILENAME}.gz \
            --outLocation=${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}/out\$.tgz \
            --dhaccess=None --farm=${MYFARM} --dataset=None \
        	--group=${MYGROUP} --procType=${MYQUEUE} \
	        --email=${MYUSER}@fnal.gov \
            --start=1 --end=${SECTIONNUM} ./${WORKER_SCRIPT} \$ ${MYTREE} ${MYQUEUE} ${MYTIME} ${MYUSER} ${MYMACHINE} ${MYOUTPUTDIR} ${MYLABEL} ${TIMESTAMP}  >> ${SCRIPTNAME}

echo "cd " >> ${SCRIPTNAME}

echo "MAILFILE=${MYUSERNAME}_temporary_mail_file.txt" >> ${SCRIPTNAME}

echo "echo ${MYUSERNAME} has submitted the requested job to the CAF!  File ${MYDIR} was submitted to ${MYFARM} with label ${MYLABEL}. > \${MAILFILE}" >> ${SCRIPTNAME}

echo "mail -s \"Submission confirmation\" ${MYUSER}@fnal.gov < \${MAILFILE}" >> ${SCRIPTNAME}
echo "rm \${MAILFILE}" >> ${SCRIPTNAME}

MAILFILE=${MYUSERNAME}_temporary_mail_file.txt

# Make sure you add the -e option to enable escape sequences
echo -e "Thank you for agreeing to submit jobs to the CAF for the matrix element analysis.  Please log in to ${MYMACHINE}, kinit, and then type ${NEWDIRECTORY}/${SCRIPTNAME} \nThe script will automatically e-mail me to let me know that you have submitted the job.\n\n Thank you for your help,\n ${MYUSER}" > ${MAILFILE}

mail -s "Please submit a CAF job for me" ${MYUSERNAME}@fnal.gov < ${MAILFILE}

rm ${MAILFILE}

echo Sent mail to ${MYUSERNAME} with submission script ${SCRIPTNAME}.

chmod 755 ${SCRIPTNAME}

mv $TARFILENAME.gz $NEWDIRECTORY
cp $WORKER_SCRIPT $NEWDIRECTORY
mv $SCRIPTNAME $NEWDIRECTORY

#CafSubmit --tarFile=./caf_me.tar.gz \
#            --outLocation=${MYUSER}@${MYMACHINE}:${MYOUTPUTDIR}/out\$.tgz \
#            --dhaccess=None --farm=${MYFARM} --dataset=None \
#            --procType=${MYQUEUE} \
#	        --email=${MYUSER}@fnal.gov \
#            --start=1 --end=${SECTIONNUM} ./${WORKER_SCRIPT} \$ ${MYTREE} ${MYQUEUE} ${MYTIME} ${MYUSER} ${MYMACHINE} ${MYOUTPUTDIR} ${MYLABEL} ${TIMESTAMP}
