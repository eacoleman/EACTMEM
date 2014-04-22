#!/bin/tcsh
setenv SCRAM_ARCH slc5_amd64_gcc434
source /uscmst1/prod/sw/cms/cshrc prod
setenv X509_USER_PROXY /uscms/home/aperloff/x509up_u43841
cd /uscms_data/d2/aperloff/YOURWORKINGAREA/MatrixElement/gitty/CMSSW_5_3_2_patch4/src/TAMUWW/MVA/bin/
eval `scramv1 runtime -csh`
setenv BASEPATH ${PWD}
echo "BASEPATH="$BASEPATH

cd ${_CONDOR_SCRATCH_DIR}

set START_TIME=`/bin/date +%m_%d_%y_%H_%M_%S`

#TMVAClassification_x -lep electron -train true -plot true -myMethodList BDT:::MLP -luminosity 19148 -ofileBase TMVA_electron -ifilePath /uscms/home/aperloff/nobackup/Summer12ME8TeV/MEResults/microNtuples_matched/ >& /uscms_data/d2/aperloff/YOURWORKINGAREA/MatrixElement/gitty/CMSSW_5_3_2_patch4/src/TAMUWW/MVA/bin/out_TMVA_${START_TIME}.txt
#TMVAClassification_x -lep electron -train true -plot true -myMethodList BDT,MLP -ofileBase TMVA_electron
#TMVAClassification_x -lep electron -train true -plot true -myMethodList MLP -ofileBase TMVA_electron -signals ggH125 -backgrounds WJets -eventProbs 0 1 3 5 6 8 9 10 11 12 19 54 -batch true

#TMVAClassification_x -lep electron -train true -plot true -myMethodList BDT -ofileBase TMVA_electron -signals ggH125 -backgrounds WJets -eventProbs 0 1 3 5 8 12 19 54 -batch true
TMVAClassification_x -lep electron -train true -plot true -myMethodList BDT,MLP,Likelihood,LikelihoodD,CutsGA,LikelihoodCat,kNN,LikelihoodPCA,PDERS -ofileBase TMVA_electron -signals ggH125 -backgrounds WJets -eventProbs 0 1 3 5 8 12 19 54 -batch true

date

