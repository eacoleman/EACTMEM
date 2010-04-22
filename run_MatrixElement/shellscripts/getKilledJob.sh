#!/bin/bash 
if [[ $# -lt 2 ]] ; then
	echo "Usage: $0 <username/JID> <outputdir>"
  exit;
fi

MYNAME=$1
MYOUTPUTDIR=$2

MYFILENAME=icaf_out_${MYNAME}.tgz
FULLFILENAME=fcdfdata114.fnal.gov:/export/data3/cdfdata/icaf/${MYFILENAME}
#FULLFILENAME=cdfdata01.cnaf.infn.it:/cdf/data/01/cdfdata/icaf/${MYFILENAME}

TEMPDIR=temporaryTarFiles

mkdir -p ${TEMPDIR}

scp ${FULLFILENAME} ${TEMPDIR}

cd ${TEMPDIR}

tar -zxf ${MYFILENAME}

echo "Moving file" `ls *ME*.root`
mv *ME*.root* ${MYOUTPUTDIR}

rm -r *

cd ..
rmdir ${TEMPDIR}
