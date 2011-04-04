###Run with 3_8_7 (on 3_5_X Monte Carlo)
import FWCore.ParameterSet.Config as cms

## Define the process
#process = cms.Process("Test")

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## uncomment this line to run on an 35X input sample in 36X
#run36xOn35xInput(process)
run36xOn35xInput(process,
                               genJets = "ak5GenJets")

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p2') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

# output file name
process.out.fileName = 'TTbarJ_TestRun.root'

#### Primary Vertex Filter ####
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24.0), 
                                           maxd0 = cms.double(2.0) 
                                           )

#### Lepton Candidate Filters ####
#Kinematic Filters
process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patMuonsPFlow"),                              
  cut = cms.string('pt > 10. & abs(eta) < 5.0'),
  filter = cms.bool(True)                                
)
process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patElectronsPFlow"),                              
  cut = cms.string('et > 15. & abs(eta) < 5.0'),
  filter = cms.bool(True)                                
)

#### HLT filters ####
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
#process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

#### Additional Modifications ####
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('START38_V14::All')
#process.GlobalTag.globaltag = cms.string('START38_V12::All')

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
process.patElectrons.usePV = cms.bool(False)
process.patMuons.usePV = cms.bool(False)

## Implement Simple Cut Based ID for electrons
process.patElectrons.addElectronID = cms.bool(True)
process.patElectrons.electronIDSources = cms.PSet(
    eidRobustLoose= cms.InputTag("eidRobustLoose"),
    eidRobustTight= cms.InputTag("eidRobustTight"),
    simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
    simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
    simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
    simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
    simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
    simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
    simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
    simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
    simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
    simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
    simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
    simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),    
    )

process.load("TAMUWW.CRAB_Launch_Files.simpleEleIdSequence_cff")
process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
process.makePatElectrons = cms.Sequence(process.patElectronIDs*process.patElectrons)

##removeMCMatching(process, ['All'])
removeMCMatching(process, ['Electrons'])


#### Add the necessary jet collections ###
#mcjetcorrections=['L2Relative', 'L3Absolute']
## Add the L1 JPT offset correction for JPT jets
#jptcorrections = ['L1JPTOffset']
#jptcorrections += mcjetcorrections


### from PhysicsTools.PatAlgos.tools.jetTools import *

#process.load("RecoJets.JetProducers.ak5PFJets_cfi")
#addJetCollection(process,cms.InputTag('ak5PFJets'),
#                 'AK5', 'PF',
##                 doJTA        = False,
#                 doBTagging   = True,
##		 jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
#                 jetCorrLabel = ('AK5PF', cms.vstring(mcjetcorrections)),
##                jetCorrLabel = ('AK5','PF'),
##                 doType1MET   = True,
#                 doType1MET   = False,
##                 doL1Cleaning = False,
##                 doL1Counters = False,
#                 genJetCollection=cms.InputTag("ak5GenJets"),
#                 doJetID      = False
##                 doJetID      = False
#                )

## # 1. re-cluster ak5 pfjets and pass them to pat (only for pre-36x input):
## ## process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## ## addJetCollection(process, cms.InputTag('ak5PFJets::Test'), 'AK5', 'PF', jetCorrLabel=('AK5','PF'), doType1MET=False, doJetID = False)
## ## # make sure to run process.ak5PFJets before PAT, for example:
## ## process.patDefaultSequence = cms.Sequence(process.ak5PFJets * process.patDefaultSequence)

## # 2. add jet plus tracks (only for pre-36x input):
## process.load("RecoJets.Configuration.RecoJPTJets_cff")
## addJetCollection(process, cms.InputTag('JetPlusTrackZSPCorJetAntiKt5'), 'AK5', 'JPT', doBTagging   = True,
## #                 jetCorrLabel = ('AK5JPT', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
##                   jetCorrLabel = ('AK5JPT', cms.vstring(jptcorrections)),
## #                 jetCorrLabel = ('AK5','JPT'),
##                  doType1MET=False )
## #run process.jetPlusTrackZSPCorJetAntiKt5 somewhere before PAT, e.g.,
## #process.patDefaultSequence = cms.Sequence(process.jetPlusTrackZSPCorJetAntiKt5 * process.patDefaultSequence)


