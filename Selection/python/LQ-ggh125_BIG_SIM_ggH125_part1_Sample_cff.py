import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_10_1_jYl.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_1_1_lcJ.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_29_1_LDt.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_3_1_nSF.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_11_1_U7t.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_20_1_Xps.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_2_1_6At.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_4_1_ed9.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_12_1_2da.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_21_1_8SQ.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_30_1_T65.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_5_1_lSd.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_13_1_snp.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_22_1_fdA.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_31_1_R86.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_6_1_xaZ.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_14_1_R99.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_23_1_p8N.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_32_1_W1C.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_7_1_A5S.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_15_1_TWd.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_24_1_cIW.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_33_1_8nv.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_8_1_ur1.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_16_1_frP.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_25_1_HpG.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_34_1_y5q.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_9_1_Eev.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_17_1_nFd.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_26_1_Sfy.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_35_1_eGr.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_18_1_EsU.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_27_1_FMp.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_36_1_c3b.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_19_1_Vvg.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_28_1_wzk.root',
	'/store/user/aperloff/MatrixElement/LQ-ggh125_BIG_PAT/aperloff/LQ-ggh125_BIG_SIM_ggH125_part1/SQWaT_PAT_53X_Spring14_HWW125_Part1/33519632dfb71bf616e1fd5cd9d10cef/pat_53x_test_v03_37_1_u53.root'
	] );


secFiles.extend( [
               ] )

