import FWCore.ParameterSet.Config as cms

#!
#! DEFAULT PARAMETERS (CHANGE WHEN NEEDED)
#!
#import TAMUWW.Selection.Defaults_cff as Defaults;


#!
#! PROCESS
#!
process = cms.Process("PerformSelectionProcess")


#!
#! SERVICES
#!
#process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
process.load('CommonTools.UtilAlgos.TFileService_cfi')
process.TFileService.fileName=cms.string('PS.root')


#!
#! INPUT
#!
inputFiles = cms.untracked.vstring(
	'file:/gluster/hepx/store/user/aperloff/lnjj/pat_42x_fall11_10_1_jEn.root'
#	'/store/mc/Summer11/QCD_Pt-15to3000_TuneZ2_Flat_7TeV_pythia6/GEN-SIM-RECO/PU_S3_START42_V11-v2/0004/FEE60FB5-457E-E011-98A5-0026189438B8.root',
#	'/store/mc/Summer11/QCD_Pt-15to3000_TuneZ2_Flat_7TeV_pythia6/GEN-SIM-RECO/PU_S3_START42_V11-v2/0004/FE47083B-487E-E011-9BF9-003048678B38.root',
#	'/store/mc/Summer11/QCD_Pt-15to3000_TuneZ2_Flat_7TeV_pythia6/GEN-SIM-RECO/PU_S3_START42_V11-v2/0004/FE15C5FE-537E-E011-8C4E-001A92971B68.root',
#	'/store/mc/Summer11/QCD_Pt-15to3000_TuneZ2_Flat_7TeV_pythia6/GEN-SIM-RECO/PU_S3_START42_V11-v2/0004/FC672445-4F7E-E011-85C8-001A92971B20.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
                            skipEvents = cms.untracked.uint32(0),
                            fileNames = inputFiles )


#!
#! PARAMETERS
#!
process.PS = cms.EDAnalyzer('PerformSelection')

    #-----Input Tags For Handles
    #
    # From PF2PAT, standard postfix=PFlow
    #
process.PS.vtxSource         = cms.InputTag('offlinePrimaryVertices')    # InputTag for vertex collection
process.PS.genParticleSource = cms.InputTag('genParticles')              # InputTag for genParticle collection
process.PS.pfJetSource       = cms.InputTag('selectedPatJetsPFlow')      # InputTag for jet collection
process.PS.electronSource    = cms.InputTag('selectedPatElectronsPFlow') # InputTag for electron collection
process.PS.muonSource        = cms.InputTag('selectedPatMuonsPFlow')     # InputTag for muon collection
process.PS.METSource         = cms.InputTag('patMETsPFlow')              # InputTag for MET collection
                                                                         #IS THIS RIGHT? DIFFERENT LABEL? patMETs? patMETsAK5PF?

    #-----Program Level Inputs
process.PS.outtablefilename = cms.string("outputTable.txt") # name of the file containing the output/efficiency tables
process.PS.outtablename     = cms.string("outputTable")     # name of the event selection table
process.PS.runtype          = cms.string("crab")            # tells the program if it will be run through cmsRun or CRAB
                                                            # - crab - the event table will be saved in the root file only
															# - cmsRun - the event table will be printed to a text file as well
process.PS.modename         = cms.string("none")            # Specialized mode names:
                                                            # - noMETcut - disregard the MET cut when storing the events.
                                                            # - invertEID - electrons with *fail* at least two of the EID requirements will be kept instead.
                                                            # - PFlowLoose - use the collections with the PF2PAT postfix=PFlowLoose (instead of PFlow).
                                                            # - elONLY - only save the output if the lepton is an electron.
                                                            # - muONLY - only save the output if the lepton is a muon.


#!
#! PATH
#!
process.p = cms.Path(
  process.PS
)

#process.options.wantSummary = True
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
