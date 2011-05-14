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
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p2') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

## Global Tag For Data
process.GlobalTag.globaltag = cms.string('GR_R_38X_V15::All')
##process.GlobalTag.globaltag = cms.string('START38_V14::All')

# output file name
process.out.fileName = 'DataTest.root'

#### Primary Vertex Filter ####
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24.0), 
                                           maxd0 = cms.double(2.0) 
                                           )

#### Kinematic Filters ####
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
from HLTrigger.HLTfilters.hltHighLevel_cfi import *

HLTPathNames=cms.vstring(
    "HLT_Ele10_LW_L1R",
    "HLT_Ele15_SW_L1R",
    "HLT_Ele15_SW_CaloEleId_L1R",
    "HLT_Ele17_SW_CaloEleId_L1R",
    "HLT_Ele17_SW_TightEleId_L1R",
    "HLT_Ele22_SW_TighterEleId_L1R_v2",
    "HLT_Ele22_SW_TighterEleId_L1R_v3"
)
  
# require Electron HLT trigger
from HLTrigger.HLTfilters.hltHighLevel_cfi import *
process.ElHLTDataFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = HLTPathNames )
#process.ElHLTFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = HLTPathNames, HLTPathFilters = cms.untracked.InputTag("hltL1NonIsoHLTNonIsoSingleElectronEt17CaloEleIdPixelMatchFilter","","HLT"))
process.ElHLTDataFilter.throw = False

#### HLT filters ####
process.MuHLTDataFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = cms.vstring("HLT_Mu9","HLT_Mu15*"))
process.MuHLTDataFilter.throw = False

## Implement Simple Cut Based ID for electrons
#process.patElectrons.usePV = cms.bool(False)
#process.patMuons.usePV = cms.bool(False)

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



#removeMCMatching(process, ['All'])

## Add the necessary jet collections ###
datajetcorrections=['L2Relative', 'L3Absolute', 'L2L3Residual']


from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
switchJetCollection( process,
                     jetCollection=cms.InputTag('ak5CaloJets'),
                     jetCorrLabel = ('AK5PF', cms.vstring(datajetcorrections)))


## process.load("RecoJets.JetProducers.ak5PFJets_cfi")
## addJetCollection(process,cms.InputTag('ak5PFJets'),
##                 'AK5', 'PF',
## ##                 doJTA        = False,
##                 doBTagging   = True,
## ##		 jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
##                 jetCorrLabel = ('AK5PF', cms.vstring(datajetcorrections)),
## ##                jetCorrLabel = ('AK5','PF'),
## ##                 doType1MET   = True,
##                 doType1MET   = False,
## ##                 doL1Cleaning = False,
## ##                 doL1Counters = False,
##                 genJetCollection=cms.InputTag("ak5GenJets"),
##                 doJetID      = False
##                )


####Configuration for L1FastJet (https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections).####
##-------------------- Import the JEC services -----------------------
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
##-------------------- Disable the CondDB for the L1FastJet (until they are included in a new global tag) -------
#process.ak5PFL1Fastjet.useCondDB = False
##-------------------- Import the Jet RECO modules -----------------------
process.load('RecoJets.Configuration.RecoPFJets_cff')
##-------------------- Turn-on the FastJet density calculation -----------------------
process.kt6PFJets.doRhoFastjet = True
##-------------------- Include in the path the jet reconstruction  --------------------------------
process.patDefaultSequence = cms.Sequence(process.kt6PFJets * process.patDefaultSequence)


####For PF2PAT####
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.tools.pfTools import *
postfix = "PFlow"
usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC= False , postfix=postfix) 

removeMCMatching(process, ['All'])

### Look at the vertices and remove the tracks coming from an intersection which is not a primary vertex (i.e. likely to be pileup).
process.pfPileUpPFlow.Enable = True

