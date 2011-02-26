#include "TAMUWW/SpecialTools/interface/Defs.hh"
#include <iostream>
#include <string>

namespace DEFS {

  using std::cout;
  using std::endl;
  using std::string;
  
  //---------------------------------------------------------------------------
  // A routine that returns the type given a string 
  string DEFS::getProcessTypeString(PhysicsProcessType type ){

    //the returning string
    if (type == SingleTopAnalysis)
      return "Single Top Analysis";
    else if (type == HiggsAnalysis)
      return "Higgs Analysis";
    else if (type == WWAnalysis)
      return "WW Analysis";
    
    return "ERROR DEFS::getProcessTypeString PhysicsProcessType "<<type<<" is unknown"<<endl;    
    
  }//getProcessTypeString

  //---------------------------------------------------------------------------
  DEFS::PhysicsProcessType getProcessType(std::string str){
  
    if(str == "WH100")
      return  DEFS::WH100;
    if( str == "WH105")
      return  DEFS::WH105;
    if( str == "WH110")
      return  DEFS::WH110;
    if( str == "WH115")
      return  DEFS::WH115;
    if( str == "WH120")
      return  DEFS::WH120;
    if( str == "WH125")
      return  DEFS::WH125;
    if( str == "WH130")
      return  DEFS::WH130;
    if( str == "WH135")
      return  DEFS::WH135;
    if( str == "WH140")
      return  DEFS::WH140;
    if( str == "WH145")
      return  DEFS::WH145;
    if( str == "WH150")
      return  DEFS::WH150;
    if( str == "STopS")
      return  DEFS::STopS;
    if( str == "STopT")
      return  DEFS::STopT;
    if( str == "Wbb")
      return  DEFS::Wbb;
    if( str == "Wcc/Wc")
      return  DEFS::Wcc;
    if( str == "Wjets")
      return  DEFS::Wjets;
    if( str == "Mistags")
      return  DEFS::WLight;
    if( str == "QCD100")
      return  DEFS::QCD100;
    if( str == "QCD250")
      return  DEFS::QCD250;
    if( str == "WW")
      return  DEFS::WW;
    if( str == "WZ")
      return  DEFS::WZ;
    if( str == "ZZ")
      return  DEFS::ZZ;
    if( str == "TTbar")
      return  DEFS::TTbar;
    if( str == "TTbarLJ")
      return  DEFS::TTbarLJ;
    if( str == "TTbarDil")
      return  DEFS::TTbarLJ;
    if( str == "Z+jets")
      return  DEFS::Zjets;
    if( str == "data")
      return  DEFS::Data;
  
    cout<<" ERROR  DEFS::getProcessType str ="<<str<<" not found"<<endl;
  
    return  DEFS::WH100;

  }//getProcessType

  //---------------------------------------------------------------------------
  string getProcessTypeString(PhysicsProcessType type){

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
    else if (type == STopS)
      return string("STopS");
    else if (type == STopT)
      return string("STopT");
    else if (type == Wjets)
      return string("Wjets");
    else if (type == Wbb)
      return string("Wbb");
    else if (type == Wcc) 
      return string("Wcc/Wc");
    else if (type == WLight) 
      return string("Mistags");
    else if (type == QCD100)
      return string("QCD100");
    else if (type == QCD250)
      return string("QCD250");
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
      return string("Z+jets");
    else if (type == Data)
      return string("data");
    else     
      cout<<" ERROR  DEFS::getProcessTypeString type ="<<type<<" not found"<<endl;

    return "ERROR in DEFS::getProcessTypeString ";

  }//getProcessTypeString

  //---------------------------------------------------------------------------
  string getJetBinString(JetBin jBin){

    //the returning string
    if (jBin == jets0)
      return string("0-jets");
    else if (jBin == jet1)
      return string("1-jet");
    else if (jBin == jets2)
      return string("2-jets");
    else if (jBin == jets3)
      return string("3-jets");
    else if (jBin == jets4)
      return string("4-jets");
    else if (jBin == jets5)
      return string("5-jets");
    else     
      cout<<" ERROR  DEFS::geJetBinString jBin ="<<jBin<<" not found"<<endl;

    return "ERROR in DEFS::getJetBinString ";

  }//getJetBinString


  //---------------------------------------------------------------------------
  JetBin getJetBin(std::string str){


    if (str == "0-jets")
      return jets0;
    else if (str == "1-jet")
      return jet1;
    else if (str == "2-jets")
      return jets2;
    else if (str == "3-jets")
      return jets3;
    else if (str == "4-jets")
      return jets4;
    else if (str == "5-jets")
      return jets5;

    cout<<" ERROR  DEFS::getJetBin str ="<<str<<" not found"<<endl;
  
    return  jets0;

  }


  //---------------------------------------------------------------------------
  bool isHiggs(PhysicsProcessType type){

    //the returning string
    if (type == WH100 || type == WH105 || type == WH110 ||
	type == WH115 || type == WH120 || type == WH125 ||
	type == WH130 || type == WH135 || type == WH140 ||
	type == WH145 || type == WH150)
      return true;

    return false;

  }//isHiggs

  //---------------------------------------------------------------------------
  int getHiggsIndexMass(PhysicsProcessType type){

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
  DEFS::PhysicsProcessType getHiggsType(unsigned HiggsMassIndex){

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


  // A routine that returns a string given the type
  string getTagCatString(TagCat type){

    if (type == PreTag)  return "PreTag";
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
    
    if (str == "PreTag")  return PreTag;
    if (str == "eq0TSV")  return eq0TSV;
    if (str == "eq1TSV")  return eq1TSV;
    if (str == "eq2TSV")  return eq2TSV;
    if (str == "ge0TSV")  return ge0TSV;
    if (str == "ge1TSV")  return ge1TSV;
    if (str == "ge2TSV")  return ge2TSV;

    cout<<"ERROR  PhysicsProcess::getTagCatString cannot find the given type"<<endl;
    return PreTag;
    
  }
  
}// end of namespace DEFS
