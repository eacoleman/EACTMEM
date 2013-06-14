#include "TAMUWW/SpecialTools/interface/Defs.hh"

namespace DEFS {

  using std::cout;
  using std::endl;
  using std::string;
  
  namespace Ana{

    //---------------------------------------------------------------------------
    // A routine that returns the type given a string 
    string getTypeString(Type type ){
      
      //the returning string
      if (type == SingleTopAnalysis)
	return "Single Top Analysis";
      else if (type == HiggsAnalysis)
	return "Higgs Analysis";
      else if (type == WWAnalysis)
	return "WW Analysis";
      
      cout<<"ERROR DEFS::getProcessTypeString AnalysisType "<<type<<" is unknown"<<endl;   
      
      return "";
      
    }//getAnalysisTypeString
    
  }// namespace

  namespace PhysicsProcess {

    //---------------------------------------------------------------------------
    DEFS::PhysicsProcessType getProcessType(std::string str){
  
      if(str == "WH100")
	return  DEFS::PhysicsProcess::WH100;
      if( str == "WH105")
	return  DEFS::PhysicsProcess::WH105;
      if( str == "WH110")
	return  DEFS::PhysicsProcess::WH110;
      if( str == "WH115")
	return  DEFS::PhysicsProcess::WH115;
      if( str == "WH120")
	return  DEFS::PhysicsProcess::WH120;
      if( str == "WH125")
	return  DEFS::PhysicsProcess::WH125;
      if( str == "WH130")
	return  DEFS::PhysicsProcess::WH130;
      if( str == "WH135")
	return  DEFS::PhysicsProcess::WH135;
      if( str == "WH140")
	return  DEFS::PhysicsProcess::WH140;
      if( str == "WH145")
	return  DEFS::PhysicsProcess::WH145;
      if( str == "WH150")
	return  DEFS::PhysicsProcess::WH150;


      if(str == "ggH110")
	return  DEFS::PhysicsProcess::ggH110;
      if(str == "ggH115")
	return  DEFS::PhysicsProcess::ggH115;
      if(str == "ggH120")
	return  DEFS::PhysicsProcess::ggH120;
      if(str == "ggH125")
	return  DEFS::PhysicsProcess::ggH125;
      if(str == "ggH130")
	return  DEFS::PhysicsProcess::ggH130;
      if(str == "ggH135")
	return  DEFS::PhysicsProcess::ggH135;
      if(str == "ggH140")
	return  DEFS::PhysicsProcess::ggH140;
      if(str == "ggH145")
	return  DEFS::PhysicsProcess::ggH145;
      if(str == "ggH150")
	return  DEFS::PhysicsProcess::ggH150;
      if(str == "ggH160")
	return  DEFS::PhysicsProcess::ggH160;
      if(str == "ggH170")
	return  DEFS::PhysicsProcess::ggH170;


      if( str == "STopS_T")
	return  DEFS::PhysicsProcess::STopS_T;
      if( str == "STopS_Tbar")
	return  DEFS::PhysicsProcess::STopS_Tbar;
      if( str == "STopT_T")
	return  DEFS::PhysicsProcess::STopT_T;
      if( str == "STopT_Tbar")
	return  DEFS::PhysicsProcess::STopT_Tbar;
      if( str == "STopTW_T")
	return  DEFS::PhysicsProcess::STopTW_T;
      if( str == "STopTW_bar")
	return  DEFS::PhysicsProcess::STopTW_Tbar;
      if( str == "Wcc")
	return  DEFS::PhysicsProcess::Wcc;
      if( str == "WJets")
	return  DEFS::PhysicsProcess::WJets;
      if( str == "WLg")
	return  DEFS::PhysicsProcess::WLg;
      if( str == "Wgg")
	return  DEFS::PhysicsProcess::Wgg;
      if( str == "WLL")
	return  DEFS::PhysicsProcess::WLL;
      if( str == "Wbb")
	return  DEFS::PhysicsProcess::WLb;
      if( str == "WLb")
	return  DEFS::PhysicsProcess::Wbb;
      if( str == "QCDMu")
	return  DEFS::PhysicsProcess::QCDMu;
      if( str == "QCDEl_Pt30to80")
	return  DEFS::PhysicsProcess::QCDEl_Pt30to80;
      if( str == "QCDEl_Pt80to170")
	return  DEFS::PhysicsProcess::QCDEl_Pt80to170;
      if( str == "QCDEl_BCtoE30to80")
	return  DEFS::PhysicsProcess::QCDEl_BCtoE30to80;
      if( str == "QCDEl_BCtoE80to170")
	return  DEFS::PhysicsProcess::QCDEl_BCtoE80to170;
      if( str == "QCD_ElEnriched")
	return  DEFS::PhysicsProcess::QCD_ElEnriched;
      if( str == "QCD_MuEnriched")
	return  DEFS::PhysicsProcess::QCD_MuEnriched;
//       if( str == "QCD100")
// 	return  DEFS::PhysicsProcess::QCD100;
//       if( str == "QCD250")
// 	return  DEFS::PhysicsProcess::QCD250;
      if( str == "WW")
	return  DEFS::PhysicsProcess::WW;
      if( str == "WZ")
	return  DEFS::PhysicsProcess::WZ;
      if( str == "WZbb")
	return  DEFS::PhysicsProcess::WZbb;
      if( str == "ZZ")
	return  DEFS::PhysicsProcess::ZZ;
      if( str == "TTbar")
	return  DEFS::PhysicsProcess::TTbar;
      if( str == "TTbarLJ")
	return  DEFS::PhysicsProcess::TTbarLJ;
      if( str == "TTbarDil")
	return  DEFS::PhysicsProcess::TTbarLJ;
      if( str == "ZJets")
	return  DEFS::PhysicsProcess::ZJets;
      if( str == "Ztautau")
	return  DEFS::PhysicsProcess::Ztautau;
      if( str == "SingleEl_Data")
	return  DEFS::PhysicsProcess::SingleEl_Data;
      if( str == "SingleMu_Data")
	return  DEFS::PhysicsProcess::SingleMu_Data;
  
      cout<<" ERROR  DEFS::getProcessType str ="<<str<<" not found"<<endl;
  
      return  DEFS::PhysicsProcess::WH100;

    }//getProcessType

