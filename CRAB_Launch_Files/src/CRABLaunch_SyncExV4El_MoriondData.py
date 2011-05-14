###Run with 3_8_7
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.coreTools import *


###############################
####### Parameters ############
###############################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
#                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

#corrections = ['L2Relative', 'L3Absolute', 'L2L3Residual']
# global tag for 386 data
process.GlobalTag.globaltag = cms.string('GR_R_38X_V15::All')
#process.GlobalTag.globaltag = cms.string('START38_V15::All')

#### Lepton Candidate Filters ####
#Kinematic Filters
process.ElectronsKinCutSelection = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("patElectrons"),                              
  cut = cms.string('et > 20. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)


#### HLT filters ####
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
process.ElHLTFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = HLTPathNames )
#process.ElHLTFilter = hltHighLevel.clone(TriggerResultsTag = "TriggerResults::HLT", HLTPaths = HLTPathNames, HLTPathFilters = cms.untracked.InputTag("hltL1NonIsoHLTNonIsoSingleElectronEt17CaloEleIdPixelMatchFilter","","HLT"))

process.ElHLTFilter.throw = False

## HLTDescriptionList = [
## #    [ "HLT_Photon10_L1R"                      , "hltL1NonIsoHLTNonIsoSinglePhotonEt10HcalIsolFilter"                     , True  , 15.0 ],
## #    [ "HLT_Photon15_Cleaned_L1R"              , "hltL1NonIsoHLTNonIsoSinglePhotonEt15CleanedHcalIsolFilter"              , False ,  0.0 ],
## #    [ "HLT_Ele15_SW_CaloEleId_L1R"            , "hltL1NonIsoHLTNonIsoSingleElectronEt15CaloEleIdPixelMatchFilter"        , False ,  0.0 ],
##      [ "HLT_Ele17_SW_CaloEleId_L1R"            , "hltL1NonIsoHLTNonIsoSingleElectronEt17CaloEleIdPixelMatchFilter"        , False ,  0.0 ]
## #    [ "HLT_Ele17_SW_TightEleId_L1R"           , "hltL1NonIsoHLTNonIsoSingleElectronEt17TightEleIdDphiFilter"             , False ,  0.0 ],
## #    [ "HLT_Ele17_SW_TighterEleIdIsol_L1R_v2"  , "hltL1NonIsoHLTNonIsoSingleElectronEt17TighterEleIdIsolTrackIsolFilter"  , False ,  0.0 ],
## #    [ "HLT_Ele22_SW_TighterCaloIdIsol_L1R_v1" , "hltL1NonIsoHLTNonIsoSingleElectronEt22TighterCaloIdIsolTrackIsolFilter" , False ,  0.0 ],
## #    [ "HLT_Ele22_SW_TighterCaloIdIsol_L1R_v2" , "hltL1NonIsoHLTNonIsoSingleElectronEt22TighterCaloIdIsolTrackIsolFilter" , False ,  0.0 ],
## ]


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


# HB + HE noise filtering
process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')

## # add the flavor history
## process.load("PhysicsTools.HepMCCandAlgos.flavorHistoryPaths_cfi")

 
# get the 7 TeV jet corrections
from PhysicsTools.PatAlgos.tools.jetTools import *



## # require physics declared
## process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
## process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'

# require scraping filter
process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                    applyfilter = cms.untracked.bool(True),
                                    debugOn = cms.untracked.bool(False),
                                    numtrack = cms.untracked.uint32(10),
                                    thresh = cms.untracked.double(0.25)
                                    )


# switch on PAT trigger
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
process.patTriggerEvent.processName = cms.string( "HLT" )
process.patTrigger.processName = cms.string( "HLT" )


process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(24.0), 
                                           maxd0 = cms.double(2.0) 
                                           )

# Now add the two PF sequences, one for the tight leptons, the other for the loose.
# For both, do the fastjet area calculation for possible pileup subtraction correction. 

#from PhysicsTools.PatAlgos.tools.pfTools import *
#postfix = "PFlow"
#usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC= False, postfix=postfix) 

