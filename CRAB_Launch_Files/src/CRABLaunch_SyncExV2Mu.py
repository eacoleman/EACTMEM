### Basic Template:
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## ### Some Tools and a hack for working with 3.3.1 MC
## #############################################################
## from PhysicsTools.PatAlgos.tools.jetTools import *
## from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## #from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoLayer1Cleaning
## #from PhysicsTools.PatAlgos.patEventContent_cff import *
## # run the 3.3.x software on Summer 09 MC from 3.1.x:
## #   - change the name from "ak" (3.3.x) to "antikt) (3.1.x)
## #   - run jet ID (not run in 3.1.x)
## ### run33xOn31xMC( process,
## ##               jetSrc = cms.InputTag("antikt5CaloJets"),
## ##               jetIdTag = "antikt5"
## ##               )

from PhysicsTools.PatAlgos.tools.coreTools import *



from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
## uncomment this line to run on an 35X input sample
run36xOn35xInput(process)

#################################################################
##Output Module Configuration: must pass either path 'p1' or path 'p2'.
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

# output file name

process.out.fileName = 'SyncExV2Mu.root'

##SkipEvent = cms.untracked.vstring('ProductNotFound')
## Set the right sample for JEC factors here
#process.patJetCorrFactors.corrSample      = 'Summer09_7TeV_ReReco332'
#process.patJetCorrFactorsAK5PF.corrSample = 'Summer09_7TeV_ReReco332'


## ## Lepton Candidate Filters
## #Kinematic Filters
## process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
##   src = cms.InputTag("allLayer1Muons"),                              
##   cut = cms.string('pt > 15. & abs(eta) < 2.5'),
##   filter = cms.bool(True)                                
## )
## process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
##   src = cms.InputTag("allLayer1Electrons"),                              
##   cut = cms.string('pt > 15. & abs(eta) < 2.5'),
##   filter = cms.bool(True)                                
## )

## # HLT filters
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
#process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","REDIGI")
process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

## process.ElHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
## # Uncomment this to access 8E29 menu and filter on it
## process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
## ## ###after ICHEP2010
## ## process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]
## ###before ICHEP2010
## process.ElHLTFilter.HLTPaths = ["HLT_Photon10_L1R"]
## # Uncomment this to filter on 1E31 HLT menu
## #process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]



## ## from TrackingTools.TransientTrack.TransientTrackBuilder_cfi import*
## ## process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
## ## process.load('Configuration/StandardSequences/GeometryIdeal_cff')
## ## process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
## ## process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.GlobalTag.globaltag = cms.string('START36_V9::All')

##process.patMuons.usePV = False
process.patElectrons.usePV = cms.bool(False)

## Run the pat sequences
## process.p1 = cms.Path(
##   process.patDefaultSequence
##   *process.MuonsKinCutBasic
##   *process.MuHLTFilter  
## )

process.p2 = cms.Path(
  process.patDefaultSequence
##  *process.ElectronsKinCutBasic
  *process.MuHLTFilter
##  *process.demo
##  *process.ModifiedCleanMuons
)

## process.p2 = cms.Path(
##   process.patDefaultSequence
## ##  *process.ElectronsKinCutBasic
##   *process.ElHLTFilter
## #  *process.patElectronIDs
## ##  *process.demo
## ##  *process.ModifiedCleanElectrons
## )

## ## Run the pat default dequence
##process.p = cms.Path(
##  process.patDefaultSequence
##)


## Define and cleanup event content
#removeSpecificPATObjects(process, ['Photons'])
#removeAllPATObjectsBut(process, ['Electrons','Muons','Jets','METs'])

removeMCMatching(process, ['All'])
process.patMuons.isoDeposits = cms.PSet()
process.patElectrons.isoDeposits = cms.PSet()



## process.load("TAMUWW.CRAB_Launch_Files.simpleEleIdSequence_cff")
## process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
## process.makePatElectrons = cms.Sequence(process.patElectronIDs*process.patElectrons)


## Handle<reco::MuonCollection> MyMuonsHandle;
## iEvent.getByLabel("muons", MyMuonsHandle);

## pat::Muon * someMuon = getAMuonFromSomewhere.clone();
## float relIso = someMuon->pt() / ( someMuon->pt() + someMuon->caloIso() + someMuon->trackIso() );
## someMuon->addUserFloat( "relIso", relIso);


## ### this works:
## process.patMuons.userData.userFunctions = cms.vstring('pt() / (trackIso() + caloIso() + pt())')
## process.patMuons.userData.userFunctionLabels = cms.vstring('RRRelIso')


