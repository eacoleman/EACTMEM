////////////////////////////////////////////////////////////////////////////////
//
// Jet
// ---
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is a generalized container for a jet and all of its associated
// information. It inherets from TLorentzVector, as that is the most basic
// and required information for a jet.
////////////////////////////////////////////////////////////////////////////////

#ifndef JET_HH
#define JET_HH

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class Jet : public PhysicsObject {
public:
   //
   // Construction/Destruction
   //
   Jet(double x=0.0, double y=0.0, double z=0.0, double t=0.0);
   ~Jet();

   //
   // Special Copy Constructor and Assignment Operator
   //
   Jet(const TLorentzVector& rhs);
   virtual PhysicsObject & operator=(TLorentzVector& rhs);
   Jet & operator=(TLorentzVector& rhs) const;

   // Transfer Function Variables
   TLorentzVector matchedGenParticles;
   int matchedpdgId;
   double matchedDeltaR;

   // B-tag Variables
   int jBtagSSV, jBtagTC, jBtagCSV, jBtagCSVMVA;
   double jBtagDiscriminatorSSV, jBtagDiscriminatorTC, jBtagDiscriminatorCSV, jBtagDiscriminatorCSVMVA;

   // Lepton Variables
   double DRlj;

   // QGLikelihood Variables
   double jPtD, jNeutralMultiplicity, jChargedMultiplicity;

   // Other Selection Variables
   double jNeutralHadronEnergyFraction, jNeutralEmEnergyFraction, jChargedHadronEnergyFraction, jChargedEmEnergyFraction;
   double jNumberOfDaughters;

  // to sort the Jets in order of decreasing Pt
  static bool sortInDecreasingPt( const Jet & j1, const Jet & j2){
    return j1.Pt() > j2.Pt();
  }

  
  ClassDef(Jet,1)
};

#endif
