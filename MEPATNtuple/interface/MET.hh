////////////////////////////////////////////////////////////////////////////////
//
// MET
// ---
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is a generalized container for MET and all of its associated
// information. It inherets from TLorentzVector, as that is the most basic
// and required information for MET.
////////////////////////////////////////////////////////////////////////////////

#ifndef MET_HH
#define MET_HH

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class MET : public PhysicsObject {
public:
   //
   // Construction/Destruction
   //
   MET(double x=0.0, double y=0.0, double z=0.0, double t=0.0);
   ~MET();

   //
   // Special Copy Constructor and Assignment Operator
   //
   MET(const TLorentzVector& rhs);
   virtual PhysicsObject & operator=(TLorentzVector& rhs);
   MET & operator=(TLorentzVector& rhs) const;

   ClassDef(MET,1)
};

#endif
