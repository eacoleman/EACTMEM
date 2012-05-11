#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include <iostream>
#include <string>

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


      if(str == "HWW110")
	return  DEFS::PhysicsProcess::HWW110;
      if(str == "HWW115")
	return  DEFS::PhysicsProcess::HWW115;
      if(str == "HWW120")
	return  DEFS::PhysicsProcess::HWW120;
      if(str == "HWW125")
	return  DEFS::PhysicsProcess::HWW125;
      if(str == "HWW130")
	return  DEFS::PhysicsProcess::HWW130;
      if(str == "HWW135")
	return  DEFS::PhysicsProcess::HWW135;
      if(str == "HWW140")
	return  DEFS::PhysicsProcess::HWW140;
      if(str == "HWW145")
	return  DEFS::PhysicsProcess::HWW145;
      if(str == "HWW150")
	return  DEFS::PhysicsProcess::HWW150;
      if(str == "HWW155")
	return  DEFS::PhysicsProcess::HWW155;
      if(str == "HWW160")
	return  DEFS::PhysicsProcess::HWW160;
      if(str == "HWW165")
	return  DEFS::PhysicsProcess::HWW165;
      if(str == "HWW170")
	return  DEFS::PhysicsProcess::HWW170;


      if( str == "STopS")
	return  DEFS::PhysicsProcess::STopS;
      if( str == "STopT")
	return  DEFS::PhysicsProcess::STopT;
      if( str == "STopTW")
	return  DEFS::PhysicsProcess::STopTW;
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
      if( str == "Zjets")
	return  DEFS::PhysicsProcess::Zjets;
      if( str == "Ztautau")
	return  DEFS::PhysicsProcess::Ztautau;
      if( str == "data")
	return  DEFS::PhysicsProcess::Data;
  
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

      else if (type == HWW110)
	return string("HWW110");
      else if (type == HWW115)
	return string("HWW115");
      else if (type == HWW120)
	return string("HWW120");
      else if (type == HWW125)
	return string("HWW125");
      else if (type == HWW130)
	return string("HWW130");
      else if (type == HWW135)
	return string("HWW135");
      else if (type == HWW140)
	return string("HWW140");
      else if (type == HWW145)
	return string("HWW145");
      else if (type == HWW150)
	return string("HWW150");
      else if (type == HWW155)
	return string("HWW155");
      else if (type == HWW160)
	return string("HWW160");
      else if (type == HWW165)
	return string("HWW165");
      else if (type == HWW170)
	return string("HWW170");

      else if (type == STopS)
	return string("STopS");
      else if (type == STopT)
	return string("STopT");
      else if (type == STopTW)
	return string("STopTW");
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
      else if (type == Zjets)
	return string("Zjets");
      else if (type == Ztautau)
	return string("Ztautau");
      else if (type == Data)
	return string("data");
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

      return STopS;

    }//getHiggsType

  } //namespace


  //---------------------------------------------------------------------------
  // A routine that returns the string given the Event Category
  string getEventCatString(LeptonCat type){
    if (type == electron)
      return "electron";
    else if (type == muon)
      return "muon";

    cout<<"ERROR in DEFS::getEventCatString type="<<type<<" not defined"<<endl;

    return "";

  }//getEventCatString

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

    cout<<"ERROR  PhysicsProcess::getTagCatString cannot find the given type"<<endl;
    return pretag;
    
  }
  
}// end of namespace DEFS
