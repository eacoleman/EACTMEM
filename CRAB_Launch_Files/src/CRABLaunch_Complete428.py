###Run with CMSSW_4_2_8
import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *

##---------  Load standard Reco modules ------------
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')



##----- this config frament brings you the generator information ----
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load("PhysicsTools.HepMCCandAlgos.genParticles_cfi")
process.load("Configuration.StandardSequences.Generator_cff")


##----- Detector geometry : some of these needed for b-tag -------
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
process.load("Geometry.CommonDetUnit.globalTrackingGeometry_cfi")
process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")


##----- B-tags --------------
process.load("RecoBTag.Configuration.RecoBTag_cff")


##----- Global tag: conditions database ------------
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#############################################################################################################################
###############################################  Define The Flags ###########################################################
#############################################################################################################################
runwithPF2PATOnly = True

##isData = True
isData = False

#############################################################################################################################
############    Output Module Configuration: must pass either path 'pathEl' or path 'pathMu'   ##############################
#############################################################################################################################
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
			       fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
			       SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('pathEl','pathMu') ),
			       # SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('pathMu') ),
			       # save PAT Layer 1 output; you need a '*' to
			       # unpack the list of commands 'patEventContent'
			       outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
			       )
process.outpath = cms.EndPath(process.out)

### configure input/output
process.out.fileName = 'MC428.root'

### reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

### number of events - Normally Overwritten by CRAB
process.maxEvents.input = 1000

#############################################################################################################################
##########################################    General Preparation   #########################################################
#############################################################################################################################
if isData :
    process.GlobalTag.globaltag = 'GR_R_42_V19::All'
else :
    process.GlobalTag.globaltag = 'START42_V13::All'

#### Scraping Filter ####
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )

#### HB + HE noise filtering ####
##from CommonTools.RecoAlgos.HBHENoiseFilter_cfi import HBHENoiseFilter
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
process.HBHENoiseFilter.minIsolatedNoiseSumE = cms.double(999999.)
process.HBHENoiseFilter.minNumIsolatedNoiseChannels = cms.int32(999999)
process.HBHENoiseFilter.minIsolatedNoiseSumEt = cms.double(999999.)


#### Primary Vertex Filter ####
process.primaryVertex = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),                                
    cut = cms.string("!isFake && ndof >= 4 && abs(z) <= 24 && position.Rho <= 2"), # tracksSize() > 3 for the older cut
    filter = cms.bool(True),   # otherwise it won't filter the events, just produce an empty vertex collection.
)



#############################################################################################################################
##########################################           Jets           #########################################################
#############################################################################################################################
### define the corrections
if isData:
    standardjetcorrections=cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])
else :
    standardjetcorrections=cms.vstring(['L2Relative', 'L3Absolute'])

### Add the PF2PAT Jets ###
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC = not isData, postfix=postfix)
process.pfPileUp.Enable = True
process.pfPileUp.checkClosestZVertex = cms.bool(False)
process.pfPileUp.Vertices = cms.InputTag('primaryVertex') ##** or process.primaryVertex
process.pfJets.doAreaFastjet = True
process.pfJets.doRhoFastjet = False

##-------------------- Import the Jet RECO modules -----------------------
##-------------------- Turn-on the FastJet density calculation -----------------------
from RecoJets.JetProducers.kt4PFJets_cfi import *
process.kt6PFJets = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, voronoiRfact = 0.9)
process.kt6PFJets.Rho_EtaMax = cms.double(4.5)
## ## ??? or from JEC twiki
## from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
## process.kt6PFJetsPFlow = kt4PFJets.clone(
##     rParam = cms.double(0.6),
##     src = cms.InputTag('pfNoElectron'+postfix),
##     doAreaFastjet = cms.bool(True),
##     doRhoFastjet = cms.bool(True)
##     )

## Add them all to the sequence

## getattr(process,"patPF2PATSequence"+postfix).replace( getattr(process,"pfNoElectron"+postfix), getattr(process,"pfNoElectron"+postfix)*process.kt6PFJetsPFlow )
## process.patseq = cms.Sequence(    
##     process.goodOfflinePrimaryVertices*
##     getattr(process,"patPF2PATSequence"+postfix)
##     )


# to compute FastJet rho to correct isolation (note: EtaMax restricted to 2.5)
process.kt6PFJetsForIsolation = kt4PFJets.clone( rParam = 0.6, doRhoFastjet = True, voronoiRfact = 0.9 )
process.kt6PFJetsForIsolation.Rho_EtaMax = cms.double(2.5)