##-------------------- Turn-on the FastJet jet area calculation for your favorite algorithm -----------------------
#process.ak5PFJets.doAreaFastjet = True
process.pfJetsPFlow.doAreaFastjet = True
#getattr(process,"patJets"+postfix).doAreaFastjet = True

process.pfJetsPFlow.JetCorrectionService = cms.string('ak5PFL1FastL2L3Residual')

#### Additiona Modifications
##process.selectedPatJetsPFlow.cut = cms.string('pt > 20 & abs(eta) < 2.4')

#process.patMuonsPFlow.pfMuonSource = 'pfAllMuonsPFlow'
process.patMuonsPFlow.isoDeposits = cms.PSet()
process.patMuonsPFlow.isolationValues = cms.PSet()
#process.patElectronsPFlow.pfElectronSource = 'pfAllElectronsPFlow'
process.patElectronsPFlow.isoDeposits = cms.PSet()
process.patElectronsPFlow.isolationValues = cms.PSet()

## process.selectedPatMuonsPFlow.cut = cms.string("pt > 10")
## process.patMuonsPFlow.usePV = False
## process.patMuonsPFlow.embedTrack = True
## process.selectedPatElectronsPFlow.cut = cms.string("pt > 10")
## process.patElectronsPFlow.usePV = False
## process.patElectronsPFlow.embedTrack = True

## Use the absolute 2D impact parameter (extracted by calling electron.dB()) w.r.t. the average beam spot using the electron track or innerTrack (tracker track) of the muon.
process.patElectronsPFlow.usePV = cms.bool(False)
process.patMuonsPFlow.usePV = cms.bool(False)

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

#### Run the pat sequences ##
process.seq1Data = cms.Sequence(
    process.ElHLTDataFilter*
    process.primaryVertexFilter*
    process.scrapingVeto*
    process.HBHENoiseFilter*
    process.patDefaultSequence
    *getattr(process,"patPF2PATSequence"+postfix)
    *process.ElectronsKinCutBasic
)



process.seq2Data = cms.Sequence(
   process.MuHLTDataFilter*
   process.primaryVertexFilter*
   process.scrapingVeto*
   process.HBHENoiseFilter*
   process.patDefaultSequence
   *getattr(process,"patPF2PATSequence"+postfix)
   *process.MuonsKinCutBasic
)

process.p1 = cms.Path(process.seq1Data)
process.p2 = cms.Path(process.seq2Data)


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
process.out.outputCommands += [
    "drop *_selectedPatElectrons_*_*",
    "drop *_selectedPatMuons_*_*",
#    "drop *_selectedPatPFParticlesPFlow*_*_*",
#    "drop CaloTowers_*_*_*",
    #                               "drop *_selectedPatJetsAK5JPT*_*_*",
#                               "drop *_selectedPatJetsAK5PF_*_*",
    "drop *_selectedPatJetsAK5PFPFlow_*_*",
    "drop recoBaseTagInfosOwned_selectedPatJetsAK5PF_*_*",
    "drop recoBaseTagInfosOwned_selectedPatJetsPFlow_*_*",
    "drop patPFParticles_selectedPatPFParticlesPFlow_*_*",
    "drop *_selectedPatJets_*_*",
    #                               "drop *_patMETsPFPFlow_*_*",
    "drop *_patMETs_*_*"
]


## process.allLayer1Muons.isoDeposits = cms.PSet()
## process.allLayer1Electrons.isoDeposits = cms.PSet()
## process.patMuonsPFlow.isoDeposits = cms.PSet()
## process.patElectronsPFlow.isoDeposits = cms.PSet()

#removeMCMatching(process, ['Electrons'])

process.selectedPatMuons.cut = cms.string('pt> 10. & abs(eta) < 5.0')
process.selectedPatElectrons.cut = cms.string('et> 15. & abs(eta) < 5.0')
#process.selectedPatJetsAK5PF.cut = cms.string('pt> 10. & abs(eta) < 2.5')
process.selectedPatJets.cut = cms.string('pt> 30. & abs(eta) < 5.0')
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

