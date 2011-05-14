###Run with 3_8_7 on Data
import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p2') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

## Global Tag For Data
process.GlobalTag.globaltag = cms.string('GR_R_38X_V15::All')
##process.GlobalTag.globaltag = cms.string('START38_V14::All')

# output file name
process.out.fileName = 'SyncV4Mu_MoriondData.root'

#### Primary Vertex Filter ####
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24.0), 
                                           maxd0 = cms.double(2.0) 
                                           )

#### Kinematic Filters ####
process.MuonsKinCutSelection = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patMuons"),                              
  cut = cms.string('isGlobalMuon & isTrackerMuon & pt > 20. & abs(eta) < 2.1'),
  filter = cms.bool(True)                                
)

#### HLT filters ####
from HLTrigger.HLTfilters.hltHighLevel_cfi import *

#### HLT filters ####
#import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = cms.vstring("HLT_Mu9","HLT_Mu15*"))
#process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
#process.MuHLTFilter01.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
#process.MuHLTFilter01.HLTPaths = ["HLT_Mu15_v1"]
#process.MuHLTFilter02.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
#process.MuHLTFilter02.HLTPaths = ["HLT_Mu15_v1"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
process.MuHLTFilter.throw = False


# HB + HE noise filtering
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')

## # add the flavor history
## process.load("PhysicsTools.HepMCCandAlgos.flavorHistoryPaths_cfi")

 
# get the 7 TeV jet corrections
from PhysicsTools.PatAlgos.tools.jetTools import *


# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
process.patElectrons.usePV = cms.bool(False)
process.patMuons.usePV = cms.bool(False)

## ## Implement Simple Cut Based ID for electrons
## process.patElectrons.addElectronID = cms.bool(True)
## process.patElectrons.electronIDSources = cms.PSet(
##     eidRobustLoose= cms.InputTag("eidRobustLoose"),
##     eidRobustTight= cms.InputTag("eidRobustTight"),
##     simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
##     simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
##     simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
##     simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
##     simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
##     simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
##     simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
##     simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
##     simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
##     simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
##     simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
##     simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),    
##     )

## process.load("TAMUWW.CRAB_Launch_Files.simpleEleIdSequence_cff")
## process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
## process.makePatElectrons = cms.Sequence(process.patElectronIDs*process.patElectrons)

removeMCMatching(process, ['All'])

## Add the necessary jet collections ###
datajetcorrections=['L2Relative', 'L3Absolute', 'L2L3Residual']


from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
switchJetCollection( process,
                     jetCollection=cms.InputTag('ak5CaloJets'),
                     jetCorrLabel = ('AK5PF', cms.vstring(datajetcorrections)))


process.load("RecoJets.JetProducers.ak5PFJets_cfi")
addJetCollection(process,cms.InputTag('ak5PFJets'),
                'AK5', 'PF',
##                 doJTA        = False,
                doBTagging   = True,
##		 jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
                jetCorrLabel = ('AK5PF', cms.vstring(datajetcorrections)),
##                jetCorrLabel = ('AK5','PF'),
##                 doType1MET   = True,
                doType1MET   = False,
##                 doL1Cleaning = False,
##                 doL1Counters = False,
                genJetCollection=cms.InputTag("ak5GenJets"),
                doJetID      = False
               )


#### Run the pat sequences ##


## process.seq1 = cms.Sequence(
##     process.primaryVertexFilter*
## #    process.jetPlusTrackZSPCorJetAntiKt5*
##     process.ak5PFJets*
##     process.patDefaultSequence
##     *getattr(process,"patPF2PATSequence"+postfix)
##     *process.ElectronsKinCutBasic
## )

process.seq2 = cms.Sequence(
   process.MuHLTFilter*
   process.primaryVertexFilter*
   process.scrapingVeto*
   process.HBHENoiseFilter*
   process.ak5PFJets*
   process.patDefaultSequence
##    *getattr(process,"patPF2PATSequence"+postfix)
   *process.MuonsKinCutSelection
)

##process.p1 = cms.Path(process.seq1)
process.p2 = cms.Path(process.seq2)


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
#process.out.outputCommands += ["keep recoGenParticles_*_*_*"] #keep
#process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
#process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


process.out.outputCommands += ["drop recoGenJets_*_*_*"]
process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]


### Currently Unnecessary:
process.out.outputCommands += [
#    "drop *_selectedPatElectrons_*_*",
#    "drop *_selectedPatMuons_*_*",
#    "drop *_selectedPatPFParticlesPFlow*_*_*",
#    "drop CaloTowers_*_*_*",
    #                               "drop *_selectedPatJetsAK5JPT*_*_*",
#                               "drop *_selectedPatJetsAK5PF_*_*",
    "drop *_selectedPatJetsAK5PFPFlow_*_*",
    "drop recoBaseTagInfosOwned_selectedPatJetsAK5PF_*_*",
    "drop recoBaseTagInfosOwned_selectedPatJetsAK5PFPFlow_*_*",
    "drop *_selectedPatJets_*_*",
    #                               "drop *_patMETsPFPFlow_*_*",
    "drop *_patMETs_*_*"
]


## process.allLayer1Muons.isoDeposits = cms.PSet()
## process.allLayer1Electrons.isoDeposits = cms.PSet()
process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()

#removeMCMatching(process, ['Electrons'])

process.selectedPatMuons.cut = cms.string('pt> 10. & abs(eta) < 2.5')
process.selectedPatElectrons.cut = cms.string('et> 15. & abs(eta) < 2.5')
#process.selectedPatJetsAK5PF.cut = cms.string('pt> 10. & abs(eta) < 2.5')
process.selectedPatJets.cut = cms.string('pt> 30. & abs(eta) < 2.4')
#process.selectedPatJets.cut = cms.string('pt> 10.')


##
# configure input/output
##
####THIS IS NORMALLY OVERWRITTEN FROM crab.cfg

#number of events
process.maxEvents.input = 100

# source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
##    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0014/84872812-1F4B-DF11-8A07-00151796C158.root',
##    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FEBDE500-4046-DF11-87C6-00151796D9A8.root'
##    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F8769D5D-4146-DF11-850D-0024E8768099.root',
##    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F69DC71E-4046-DF11-841A-00151796D88C.root',
##    '/store/mc/Spring10/WJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F614C63F-4046-DF11-BA9B-0024E8769B87.root'
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FEFE3E46-CB7B-DF11-8488-0018F3D09678.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FEA6BEE7-CA7B-DF11-BEB0-00304867BED8.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FCE7711D-CB7B-DF11-B469-003048678C26.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FAA0922C-6C7B-DF11-82E6-00304867BFC6.root',
#        '/store/mc/Summer10/WJets_7TeV-madgraph-tauola/GEN-SIM-RECO/START36_V9_S09-v1/0046/FA24AFE0-EE7B-DF11-BDB3-001A92971ACE.root'

##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root'

       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_BEBB57F5-8DE7-DF11-8E71-003048F024F6.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_EC2E0313-92E7-DF11-9BCB-001617DBD556.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_7AA51D3E-2FE7-DF11-8E15-003048F118D4.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_AAB07454-1DE7-DF11-9E39-0030487CD906.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_DC57B8EA-6AE6-DF11-9EBF-000423D9997E.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_E026308A-99E6-DF11-AE87-000423D9A2AE.root',
       'file:/uscms_data/d2/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_18F1176D-B3E6-DF11-BA41-003048F118C4.root'

   )
)

#process.p = cms.Path( process.patDefaultSequence)

