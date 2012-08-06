////////////////////////////////////////////////////////////////////////////////
//
// Jet
// ---
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/Jet.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Jet::Jet(double x, double y, double z, double t) 
   : PhysicsObject(x,y,z,t) {
   matchedpdgId = 0;
   matchedDeltaR = 0;
   jBtagSSV = 0; jBtagTC = 0;
   jBtagCSV = 0;
   jBtagCSVMVA = 0;
   jBtagDiscriminatorSSV = 0;
   jBtagDiscriminatorTC = 0;
   jBtagDiscriminatorCSV = 0;
   jBtagDiscriminatorCSVMVA = 0;
   DRlj = 0;
   jPtD = 0;
   jNeutralMultiplicity = 0;
   jChargedMultiplicity = 0;
   jNeutralHadronEnergyFraction = 0;
   jNeutralEmEnergyFraction = 0;
   jChargedHadronEnergyFraction = 0;
   jChargedEmEnergyFraction = 0;
   jNumberOfDaughters = 0;
}

//______________________________________________________________________________
Jet::~Jet() {}

////////////////////////////////////////////////////////////////////////////////
// copy/assignment
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Jet::Jet(const TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),rhs.E());
}

//______________________________________________________________________________
PhysicsObject & Jet::operator=(TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),rhs.E());
   return *this;
}

//______________________________________________________________________________
Jet & Jet::operator=(TLorentzVector& rhs) const {
   Jet res = *this;
   res = rhs;
   return res;
}

ClassImp(Jet)
