#################################################################################################
#### CMS 
#### Preliminary Sensitivity Analysis
#### Modified by Osipenkov, Ilya : ilyao@fnal.gov
#################################################################################################

from PhysicsTools.PatAlgos.patTemplate_cfg import *

#############################################################
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoLayer1Cleaning

# run the 3.3.x software on Summer 09 MC from 3.1.x:
#   - change the name from "ak" (3.3.x) to "antikt) (3.1.x)
#   - run jet ID (not run in 3.1.x)
run33xOn31xMC( process,
               jetSrc = cms.InputTag("antikt5CaloJets"),
               jetIdTag = "antikt5"
               )

from PhysicsTools.PatAlgos.tools.coreTools import *

#################################################################

#number of events
process.maxEvents.input = 100

#output file
#process.out.fileName = '/uscms_data/d2/ilyao/TTbar.root'
process.out.fileName = 'STopSMG.root'

# source
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring(
## ZMM sample
#         '/store/relval/CMSSW_3_1_4/RelValZMM/GEN-SIM-RECO/STARTUP31X_V2-v1/0006/9A42A6DC-63B1-DE11-933F-000423D99F1E.root',
#         '/store/relval/CMSSW_3_1_4/RelValZMM/GEN-SIM-RECO/STARTUP31X_V2-v1/0005/BE87B5F5-DCB0-DE11-A83A-001D09F23174.root',
#         '/store/relval/CMSSW_3_1_4/RelValZMM/GEN-SIM-RECO/STARTUP31X_V2-v1/0005/72574DE2-D9B0-DE11-B6B9-001D09F2512C.root',
#         '/store/relval/CMSSW_3_1_4/RelValZMM/GEN-SIM-RECO/STARTUP31X_V2-v1/0005/2A6D41CE-DBB0-DE11-A14F-0019B9F70468.root'
#
#   )
fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/FA7BD15D-F99E-DE11-B9AE-002481FF35F3.root',
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/EEC79AD3-709E-DE11-AB42-0015C5F8236C.root',
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/D6536569-799E-DE11-AF29-0015C5F82F97.root',
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/C2553002-599F-DE11-8F8E-002481CFE25E.root',
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/C01A9D3D-6C9E-DE11-9DEE-002481DE4938.root',
       '/store/mc/Summer09/TTbar/GEN-SIM-RECO/MC_31X_V3_7TeV-v1/0008/BEBC720B-659E-DE11-AE83-002481DE4938.root' ] );

secFiles.extend( [
               ] )


                            
#)

## Next three lines are added to reduce the size of the PATtuple. We keep only MUONS and ELECTRONS. We also drop the isodeposits for 
## MUONS and ELETCRONS 

removeAllPATObjectsBut(process, ['Muons', 'Electrons', 'Jets', 'METs'])
process.allLayer1Muons.isoDeposits = cms.PSet()
process.allLayer1Electrons.isoDeposits = cms.PSet()
#process.allLayer1Jets.isoDeposits = cms.PSet()
#process.Layer1METs.isoDeposits = cms.PSet()

process.out.outputCommands = cms.untracked.vstring('drop *_cleanLayer*_*_*', *patEventContentNoLayer1Cleaning)

process.load("PhysicsTools.PatAlgos.patSequences_cff")
process.p = cms.Path( process.patDefaultSequence )
