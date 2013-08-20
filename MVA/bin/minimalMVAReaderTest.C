gSystem->Load("libTMVA.1");
TMVA::Tools::Instance();
TMVA::Reader* reader = new TMVA::Reader( "!Color:!Silent" );
float tEventProb0     = 1.81675e-13
float tEventProb2     = 7.50977e-15
float tEventProb3     = 1.09903e-10
float tEventProb4     = 2.24957e-10
float tEventProb5     = 1.50811e-12
float tEventProb6     = 6.4572e-15
float tEventProb7     = 1.89464e-13
float tEventProb8     = 1.78002e-10
float tEventProb11    = 4.50713e-16
reader->AddVariable("tEventProb0 := tEventProb[0]",(&tEventProb0));
reader->AddVariable("tEventProb2 := tEventProb[2]",(&tEventProb2));
reader->AddVariable("tEventProb3 := tEventProb[3]",(&tEventProb3));
reader->AddVariable("tEventProb4 := tEventProb[4]",(&tEventProb4));
reader->AddVariable("tEventProb5 := tEventProb[5]",(&tEventProb5));
reader->AddVariable("tEventProb6 := tEventProb[6]",(&tEventProb6));
reader->AddVariable("tEventProb7 := tEventProb[7]",(&tEventProb7));
reader->AddVariable("tEventProb8 := tEventProb[8]",(&tEventProb8));
reader->AddVariable("tEventProb11 := tEventProb[11]",(&tEventProb11));
int m_run = 1;
int m_event = 2.41998e+06;
reader->AddSpectator("run := m_run", &m_run);
reader->AddSpectator("event := m_event", &m_event);
reader->BookMVA( "BDT", "/uscms_data/d2/aperloff/2012_10_12_TMVA_plots_cuts_muons_reducedVariables/weights/TMVAClassification_BDT.weights.xml" );
reader->EvaluateMVA( "BDT" )
reader->GetProba("BDT")


/*
float tEventProb0     = 1.81675e-13
float tEventProb2     = 7.50977e-15
float tEventProb3     = 1.09903e-10
float tEventProb4     = 2.24957e-10
float tEventProb5     = 1.50811e-12
float tEventProb6     = 6.4572e-15
float tEventProb7     = 1.89464e-13
float tEventProb8     = 1.78002e-10
float tEventProb11    = 4.50713e-16
 weight          = 0.236702
 BDT             = -0.13385
 prob_BDT        = 0.456815
RIGHT

float tEventProb0 = 1.18367e-12
float tEventProb2 = 1.68938e-13
float tEventProb3 = 1.88823e-09
float tEventProb4 = 4.64969e-10
float tEventProb5 = 1.35214e-11
float tEventProb6 = 9.47306e-13
float tEventProb7 = 1.23926e-12
float tEventProb8 = 3.44413e-09
float tEventProb11 = 8.5211e-15
MVADiscriminator_muon = -0.306179
MVAProbability_muon = 0.176995
WRONG



*        3 *   2663602 *
 tEventProb0     = 4.98363e-10
 tEventProb2     = 7.12055e-12
 tEventProb3     = 1.01501e-08
 tEventProb4     = 8.01273e-09
 tEventProb5     = 1.09539e-10
 tEventProb6     = 8.97449e-12
 tEventProb7     = 3.05738e-11
 tEventProb8     = 7.98137e-10
 tEventProb11    = 1.041e-12
 run             = 1
 event           = 2.6636e+06
 weight          = 1.28077
 BDT             = -0.0201344
 prob_BDT        = 0.718766



tEventProb0_muon = 4.98363e-10 (0x1a971d90)
tEventProb2_muon = 7.12055e-12 (0x1a971e90)
tEventProb3_muon = 1.01501e-08 (0x1a971f10)
tEventProb4_muon = 8.01273e-09 (0x1a971f90)
tEventProb5_muon = 1.09539e-10 (0x1a972010)
tEventProb6_muon = 8.97449e-12 (0x1a972090)
tEventProb7_muon = 3.05738e-11 (0x1a972110)
tEventProb8_muon = 7.98137e-10 (0x1a972190)
tEventProb11_muon = 1.041e-12 (0x1a972310)
MVADiscriminator_muon = -0.0862605
MVAProbability_muon = 0.599529
weight = 1
*/