#process.patMuons.userData.userFunctions = cms.vstring('ddd0')
#process.patMuons.userData.userFunctionLabels = cms.vstring('RRRelIso')

##    Handle<reco::MuonCollection> MyMuonsHandle;
##    iEvent.getByLabel("muons", MyMuonsHandle);

   

## const edm::EventSetup& es;

## reco::TrackRef  elTrkRef=elIter->track();
## reco::TransientTrack * build ( elTrkRef );

## edm::ESHandle<TransientTrackBuilder> trackBuilder;
## es.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);
## eventCont.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);
## reco::TransientTrack tt = trackBuilder->build(elTrkRef);

## std::pair<bool,Measurement1D> impPar=IPTools::absoluteImpactParameter3D(tt,Vtx);
## ##std::pair<bool,Measurement1D> impParTransverse = IPTools::absoluteTransverseImpactParameter(tt,Vtx);
## cout << "impPar=" << impPar.second.value() << endl;
## ## cout << "impParSignificance=" << impPar.second.significance() << endl;

###elIter->superCluster().get()->eta()


## pat::Muon * someMuon = patMuons.clone();
## float RRRelIso = someMuon->pt() / ( someMuon->pt() + someMuon->caloIso() + someMuon->trackIso() );
## someMuon->addUserFloat( "RRRelIso", RRRelIso);

## process.selectedLayer1Muons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
## process.selectedLayer1Electrons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
## process.selectedLayer1Jets.cut = cms.string('pt> 10.')

## Define event content; keep the cleanPatCandidates which are
## the same as selectedPatCandidates PLUS some extra information
## on overlaps across collections.
## Adds the Primary Vertex Info
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += patExtraAodEventContent
process.out.outputCommands += ["drop *_towerMaker_*_*"]
process.out.outputCommands += ["drop l1extraParticles_*_*_*"]
process.out.outputCommands += ["drop *_cleanPatTaus*_*_*"]
process.out.outputCommands += ["drop *_cleanPatPhotons*_*_*"]
process.out.outputCommands += ["drop *_offlineBeamSpot_*_*"]
process.out.outputCommands += ["drop edmTriggerResults_TriggerResults*_*_*"]
process.out.outputCommands += ["drop *_hltTriggerSummaryAOD_*_*"]
##process.out.outputCommands += ["drop recoTracks_generalTracks*_*_*"]
process.out.outputCommands += ["drop recoGenParticles_genParticles*_*_*"]
process.out.outputCommands += ["drop GenEventInfoProduct_*_*_*"]
process.out.outputCommands += ["drop GenRunInfoProduct_*_*_*"]




##
# configure input/output
##
####THIS IS NORMALLY OVERWRITTEN FROM crab.cfg

#number of events
process.maxEvents.input = 100