## postfixLoose = "PFlowLoose"
## usePF2PAT(process,runPF2PAT=True, jetAlgo='AK5', runOnMC= False, postfix=postfixLoose)


# turn to false when running on data
removeMCMatching(process, ['All'])



# We'll be using a lot of AOD so re-run b-tagging to get the
# tag infos which are dropped in AOD
switchJetCollection(process,cms.InputTag('ak5CaloJets'),
                 doJTA        = True,
                 doBTagging   = True,
                 jetCorrLabel = ('AK5Calo', ['L2Relative', 'L3Absolute', 'L2L3Residual']),
                 doType1MET   = True,
                 genJetCollection=cms.InputTag("ak5GenJets"),
                 doJetID      = True
                 )


# Add the other jet collections 
datajetcorrections=['L2Relative', 'L3Absolute', 'L2L3Residual']

process.load("RecoJets.JetProducers.ak5PFJets_cfi")
addJetCollection(process,cms.InputTag('ak5PFJets'),
                 'AK5', 'PF',
#                 doJTA        = False,
                 doBTagging   = True,
#                jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
                 jetCorrLabel = ('AK5PF', cms.vstring(datajetcorrections)),
#                 doType1MET   = True,
                 doType1MET   = False,
#                 doL1Cleaning = False,
#                 doL1Counters = False,
#                 genJetCollection=cms.InputTag("ak5GenJets"),
                 doJetID      = False
#                 doJetId      = False
		 )



## addJetCollection(process,cms.InputTag('JetPlusTrackZSPCorJetAntiKt5'),
##                  'AK5', 'JPT',
##                  doJTA        = True,
##                  doBTagging   = True,
##                  jetCorrLabel = ('AK5JPT', cms.vstring(jptcorrections)),
##                  doType1MET   = False,
##                  doL1Cleaning = False,
##                  doL1Counters = True,                 
##                  genJetCollection = cms.InputTag("ak5GenJets"),
##                  doJetID      = True,
##                  jetIdLabel   = "ak5"
##                  )


# PF from RECO and not using PF2PAT
## addJetCollection(process,cms.InputTag('ak5PFJets'),
##                  'AK5', 'PF',
##                  doJTA        = True,
##                  doBTagging   = True,
##                  jetCorrLabel = ('AK5PF',corrections),
##                  doType1MET   = False,
##                  doL1Cleaning = False,
##                  doL1Counters = False,                 
##                  genJetCollection = cms.InputTag("ak5GenJets"),
##                  doJetID      = False,
##                  jetIdLabel   = "ak5"
##                  )


## # tcMET
## from PhysicsTools.PatAlgos.tools.metTools import *
## addTcMET(process, 'TC')
## addPfMET(process, 'PF')

## Add the METs
from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, 'PF')



# Selection
## process.selectedPatJetsPFlow.cut = cms.string('pt > 20 & abs(eta) < 2.4')
## process.selectedPatJetsPFlowLoose.cut = cms.string('pt > 20 & abs(eta) < 2.4')
## process.selectedPatJetsAK5JPT.cut = cms.string('pt > 20 & abs(eta) < 2.4')
## process.selectedPatJetsAK5PF.cut = cms.string('pt > 20 & abs(eta) < 2.4')
## process.selectedPatJets.cut = cms.string('pt > 20 & abs(eta) < 2.4')
## process.patJets.tagInfoSources = cms.VInputTag(
##     cms.InputTag("secondaryVertexTagInfosAOD")
##     )
## process.patJetsPFlow.tagInfoSources = cms.VInputTag(
##     cms.InputTag("secondaryVertexTagInfosAOD" + postfix)
##     )
## process.patJetsPFlowLoose.tagInfoSources = cms.VInputTag(
##     cms.InputTag("secondaryVertexTagInfosAOD" + postfixLoose)
##     )
## process.patJetsAK5JPT.tagInfoSources = cms.VInputTag(
##     cms.InputTag("secondaryVertexTagInfosAK5JPT")
##     )
## process.patJetsAK5PF.tagInfoSources = cms.VInputTag(
##     cms.InputTag("secondaryVertexTagInfosAK5PF")
##     )