#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_BEBB57F5-8DE7-DF11-8E71-003048F024F6.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_EC2E0313-92E7-DF11-9BCB-001617DBD556.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_7AA51D3E-2FE7-DF11-8E15-003048F118D4.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_AAB07454-1DE7-DF11-9E39-0030487CD906.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_DC57B8EA-6AE6-DF11-9EBF-000423D9997E.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_E026308A-99E6-DF11-AE87-000423D9A2AE.root',
#       'file:/uscms_data/d1/ilyao/SyncEx_3_8_7/Samples/Run2010BMuAODPromptReco-v2_18F1176D-B3E6-DF11-BA41-003048F118C4.root'

       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/F41A3437-7AED-DF11-A50D-002618943894.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/F26679D3-94ED-DF11-A26D-001A92811714.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/A8A590CD-89ED-DF11-BB0A-001A928116B2.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/A89DD38C-A3ED-DF11-A8B9-003048D3FC94.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/7CDBDA60-B1ED-DF11-B217-001A92971B74.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/76C91ED2-A0ED-DF11-AD0F-0030486792BA.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/5C7FF472-98ED-DF11-8511-0018F3D096A4.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/10A70E82-98ED-DF11-9CEB-001A92971B56.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0151/0816F68C-A3ED-DF11-B3D9-001A92971ACC.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/FE495F95-6CED-DF11-826E-0018F3D09700.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/F43CC38E-6CED-DF11-82FF-0026189437F2.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/DE7ED294-6CED-DF11-9CAF-0018F3D095F2.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/C0CA698E-6CED-DF11-8846-0026189437EC.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/C06F3D98-6CED-DF11-965A-003048679228.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/BA0BDE93-6CED-DF11-8C7D-00261894383E.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/B2A79494-6CED-DF11-9BF9-002618943916.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/9218EE99-6CED-DF11-8EAC-0018F3D096AA.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/76660295-6CED-DF11-A4CF-001A928116B4.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/6006DA92-6CED-DF11-805E-00248C55CC97.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/1C737394-6CED-DF11-AC4F-0018F3D0966C.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/18391B99-6CED-DF11-A88E-002618FDA287.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0150/14C3939A-6CED-DF11-962F-001A92971B0E.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0133/1A5BA256-B0EB-DF11-B4C6-00261894395A.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/F82ECB84-11EB-DF11-ADBC-00304867BFAA.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/E299E1B9-0CEB-DF11-A05E-001BFCDBD182.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/C611BD71-0EEB-DF11-8595-002618943809.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/C05256D3-0FEB-DF11-BA3C-00304867BFAA.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/ACEAE81C-23EB-DF11-BE7C-0018F3D095FA.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/AA54E286-11EB-DF11-9E50-0026189438BF.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/A0F4445E-14EB-DF11-8F4C-00304867BEDE.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/8A96D3E7-15EB-DF11-959E-00261894395C.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/7AB2B3AD-27EB-DF11-AE17-0026189438EF.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/68211260-14EB-DF11-883F-001A92810AB8.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/56D8F870-0EEB-DF11-8155-00261894393E.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/54AC9185-11EB-DF11-B092-0026189438FC.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/308639D4-0FEB-DF11-B1AC-00304867BEDE.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/1A279EE8-15EB-DF11-9F1E-002618FDA26D.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0132/049382E9-20EB-DF11-90AA-002618943967.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0131/FEB312A9-E3EA-DF11-8D3F-001BFCDBD154.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0131/FE466D03-0AEB-DF11-A161-00261894393E.root',
       '/store/data/Run2010B/Electron/AOD/Nov4ReReco_v1/0131/FCFE340F-EAEA-DF11-8CBA-002618943974.root'


   )
)

#process.p = cms.Path( process.patDefaultSequence)

