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
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_10_1_jEn.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_11_1_YeB.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_12_1_sK4.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_13_1_rwX.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_14_1_qdE.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_15_1_GX4.root',
        '/store/user/lnujj/PATProduction/ilyao/GluGluToHToWWToLNuQQ_M-150_7TeV-powheg-pythia6/HWWMH150_Fall11-PU_S6_START42_V14B-v1_prodV05/3156aed375cb9d84362f1cc2ea98a9c4/pat_42x_fall11_16_1_6Cw.root'
#	'file:/gluster/hepx/store/user/aperloff/lnjj/pat_42x_fall11_10_1_jEn.root'
    )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
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
process.PS.vtxSource                  = cms.InputTag('offlinePrimaryVertices')               # InputTag for vertex collection
process.PS.genParticleSource          = cms.InputTag('genParticles')                         # InputTag for genParticle collection
process.PS.pfJetSource                = cms.InputTag('selectedPatJetsPFlow')                 # InputTag for jet collection
process.PS.electronSource             = cms.InputTag('selectedPatElectronsPFlow')            # InputTag for electron collection
process.PS.muonSource                 = cms.InputTag('selectedPatMuonsPFlow')                # InputTag for muon collection
process.PS.METSource                  = cms.InputTag('patMETsPFlow')                         # InputTag for MET collection
process.PS.rhoSource                  = cms.InputTag("kt6PFJetsChsForIsolationPFlow", "rho") # InputTag for event rho source
process.PS.triggerSource              = cms.InputTag('patTriggerEvent')                      # InputTag for trigger collection

    #-----Program Level Inputs
process.PS.outtablefilename           = cms.string("outputTable.txt") # name of the file containing the output/efficiency tables
process.PS.outtablenameEl             = cms.string("ElectronTable")   # name of the event selection table for electrons
process.PS.outtablenameMu             = cms.string("MuonTable")       # name of the event selection table for muons
process.PS.outtablenameLp             = cms.string("LeptonTable")     # name of the event selection table for leptons
process.PS.runtype                    = cms.string("crab")            # tells the program if it will be run through cmsRun or CRAB
                                                                      # - crab - the event table will be saved in the root file only
							               # - cmsRun - the event table will be printed to a text file as well
process.PS.printEventInfo             = cms.bool(False)               # tells the program to cout which cuts the events passed and what objects they contained
process.PS.printJetInfo               = cms.bool(False)               # tells the program to cout the relevant information for the jets (for debugging purposes)
process.PS.printLeptonInfo            = cms.bool(False)               # tells the program to cout the relevant information for the leptons (for debugging purposes)
##process.PS.cutOnTrigger               = cms.bool(True)                # if true for Data, only events that pass the trigger requirements will be saved (MC always passes the trigger requirements)
process.PS.SQWaT_Version              = cms.int32(5)                  # version number used to create the common PATtuples
process.PS.doRelIso                   = cms.bool(True)                # if true, do rho corrected isolation, else do tracker only isolation

process.PS.Data                       = cms.bool(True)               # is the dataset from real data or Monte Carlo
process.PS.saveGenParticles           = cms.bool(False)               # save the generated particle information for hard scatter decays
process.PS.noMETCut                   = cms.bool(False)               # disregard the MET cut when storing the events
process.PS.invertEID                  = cms.bool(False)               # electrons which *fail* at least two of the EID requirements will be kept instead
process.PS.PFlowLoose                 = cms.bool(False)               # use the collections with the PF2PAT postfix=PFlowLoose (instead of PFlow)
process.PS.elONLY                     = cms.bool(False)               # only save the output if the lepton is an electron
process.PS.muONLY                     = cms.bool(False)               # only save the output if the lepton is a muon

    #-----Event Variable Inputs
process.PS.elcnt_Prim                 = cms.int32(0)
process.PS.elcnt_Loose                = cms.int32(0)
process.PS.mucnt_Prim                 = cms.int32(0)
process.PS.mucnt_Loose                = cms.int32(0)
process.PS.jcnt_tot                   = cms.int32(0)
process.PS.EvtTotCount                = cms.int32(0)
process.PS.mu_passAll                 = cms.bool(False)
process.PS.mu_passStandard            = cms.bool(False)
process.PS.mu_passFlag                = cms.bool(False)
process.PS.el_passAll                 = cms.bool(False)
process.PS.el_passStandard            = cms.bool(False)
process.PS.el_passFlag                = cms.bool(False)

    #-----Trigger Variable Inputs
process.PS.mu_passTrigger             = cms.bool(True)
process.PS.el_passTrigger             = cms.bool(True)


    #-----Vertex Variable Inputs