    //---------------------------------------------------------------------------
    string getTypeString(Type type){

      //the returning string
      if (type == WH100)
	return string("WH100");
      else if (type == WH105)
	return string("WH105");
      else if (type == WH110)
	return string("WH110");
      else if (type == WH115)
	return string("WH115");
      else if (type == WH120)
	return string("WH120");
      else if (type == WH125)
	return string("WH125");
      else if (type == WH130)
	return string("WH130");
      else if (type == WH135)
	return string("WH135");
      else if (type == WH140)
	return string("WH140");
      else if (type == WH145)
	return string("WH145");
      else if (type == WH150)
	return string("WH150");

      else if (type == ggH110)
	return string("ggH110");
      else if (type == ggH115)
	return string("ggH115");
      else if (type == ggH120)
	return string("ggH120");
      else if (type == ggH125)
	return string("ggH125");
      else if (type == ggH130)
	return string("ggH130");
      else if (type == ggH135)
	return string("ggH135");
      else if (type == ggH140)
	return string("ggH140");
      else if (type == ggH145)
	return string("ggH145");
      else if (type == ggH150)
	return string("ggH150");
      else if (type == ggH160)
	return string("ggH160");
      else if (type == ggH170)
	return string("ggH170");

      else if (type == STopS_T)
	return string("STopS_T");
      else if (type == STopS_Tbar)
	return string("STopS_Tbar");
      else if (type == STopT_T)
	return string("STopT_T");
      else if (type == STopT_Tbar)
	return string("STopT_Tbar");
      else if (type == STopTW_T)
	return string("STopTW_T");
      else if (type == STopTW_Tbar)
	return string("STopTW_Tbar");
      else if (type == Wcc) 
	return string("Wcc");
      else if (type == WJets)
	return string("WJets");
      else if (type == WLg)
	return string("WLg");
      else if (type == Wgg)
	return string("Wgg");
      else if (type == WLL)
	return string("WLL");
      else if (type == Wbb)
	return string("Wbb");
      else if (type == QCDMu)
	return string("QCDMu");
      else if (type == QCDEl_Pt30to80)
	return string("QCDEl_Pt30to80");
      else if (type == QCDEl_Pt80to170)
	return string("QCDEl_Pt80to170");
      else if (type == QCDEl_BCtoE30to80)
	return string("QCDEl_BCtoE30to80");
      else if (type == QCDEl_BCtoE80to170)
	return string("QCDEl_BCtoE80to170");
      else if (type == QCD_ElEnriched)
	return string("QCD_ElEnriched");
      else if (type == QCD_MuEnriched)
	return string("QCD_MuEnriched");
//       else if (type == QCD100)
// 	return string("QCD100");
//       else if (type == QCD250)
// 	return string("QCD250");
      else if (type == WW)
	return string("WW");
      else if (type == WZ)
	return string("WZ");
      else if (type == ZZ)
	return string("ZZ");
      else if (type == TTbar)
	return string("TTbar");
      else if (type == TTbarLJ)
	return string("TTbarLJ");
      else if (type == TTbarDil)
	return string("TTbarDil");
      else if (type == ZJets)
	return string("ZJets");
      else if (type == Ztautau)
	return string("Ztautau");
      else if (type == SingleEl_Data)
	return string("SingleEl_Data");
      else if (type == SingleMu_Data)
	return string("SingleMu_Data");
      else     
	cout<<" ERROR  DEFS::getProcessTypeString type ="<<type<<" not found"<<endl;

      return "ERROR in DEFS::getProcessTypeString ";

    }//getTypeString