## source
process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring(
               '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0016/6E7C4631-9D47-DF11-96CE-003048C69288.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0011/A4121AB4-0747-DF11-8984-0030487F171B.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE8DE204-C446-DF11-BF76-003048C693FA.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE14F78B-C446-DF11-818D-003048C6930E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FE0DD96C-B846-DF11-B5AE-0030487EB003.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FCBBBE31-C646-DF11-95DE-003048C66182.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FC49E900-B746-DF11-8409-003048D437F2.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FC231BCA-AF46-DF11-AF89-0030487D5E75.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FADB0E57-BF46-DF11-BD27-0030487D5DBB.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/FA1F946C-B846-DF11-A79B-003048C68A7C.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F8E786A5-BC46-DF11-81DE-0030487F1BD5.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F8BAA704-C446-DF11-957A-003048D4399E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F83E4870-B146-DF11-9275-0030487F92A5.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F6E8B0CA-AF46-DF11-B32C-003048D4DF90.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F6D86292-CD46-DF11-B2FD-003048C69312.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F460AD1A-BB46-DF11-9409-0030487F1BD3.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F21C6A2C-C946-DF11-8A4D-003048C692F2.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/F0131DD2-AD46-DF11-A5A2-0030487FA623.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/EE4479CE-AD46-DF11-94AC-003048D3CA06.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/EE24725C-BF46-DF11-BC0C-0030487D5D6B.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/ECF5CBCA-AF46-DF11-85D3-0030487F1A31.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/EC1134A6-BE46-DF11-8BE5-003048C692B6.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E8A4F09C-CC46-DF11-97FC-003048C6903E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E8707C6F-CF46-DF11-8EF9-0030487E54B7.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E81F50CB-C846-DF11-88B1-003048D436D2.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E6895216-B446-DF11-81E2-003048D4396E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E4C530A5-BC46-DF11-BCC8-003048D439C0.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E4BC49A4-BC46-DF11-9173-0030487D5EB5.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E46DE7D2-AD46-DF11-AC96-0030488A1170.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E274F8BF-BE46-DF11-9B69-0030487D5D6B.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E0EE7168-B146-DF11-A49C-0030487F1A31.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E0B9E819-B446-DF11-B00F-0030487EB003.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E05231BA-AF46-DF11-B988-003048D4397E.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/E0477632-C146-DF11-B763-0030487F171D.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/DE798258-B446-DF11-9C85-003048C692B8.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/DC85EA07-B646-DF11-9DFB-0030487E5101.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/DC7C20E6-BE46-DF11-BB62-0030487D5EB5.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/DAF6BF5A-BF46-DF11-8E6E-003048D43642.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/DA09FACE-AD46-DF11-BC3E-0030487F91D9.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D87D77D2-C946-DF11-AD67-0030487D5E81.root',
        '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D81AB990-B946-DF11-8652-003048D439C6.root'
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D6B892D1-AD46-DF11-9E74-003048C68A9C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D66C7B6A-B946-DF11-BC60-0030487D5DB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D4CF8746-B146-DF11-8C55-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D4BABE8C-C646-DF11-9696-003048D43838.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D4B6D058-BF46-DF11-BB09-0030487D5DC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D0E15856-AE46-DF11-A57C-0030487E4D11.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/D0BEF320-C646-DF11-A1F9-0030487D5EB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/CE2C4F52-AF46-DF11-A3B1-003048C692FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/CC9C8F5C-BF46-DF11-A6D3-0030487F9367.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/CA4DAD5B-AD46-DF11-9798-003048C66184.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C8C6F907-B646-DF11-8E20-0030487E4B8D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C8ACDC06-B746-DF11-A5FB-003048D439B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C8530C8C-C446-DF11-A4BE-0030487D5DC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C6E4F4E7-BE46-DF11-8D21-003048D439B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C4D7846D-B846-DF11-95E6-0030488A0EFA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C42E9C5C-BF46-DF11-8CEA-003048C69424.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C41AD1AB-BD46-DF11-A5D2-0030487D5EB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C266836D-B946-DF11-81F3-0030487D605B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C085A4A7-BE46-DF11-9AC3-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C0656390-B946-DF11-A30A-0030487EB003.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/C03D19D2-AD46-DF11-A97C-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/BE7E89AA-AD46-DF11-9625-0030487F1A47.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/BE526221-C646-DF11-97B5-003048C6903E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/BE44D331-C146-DF11-973A-003048D3CA06.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/BA990A41-C546-DF11-8114-003048D4363C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/BA5D658C-C446-DF11-9779-003048D4DFA6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B8C0E4D1-AD46-DF11-A664-003048C66182.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B635B322-C646-DF11-AA32-0030487F1BD3.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B4E69892-CB46-DF11-A646-0030487E4ED5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B4C29C8C-C446-DF11-A873-0030487F1BD7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B47C6690-C946-DF11-8BC0-003048C692FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/B2228D5C-B546-DF11-B824-003048D437F2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AEE2DD45-B046-DF11-870A-0030487D5DC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AECE5AA8-BC46-DF11-9E69-0030487FA4C5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AE1706AE-BD46-DF11-B831-003048C693EE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AC83C8A5-BC46-DF11-8F91-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AC12C840-C546-DF11-9BDC-0030487D5D67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AACD846B-B146-DF11-92BF-003048C691DC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AAB08E60-B646-DF11-8BEB-0030487D5EA7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AA42EEA4-BC46-DF11-9FAD-0030487F0EDF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AA41DD59-BF46-DF11-A046-0030487F1C57.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/AA373E6F-AD46-DF11-AF34-00304889D54E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A8D8A0D1-AD46-DF11-BBCD-003048C6903C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A629B3AE-C946-DF11-8D0C-0030487F1A4F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A44BC606-B746-DF11-9C10-003048C693BA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A2BB7E5A-BF46-DF11-BFC4-0030487E55B9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A2A6DA67-B146-DF11-A28C-003048D47912.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A27ECA5A-BF46-DF11-9F35-0030487E4EC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A0C17E5B-BF46-DF11-8D86-003048D47976.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A09330A6-BE46-DF11-BE6C-003048D43726.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A03E9C2F-C946-DF11-B156-0030487F1659.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/A0191008-B646-DF11-B703-003048D462FE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/9C6A9C58-B446-DF11-8663-003048C692F2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/98EEB601-B746-DF11-91BF-003048C693E6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/98E18A5F-B546-DF11-87AC-003048D47912.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/980CDB37-C146-DF11-A40E-003048C692A2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/98022C5C-BF46-DF11-9298-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/96E66E04-B646-DF11-9C17-0030487D5D7B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/96D6C78C-C646-DF11-9F6B-003048D436EA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/94F628D1-AD46-DF11-96AA-0030487D5D95.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/94A36D8C-C446-DF11-A9EC-003048D439C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/94A1A2D4-AD46-DF11-809E-003048C69296.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/94956959-B446-DF11-9AAD-003048D43726.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/92CE4F66-AD46-DF11-BE48-00304889D54E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/9299B545-B146-DF11-93A9-003048D43642.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/9252E716-C846-DF11-A3CD-0030487F9297.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/9218FD37-C146-DF11-8938-0030487F0EDD.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/90403360-B546-DF11-9612-003048D4DF90.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/90369E5E-B546-DF11-9F01-0030487D5E4B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8E9D848E-C646-DF11-A19B-003048D436EA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8E838732-C646-DF11-9C20-0030487F1F23.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8C7BC7C2-BE46-DF11-8805-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8C69F28D-C646-DF11-9764-003048D4DFA6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8C04BD37-C146-DF11-8541-003048D4399E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8A21685B-BE46-DF11-937A-003048D439B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/88F21FA9-AD46-DF11-9126-003048C69416.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/88C858C0-BE46-DF11-AA12-003048C693EE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/88589000-B746-DF11-9C76-0030487D857D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/80A5E062-B546-DF11-A412-0030487E52A1.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/8095D857-BF46-DF11-8C98-0030487D857D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/808BAA30-C946-DF11-AA33-0030487D5E4B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/802F5C06-B746-DF11-8A00-0030487D5D8F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7EB5C15A-BF46-DF11-A253-003048D43642.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7E7E67BB-BD46-DF11-8D22-0030487F1C57.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7E0A4A5A-BF46-DF11-8775-0030487D5DC3.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7CFDB856-AE46-DF11-9B4E-003048D437DA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7CD93B42-CD46-DF11-B99C-003048C6931C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7C948992-C446-DF11-AA8C-0030487D86C9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7C45948C-CD46-DF11-B2DF-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7AF98531-C146-DF11-9B29-0030487D5D67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7ADBD807-B646-DF11-A69A-0030487E4B8D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7ABD3160-C846-DF11-AD5C-0030487D5DB1.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7AA8A857-BF46-DF11-839E-0030487D5E81.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/78EEFB93-B946-DF11-A90C-0030487F933D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7899E434-C146-DF11-A03A-003048D43642.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/786B9BBD-AF46-DF11-8787-003048D37394.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/7817D045-B046-DF11-8B6C-003048D43642.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/720766A9-BD46-DF11-821D-003048D43980.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/70C9AC5A-BF46-DF11-A0EF-0030487D5E4B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/70251D91-C646-DF11-B69C-003048D4DF80.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6EFBB018-B446-DF11-B895-003048D43726.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6ED11ABB-BD46-DF11-888C-003048D436FE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6ECC81AA-BC46-DF11-BF70-0030487E4EC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6E5B0B90-B946-DF11-B9D0-003048D436D2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6C1EECA4-BC46-DF11-90FA-0030487E4EC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6A7CCEBE-AF46-DF11-892B-00304889D54E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/66C8BEC2-BE46-DF11-94CD-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/64FA5E6C-B846-DF11-AD26-003048D436D2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/643E178C-C446-DF11-8F9F-003048D43726.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/62AB2B8F-C446-DF11-9D38-003048C6931E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6272885C-BF46-DF11-BB84-003048C692DE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/6226326C-B146-DF11-B329-003048D2BB22.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5EAC389A-D246-DF11-8165-0030487E55B9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5C8C3D32-C146-DF11-A28B-0030487F9297.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5AA62D6E-B946-DF11-B09F-0030487F1F23.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5A6A9E5B-BF46-DF11-8456-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5A40BD37-C146-DF11-B0B0-003048D439C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5A3040BD-AF46-DF11-A798-0030487D5DBB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5A204EBA-AF46-DF11-BEEC-003048D3CD92.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/5A1ADC45-B046-DF11-B585-003048D47976.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/589FA1AA-BC46-DF11-8F38-0030487E4EC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/569276A6-B946-DF11-9CE7-003048D4DFAA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/54ECA4A7-BE46-DF11-8484-0030487F9381.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/54C009A8-BC46-DF11-AF7F-003048C66182.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/543BBCBF-BD46-DF11-BB05-0030487D5DBB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/507269C0-BE46-DF11-987E-003048C693EC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/500CB2AC-BD46-DF11-B730-0030487F3EA7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/4C98C360-B646-DF11-95AE-0030487D8563.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/4C4A0E8D-C946-DF11-BCAC-003048D437D2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/4A10084C-B146-DF11-BBDD-003048D2BB22.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/48DE17CE-AD46-DF11-9D04-003048C692B4.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/46833E94-B946-DF11-8139-0030487F92B7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/44E8F92C-C646-DF11-A761-003048D3C7DC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/42C76207-C446-DF11-AB86-003048C68A9C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/40F58657-BF46-DF11-A306-003048C693F8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/40C76301-B746-DF11-A60A-003048C692E0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/40C0E4E3-BE46-DF11-90E4-0030487D5D67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3E9421CB-AF46-DF11-8129-0030487F91D9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3E034FD1-AD46-DF11-9FD4-0030487D5DA9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3CEB4E5C-BF46-DF11-A31A-0030487F9295.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3C781E41-C446-DF11-8ABF-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3C4C08C0-BE46-DF11-98BC-0030487E55B9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3C0CFF6D-B946-DF11-BB29-0030487D5DC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3ADAD2B9-AF46-DF11-8306-0030487EBB2B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3A115A21-C646-DF11-9E9E-003048C68A9C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3A0D0AF5-C346-DF11-B597-003048D43986.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/369ECACA-AF46-DF11-9EC8-0030487F91DB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/36602C16-C846-DF11-B0E5-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/36378C6C-B846-DF11-93DB-0030487F9297.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/34DAFBB9-AF46-DF11-9CF0-0030487D5D95.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/34CD484F-BC46-DF11-8E9E-003048C692B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3488A55C-BF46-DF11-980B-003048C692DE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/30E66E80-C846-DF11-85B2-003048C693EE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/3078B457-BF46-DF11-8C82-003048C69418.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/30239CA4-BC46-DF11-A633-003048C6903E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2E391890-B946-DF11-BEB7-003048D4396E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2E29D66C-B146-DF11-8437-0030487F9295.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2AE03932-C146-DF11-B19E-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2A7013A9-BD46-DF11-90FA-003048C692B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2A0BF06A-B146-DF11-AAF6-0030487D5E99.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/26F0B344-C446-DF11-A3C9-0030487F1BD3.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/269820C9-AD46-DF11-BE85-003048C692D8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/267F13CA-AF46-DF11-A622-0030487D5DA9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/24610268-B146-DF11-AA43-003048D4DF90.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/22DBC25B-AD46-DF11-9BF4-003048C66184.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/22C16517-B446-DF11-94B5-003048D43642.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/2245B959-AE46-DF11-B4C7-003048C68A84.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/22131BBB-BD46-DF11-9289-003048C692B8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/205D2647-B146-DF11-B1C4-0030488A0F1C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/1EAF1BAB-BC46-DF11-89EE-003048C6931E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/1E7EB265-B546-DF11-AF88-0030487D5DB7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/1C79D658-B446-DF11-A32F-0030487F0EDF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/1AD51B75-AD46-DF11-BF4A-003048C66184.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/18DBE360-C846-DF11-81EC-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/18604D6B-B146-DF11-9459-0030487F1A73.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/161E3D54-AE46-DF11-AF49-003048C693E6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/1283FB2C-CF46-DF11-B494-0030487EB003.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/10DB5521-C646-DF11-85E9-003048D43986.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0EF92068-B146-DF11-8B9C-003048C692E0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0E8F19AA-BD46-DF11-8E6A-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0CA52B8C-C446-DF11-B40E-003048C6929C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0C7F2D5B-BF46-DF11-8D91-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0C538037-C146-DF11-A9B9-003048D4DFA6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0AA6E521-C646-DF11-8167-0030487E54B5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/081D4606-B746-DF11-B52D-0030487D5DBB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/06311905-C446-DF11-83FC-0030487F1307.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/04B06630-C946-DF11-8BA8-003048C6903C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0217C251-C446-DF11-8A2F-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0006/0028A857-BF46-DF11-87B2-003048C68F6A.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/FAFA483E-A746-DF11-B29B-0030487E52A5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/FADEF56F-9946-DF11-AA1C-003048C692CA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/FAB133BB-9C46-DF11-B344-0030487D5E9D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/FA929BCF-A846-DF11-AC39-003048C69316.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/FA3506A0-AC46-DF11-8053-0030487D605B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F881432C-9E46-DF11-928A-003048D436B4.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F6AF6F1F-9E46-DF11-871E-0030487F92A9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F62C1534-A346-DF11-9F22-0030487FA60D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F4EC7032-A446-DF11-967A-0030487F1BD7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F4608C96-9C46-DF11-8AF9-0030487F1735.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/F25F812F-AD46-DF11-BE64-0030487E4D11.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EEEEFE86-A746-DF11-93A3-0030487D5DB1.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EEED8920-9846-DF11-B8BF-003048C692FA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EE26C811-A346-DF11-9D52-003048C65E48.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/ECFA4733-A346-DF11-AB40-003048C69424.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EC5593BB-9C46-DF11-9654-003048C662C8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EC263E20-AD46-DF11-A893-0030487D5EAF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/EA0075A2-9946-DF11-94C8-003048C33984.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E87B584C-9846-DF11-B746-003048C6931C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E614D21C-9E46-DF11-8311-0030488A0EFA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E608BFEB-A246-DF11-B4AE-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E4C7899E-A446-DF11-A7CC-003048C693D8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E46EA631-A346-DF11-8949-0030488A0EFA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E4666BDA-9C46-DF11-A8D4-0030487E5399.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E44C668F-9D46-DF11-A82B-003048D3CD6C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E274FE2F-AD46-DF11-BF6E-0030487F1A4F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E24EF03D-9C46-DF11-BD53-0030487FA607.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E2250597-9846-DF11-991F-0030487F1A67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E20F5B4D-A746-DF11-817E-003048D4DCDE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E0EC5BA1-A446-DF11-ABD9-003048D4363C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/E0509C40-A746-DF11-8059-0030487F3EA7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/DEB33841-9A46-DF11-AC39-0030487F92B5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/DE2ACBC3-A446-DF11-BD0D-0030487E55BB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/DC58F41F-AD46-DF11-83A8-0030487D5E4B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/DC43B7D7-9C46-DF11-98ED-0030487D5EB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D8B2277C-A446-DF11-A1DA-0030487F1737.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D4BD721D-A746-DF11-8879-003048D47746.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D449301A-A746-DF11-9DEB-0030487E54B5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D427A4E8-A446-DF11-A6E1-003048C692AC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D2E63370-9946-DF11-B3D2-0030487D5EB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D2E36BBF-9C46-DF11-A1D9-0030487FA60D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D2A33E4A-A746-DF11-B009-003048C6929C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D0C25C50-A746-DF11-A96E-003048C6941C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/D0851434-A346-DF11-89E8-0030487F1A57.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/CCA71EA7-A846-DF11-B05C-0030487DA061.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/CC52809E-A446-DF11-BF3F-003048D462BE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/CC32F975-9746-DF11-A027-0030487F8E69.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/CA96B1A4-A846-DF11-A12D-0030487F1A3F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/CA1B6A4A-A746-DF11-B8C8-003048C69416.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/C667E3BB-9C46-DF11-B63F-0030487F929F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/C60EF985-A746-DF11-9C2C-003048C6903E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/C200EFBB-9C46-DF11-AAAA-003048D4DFBA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/C05EFEBE-9C46-DF11-A23B-003048D479C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BEF90E83-A746-DF11-9A37-0030487F16BF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BE106F0E-A546-DF11-B261-003048C693FE.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BCF8FB15-AD46-DF11-A2C2-0030487D5DA9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BCBF4136-A346-DF11-95F0-003048C692E4.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BC6B6568-A646-DF11-9B3A-0030487D5E67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BC44E51C-9E46-DF11-8306-0030487F1775.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BC442CC1-9446-DF11-8E8E-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BC3255C0-9C46-DF11-9525-003048D479C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BAC86C80-A446-DF11-B688-0030488A0FD4.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BAA218BC-9C46-DF11-A934-003048C69036.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/BA31623D-A746-DF11-BE99-003048D4DEAC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B8E00316-AD46-DF11-A92F-0030487D5EBD.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B8558647-9D46-DF11-AD71-003048D3CA06.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B8499034-A346-DF11-B3C5-00304889D562.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B833FD14-9946-DF11-9D0C-0030487F1BCF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B45DEBAA-A846-DF11-AC4A-003048D436B4.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/B0DB452C-9E46-DF11-9D8D-003048C693F8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/AE934168-A646-DF11-BE1E-0030487D5E53.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/AE0F68EA-A446-DF11-B0C8-0030488A1164.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/AC21A8C3-9446-DF11-A595-0030487D5DB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A8758D16-AD46-DF11-9E5F-0030487DA061.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A863E2BB-9C46-DF11-AEE1-0030487F933D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A6F3A00E-A346-DF11-9A2A-0030487E9881.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A6D44218-9E46-DF11-AC41-003048C692A2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A6C34710-A346-DF11-B606-003048D4364C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A605C0B7-A546-DF11-86BF-003048D4DFB8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A4B81DCE-AD46-DF11-B70A-0030487D605B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A48600B7-A546-DF11-A2E1-003048D437D2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A4464D70-9946-DF11-AC0E-0030487F1BE5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A42A4EC2-9446-DF11-9486-003048C66184.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A2DEA30E-A546-DF11-8A82-0030487F9307.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A2290A1E-9E46-DF11-BB7B-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A213726D-9846-DF11-8C03-0030487DA061.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/A03F8E96-9C46-DF11-AFD9-0030487F1BCF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/9EBC870F-A546-DF11-B936-003048C692D6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/9E1C2EBC-9C46-DF11-9CD6-0030487F1715.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/9C9F7140-9A46-DF11-A850-0030487D8635.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/984B9016-AD46-DF11-9F8F-0030487D8581.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/9828399B-9D46-DF11-AB7F-0030487F16BF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/96F54A09-AD46-DF11-8ED3-0030487D5D95.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/94CCCDC3-A446-DF11-BC70-0030487E55BB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8EBFDAB8-9C46-DF11-A7B3-003048C692B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8CD18ECF-A846-DF11-90D9-003048D47912.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8C065FC0-9446-DF11-BD8D-0030487DE7C1.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8A84507F-A446-DF11-BEF0-003048D3739C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8A411657-A446-DF11-B08D-0030487F92E3.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/885272EB-A446-DF11-B018-003048D4DEAC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/867A55DB-9C46-DF11-8212-0030487F1775.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/863DFB1C-A746-DF11-83A2-0030487D5DA5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8440EA2B-9E46-DF11-BE75-003048C64787.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/8245F99F-A446-DF11-85D3-0030487D5D8F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/7CFB6141-9A46-DF11-9CCD-0030487F1C55.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/7C4BCA2F-AD46-DF11-A7C0-003048C69272.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/7A961EE8-AC46-DF11-9A97-0030487F4B8B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/78EF101D-A746-DF11-B1F1-0030487D5EBD.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/781DC01B-9E46-DF11-9537-0030487F1F23.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/76C28F1D-A746-DF11-B83C-003048C68A8E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/74BC1C0E-A546-DF11-BD74-0030487D70FD.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/7478C38F-9D46-DF11-8DA2-0030487F1A4F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/7436C421-9846-DF11-9584-003048D47976.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/72672DF8-A646-DF11-B83A-0030487D5D3F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/70F3281C-9E46-DF11-B28C-003048C692A2.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/703E8878-9746-DF11-A5E7-003048C693F0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/703908A7-9846-DF11-871A-0030487F932D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/6CB8DC48-9846-DF11-A43A-003048C68A84.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/6C935A6D-9D46-DF11-A8CC-003048C6617E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/6C1D3220-A746-DF11-8ADD-0030487D5DB1.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/6A042BBA-9846-DF11-B085-0030487F3EA7.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/68B8382F-A546-DF11-A66C-0030487D5DC5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/68709171-9846-DF11-9708-003048D47976.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/686C53BE-9C46-DF11-B67B-0030488A0B60.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/68413099-9C46-DF11-B227-0030487D5E75.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/62DD7E6D-9846-DF11-8440-0030487D5D95.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/62930B70-9946-DF11-AD52-003048D3739A.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/62880B30-AD46-DF11-B490-003048C68A84.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5E1AF121-A746-DF11-8EED-0030488A0F9C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5CDB9473-9846-DF11-9F3A-0030487F1C57.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5C426A6E-9846-DF11-B29A-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5C104F17-9C46-DF11-892C-003048C65E48.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5A8BB12F-AD46-DF11-A2FF-0030487E5179.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/58B389A9-9446-DF11-9FB7-003048D4DFA6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/584C02DB-9C46-DF11-BB67-003048C662BC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5417130C-AD46-DF11-8E73-0030488A0AB8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/52D5506D-9D46-DF11-BCCB-003048C6617E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/529F204C-9846-DF11-9DA0-003048C692C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/52271869-A646-DF11-81FA-0030487F1A47.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/50C2D692-9D46-DF11-9605-0030487D86C9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/506FBAC4-9446-DF11-A8BA-003048D4DEA6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/5013981B-9E46-DF11-A24B-0030487F1BE5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4EF00097-9846-DF11-BB3D-003048C65E48.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4EA11586-A746-DF11-BEDB-0030487F4B8B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4C77B44A-A746-DF11-A269-0030487F1A4F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4C5ABE16-AD46-DF11-8C44-003048C69424.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4C2A7F4F-A746-DF11-877F-0030487D5E75.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4AF8121C-9E46-DF11-B2B6-0030487F9381.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/48E0E535-A346-DF11-8C37-0030487E4D11.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/48BCD22B-AD46-DF11-884A-003048C6942A.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/46AF0069-A646-DF11-867A-003048C33984.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4687FCAB-9446-DF11-BA26-003048C692D6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/42AD311A-A746-DF11-B47B-0030487D5D8F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/420D6BC0-9446-DF11-8C1A-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/4048A44A-A746-DF11-8EC3-0030487F1A3F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3ECD644D-9846-DF11-8500-0030487F1A57.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3EB2B309-AD46-DF11-974B-003048D37394.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3A3981B3-9446-DF11-A1A0-003048D3739A.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3A2A15CF-AD46-DF11-BBCD-003048D4399E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/38B95940-A746-DF11-9319-003048D4DF08.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/360C4B80-9846-DF11-AAEA-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3608E935-A346-DF11-8C93-0030487D5E53.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/349C32EC-A246-DF11-939F-003048C693E8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/32334793-9846-DF11-BBE5-0030487D7109.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/305B5550-A746-DF11-A2B4-003048C6941C.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/3029B293-9846-DF11-8C57-003048C68F6A.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2E491798-9D46-DF11-9C59-003048D439B6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2CF3DCDA-9C46-DF11-9EBC-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2CCCC6B1-9446-DF11-B59D-0030487D70FF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2C7A1369-A646-DF11-B7B1-0030487F1A47.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2C6DCA17-9E46-DF11-AF08-0030487D5D6B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2AA58B20-AD46-DF11-9274-003048C69032.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/284B9928-9E46-DF11-B066-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/262B29F1-A846-DF11-94EF-0030487D605B.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/2484E92F-AD46-DF11-AD7A-003048C693EC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/20BA58C4-A446-DF11-A61A-0030487F1A73.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/20909C62-A646-DF11-A322-0030487D8151.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/208A1E17-AD46-DF11-8B6D-003048C65E48.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/1C7C8B56-A446-DF11-8476-0030487D5DA5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/1A916282-A746-DF11-8D56-0030487D5D67.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/1A61B4BB-9C46-DF11-8FA3-003048C690A0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/18AB9C6C-9D46-DF11-A43A-0030487D8121.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/1215842A-AD46-DF11-A548-003048C693DA.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/10F3F3DA-9C46-DF11-B209-003048C662BC.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0EF1E9B2-A546-DF11-B9F0-0030487D5E53.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0EE2CC18-9E46-DF11-A687-0030487F1A3D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0CECC84C-A746-DF11-A085-0030487D5DAF.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0C774B63-A646-DF11-998F-0030487F16F9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0C5FD02B-9E46-DF11-95EE-0030487D5059.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0A7290F5-A846-DF11-BAB9-003048C693E8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0A0A7BA6-9846-DF11-9053-003048C69408.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/08D1C8B8-A546-DF11-B632-0030488A0CB6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/08C375C6-9446-DF11-92A5-0030487E55BB.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0869DBDA-9C46-DF11-A989-003048D4397E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/06BFCC56-A446-DF11-B417-003048D439C0.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0633C5B6-A546-DF11-8196-003048C6903E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/044815A6-A846-DF11-AF68-0030488A0AB8.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/04013230-A346-DF11-A961-003048C692D6.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/02E26B0B-A546-DF11-A1A1-0030487F1A3F.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/025FC393-9846-DF11-972B-003048C68A8E.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/023A194A-A746-DF11-B5D1-0030487D5DB5.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/00F662BF-9C46-DF11-9766-0030487FA60D.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/00766E0F-A346-DF11-8CAC-0030487D86C9.root',
##         '/store/mc/Spring10/TTbarJets-madgraph/GEN-SIM-RECO/START3X_V26_S09-v1/0005/0023C84A-A746-DF11-85E7-0030487F92A5.root'

   )
 )

##process.p = cms.Path( process.patDefaultSequence)
##process.e = cms.EndPath(process.out)
