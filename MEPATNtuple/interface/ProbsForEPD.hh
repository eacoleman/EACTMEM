#ifndef PROBSFOREPD_HH
#define PROBSFOREPD_HH

#include <string>

// Class for EPD is used in the EPD computation and contains as many members  
// as elements in DEFS::EP::type and in the same order. Basically each 
// matrix element probability has a member in this group.
class ProbsForEPD {
public:
  // Default C'tor
  ProbsForEPD();

  // The set with initial value.
  ProbsForEPD(double i);

  // Make sure we have a quick assignment c'tor
  ProbsForEPD(double , double , double ,
	      double , double , double ,
	      double , double , double ,
	      double );
 
  
  // The multiplication operator
  ProbsForEPD operator * (const ProbsForEPD & ) const;
  
  // The multiplication assignement
  ProbsForEPD& operator *=(const ProbsForEPD & );

  // The members of the 
  double wh     ; // for whatever mass we want
  double hww    ; // for whatever mass we want
  double stop   ;
  double twchan ;
  //  double tchan2 ;
  double tt     ;
  double wjets ; 
  double zjets ;
  //  double wbb    ;
  //  double wc     ;
  //  double wgg    ;
  double ww     ;
  double wz     ;
  //  double zz     ;
  double qcd    ;

  // This just print all the elements to screen
  void show(std::string);

  // This just prints how to call the 
  std::string getStringConstructor();

  // This divides each member to the sum of all members
  void normalize();

};

#endif
