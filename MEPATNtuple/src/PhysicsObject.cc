////////////////////////////////////////////////////////////////////////////////
//
// PhysicsObject
// -------------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
PhysicsObject::PhysicsObject(double x, double y, double z, double t)
   : TLorentzVector(x,y,z,t) {
   refpdgId = 0;
}

//______________________________________________________________________________
PhysicsObject::~PhysicsObject() {}

////////////////////////////////////////////////////////////////////////////////
// copy constructor
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
PhysicsObject::PhysicsObject(const TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),rhs.E());
}

ClassImp(PhysicsObject)