    //---------------------------------------------------------------------------
    bool isHiggs(Type type){

      //the returning string
      if (type == WH100 || type == WH105 || type == WH110 ||
	  type == WH115 || type == WH120 || type == WH125 ||
	  type == WH130 || type == WH135 || type == WH140 ||
	  type == WH145 || type == WH150)
	return true;

      return false;

    }//isHiggs

    //---------------------------------------------------------------------------
    int getHiggsIndexMass(Type type){

      //the returning string
      if (type == WH100)
	return 0;
      else if (type == WH105)
	return 1;
      else if (type == WH110)
	return 2;
      else if (type == WH115)
	return 3;
      else if (type == WH120)
	return 4;
      else if (type == WH125)
	return 5;
      else if (type == WH130)
	return 6;
      else if (type == WH135)
	return 7;
      else if (type == WH140)
	return 8;
      else if (type == WH145)
	return 9;
      else if (type == WH150)
	return 10;
      else 
	cout<<"ERROR  DEFS::getHiggsIndexMass type does not corresponds to Higgs"<<endl;

      return -1;

    }//getHiggsIndexMass

    //---------------------------------------------------------------------------
    Type getHiggsType(unsigned HiggsMassIndex){

      //the returning string
      if (HiggsMassIndex == 0)
	return WH100;
      else if (HiggsMassIndex == 1)
	return WH105;
      else if (HiggsMassIndex == 2)
	return WH110;
      else if (HiggsMassIndex == 3)
	return WH115;
      else if (HiggsMassIndex == 4)
	return WH120;
      else if (HiggsMassIndex == 5)
	return WH125;
      else if (HiggsMassIndex == 6)
	return WH130;
      else if (HiggsMassIndex == 7)
	return WH135;
      else if (HiggsMassIndex == 8)
	return WH140;
      else if (HiggsMassIndex == 9)
	return WH145;
      else if (HiggsMassIndex == 10)
	return WH150;
      else
	cout<<"ERROR  PhysicsProcess::getHiggsType HiggsMassIndex  does not corresponds to Higgs"<<endl;

      return STopS_T;

    }//getHiggsType
     
    //---------------------------------------------------------------------------
    // A routine that returns the title for a given process type
    std::string getTypeTitle(Type type) {
       if (type == WW || type == WZ || type == ZZ)
          return string("WW+WZ+ZZ");
       else if (type == STopS_T || type == STopS_Tbar || type == STopT_T || type == STopT_Tbar ||
                type == STopTW_T || type == STopTW_Tbar)
          return string("Single Top");
       else if (type == ggH125 || type == qqH125 || type == WH125)
          return string("ggH+WH+qqH(125)");
       else if (type == WJets)
          return string("W+Jets");
       else if (type == SingleEl_Data || type == SingleMu_Data)
          return string("DATA");
       else if (type == QCD_ElEnriched || type == QCD_MuEnriched)
          return string("QCD");
       else
          return getTypeString(type);
    }//getTypeTitle

