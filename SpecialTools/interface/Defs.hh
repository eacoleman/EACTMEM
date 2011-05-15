#ifndef DEFS_DEF
#define DEFS_DEF

#include <string>

// This namespace holds the definitions of all physics processes 
// author: Ricardo Eusebi, Feb 12, 2009


namespace DEFS{

  // ---------------------------------------------------------------
  //            ALL ABOUT THE ANALYSIS WE WANT TO DO 
  // ---------------------------------------------------------------  
  namespace Ana {
    enum Type{
      SingleTopAnalysis,
      HiggsAnalysis,
      WWAnalysis
    };
  
    // A routine that returns the string given the type 
    std::string getTypeString(Type );

  }// Ana namespace
  
  // ---------------------------------------------------------------
  //            ALL ABOUT THE PHYSICSPROCESS TYPES
  // ---------------------------------------------------------------
  namespace PhysicsProcess {
    enum Type {WH100, WH105, WH110, WH115, WH120, WH125,
	       WH130, WH135, WH140, WH145, WH150,
	       STopS, STopT, Wjets, Wbb, Wcc, WLight, 
	       QCD100, QCD250, WW, WZ, ZZ, TTbar, TTbarLJ, TTbarDil, Zjets, Data};


    // A routine that returns the type given a string 
    Type getProcessType(std::string str);///
    
    // A routine that returns the type given a string 
    std::string getTypeString(Type );
    
    // A routine that tells whether this process is Higgs or not
    bool isHiggs(Type type);
    
    // A routine that give the Higgs Mass Index given the type
    int getHiggsIndexMass(Type type);
    
    // A routine that gives the type given the Higgs Mass index
    Type getHiggsType(unsigned HiggsMassIndex);
    
    
  }// PhysicsProcess namespace
  typedef DEFS::PhysicsProcess::Type PhysicsProcessType ;

  // ---------------------------------------------------------------
  //            ALL ABOUT THE DIFFERENT EVENT CATEGORIES
  // ---------------------------------------------------------------
  enum EvtCat {electron, muon};
  
    // A routine that returns the string given the EvtType 
  std::string getEventCatString(EvtCat );

  // -----------------------------------------------------------
  //            ALL ABOUT THE JET TYPES
  // ---------------------------------------------------------------
  enum JetBin {jets0, jet1, jets2, jets3, jets4, jets5};

  // A routine that returns a string given the type
  std::string getJetBinString(JetBin type);

  // A routine that returns a jetBin given
  JetBin getJetBin(std::string str);


  // ---------------------------------------------------------------
  //            ALL ABOUT THE TAGGING CATEGORIES
  // ---------------------------------------------------------------

  // The tagging categories per event
  // TSV = tight SecVtx
  // JP5 = JP < 5%
  enum TagCat {

    pretag,   // The pretag category
    eq0TSV,   // Exactly zero SVX tags, i.e. the untag category.
    eq1TSV,   // Exactly one SVX tag 
    eq2TSV,   // Exactly two SVX tags 
    ge0TSV,   // Greater equal than 0 SVX tags 
    ge1TSV,   // Greater equal than 1 SVX tag 
    ge2TSV,   // Greater equal than 2 SVX tags 

    // For the tagging with JP in descendant exclusive orthogonal category
    TSVTSV,   // for some reason this is not exactly equal to eq2TSV
    TSVJP5,   // one jet with a tight SVX tag and the other one with a loose JP tag which is not a tight SVX
    TSVNOJP5, // one jet with a tight SVX tag and the other one without a loose JP or tight SVX tag.
    JP5NOTSV // one jet with a Loose SVT tag and neither jet is a tight SVX tag.
   

    // For the tagging with Loose in descendant exclusive orthogonal category
    // TSVTSV  //  for some reason this is not exaclty equal to eq2TSV
    ///TSVLSV,    // one jet with a tight SVX tag and the other one with a loose SecVTx tag which is not a tight SVX
    //TSVNOLSV,  // one jet with a tight SVX tag and the other one without a loose SVX tag.   
    //LSVNOTSV   // one jet with a Loose SVT tag and neither jet is a tight SVX tag.

    // In principle we can have four choices, each with 4 exclusive categories in which in a 
    // given column a given line is always orthogonal to all lines above:
    //       | CHOICE A    | CHOICE B  |  CHOICE C | CHOICE D
    // cat 0 | TSVTSV      | TSVTSV    | TSVTSV    | TSVTSV   
    // cat 1 | TSVJP5      | TSVLSV    | TSVJP5    | TSVLSV   
    // cat 2 | TSVNOJP5    | TSVNOLSV  | TSVNOJP5  | TSVNOLSV 
    // cat 3 | JP5NOTSV    | LSVNOTSV  | LSVNOTSV  | JP5NOTSV    
    
  };

  // A routine that returns a string given the type
  std::string getTagCatString(TagCat type);

  // A routine that returns a jetBin given a string
  TagCat getTagCat(std::string str);

}

#endif

