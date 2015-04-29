#!/bin/tcsh
setenv SCRAM_ARCH slc5_amd64_gcc462
setenv X509_USER_PROXY /uscms/home/aperloff/x509up_u43841
source /cvmfs/cms.cern.ch/cmsset_default.csh
set CONDORDIR=$PWD
set CMSSW_VERSION=CMSSW_5_3_2_patch5

scram p $CMSSW_VERSION

mkdir -p $CMSSW_VERSION/src/TAMUWW
mkdir -p $CMSSW_VERSION/src/TAMUWW/MVA
mkdir -p $CMSSW_VERSION/lib/${SCRAM_ARCH}
mkdir -p plots
mkdir -p weights

mv ConfigFiles $CMSSW_VERSION/src/TAMUWW
mv macros $CMSSW_VERSION/src/TAMUWW/MVA
mv lib*.so $CMSSW_VERSION/lib/slc5_amd64_gcc462
cp $CMSSW_VERSION/src/TAMUWW/MVA/macros/tmva_logo.gif .

cd $CMSSW_VERSION
scram b
eval `scram runtime -csh`
cd ..

setenv BASEPATH ${PWD}
echo "BASEPATH="$BASEPATH

set START_TIME=`/bin/date +%m_%d_%y_%H_%M_%S`

./TMVAClassification_x -batch true -myMethodList METHOD -lep LEPTONCAT -jetBin JETBIN -tagcat TAGCAT -signals SIGNALS -backgrounds BACKGROUNDS -eventProbs EVENTPROBS -kinVar KINVAR

date