## process.patMuonsPFlowLoose.pfMuonSource = 'pfAllMuonsPFlowLoose'
## process.patMuonsPFlowLoose.isoDeposits = cms.PSet()
## process.patMuonsPFlowLoose.isolationValues = cms.PSet()
## process.patElectronsPFlowLoose.pfElectronSource = 'pfAllElectronsPFlowLoose'
## process.patElectronsPFlowLoose.isoDeposits = cms.PSet()
## process.patElectronsPFlowLoose.isolationValues = cms.PSet()

## process.selectedPatMuons.cut = cms.string("pt > 10")
## process.selectedPatMuonsPFlow.cut = cms.string("pt > 10")
## process.selectedPatMuonsPFlowLoose.cut = cms.string("pt > 10")
process.patMuons.usePV = False
#process.patMuonsPFlow.usePV = False
#process.patMuonsPFlowLoose.usePV = False

## process.patMuons.embedTrack = True
## process.patMuonsPFlow.embedTrack = True
## process.patMuonsPFlowLoose.embedTrack = True

## process.selectedPatElectrons.cut = cms.string("pt > 10")
## process.selectedPatElectronsPFlow.cut = cms.string("pt > 10")
## process.selectedPatElectronsPFlowLoose.cut = cms.string("pt > 10")
process.patElectrons.usePV = False
#process.patElectronsPFlow.usePV = False
#process.patElectronsPFlowLoose.usePV = False

## process.patElectrons.embedTrack = True
## process.patElectronsPFlow.embedTrack = True
## process.patElectronsPFlowLoose.embedTrack = True

#### Run the pat sequences ##
process.seq1 = cms.Sequence(
    process.ElHLTFilter*
    process.scrapingVeto*
    process.primaryVertexFilter*
    process.HBHENoiseFilter*
    process.patDefaultSequence
    *process.ElectronsKinCutSelection
#    *getattr(process,"patPF2PATSequence"+postfix)
)

process.p1 = cms.Path(
  process.seq1
)
  
process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()


#process.out.SelectEvents.SelectEvents = cms.vstring('p1')


# rename output file
process.out.fileName = cms.untracked.string('SyncV4El_MoriondData.root')

# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

# process all the events
#process.maxEvents.input = -1
process.maxEvents.input = 1000
#process.options.wantSummary = True
#process.out.dropMetaData = cms.untracked.string("DROPPED")

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
process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]
process.out.outputCommands += ["drop recoGenJets_*_*_*"]
process.out.outputCommands += ["drop recoPFCandidates_*_*_*"]

#process.out.outputCommands += ["drop reco*_*_*"]
process.out.outputCommands += ['keep recoTracks_generalTracks*_*_*','keep patTriggerPaths_patTrigger_*_*','keep patTriggerEvent_patTriggerEvent_*_*']
process.out.outputCommands += ["drop *_selectedPatElectronsPFlow*_*_*"]
process.out.outputCommands += ["drop *_selectedPatMuonsPFlow*_*_*"]


### Currently Unnecessary:
process.out.outputCommands += [
#			       "drop *_selectedPatElectronsPFlow*_*_*",
#                               "drop *_selectedPatMuonsPFlow*_*_*",
#                               "drop *_selectedPatPFParticlesPFlow*_*_*",
                               "drop CaloTowers_*_*_*",
#                               "drop *_selectedPatJetsAK5JPT*_*_*",
#                               "drop *_selectedPatJetsAK5PFPFlow_*_*",
#                               "drop recoBaseTagInfosOwned_selectedPatJetsAK5PFPFlow_*_*",
                               "drop *_selectedPatJets_*_*",
#                               "drop *_patMETsPFPFlow_*_*",
                               "drop *_patMETs_*_*"
]

process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()


process.selectedPatMuons.cut = cms.string('pt> 10. & abs(eta) < 2.5')
process.selectedPatElectrons.cut = cms.string('et> 20. & abs(eta) < 2.5')
process.selectedPatJetsAK5PF.cut = cms.string('pt> 30. & abs(eta) < 2.5' )