## # 3. add tcMet and pfmet tp PAT:
## #from PhysicsTools.PatAlgos.tools.metTools import *
## #addPfMET(process, 'PF')
## #addTcMET(process, 'TC')
  


## ####Configuration for L1FastJet (https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections).####
## ##-------------------- Import the JEC services -----------------------
## process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
## ##-------------------- Disable the CondDB for the L1FastJet (until they are included in a new global tag) -------
## #process.ak5PFL1Fastjet.useCondDB = False
## ##-------------------- Import the Jet RECO modules -----------------------
## process.load('RecoJets.Configuration.RecoPFJets_cff')
## ##-------------------- Turn-on the FastJet density calculation -----------------------
## process.kt6PFJets.doRhoFastjet = True

## ##-------------------- Include in the path the jet reconstruction  --------------------------------
## process.patDefaultSequence = cms.Sequence(process.kt6PFJets * process.patDefaultSequence)
## #)


####For PF2PAT####
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC= True , postfix=postfix) 

### Look at the vertices and remove the tracks coming from an intersection which is not a primary vertex (i.e. likely to be pileup).
process.pfPileUpPFlow.Enable = True

## ##-------------------- Turn-on the FastJet jet area calculation for your favorite algorithm -----------------------
## process.patJetsPFlow.doAreaFastjet = True
## #getattr(process,"patJets"+postfix).doAreaFastjet = True
## process.ak5PFJets.doAreaFastjet = True

## #.............................................................................................
## ##-------------------- User analyzer  --------------------------------
## #process.MyAnalyzer  = cms.EDAnalyzer('MyAnalyzer',
## #...............................................................................................
## #process.patJetsPFlow.JetCorrectionService = cms.string('ak5PFL1FastL2L3Residual')
## process.ak5PFJets.JetCorrectionService = cms.string('ak5PFL1FastL2L3Residual')


#### Run the pat sequences ##


process.seq1 = cms.Sequence(
    process.primaryVertexFilter*
#    process.jetPlusTrackZSPCorJetAntiKt5*
#    process.ak5PFJets*
    process.patDefaultSequence
    *getattr(process,"patPF2PATSequence"+postfix)
    *process.ElectronsKinCutBasic
)

process.seq2 = cms.Sequence(
    process.primaryVertexFilter*
    process.MuHLTFilter* 
#    process.jetPlusTrackZSPCorJetAntiKt5*
#    process.ak5PFJets*
    process.patDefaultSequence
    *getattr(process,"patPF2PATSequence"+postfix)
    *process.MuonsKinCutBasic
)

process.p1 = cms.Path(process.seq1)

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
process.out.outputCommands += ["keep recoGenParticles_*_*_*"] #keep
#process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
#process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]


process.out.outputCommands += ["drop recoGenJets_*_*_*"]
process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]


### Currently Unnecessary:
process.out.outputCommands += ["drop *_selectedPatElectrons_*_*",
                               "drop *_selectedPatMuons_*_*",
                               "drop *_selectedPatPFParticlesPFlow*_*_*",
                               "drop CaloTowers_*_*_*",
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

process.selectedPatMuonsPFlow.cut = cms.string('pt> 10. & abs(eta) < 5.0')
process.selectedPatElectronsPFlow.cut = cms.string('pt> 10. & abs(eta) < 5.0')
#process.selectedPatJetsAK5PF.cut = cms.string('pt> 10. & abs(eta) < 2.5')
process.selectedPatJetsPFlow.cut = cms.string('pt> 10. & abs(eta) < 5.0')
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

        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root'

   )
)

#process.p = cms.Path( process.patDefaultSequence)

