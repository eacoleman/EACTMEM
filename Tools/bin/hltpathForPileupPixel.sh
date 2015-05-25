#!/bin/tcsh
##############################################################################
# This script will produce customized pile-up profiles for 2012 jet triggers #
# Update lumiSummary*.json and pileup_JSON_*.txt below as needed             #
##############################################################################
#JSON=lumiSummary_July14th_sanmay.json
#setenv JSON Cert_190456-203002_8TeV_PromptReco_Collisions12_JSON.txt
setenv JSON $CMSSW_BASE/src/TAMUWW/Selection/test/JSON/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt
#lumiSummary_June28th_3fb.json
#PUJSON=pileup_JSON_DCSONLY_190389-200041_pixelcorr.txt
#setenv PUJSON pileup_JSON_DCSONLY_190389-203002_corr.txt
setenv PUJSON $CMSSW_BASE/src/TAMUWW/Selection/test/JSON/pileup_JSON_DCSONLY_190389-208686_corr.txt
#pileup_JSON_DCSONLY_190389-196531_patch2.txt
#pileup_JSON_DCSONLY_190389-196531_patch2.txt

#setenv MBXSEC 69400
#setenv MBXSEC 74258
setenv MBXSEC 64542

# Get the centrally produced pile-up JSON file:
# https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData#2012_Pileup_JSON_Files
# rfdir /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp/
# rfcp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/PileUp/$PUJSON .

# Customize pile-up JSON for each trigger
# Replace lumiCalc2.py below with pixelLumiCalc.py, when available for 2012
# https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData#Modifying_Pileup_JSON_for_Use_wi

#echo "Using input JSON file: "$JSON
#echo "Using pile-up JSON file: "$PUJSON

echo "noTrig"
#pixelLumiCalc.py recorded -i $JSON --hltpath "HLT_PFJet400_*"
#pixelLumiCalc.py lumibyls -i $JSON --hltpath "HLT_PFJet400_*" -o pixellumi_jt400.csv
#pileupReCalc_HLTpaths.py -i pixellumi_jt400.csv --inputLumiJSON $PUJSON -o pileupJSON12_jt400.txt
#pileupCalc.py -i $JSON --inputLumiJSON=pileupJSON12_jt400.txt --calcMode=true --minBiasXsec=69400 --maxPileupBin=60 --numPileupBins=600 --pileupHistName=pileup_jt400 pileup12_jt400.root --verbose

#pileupCalc.py -i $JSON --inputLumiJSON=$PUJSON --calcMode=true --minBiasXsec=69400 --maxPileupBin=60 --numPileupBins=600 --pileupHistName=pileup_noTrig pileup12_noTrig.root --verbose
pileupCalc.py -i $JSON --inputLumiJSON=$PUJSON --calcMode=true --minBiasXsec=$MBXSEC --maxPileupBin=60 --numPileupBins=60 --pileupHistName=pileup_noTrig pileup12_noTrig_minBiasXsec${MBXSEC}_coarseBinning.root --verbose
#pileupCalc.py -i $JSON --inputLumiJSON=$PUJSON --calcMode=true --minBiasXsec=69400 --maxPileupBin=200 --numPileupBins=2000 --pileupHistName=pileup_noTrig pileup12_noTrig_forDelphes.root --verbose

#echo "Adding pileup12_*.root files together in pileup12_July14th_mb694pxlcorr.root"
#hadd pileup12_July14th_mb694pxlcorr.root pileup12_jt400.root
