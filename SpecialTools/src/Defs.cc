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
      if (type == SingleTopAnalysis)  return "Single Top Analysis";
      else if (type == HiggsAnalysis) return "Higgs Analysis";
      else if (type == WWAnalysis) 	  return "WW Analysis";
      
      cout<<"ERROR DEFS::getProcessTypeString AnalysisType "<<type<<" is unknown"<<endl;   
      
      return "";
      
    }//getAnalysisTypeString
    
  }// namespace

  namespace PhysicsProcess {

    //---------------------------------------------------------------------------
    DEFS::PhysicsProcessType getProcessType(std::string str){
  
      if(str == "WH100")    	return  DEFS::PhysicsProcess::WH100;
      else if(str == "WH105")	return  DEFS::PhysicsProcess::WH105;
      else if(str == "WH110")	return  DEFS::PhysicsProcess::WH110;
      else if(str == "WH115")	return  DEFS::PhysicsProcess::WH115;
      else if(str == "WH120")	return  DEFS::PhysicsProcess::WH120;
      else if(str == "WH125")	return  DEFS::PhysicsProcess::WH125;
      else if(str == "WH130")	return  DEFS::PhysicsProcess::WH130;
      else if(str == "WH135")	return  DEFS::PhysicsProcess::WH135;
      else if(str == "WH140")	return  DEFS::PhysicsProcess::WH140;
      else if(str == "WH145")	return  DEFS::PhysicsProcess::WH145;
      else if(str == "WH150")	return  DEFS::PhysicsProcess::WH150;
      else if(str == "WH160")	return  DEFS::PhysicsProcess::WH160;
      else if(str == "WH170")	return  DEFS::PhysicsProcess::WH170;
      else if(str == "WH180")	return  DEFS::PhysicsProcess::WH180;
      else if(str == "WH190")	return  DEFS::PhysicsProcess::WH190;
      else if(str == "WH200")	return  DEFS::PhysicsProcess::WH200;
      else if(str == "WH250")	return  DEFS::PhysicsProcess::WH250;
      else if(str == "WH300")	return  DEFS::PhysicsProcess::WH300;
      else if(str == "WH350")	return  DEFS::PhysicsProcess::WH350;
      else if(str == "WH400")	return  DEFS::PhysicsProcess::WH400;
      else if(str == "WH450")	return  DEFS::PhysicsProcess::WH450;
      else if(str == "WH500")	return  DEFS::PhysicsProcess::WH500;
      else if(str == "WH550")	return  DEFS::PhysicsProcess::WH550;
      else if(str == "WH600")	return  DEFS::PhysicsProcess::WH600;
      else if(str == "WH700")	return  DEFS::PhysicsProcess::WH700;
      else if(str == "WH800")	return  DEFS::PhysicsProcess::WH800;
      else if(str == "WH900")	return  DEFS::PhysicsProcess::WH900;
      else if(str == "WH1000")	return  DEFS::PhysicsProcess::WH1000;

      else if(str == "ggH110")	return  DEFS::PhysicsProcess::ggH110;
      else if(str == "ggH115")	return  DEFS::PhysicsProcess::ggH115;
      else if(str == "ggH120")	return  DEFS::PhysicsProcess::ggH120;
      else if(str == "ggH125")	return  DEFS::PhysicsProcess::ggH125;
      else if(str == "ggH130")	return  DEFS::PhysicsProcess::ggH130;
      else if(str == "ggH135")	return  DEFS::PhysicsProcess::ggH135;
      else if(str == "ggH140")	return  DEFS::PhysicsProcess::ggH140;
      else if(str == "ggH145")	return  DEFS::PhysicsProcess::ggH145;
      else if(str == "ggH150")	return  DEFS::PhysicsProcess::ggH150;
      else if(str == "ggH160")	return  DEFS::PhysicsProcess::ggH160;
      else if(str == "ggH170")	return  DEFS::PhysicsProcess::ggH170;
      else if(str == "ggH180")	return  DEFS::PhysicsProcess::ggH180;
      else if(str == "ggH190")	return  DEFS::PhysicsProcess::ggH190;
      else if(str == "ggH200")	return  DEFS::PhysicsProcess::ggH200;
      else if(str == "ggH250")	return  DEFS::PhysicsProcess::ggH250;
      else if(str == "ggH300")	return  DEFS::PhysicsProcess::ggH300;
      else if(str == "ggH350")	return  DEFS::PhysicsProcess::ggH350;
      else if(str == "ggH400")	return  DEFS::PhysicsProcess::ggH400;
      else if(str == "ggH450")	return  DEFS::PhysicsProcess::ggH450;
      else if(str == "ggH500")	return  DEFS::PhysicsProcess::ggH500;
      else if(str == "ggH550")	return  DEFS::PhysicsProcess::ggH550;
      else if(str == "ggH600")	return  DEFS::PhysicsProcess::ggH600;
      else if(str == "ggH700")	return  DEFS::PhysicsProcess::ggH700;
      else if(str == "ggH800")	return  DEFS::PhysicsProcess::ggH800;
      else if(str == "ggH900")	return  DEFS::PhysicsProcess::ggH900;
      else if(str == "ggH1000")	return  DEFS::PhysicsProcess::ggH1000;

      else if(str == "qqH110")	return  DEFS::PhysicsProcess::qqH110;
      else if(str == "qqH115")	return  DEFS::PhysicsProcess::qqH115;
      else if(str == "qqH120")	return  DEFS::PhysicsProcess::qqH120;
      else if(str == "qqH125")	return  DEFS::PhysicsProcess::qqH125;
      else if(str == "qqH130")	return  DEFS::PhysicsProcess::qqH130;
      else if(str == "qqH135")	return  DEFS::PhysicsProcess::qqH135;
      else if(str == "qqH140")	return  DEFS::PhysicsProcess::qqH140;
      else if(str == "qqH145")	return  DEFS::PhysicsProcess::qqH145;
      else if(str == "qqH150")	return  DEFS::PhysicsProcess::qqH150;
      else if(str == "qqH160")	return  DEFS::PhysicsProcess::qqH160;
      else if(str == "qqH170")	return  DEFS::PhysicsProcess::qqH170;
      else if(str == "qqH180")	return  DEFS::PhysicsProcess::qqH180;
      else if(str == "qqH190")	return  DEFS::PhysicsProcess::qqH190;
      else if(str == "qqH200")	return  DEFS::PhysicsProcess::qqH200;
      else if(str == "qqH250")	return  DEFS::PhysicsProcess::qqH250;
      else if(str == "qqH300")	return  DEFS::PhysicsProcess::qqH300;
      else if(str == "qqH350")	return  DEFS::PhysicsProcess::qqH350;
      else if(str == "qqH400")	return  DEFS::PhysicsProcess::qqH400;
      else if(str == "qqH450")	return  DEFS::PhysicsProcess::qqH450;
      else if(str == "qqH500")	return  DEFS::PhysicsProcess::qqH500;
      else if(str == "qqH550")	return  DEFS::PhysicsProcess::qqH550;
      else if(str == "qqH600")	return  DEFS::PhysicsProcess::qqH600;
      else if(str == "qqH700")	return  DEFS::PhysicsProcess::qqH700;
      else if(str == "qqH800")	return  DEFS::PhysicsProcess::qqH800;
      else if(str == "qqH900")	return  DEFS::PhysicsProcess::qqH900;
      else if(str == "qqH1000")	return  DEFS::PhysicsProcess::qqH1000;

      else if(str == "STopS_T")	           return  DEFS::PhysicsProcess::STopS_T;
      else if(str == "STopS_Tbar")	       return  DEFS::PhysicsProcess::STopS_Tbar;
      else if(str == "STopT_T")	           return  DEFS::PhysicsProcess::STopT_T;
      else if(str == "STopT_Tbar")	       return  DEFS::PhysicsProcess::STopT_Tbar;
      else if(str == "STopTW_T")	       return  DEFS::PhysicsProcess::STopTW_T;
      else if(str == "STopTW_bar")	       return  DEFS::PhysicsProcess::STopTW_Tbar;
      else if(str == "Wcc")	               return  DEFS::PhysicsProcess::Wcc;
      else if(str == "WJets")	           return  DEFS::PhysicsProcess::WJets;
      else if(str == "WLg")	               return  DEFS::PhysicsProcess::WLg;
      else if(str == "Wgg")	               return  DEFS::PhysicsProcess::Wgg;
      else if(str == "WLL")	               return  DEFS::PhysicsProcess::WLL;
      else if(str == "Wbb")	               return  DEFS::PhysicsProcess::WLb;
      else if(str == "WLb")	               return  DEFS::PhysicsProcess::Wbb;
      else if(str == "QCDMu")	           return  DEFS::PhysicsProcess::QCDMu;
      else if(str == "QCDEl_Pt30to80")	   return  DEFS::PhysicsProcess::QCDEl_Pt30to80;
      else if(str == "QCDEl_Pt80to170")	   return  DEFS::PhysicsProcess::QCDEl_Pt80to170;
      else if(str == "QCDEl_BCtoE30to80")  return  DEFS::PhysicsProcess::QCDEl_BCtoE30to80;
      else if(str == "QCDEl_BCtoE80to170") return  DEFS::PhysicsProcess::QCDEl_BCtoE80to170;
      else if(str == "QCD_ElEnriched")	   return  DEFS::PhysicsProcess::QCD_ElEnriched;
      else if(str == "QCD_MuEnriched")	   return  DEFS::PhysicsProcess::QCD_MuEnriched;
      //else if(str == "QCD100") 	           return  DEFS::PhysicsProcess::QCD100;
      //else if(str == "QCD250") 	           return  DEFS::PhysicsProcess::QCD250;
      else if(str == "WW")	               return  DEFS::PhysicsProcess::WW;
      else if(str == "WZ")	               return  DEFS::PhysicsProcess::WZ;
      else if(str == "WZbb")	           return  DEFS::PhysicsProcess::WZbb;
      else if(str == "ZZ")	               return  DEFS::PhysicsProcess::ZZ;
      else if(str == "TTbar")	           return  DEFS::PhysicsProcess::TTbar;
      else if(str == "TTbarLJ")	           return  DEFS::PhysicsProcess::TTbarLJ;
      else if(str == "TTbarDil")           return  DEFS::PhysicsProcess::TTbarLJ;
      else if(str == "ZJets")	           return  DEFS::PhysicsProcess::ZJets;
      else if(str == "Ztautau")	           return  DEFS::PhysicsProcess::Ztautau;
      else if(str == "SingleEl_Data")	   return  DEFS::PhysicsProcess::SingleEl_Data;
      else if(str == "SingleMu_Data")	   return  DEFS::PhysicsProcess::SingleMu_Data;
  
      cout<<" ERROR  DEFS::getProcessType str ="<<str<<" not found"<<endl;
  
      return  DEFS::PhysicsProcess::WH100;

    }//getProcessType

    //---------------------------------------------------------------------------
    string getTypeString(Type type){

      //the returning string
      if (type == WH100)	    return string("WH100");
      else if (type == WH105)	return string("WH105");
      else if (type == WH110)	return string("WH110");
      else if (type == WH115)	return string("WH115");
      else if (type == WH120)	return string("WH120");
      else if (type == WH125)	return string("WH125");
      else if (type == WH130)	return string("WH130");
      else if (type == WH135)	return string("WH135");
      else if (type == WH140)	return string("WH140");
      else if (type == WH145)	return string("WH145");
      else if (type == WH150)	return string("WH150");
      else if (type == WH160)	return string("WH160");
      else if (type == WH170)	return string("WH170");
      else if (type == WH180)	return string("WH180");
      else if (type == WH190)	return string("WH190");
      else if (type == WH200)	return string("WH200");
      else if (type == WH250)	return string("WH250");
      else if (type == WH300)	return string("WH300");
      else if (type == WH350)	return string("WH350");
      else if (type == WH400)	return string("WH400");
      else if (type == WH450)	return string("WH450");
      else if (type == WH500)	return string("WH500");
      else if (type == WH550)	return string("WH550");
      else if (type == WH600)	return string("WH600");
      else if (type == WH700)	return string("WH700");
      else if (type == WH800)	return string("WH800");
      else if (type == WH900)	return string("WH900");
      else if (type == WH1000)	return string("WH1000");

      else if (type == ggH110)	 return string("ggH110");
      else if (type == ggH115)	 return string("ggH115");
      else if (type == ggH120)	 return string("ggH120");
      else if (type == ggH125)	 return string("ggH125");
      else if (type == ggH130)	 return string("ggH130");
      else if (type == ggH135)	 return string("ggH135");
      else if (type == ggH140)	 return string("ggH140");
      else if (type == ggH145)	 return string("ggH145");
      else if (type == ggH150)	 return string("ggH150");
      else if (type == ggH160)	 return string("ggH160");
      else if (type == ggH170)	 return string("ggH170");
      else if (type == ggH180)	 return string("ggH180");
      else if (type == ggH190)	 return string("ggH190");
      else if (type == ggH200)	 return string("ggH200");
      else if (type == ggH250)	 return string("ggH250");
      else if (type == ggH300)	 return string("ggH300");
      else if (type == ggH350)	 return string("ggH350");
      else if (type == ggH400)	 return string("ggH400");
      else if (type == ggH450)	 return string("ggH450");
      else if (type == ggH500)	 return string("ggH500");
      else if (type == ggH550)	 return string("ggH550");
      else if (type == ggH600)	 return string("ggH600");
      else if (type == ggH700)	 return string("ggH700");
      else if (type == ggH800)	 return string("ggH800");
      else if (type == ggH900)	 return string("ggH900");
      else if (type == ggH1000)  return string("ggH1000");

      else if (type == qqH110)	 return string("qqH110");
      else if (type == qqH115)	 return string("qqH115");
      else if (type == qqH120)	 return string("qqH120");
      else if (type == qqH125)	 return string("qqH125");
      else if (type == qqH130)	 return string("qqH130");
      else if (type == qqH135)	 return string("qqH135");
      else if (type == qqH140)	 return string("qqH140");
      else if (type == qqH145)	 return string("qqH145");
      else if (type == qqH150)	 return string("qqH150");
      else if (type == qqH160)	 return string("qqH160");
      else if (type == qqH170)	 return string("qqH170");
      else if (type == qqH180)	 return string("qqH180");
      else if (type == qqH190)	 return string("qqH190");
      else if (type == qqH200)	 return string("qqH200");
      else if (type == qqH250)	 return string("qqH250");
      else if (type == qqH300)	 return string("qqH300");
      else if (type == qqH350)	 return string("qqH350");
      else if (type == qqH400)	 return string("qqH400");
      else if (type == qqH450)	 return string("qqH450");
      else if (type == qqH500)	 return string("qqH500");
      else if (type == qqH550)	 return string("qqH550");
      else if (type == qqH600)	 return string("qqH600");
      else if (type == qqH700)	 return string("qqH700");
      else if (type == qqH800)	 return string("qqH800");
      else if (type == qqH900)	 return string("qqH900");
      else if (type == qqH1000)  return string("qqH1000");

      else if (type == STopS_T)	           return string("STopS_T");
      else if (type == STopS_Tbar)	       return string("STopS_Tbar");
      else if (type == STopT_T)	           return string("STopT_T");
      else if (type == STopT_Tbar)	       return string("STopT_Tbar");
      else if (type == STopTW_T)	       return string("STopTW_T");
      else if (type == STopTW_Tbar)	       return string("STopTW_Tbar");
      else if (type == Wcc) 	           return string("Wcc");
      else if (type == WJets)	           return string("WJets");
      else if (type == WLg)	               return string("WLg");
      else if (type == Wgg)	               return string("Wgg");
      else if (type == WLL)	               return string("WLL");
      else if (type == Wbb)	               return string("Wbb");
      else if (type == QCDMu)	           return string("QCDMu");
      else if (type == QCDEl_Pt30to80)     return string("QCDEl_Pt30to80");
      else if (type == QCDEl_Pt80to170)	   return string("QCDEl_Pt80to170");
      else if (type == QCDEl_BCtoE30to80)  return string("QCDEl_BCtoE30to80");
      else if (type == QCDEl_BCtoE80to170) return string("QCDEl_BCtoE80to170");
      else if (type == QCD_ElEnriched)	   return string("QCD_ElEnriched");
      else if (type == QCD_MuEnriched)	   return string("QCD_MuEnriched");
      //else if (type == QCD100) 	         return string("QCD100");
      //else if (type == QCD250)             return string("QCD250");
      else if (type == WW)	               return string("WW");
      else if (type == WZ)	               return string("WZ");
      else if (type == ZZ)	               return string("ZZ");
      else if (type == TTbar)	           return string("TTbar");
      else if (type == TTbarLJ)	           return string("TTbarLJ");
      else if (type == TTbarDil)           return string("TTbarDil");
      else if (type == ZJets)	           return string("ZJets");
      else if (type == Ztautau)	           return string("Ztautau");
      else if (type == SingleEl_Data)      return string("SingleEl_Data");
      else if (type == SingleMu_Data)	   return string("SingleMu_Data");
      else cout<<" ERROR  DEFS::getProcessTypeString type ="<<type<<" not found"<<endl;

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
      if (type == WH100)	  return 0;
      else if (type == WH105) return 1;
      else if (type == WH110) return 2;
      else if (type == WH115) return 3;
      else if (type == WH120) return 4;
      else if (type == WH125) return 5;
      else if (type == WH130) return 6;
      else if (type == WH135) return 7;
      else if (type == WH140) return 8;
      else if (type == WH145) return 9;
      else if (type == WH150) return 10;
      else cout<<"ERROR  DEFS::getHiggsIndexMass type does not corresponds to Higgs"<<endl;

      return -1;

    }//getHiggsIndexMass

    //---------------------------------------------------------------------------
    Type getHiggsType(unsigned HiggsMassIndex){

      //the returning string
      if (HiggsMassIndex == 0)       return WH100;
      else if (HiggsMassIndex == 1)	 return WH105;
      else if (HiggsMassIndex == 2)	 return WH110;
      else if (HiggsMassIndex == 3)	 return WH115;
      else if (HiggsMassIndex == 4)	 return WH120;
      else if (HiggsMassIndex == 5)	 return WH125;
      else if (HiggsMassIndex == 6)	 return WH130;
      else if (HiggsMassIndex == 7)	 return WH135;
      else if (HiggsMassIndex == 8)	 return WH140;
      else if (HiggsMassIndex == 9)	 return WH145;
      else if (HiggsMassIndex == 10) return WH150;
      else cout<<"ERROR  PhysicsProcess::getHiggsType HiggsMassIndex  does not corresponds to Higgs"<<endl;

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
       
       if (type == WW)                  return kPink;
       else if (type == WZ)             return kBlue;
       else if (type == ZZ)             return kBlue-1;
       else if (type == WJets)          return kTeal+2;
       else if (type == ZJets)          return kPink-8;
       else if (type == QCD_ElEnriched) return kYellow+1;
       else if (type == QCD_MuEnriched) return kYellow+1;
       else if (type == STopT_T)        return kOrange+1;
       else if (type == STopT_Tbar)     return kOrange+1; //kCyan+3;
       else if (type == STopS_T)        return kOrange+1; //kBlue;
       else if (type == STopS_Tbar)     return kOrange+1; //kBlue+3;
       else if (type == STopTW_T)       return kOrange+1; //kMagenta;
       else if (type == STopTW_Tbar)    return kOrange+1; //kGreen+3;
       else if (type == STopTW_Tbar)    return kOrange+1; //kGreen+3;
       else if (type == TTbar)          return kAzure-2; 
       else if (type == ggH125)         return kRed+2; 
       else if (type == qqH125)         return kRed+2; 
       else if (type == WH125)          return kRed+2; 
       else if (type == SingleEl_Data)  return kBlack;
       else if (type == SingleMu_Data)  return kBlack;
       else{
          cout << "WARNING Plotter::GetProcessColor() Unknown process name=|"<<getTypeString(type)
               <<"|. Returning process color as kBlack." << endl;
       }
       
       return kBlack;
       
    }//getProcessColor

  } //namespace   
   
  //---------------------------------------------------------------------------
  // A routine that returns the string given the Lepton Category
  string getLeptonCatString(LeptonCat type){
    if (type == electron)  return "electron";
    else if (type == muon) return "muon";
    else if (type == both) return "both";

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

    if (s.Contains("NONE"))          return none;
    else if (s.Contains("MUON"))     return muon;
    else if (s.Contains("ELECTRON")) return electron;
    else if (s.Contains("BOTH"))     return both;
    else cout<<"ERROR in DEFS::getLeptonCatString type="<<type<<" not defined!. Returning none."<<endl;

    return none;

  }//getLeptonCat


  //---------------------------------------------------------------------------
  string getJetBinString(JetBin jBin){

    //the returning string
    if (jBin == jets0)      return string("jets0");
    else if (jBin == jet1)  return string("jet1");
    else if (jBin == jets2) return string("jets2");
    else if (jBin == jets3) return string("jets3");
    else if (jBin == jets4) return string("jets4");
    else if (jBin == jets5) return string("jets5");
    else cout<<" ERROR  DEFS::geJetBinString jBin ="<<jBin<<" not found"<<endl;

    return "ERROR in DEFS::getJetBinString ";

  }//getJetBinString


  //---------------------------------------------------------------------------
  JetBin getJetBin(std::string str){


    if (str == "jets0")      return jets0;
    else if (str == "jet1")  return jet1;
    else if (str == "jets2") return jets2;
    else if (str == "jets3") return jets3;
    else if (str == "jets4") return jets4;
    else if (str == "jets5") return jets5;

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
     
    if (type == NPATtupleEvts) return "NPATtupleEvts";
    else if (type == c0)       return "c0:NJets";
    else if (type == c1)       return "c1:HLT+Kin";
    else if (type == c2)       return "c2:VtxCut";
    else if (type == c3)       return "c3:PrimaryEl/Mu";
    else if (type == c4)       return "c4:NotLooseMu";
    else if (type == c5)       return "c5:NotLooseEl";
    else if (type == c6)       return "c6:METE";
    else if (type == FNAL1)    return "FNAL:MET";
    else if (type == FNAL2)    return "FNAL:Mt";
    else if (type == FNAL3)    return "FNAL:LepPt";
    else if (type == FNAL4)    return "FNAL:LepEta";
    else if (type == FNAL5)    return "FNAL:JetPt1";
    else if (type == FNAL6)    return "FNAL:JetPt2";
    else if (type == FNAL7)    return "FNAL:JetEta1";
    else if (type == FNAL8)    return "FNAL:JetEta2";
    else if (type == FNAL9)    return "FNAL:drJetLep";
    else if (type == FNAL10)   return "FNAL:Mjj";
    else if (type == BTag0)    return "BTag0";
    else if (type == BTag1)    return "BTag1";
    else if (type == BTag2)    return "BTag2";
    else if (type == BTag3p)   return "BTag3+";

    cout <<"ERROR  DEFS::getCutLevelString cannot find the given type"<<endl;
    return "ERROR";

   }

   DEFS::CutLevel getCutLevel(std::string str) {

    if (str == "NPATtupleEvts")        return NPATtupleEvts;
    else if (str == "c0:NJets")        return c0;
    else if (str == "c1:HLT+Kin")      return c1;
    else if (str == "c2:VtxCut")       return c2;
    else if (str == "c3:PrimaryEl/Mu") return c3;
    else if (str == "c4:NotLooseMu")   return c4;
    else if (str == "c5:NotLooseEl")   return c5;
    else if (str == "c6:METE")         return c6;
    else if (str == "FNAL:MET")        return FNAL1;
    else if (str == "FNAL:Mt")         return FNAL2;
    else if (str == "FNAL:LepPt")      return FNAL3;
    else if (str == "FNAL:LepEta")     return FNAL4;
    else if (str == "FNAL:JetPt1")     return FNAL5;
    else if (str == "FNAL:JetPt2")     return FNAL6;
    else if (str == "FNAL:JetEta1")    return FNAL7;
    else if (str == "FNAL:JetEta2")    return FNAL8;
    else if (str == "FNAL:drJetLep")   return FNAL9;
    else if (str == "FNAL:Mjj")        return FNAL10;
    else if (str == "BTag0")           return BTag0;
    else if (str == "BTag1")           return BTag1;
    else if (str == "BTag2")           return BTag2;
    else if (str == "BTag3+")          return BTag3p;

    cout<<"ERROR  DEFS::getCutLevel cannot find the given string"<<endl;
    return NPATtupleEvts;

   }
   
   //---------------------------------------------------------------------------
   // A routine that returns a string given the type
   std::string getNtupleTypeString(NtupleType type){
      
      if (type == EventNtuple)      return "EventNtuple";
      else if (type == METree)      return "METree";
      else if (type == MicroNtuple) return "MicroNtuple";
      else if (type == Other)       return "Other";

      cout<<"ERROR DEFS::getNtupleTypeString cannot find the given type"<<endl;
      return "ERROR";

   }
   
   // A routine that returns a NtupleType given a string
   DEFS::NtupleType getNtupleType(std::string str){
      
      if (str == "EventNtuple")      return EventNtuple;
      else if (str == "MEtree")      return METree;
      else if (str == "MicroNtuple") return MicroNtuple;
      else if (str == "Other")       return Other;

      cout<<"ERROR  DEFS::getNtupleType cannot find the given string"<<endl;
      return Other;

   }

}// end of namespace DEFS