# Add the files 
#process.source.fileNames = readFiles
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/D2078F1E-7EEB-DF11-81DB-003048F0E838.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/B656E675-7EEB-DF11-9EFA-003048F0E524.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/5C99DD8C-6EEB-DF11-BCAE-003048D3CB3C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/54F60D8A-6EEB-DF11-B1FB-003048D43656.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/46259834-6EEB-DF11-91BF-003048C69046.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/3878CD69-6EEB-DF11-824C-002481E0D2EA.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/320B0CA8-6EEB-DF11-B3A8-003048D4385C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/2871932D-C9EB-DF11-96B4-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0039/0A61EA3B-C9EB-DF11-BE92-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0038/56CDFC97-5CEB-DF11-A08D-003048F0E1E8.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0038/42E037F4-5CEB-DF11-B2E9-003048F02CB8.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FECBE0C3-23EB-DF11-ACC7-003048F0E528.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FEC22A3C-24EB-DF11-8E98-003048F0E18C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FEA8A4A1-22EB-DF11-9B1C-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FEA823E9-24EB-DF11-AC94-003048D4363C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FE66E7B5-21EB-DF11-AFF0-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FE64D7EF-21EB-DF11-A9F0-003048F0E5A4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FE5FCAEC-22EB-DF11-B0A0-003048F02CB4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FE5B1FBD-24EB-DF11-9D8E-003048D4DFAA.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FE21A2B3-21EB-DF11-AF69-003048F0E18E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FC650DB9-23EB-DF11-A110-003048CF6334.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FC4B3DB9-21EB-DF11-8656-003048F02CB4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FC1F1365-21EB-DF11-867C-003048F02D36.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FAFE3AB3-23EB-DF11-AC8B-003048F0E186.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FA6AAD5B-24EB-DF11-AED8-002481E0DC4E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/FA4BE674-23EB-DF11-A80E-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F8AA55B9-21EB-DF11-9B3E-003048CFCBB0.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F6DC57DD-20EB-DF11-9C45-003048CFCBB2.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F6D1E770-21EB-DF11-AF62-002481E0D708.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F640DAE7-20EB-DF11-BB03-002481E0D568.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F4FB1BBA-21EB-DF11-B83A-002481E1026A.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F4EAE374-23EB-DF11-B7F2-003048D4DEDC.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F4CBB6C8-20EB-DF11-B9BE-002481E0D568.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F4A03093-20EB-DF11-97E1-003048F0E1B0.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F49C08E4-23EB-DF11-BF21-002481E0DC4C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F47F825C-21EB-DF11-A8A1-003048CF632E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F2EDBD76-23EB-DF11-AF3B-003048D4DFA4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F2D5F42E-24EB-DF11-B26E-003048CFCBB2.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F2A6C76D-23EB-DF11-90B0-003048F0E18A.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F292FDF7-21EB-DF11-9F92-003048CF632E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F2588340-24EB-DF11-B06D-003048F010F4.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F21B4972-23EB-DF11-A4A7-002481E0DC4E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F0C920D8-20EB-DF11-AD40-003048CF6334.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F09D7517-22EB-DF11-A1F7-002481E0D5E2.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/F0550790-23EB-DF11-875E-003048F0E3AC.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EE9F7106-23EB-DF11-906B-002481E101DC.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EE999CB3-23EB-DF11-BD3C-003048F0EBBC.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EE3606C9-20EB-DF11-91A2-003048F0E83C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EE0CFA7E-23EB-DF11-B4B0-003048D3CB3C.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EC568FD5-21EB-DF11-A7CA-003048F0E18E.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EC44C6ED-20EB-DF11-BD40-003048F0E55A.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EC351CBA-21EB-DF11-82BF-002481E1026A.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EAFC96B0-22EB-DF11-92D3-003048F0E184.root',
##        '/store/data/Run2010A/EG/AOD/Nov4ReReco_v1/0037/EAFAACC1-20EB-DF11-915E-003048F0E55A.root'

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
