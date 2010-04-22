#!/bin/bash 
if [[ $# -lt 4 ]] ; then
	echo "Usage: $0 <treename> <TNT file> <ME file> <recovery file> [option]"
  exit;
fi
if [[ $# -gt 5 ]] ; then
	echo "Usage: $0 <treename> <TNT file> <ME file> <recovery file> [option]"
	echo Did you remember to backwhack your asterisks?
  exit;
fi

source ~cdfsoft/cdf2.shrc
setup cdfsoft2 6.1.4

MYTREENAME=$1
MYTNTFILE=$2
MYMEFILE=$3
MYRECOVERY=$4
MYOPTION=$5

# Disable file name generation using metacharacters (globbing).
set -f

TEMPNAME=thisIsATemporaryFile.root

NEVENT=`../macros/getEntriesAndSkim.exe ${MYTREENAME} ${MYTNTFILE} ${TEMPNAME} ${MYOPTION}`
echo "MakeRecovery.sh NEVENT= ${NEVENT}"
if [[ $NEVENT -eq -1 ]] ; then 
    exit
fi
if [[ $NEVENT -eq 0 ]] ; then 
    echo " BELOW YOU WILL GET AN SEG VIO. IGNORE IT!! WE ONLY HAVE 0 EVENTS!!"
fi


SCRIPTNAME=thisIsAlsoATemporaryFile.C

# For some reason I cannot run this command directly
# It only works when sourced from a script, like this
echo root -q -l "'makeRecoveryFile.C+ (\"${MYTREENAME}\", \"${TEMPNAME}\", \"${MYMEFILE}\", \"${MYRECOVERY}\")'" > ${SCRIPTNAME}
source ${SCRIPTNAME}

rm ${TEMPNAME}
rm ${SCRIPTNAME}

# Enable file name generation using metacharacters (globbing).
set +f

