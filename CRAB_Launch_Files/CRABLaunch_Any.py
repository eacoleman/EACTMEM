###Runs with 3.3.5
###~7.5 K per event


### Basic Template:
from PhysicsTools.PatAlgos.patTemplate_cfg import *

### Some Tools and a hack for working with 3.3.1 MC
#############################################################
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
#from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoLayer1Cleaning
#from PhysicsTools.PatAlgos.patEventContent_cff import *
# run the 3.3.x software on Summer 09 MC from 3.1.x:
#   - change the name from "ak" (3.3.x) to "antikt) (3.1.x)
#   - run jet ID (not run in 3.1.x)
run33xOn31xMC( process,
              jetSrc = cms.InputTag("antikt5CaloJets"),
              jetIdTag = "antikt5"
              )

from PhysicsTools.PatAlgos.tools.coreTools import *

#################################################################
## Output Module Configuration: must pass either path 'p1' or path 'p2'.
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('PATLayer1_Output.fromAOD_full.root'),
                               # save only events passing the full path
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1','p2') ),
                               #SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p1') ),
                               # save PAT Layer 1 output; you need a '*' to
                               # unpack the list of commands 'patEventContent'
                               outputCommands = cms.untracked.vstring('drop *', *patEventContent ) 
                               )
process.outpath = cms.EndPath(process.out)

# output file name

process.out.fileName = 'WZPY_TestRun.root'

## Lepton Candidate Filters
#Kinematic Filters
process.MuonsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("allLayer1Muons"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)
process.ElectronsKinCutBasic = cms.EDFilter("CandViewSelector",
  src = cms.InputTag("allLayer1Electrons"),                              
  cut = cms.string('pt > 15. & abs(eta) < 2.5'),
  filter = cms.bool(True)                                
)

# HLT filters
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.MuHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
process.MuHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]
# Uncomment this to filter on 1E31 HLT menu
#process.MuHLTFilter.HLTPaths = ["HLT_Mu9"]

process.ElHLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
# Uncomment this to access 8E29 menu and filter on it
process.ElHLTFilter.TriggerResultsTag = cms.InputTag("TriggerResults","","HLT8E29")
process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]
# Uncomment this to filter on 1E31 HLT menu
#process.ElHLTFilter.HLTPaths = ["HLT_Ele15_LW_L1R"]


## Run the pat sequences
process.p1 = cms.Path(
  process.patDefaultSequence
  *process.MuonsKinCutBasic
  *process.MuHLTFilter  
)
process.p2 = cms.Path(
  process.patDefaultSequence
  *process.ElectronsKinCutBasic
  *process.ElHLTFilter  
)


## Define and cleanup event content
removeAllPATObjectsBut(process, ['Electrons','Muons','Jets','METs'])

removeMCMatching(process, ['All'])
process.allLayer1Muons.isoDeposits = cms.PSet()
process.allLayer1Electrons.isoDeposits = cms.PSet()

process.selectedLayer1Muons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
process.selectedLayer1Electrons.cut = cms.string('pt> 15. & abs(eta) < 2.5')
process.selectedLayer1Jets.cut = cms.string('pt> 10.')



##
# configure input/output
##
####THIS IS NORMALLY OVERWRITTEN FROM crab.cfg

#number of events
process.maxEvents.input = 100

# source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       '/store/mc/Summer09/WJets-madgraph/GEN-SIM-RECO/MC_31X_V3_7TeV-v3/0009/FAFEBA7D-B32A-DF11-A437-0015178C4900.root',
       '/store/mc/Summer09/WJets-madgraph/GEN-SIM-RECO/MC_31X_V3_7TeV-v3/0009/FAE931CA-B42A-DF11-9496-0024E8768CD9.root',
       '/store/mc/Summer09/WJets-madgraph/GEN-SIM-RECO/MC_31X_V3_7TeV-v3/0009/F6705418-B22A-DF11-84BB-0024E8768874.root',
       '/store/mc/Summer09/WJets-madgraph/GEN-SIM-RECO/MC_31X_V3_7TeV-v3/0009/F024F630-B32A-DF11-B57B-00151796C470.root',
       '/store/mc/Summer09/WJets-madgraph/GEN-SIM-RECO/MC_31X_V3_7TeV-v3/0009/EC855505-AA2A-DF11-B1A8-001EC9B9CC9F.root'
   )
)

#process.p = cms.Path( process.patDefaultSequence)