##--------- Turn-on the FastJet jet area calculation for your favorite algorithm ----
from RecoJets.JetProducers.ak5PFJets_cfi import *
ak5PFJets.Rho_EtaMax = cms.double(4.5)
ak5PFJets.doAreaFastjet = True


## ##-------------------- Include in the path the jet reconstruction  --------------------------------##
## ##process.patDefaultSequence = cms.Sequence(  process.kt6PFJets*process.ak5PFJets*process.patDefaultSequence)
## ### Perform Charged Hadron Subtraction (aka PFnoPU)
## process.pfPileUpPFlow.Enable = True
## ##process.pfPileUpPFlow.Vertices = cms.InputTag('offlinePrimaryVerticesDAF')
## process.pfPileUpPFlow.Vertices = cms.InputTag('goodOfflinePrimaryVertices')
## process.pfJetsPFlow.doAreaFastjet = True
## process.pfJetsPFlow.Rho_EtaMax = cms.double(4.5)
## process.pfJetsPFlow.doRhoFastjet = False
## process.patJetCorrFactorsPFlow.rho = cms.InputTag("kt6PFJets", "rho")
## process.pfPileUpPFlow.checkClosestZVertex = cms.bool(False)


removeMCMatching(process, ['All']) # Can't put this after making the standard ak5PFJets & running on Data


### Add the standard PFlow Jets
from PhysicsTools.PatAlgos.tools.jetTools import *
process.load("RecoJets.JetProducers.ak5PFJets_cfi")
from JetMETCorrections.Configuration.DefaultJEC_cff import *
#ak5PFL1Fastjet.srcRho = cms.InputTag('kt6PFJets', 'rho')

addJetCollection(process,cms.InputTag('ak5PFJets'),
                 'AK5', 'PF',
		 doJTA        = True,
                 doBTagging   = True,
                 jetCorrLabel = ('AK5PF',standardjetcorrections),
		 doType1MET   = True,
		 doL1Cleaning = False,
                 doL1Counters = False,                 
                 genJetCollection = cms.InputTag("ak5GenJets")
##                 doJetID      = True
                 )

process.ak5PFJetsL1FastL2L3Residual = ak5PFJetsL1FastL2L3Residual.clone()
## process.ak5PFJetsCor.srcRho = cms.InputTag('kt6PFJets', 'rho')
#from PhysicsTools.PatAlgos.tools.jetTools import *
#addJetCollection(process,cms.InputTag('ak5PFJetsL1FastL2L3'),'AK5','PFL1FastL2L3',
addJetCollection(process,cms.InputTag('ak5PFJetsL1FastL2L3Residual'),'AK5','PFL1FastL2L3Residual',
                 doJTA=False,
                 doBTagging=True,
                 jetCorrLabel=None,
                 doType1MET=True,
                 doL1Cleaning=False,
                 doL1Counters=False,
                 genJetCollection = None
#                 doJetID=True
                 )


## addJetCollection(process,cms.InputTag('ak5PFJetsL2L3'),
##                  'AK5', 'PFInputL2L3',
## ##                 doJTA        = True,
##                  doBTagging   = True,
##                  jetCorrLabel = None,
##                  doType1MET   = False,
##                  doL1Cleaning = False,
##                  doL1Counters = False,                 
##                  genJetCollection = None
## ##                 doJetID      = True
##                  )

###Place at pt cut to clean up the collections
process.selectedPatJetsAK5PFL1FastL2L3Residual.cut = cms.string('pt> 15. & abs(eta) < 2.5')
process.selectedPatJetsPFlow.cut = cms.string('pt> 15. & abs(eta) < 2.5')

### Add pfmet to PAT:
from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, 'PF')

#############################################################################################################################
##########################################          Leptons         #########################################################
#############################################################################################################################

#### HLT filters ####
process.HLTEl =cms.EDFilter("HLTHighLevel",
     TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
     HLTPaths = cms.vstring("HLT_Photon15_Cleaned_L1R", "HLT_Ele15_*", "HLT_Ele17_*", "HLT_Ele22_*", "HLT_Ele25_*","HLT_Ele27_*", "HLT_Ele32_*"),
     eventSetupPathsKey = cms.string(''),
     andOr = cms.bool(True), #----- True = OR, False = AND between the HLTPaths
     throw = cms.bool(False) # throw exception on unknown path names
)
#process.HLTEle.throw = False

