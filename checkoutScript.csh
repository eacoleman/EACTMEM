#!/bin/tcsh
echo "Have you run 'kserver_init' in the last 24 hrs? (y/n) "
set KS = $<
if ( $KS != y ) then
	echo "WARNING::Cannot continue without running that command being run first!"
	echo "Running kserver_init ... "
	kserver_init
	echo "DONE"
endif

echo " Type 428 for CMSSW_4_2_8, 525 for CMSSW_5_2_5 (Default = 525) "
set version = $<

source /uscmst1/prod/sw/cms/cshrc prod

if ( $version == 428 ) then
	setenv SCRAM_ARCH slc5_amd64_gcc434
	cmsrel CMSSW_4_2_8
	cd CMSSW_4_2_8/src
	cmsenv
	set CVSROOT=:gserver:cmssw.cvs.cern.ch:/local/reps/CMSSW
	cvs co -r branch42X -d TAMUWW UserCode/TAMUWW
	cvs co FWCore/Common/interface/EventBase.h

	echo "Commenting out line 86 from FWCore/Common/interface/EventBase.h"
	sed -i 's/virtual ProcessHistory const\& processHistory() const = 0;/\/\/virtual ProcessHistory const\& processHistory() const = 0;/g' FWCore/Common/interface/EventBase.h

else
	setenv SCRAM_ARCH slc5_amd64_gcc462
	cmsrel CMSSW_5_2_5
	cd CMSSW_5_2_5/src
	cmsenv
	set CVSROOT=:gserver:cmssw.cvs.cern.ch:/local/reps/CMSSW
	cvs co -d TAMUWW UserCode/TAMUWW
	echo "Removing TAMUWW/Selection42X because it is not compatible with the 52X ntuple"
	rm -r TAMUWW/Selection42X/
	cvs co -r V06-02-04    TopQuarkAnalysis/Configuration
	cvs co -r V06-05-01    DataFormats/PatCandidates
	cvs co -r V08-09-05    PhysicsTools/PatAlgos
	cvs co -r V00-03-11    CommonTools/ParticleFlow
	cvs co -r V00-01-04 -d SQWaT UserCode/SQWaT
	cvs co -d PhysicsTools/NtupleUtils UserCode/Bicocca/PhysicsTools/NtupleUtils
	cvs co -d Muon/MuonAnalysisTools/ UserCode/sixie/Muon/MuonAnalysisTools
	cvs co -r V00-00-08 -d EGamma/EGammaAnalysisTools UserCode/EGamma/EGammaAnalysisTools
	cd EGamma/EGammaAnalysisTools/data
	cat download.url | xargs wget
	cd -
	
	echo "Removing line 1 from TAMUWW/Selection/BuildFile.xml and replacing with <bin>"
	sed -i 's/<bin name=\"PerformSelection428.exe\" file=\"PerformSelection428.cc\">/<bin>/g' TAMUWW/Selection/bin/BuildFile.xml

endif

cvs co JetMETAnalysis/JetUtilities
cvs co ElectroWeakAnalysis/VPlusJets/BuildFile.xml
cvs co ElectroWeakAnalysis/VPlusJets/interface/QGLikelihoodCalculator.h
cvs co ElectroWeakAnalysis/VPlusJets/src/QGLikelihoodCalculator.C
cvs co ElectroWeakAnalysis/VPlusJets/test/QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root.gz

echo "Unzipping ElectroWeakAnalysis/VPlusJets/test/QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root.gz"
cd ElectroWeakAnalysis/VPlusJets/test/
gunzip QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root.gz
cd -

echo "Copying ElectroWeakAnalysis/VPlusJets/test/QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root to TAMUWW/Selection/test/"
cp ElectroWeakAnalysis/VPlusJets/test/QG_QCD_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6_Spring11-PU_S1_START311_V1G1-v1.root TAMUWW/Selection/test/

echo "Removing EDM_PLUGIN line from ElectroWeakAnalysis/VPlusJets/BuildFile.xml"
cat ElectroWeakAnalysis/VPlusJets/BuildFile.xml | grep -v -i EDM_PLUGIN > ElectroWeakAnalysis/VPlusJets/BuildFile_2.xml
mv ElectroWeakAnalysis/VPlusJets/BuildFile_2.xml ElectroWeakAnalysis/VPlusJets/BuildFile.xml

echo
echo "Do you want to compile these packages? (y/n) "
set comp = $<

if ($comp == "y" ) then
	echo "Compiling..."
	scram b -j 16
	echo "DONE"
	echo
	echo "Compiling again for good measure..."
	scram b -j 16
	echo "DONE"
	echo
	echo "Making rootmaps"
	cd TAMUWW/Selection/test
	rlibmap -f -o TAMUWWSpecialTools.rootmap -l ../../../../lib/slc5_amd64_gcc462/libTAMUWWSpecialTools.so -c ../../SpecialTools/src/SpecialToolsLinkDef.h
	rlibmap -f -o TAMUWWMEPATNtuple.rootmap -l ../../../../lib/slc5_amd64_gcc462/libTAMUWWMEPATNtuple.so -c ../../MEPATNtuple/src/MEPATNtupleLinkDef.h
	cd ../../..
else
	echo "WARNING::Packages will not be compiled"
	echo "WARNING::Rootmaps will not be created in TAMUWW/Selection/test"
	echo
endif

echo "//////////////"
echo "// FINISHED //"
echo "//////////////"
