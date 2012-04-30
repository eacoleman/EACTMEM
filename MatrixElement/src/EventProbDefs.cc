#include  "TAMUWW/MatrixElement/interface/EventProbDefs.hh"
#include <iostream>
#include <string>

namespace DEFS {

  namespace EP{

    using std::cout;
    using std::endl;
    using std::string;
    
    //---------------------------------------------------------------------------
    string getTypeString(Type type){
      
      if (type == UNKNOWN)
	return string ("UNKNOWN");
      
      // Related to Higgs 
      else if (type == WH)
	return string("WH");
      else if (type == HWW)
	return string("HWW");
      
      // Related to single top production
      else if (type == TopS)
	return string("TopS");
      else if (type == TopT)
	return string("TopT");
      else if (type == TopTAlt)
	return string("TopTAlt");
      
      // Related to top pair production
      else if (type == TTbar)
	return string("TTbar");
      
      // Related to single vector boson production
      else if (type == ZLight)
	return string("ZLight");
      else if (type == Wc)
	return string("Wc");
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
      
      // Related to double vector boson production
      else if (type == WW)
	return string("WW");
      else if (type == WZ)
	return string("WZ");
      else if (type == ZZ)
	return string("ZZ");
      
      // Related to QCD
      else if (type == QCD)
	return string("QCD");
      
      // Related to other things
      else if (type == CorrEPD)
	return string("CorrEPD");
      
      // Return error otherwise
      cout<<"ERROR DEFS::getEPTypeString (in file EventProbDefs.cc) called with unknown type "
	  <<type<<endl;
      
      return "ERROR DEFS::getEPTypeString (in file EventProbDefs.cc) called with unknown type.";
      
    }//getTypeString
    
  }// EP namespace
    
}// DEFS namespace