    //---------------------------------------------------------------------------
    // A  routine that returns the Color_t for a given process type
    Color_t getProcessColor(Type type){
       
       if (type == WW)
          return kPink;
       else if (type == WZ)
          return kBlue;
       else if (type == ZZ)
          return kBlue-1;
       else if (type == WJets)
          return kTeal+2;
       else if (type == ZJets)
          return kPink-8;
       else if (type == QCD_ElEnriched)
          return kYellow+1;
       else if (type == QCD_MuEnriched)
          return kYellow+1;
       //right now these aren't being used because we are combining all the STop's in Plots.cc
       else if (type == STopT_T)
          return kOrange+1;
       else if (type == STopT_Tbar)
          return kOrange+1; //kCyan+3;
       else if (type == STopS_T)
          return kOrange+1; //kBlue;
       else if (type == STopS_Tbar)
          return kOrange+1; //kBlue+3;
       else if (type == STopTW_T)
          return kOrange+1; //kMagenta;
       else if (type == STopTW_Tbar)
          return kOrange+1; //kGreen+3;
       else if (type == STopTW_Tbar)
          return kOrange+1; //kGreen+3;
       else if (type == TTbar)
          return kAzure-2; 
       else if (type == ggH125)
          return kRed+2; 
       else if (type == qqH125)
          return kRed+2; 
       else if (type == WH125)
          return kRed+2; 
       else if (type == SingleEl_Data)
          return kBlack;
       else if (type == SingleMu_Data)
          return kBlack;
       else{
          cout << "WARNING Plotter::GetProcessColor() Unknown process name=|"<<getTypeString(type)
               <<"|. Returning process color as kYellow." << endl;
       }
       
       return kYellow;
       
    }//getProcessColor

  } //namespace   
   
  //---------------------------------------------------------------------------
  // A routine that returns the string given the Lepton Category
  string getLeptonCatString(LeptonCat type){
    if (type == electron)
      return "electron";
    else if (type == muon)
      return "muon";
    else if (type == both)
       return "both";

    cout<<"ERROR in DEFS::getLeptonCatString type="<<type<<" not defined"<<endl;

    return "";

  }//getLeptonCatString

  //---------------------------------------------------------------------------
  // for historical reasons
  std::string getEventCatString(LeptonCat a){
    return  getLeptonCatString(a);
  }


  // A routine that returns the string given the Lepton Category
  LeptonCat getLeptonCat(string type){
    TString s = type;
    s.ToUpper();

    if (s.Contains("NONE"))
      return none;
    else if (s.Contains("MUON"))
      return muon;
    else if (s.Contains("ELECTRON"))
      return electron;
    else if (s.Contains("BOTH"))
      return both;
    else
      cout<<"ERROR in DEFS::getLeptonCatString type="<<type<<" not defined!. Returning none."<<endl;

    return none;

  }//getLeptonCat


  //---------------------------------------------------------------------------
  string getJetBinString(JetBin jBin){

    //the returning string
    if (jBin == jets0)
      return string("jets0");
    else if (jBin == jet1)
      return string("jet1");
    else if (jBin == jets2)
      return string("jets2");
    else if (jBin == jets3)
      return string("jets3");
    else if (jBin == jets4)
      return string("jets4");
    else if (jBin == jets5)
      return string("jets5");
    else     
      cout<<" ERROR  DEFS::geJetBinString jBin ="<<jBin<<" not found"<<endl;

    return "ERROR in DEFS::getJetBinString ";

  }//getJetBinString


  //---------------------------------------------------------------------------
  JetBin getJetBin(std::string str){


    if (str == "jets0")
      return jets0;
    else if (str == "jet1")
      return jet1;
    else if (str == "jets2")
      return jets2;
    else if (str == "jets3")
      return jets3;
    else if (str == "jets4")
      return jets4;
    else if (str == "jets5")
      return jets5;

    cout<<" ERROR  DEFS::getJetBin str ="<<str<<" not found"<<endl;
  
    return  jets0;

  }