process.HLTMu =cms.EDFilter("HLTHighLevel",
     TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
     HLTPaths = cms.vstring('HLT_Mu9','HLT_Mu11','HLT_Mu13','HLT_Mu15_v*','HLT_Mu17_v*','HLT_Mu24_v*','HLT_Mu30_v*','HLT_IsoMu17_v*','HLT_IsoMu24_v*','HLT_IsoMu30_v*'),
     eventSetupPathsKey = cms.string(''),
     andOr = cms.bool(True), #----- True = OR, False = AND between the HLTPaths
     throw = cms.bool(False) # throw exception on unknown path names
 )

#### Basic Kinematic Filters ####
process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patMuonsPFlow"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.4'),
  filter = cms.bool(True)                                
)

process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patElectronsPFlow"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.4'),
  filter = cms.bool(True)
)

#### Some Kinematic Cuts ####
process.selectedPatMuons.cut = cms.string('pt> 15. & abs(eta) < 2.4')
process.selectedPatElectrons.cut = cms.string('pt> 15. & abs(eta) < 2.4')
process.selectedPatMuonsPFlow.cut = cms.string('pt> 15. & abs(eta) < 2.4')
process.selectedPatElectronsPFlow.cut = cms.string('pt> 15. & abs(eta) < 2.4')


#############################################################################################################################
##########################################     Define The Output    #########################################################
#############################################################################################################################
##removeMCMatching(process, ['All']) 
## runOnData(process)

#### Run the pat sequences ##
## process.seqMC = cms.Sequence(
## # process.primaryVertexFilter*
##  process.offlinePrimaryVerticesDAF*   
##  process.primaryDAFVertexFilter*
##  process.scrapingVeto*
##  process.HBHENoiseFilter*
##  process.kt6PFJets*
##  process.kt6PFJetsForIsolation*
## # process.pfJetsPFlow*
##  process.ak5PFJets*
##  process.ak5PFJetsL2L3*
##  process.ak5PFJetsL1FastL2L3*
##  process.patDefaultSequence
##  *getattr(process,"patPF2PATSequence"+postfix)
## )

process.seqEl = cms.Sequence(
# process.primaryVertexFilter*
# process.offlinePrimaryVerticesDAF*   
# process.primaryDAFVertexFilter*
 process.primaryVertex*
 process.scrapingVeto*
 process.HBHENoiseFilter*
 process.HLTEl*
 process.kt6PFJets*
 process.kt6PFJetsForIsolation*
# process.pfJetsPFlow*
 process.ak5PFJets*
 process.ak5PFJetsL1FastL2L3Residual*
 process.patDefaultSequence*
 getattr(process,"patPF2PATSequence"+postfix)
 *process.ElectronsKinCutBasic
)

process.seqMu = cms.Sequence(
# process.offlinePrimaryVerticesDAF*   
# process.primaryDAFVertexFilter*
 process.primaryVertex*
 process.scrapingVeto*
 process.HBHENoiseFilter*
 process.HLTMu*
 process.kt6PFJets*
 process.kt6PFJetsForIsolation*
# process.pfJetsPFlow*
 process.ak5PFJets*
 process.ak5PFJetsL1FastL2L3Residual*
 process.patDefaultSequence*
 getattr(process,"patPF2PATSequence"+postfix)
 *process.MuonsKinCutBasic
)

if not isData :
    process.seqEl.remove( process.HLTEl )
    process.seqMu.remove( process.HLTMu )

if runwithPF2PATOnly :
    process.seqEl.remove( process.ak5PFJets )
    process.seqEl.remove( process.ak5PFJetsL1FastL2L3Residual )
    process.seqEl.remove( process.patDefaultSequence )
    process.seqMu.remove( process.ak5PFJets )
    process.seqMu.remove( process.ak5PFJetsL1FastL2L3Residual )
    process.seqMu.remove( process.patDefaultSequence )


process.pathEl = cms.Path(process.seqEl)
process.pathMu = cms.Path(process.seqMu)


#### Define and cleanup event content ####
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += ['keep *_selectedPat*_*_*','drop *_cleanPat*_*_*']
process.out.outputCommands += patExtraAodEventContent
process.out.outputCommands += ["drop *_towerMaker_*_*"]
process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
process.out.outputCommands += ["drop *_selectedPatTaus*_*_*"]
process.out.outputCommands += ["drop *_selectedPatPhotons*_*_*"]
process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
###process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
process.out.outputCommands += ["keep recoGenParticles_*_*_*"] #keep
#process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
#process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


process.out.outputCommands += ["drop recoGenJets_*_*_*"]
process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]
process.out.outputCommands += ["drop *_offlinePrimaryVerticesWithBS_*_*"]

