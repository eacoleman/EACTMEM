# EACTMEM | Top Mass Measurement with Matrix Elements

## PAT Skimming
1. Reduce the Monte Carlo (usually obtained via https://twiki.cern.ch/twiki/bin/viewauth/CMS/ProductionSummer2009at7TeV) to PAT Skims using CRAB_Launch_Files: 
  1. CRABLaunch_Any.py - define the PAT content, inplement the HLT and place loose cuts.
  2. QCDHT100to250MG.py, QCDHT250to500MG.py, STopSMG.py, STopTMG.py, STopTWMG.py, TTbarMG.py, WWPY.py, WZPY.py, WpJMG.py, ZZPY.py, ZpJMG.py (MG=MadGraph PY=PYTHIA) - specify the output file names which should match the ones in crab.cfg.
  3. crab.cfg - Specify the MC source files, output files, the number of total events as well as the number of events per job. Last used with CRAB 2_7_1.

2. Process the PAT Skims and create a user generated output Selection/bin/PerformSelection.cc, BuildFile.xml:
  1. Create the CMSSW_3_5_4/bin/slc5_ia32_gcc434/PerformSelection.exe, which runs over the (small collections of) PAT tuples and generates the results. E.g. /uscms/home/ilyao/MATRIXELEMENT/CMSSW_3_5_4/bin/slc5_ia32_gcc434/PerformSelection.exe inputFiles=WZPY_TestRun_1_1.root,WZPY_TestRun_2_1.root,WZPY_TestRun_3_1.root  \outputEvery=5000 WZTestTAMUWW_outfile.root First100K_WZTestTAMUWW_Params.txt
  2. The EvtTree is stored in the .root output file (i.e. WZTestTAMUWW_outfile.root)
  3. The .txt file (i.e. First100K_WZTestTAMUWW_Params.txt) is a table with the information on how many events pass each of the cuts. (Note that the user defined .root & .txt output files should be the last two listed on the command line.)

3. Work with large collections of PAT and user defined skims with Tools/ProcessSkims.cc:
  1. Process larage collections of PATTuple and/or compbine the results (via the ProcessPATSet function).
  2. Make a table summarizing an individual process ().
  3. Make a Global Table summarizing all of the processes ().

    -The .txt table is then used by MakeSelectionTable.C

## ?

2. MakeSelectionTable.C - place in CMSSW_3_3_0/src/ :
A macro, which reads in the input tables for the desired processes and produces the final SelectionTable.

3. InputXSections_PerformSelectionMG.txt - 
A listing of input CrossSections & Filter efficiencies for the madgraph MC.
  


