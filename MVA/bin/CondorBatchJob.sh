#!/bin/tcsh
setenv SCRAM_ARCH slc5_amd64_gcc434
source /uscmst1/prod/sw/cms/cshrc prod
cd /uscms_data/d2/aperloff/CMSSW_4_2_8/src/TAMUWW/MVA/bin/

eval `scramv1 runtime -csh`
#setenv >& /uscms/home/aperloff/MatrixElement/CMSSW_3_8_7/src/TAMUWW/TransferFunction/temp/env.txt
set START_TIME=`/bin/date +%m_%d_%y_%H_%M_%S`
TMVAClassification_x -train true -plot true -myMethodList MLP -luminosity 5020 -ofileBase TMVA_muon -ifilePath /uscms/home/aperloff/nobackup/Spring12ME7TeV/MEResults/microNtuples_oldStructure/ >& /uscms_data/d2/aperloff/CMSSW_4_2_8/src/TAMUWW/MVA/bin/MVA_out/out_TMVA_${START_TIME}.txt
date