### Currently Unnecessary:
process.out.outputCommands += [
			       "drop CaloTowers_*_*_*",
			       "drop recoBaseTagInfosOwned_*_*_*",
			       "drop *_selectedPatJets_*_*",
			       "drop *_selectedPatJetsAK5PF_*_*",
			       "drop *_patMETs_*_*",
			       "keep *_*kt6PF*_*_*",
			       "drop double_kt6PFJets*_*_RECO",
			       "drop doubles_*_*_*",
			       "drop recoPFJets_kt6PFJets*_*_*"
			       ]
    
### Input Files For Testing:
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
## ##/SingleElectron/Run2011A-May10ReReco-v1/AOD
##         '/store/data/Run2011A/SingleElectron/AOD/May10ReReco-v1/0005/F6AF587D-807B-E011-ADAD-0025901D4D6C.root',
##         '/store/data/Run2011A/SingleElectron/AOD/May10ReReco-v1/0005/F40E6321-6B7B-E011-9C32-00266CF32F18.root',
##         '/store/data/Run2011A/SingleElectron/AOD/May10ReReco-v1/0005/ECFFF631-A17B-E011-92DE-00266CF33054.root',
##         '/store/data/Run2011A/SingleElectron/AOD/May10ReReco-v1/0005/DE8C0527-927B-E011-A62B-0025901D4936.root',
##         '/store/data/Run2011A/SingleElectron/AOD/May10ReReco-v1/0005/DAB60DF6-677B-E011-AE0B-003048F0E828.root'
## ############ Summer11 W+jets samples ############################
##        '/store/mc/Summer11/WZ_TuneZ2_7TeV_pythia6_tauola/AODSIM/PU_S4_START42_V11-v1/0000/FE05A6E5-6DA2-E011-8066-003048679076.root'
############ Late Summer11 Data samples (/SingleMu/Run2011A-PromptReco-v6/AOD, /SingleElectron/Run2011A-PromptReco-v6/AOD) ############################
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/286219C6-B2CE-E011-9667-001D09F29619.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/2636BEAD-8ACF-E011-BB25-003048F117B4.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/2625BFC8-B2CE-E011-B028-001D09F24303.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/1E5209AB-9CCF-E011-B38C-BCAEC532972E.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/18AA84C5-ABCE-E011-9BAF-001D09F231C9.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/167EC617-B9CE-E011-82C7-BCAEC53296F8.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/160EBB29-36CF-E011-910F-0030487CD710.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/160222BE-ABCE-E011-867A-0019B9F72CE5.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/14144076-83CF-E011-8E9B-003048F01E88.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/0EDAFE6E-C6CE-E011-BEAF-001D09F24763.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/0ADC3B53-E3CE-E011-81D5-E0CB4E4408E3.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/087BD02A-D5CE-E011-97EE-001D09F254CE.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/0825954D-39CF-E011-BD5E-0030486780EC.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/062289AC-31CF-E011-A891-003048F024DE.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/0412002C-D5CE-E011-87D4-0019B9F70468.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/02A75D65-BFCE-E011-82E7-BCAEC5364CFB.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/692/002043AB-DBCE-E011-B097-BCAEC5364C93.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/688/522F0470-D7CD-E011-B6EF-003048F024E0.root',
##        '/store/data/Run2011A/SingleMu/AOD/PromptReco-v6/000/173/677/7492B2BD-B0CD-E011-9549-BCAEC532972C.root'
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/DA2C7B1F-3ECF-E011-AAEF-003048678098.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/C455A2DF-B2CE-E011-8114-0030487CD77E.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/BE5813D0-DBCE-E011-98C1-003048F0258C.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/BC987AD0-F9CE-E011-8E59-003048D374F2.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/BC32D6CA-72CF-E011-BF2A-E0CB4E4408C4.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/BA6D792F-F0CF-E011-AE3E-003048F118D4.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/B8AB1F26-B9CE-E011-895E-E0CB4E5536AE.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/B4D15F6D-BFCE-E011-B594-001D09F24024.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/AE3C2BD0-F9CE-E011-BAA0-003048D37560.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/A0A20932-D5CE-E011-9E6F-BCAEC532970D.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/8E5EFE6C-BFCE-E011-ADC7-003048D37560.root',
       '/store/data/Run2011A/SingleElectron/AOD/PromptReco-v6/000/173/692/8C8F5DE1-B2CE-E011-BFF8-0030487CD14E.root'
   )
)

