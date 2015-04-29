import FWCore.ParameterSet.Config as cms

skip = cms.untracked.uint32(1)
maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles, skipEvents = skip)
readFiles.extend( [
#	'/store/user/lnujj/PatTuples_8TeV_53X/aperloff/WJetsToLNu_matchingdown_8TeV-madgraph-tauola/SQWaT_PAT_53X_Summer12_v1/829f288d768dd564418efaaf3a8ab9aa/pat_53x_test_v03_10_1_xLL.root' ] );
	    '/store/user/lnujj/PatTuples_8TeV_53X/jdamgov/WH_WToLNu_HToBB_M-125_8TeV-powheg-herwigpp/SQWaT_PAT_53X_Summer12_v1/829f288d768dd564418efaaf3a8ab9aa/pat_53x_test_v03_828_1_5Rd.root' ] );
#       '/store/user/whitbeck/SMS-MadGraph_Pythia6Zstar_8TeV_T1tttt_2J_mGo-1100to1400_mLSP-25to500_50GeVX50GeV_Binning/SMS-MadGraph_Pythia6Zstar_8TeV_T1tttt_2J_mGo-1150_mLSP-475_B2Gpattuples/6a48e960161f4ab4cc640d464b9b3a3e/genParticlesForJets_8_1_18Q.root',
#       '/store/user/whitbeck/SMS-MadGraph_Pythia6Zstar_8TeV_T1tttt_2J_mGo-1100to1400_mLSP-25to500_50GeVX50GeV_Binning/SMS-MadGraph_Pythia6Zstar_8TeV_T1tttt_2J_mGo-1150_mLSP-475_B2Gpattuples/6a48e960161f4ab4cc640d464b9b3a3e/genParticlesForJets_9_1_MQy.root' ] );


secFiles.extend( [
               ] )