  //---------------------------------------------------------------------------
  // A routine that returns a string given the type
  string getTagCatString(TagCat type){

    if (type == pretag)  return "pretag";
    if (type == eq0TSV)  return "eq0TSV";
    if (type == eq1TSV)  return "eq1TSV";
    if (type == eq2TSV)  return "eq2TSV";
    if (type == ge0TSV)  return "ge0TSV";
    if (type == ge1TSV)  return "ge1TSV";
    if (type == ge2TSV)  return "ge2TSV";

    cout <<"ERROR  PhysicsProcess::getTagCatString cannot find the given type"<<endl;
    return "ERROR";

  }
  
  // A routine that returns a jetBin given a string
  DEFS::TagCat getTagCat(std::string str){
    
    if (str == "pretag")  return pretag;
    if (str == "eq0TSV")  return eq0TSV;
    if (str == "eq1TSV")  return eq1TSV;
    if (str == "eq2TSV")  return eq2TSV;
    if (str == "ge0TSV")  return ge0TSV;
    if (str == "ge1TSV")  return ge1TSV;
    if (str == "ge2TSV")  return ge2TSV;

    cout<<"ERROR  PhysicsProcess::getTagCat cannot find the given string"<<endl;
    return pretag;
    
  }

  //---------------------------------------------------------------------------
  string getCutLevelString(CutLevel type) {
     
    if (type == NPATtupleEvts)  return "NPATtupleEvts";
    if (type == c0)             return "c0:NJets";
    if (type == c1)             return "c1:HLT+Kin";
    if (type == c2)             return "c2:VtxCut";
    if (type == c3)             return "c3:PrimaryEl/Mu";
    if (type == c4)             return "c4:NotLooseMu";
    if (type == c5)             return "c5:NotLooseEl";
    if (type == c6)             return "c6:METE";
    if (type == FNAL1)          return "FNAL:MET";
    if (type == FNAL2)          return "FNAL:Mt";
    if (type == FNAL3)          return "FNAL:LepPt";
    if (type == FNAL4)          return "FNAL:LepEta";
    if (type == FNAL5)          return "FNAL:JetPt1";
    if (type == FNAL6)          return "FNAL:JetPt2";
    if (type == FNAL7)          return "FNAL:JetEta1";
    if (type == FNAL8)          return "FNAL:JetEta2";
    if (type == FNAL9)          return "FNAL:drJetLep";
    if (type == FNAL10)         return "FNAL:Mjj";
    if (type == BTag0)          return "BTag0";
    if (type == BTag1)          return "BTag1";
    if (type == BTag2)          return "BTag2";
    if (type == BTag3p)         return "BTag3+";

    cout <<"ERROR  PhysicsProcess::getCutLevelString cannot find the given type"<<endl;
    return "ERROR";

   }

   DEFS::CutLevel getCutLevel(std::string str) {

    if (str == "NPATtupleEvts")    return NPATtupleEvts;
    if (str == "c0:NJets")         return c0;
    if (str == "c1:HLT+Kin")       return c1;
    if (str == "c2:VtxCut")        return c2;
    if (str == "c3:PrimaryEl/Mu")  return c3;
    if (str == "c4:NotLooseMu")    return c4;
    if (str == "c5:NotLooseEl")    return c5;
    if (str == "c6:METE")          return c6;
    if (str == "FNAL:MET")         return FNAL1;
    if (str == "FNAL:Mt")          return FNAL2;
    if (str == "FNAL:LepPt")       return FNAL3;
    if (str == "FNAL:LepEta")      return FNAL4;
    if (str == "FNAL:JetPt1")      return FNAL5;
    if (str == "FNAL:JetPt2")      return FNAL6;
    if (str == "FNAL:JetEta1")     return FNAL7;
    if (str == "FNAL:JetEta2")     return FNAL8;
    if (str == "FNAL:drJetLep")    return FNAL9;
    if (str == "FNAL:Mjj")         return FNAL10;
    if (str == "BTag0")            return BTag0;
    if (str == "BTag1")            return BTag1;
    if (str == "BTag2")            return BTag2;
    if (str == "BTag3+")           return BTag3p;


    cout<<"ERROR  PhysicsProcess::getCutLevel cannot find the given string"<<endl;
    return NPATtupleEvts;

   }
  
}// end of namespace DEFS