process.PS.PVfound                    = cms.bool(False)
process.PS.zvtx                       = cms.double(-999)
process.PS.vtxcnt                     = cms.int32(0)

    #-----Jet Variable Inputs
process.PS.j_ptMin                    = cms.double(20.0)
process.PS.j_aetaMax                  = cms.double(2.4)
process.PS.j_DRelMin                  = cms.double(0.3)
process.PS.muPrim_DRjMin              = cms.double(0.3) # was 0.1 before synchronization

    #-----B-Tag Variable Inputs

process.PS.nBtagSSV                   = cms.int32(0)
process.PS.nBtagTC                    = cms.int32(0)
process.PS.bDiscriminatorSSVMin       = cms.double(1.74)     # corresponds to the 'medium' cut (per https://twiki.cern.ch/twiki/bin/view/CMS/BTV10001)
process.PS.bDiscriminatorTCMin        = cms.double(1.74)     #Should change

    #-----Muon Variable Inpust
process.PS.muPrim_ptMin               = cms.double(20.0)
process.PS.muPrim_aetaMax             = cms.double(2.1)
process.PS.muPrim_dBMax               = cms.double(0.02)
process.PS.muPrim_RelIsoMax           = cms.double(0.1)
process.PS.muPrim_TrkIsoMax           = cms.double(0.05)
process.PS.isProperlyIsolatedMu       = cms.bool(False)
process.PS.muLoose_ptMin              = cms.double(10.0)
process.PS.muLoose_aetaMax            = cms.double(2.5)
process.PS.muLoose_RelIsoMax          = cms.double(0.2)
process.PS.muLoose_TrkIsoMax          = cms.double(0.2)

    #-----Electron Variable Inputs
process.PS.elPrim_ptMin               = cms.double(20.0)
process.PS.elPrim_aetaMax             = cms.double(2.5)
process.PS.elPrim_aetascExcludeMax    = cms.double(1.5660)
process.PS.elPrim_aetascExcludeMin    = cms.double(1.4442)
process.PS.elPrim_dBMax               = cms.double(0.02)
process.PS.elPrim_RelIsoMax           = cms.double(0.05)
process.PS.elPrim_TrkIsoMax           = cms.double(0.1)

process.PS.elPrim_sigmaIetaIetaMaxEB  = cms.double(0.01)
process.PS.elPrim_aDeltaPhiMaxEB      = cms.double(0.03) #Changed from 0.06 on 03/05/2012
process.PS.elPrim_aDeltaEtaMaxEB      = cms.double(0.004)
process.PS.elPrim_HoEMaxEB            = cms.double(0.025) #Changed from 0.040 
process.PS.elPrim_sigmaIetaIetaMaxEE  = cms.double(0.03)
process.PS.elPrim_aDeltaPhiMaxEE      = cms.double(0.02) #Changed from 0.03
process.PS.elPrim_aDeltaEtaMaxEE      = cms.double(0.005) #Changed from 0.007
process.PS.elPrim_HoEMaxEE            = cms.double(0.025)

process.PS.elLoose_ptMin              = cms.double(15.0)
process.PS.elLoose_RelIsoMax          = cms.double(0.2)
process.PS.elLoose_TrkIsoMax          = cms.double(0.2)

process.PS.elLoose_sigmaIetaIetaMaxEB = cms.double(0.01)
process.PS.elLoose_aDeltaPhiMaxEB     = cms.double(0.8)
process.PS.elLoose_aDeltaEtaMaxEB     = cms.double(0.007)
process.PS.elLoose_HoEMaxEB           = cms.double(0.15)
process.PS.elLoose_sigmaIetaIetaMaxEE = cms.double(0.03)
process.PS.elLoose_aDeltaPhiMaxEE     = cms.double(0.7)
process.PS.elLoose_aDeltaEtaMaxEE     = cms.double(0.01)
process.PS.elLoose_HoEMaxEE           = cms.double(0.07)

process.PS.el_aetaPass                = cms.bool(False)
process.PS.el_convPass_a              = cms.bool(False)
process.PS.el_convPass_b              = cms.bool(False)

    #-----Lepton Variable Inputs

    #-----MET Variable Inputs
process.PS.MET_EtMin                  = cms.double(25)

    #-----Additional Variable Inputs
process.PS.lTotIso                    = cms.double(-1)
process.PS.lecalIso                   = cms.double(-10)
process.PS.lhcalIso                   = cms.double(-10)
process.PS.ltrkIso                    = cms.double(-1)
process.PS.lrelIso                    = cms.double(-1)
process.PS.Mjj                        = cms.double(-1)
process.PS.lQ                         = cms.int32(-10)
process.PS.lEta                       = cms.double(-1)

    #-----Constant Inputs
process.PS.etaBarrelMax               = cms.double(1.442) # the Maximum value of eta for the Barrel
