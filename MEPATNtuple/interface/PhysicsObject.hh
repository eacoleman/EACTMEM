////////////////////////////////////////////////////////////////////////////////
//
// PhysicsObject
// -------------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is a generalized container for a physics object (ex: jets, 
// leptons, etc.). It inherets from TLorentzVector, as that is the most basic
// and required information for a physics object. This is a base class for more
// specific types of objects.
////////////////////////////////////////////////////////////////////////////////

#ifndef PHYSICSOBJECT_HH
#define PHYSICSOBJECT_HH

//
// User Defined Includes
//
#include "TAMUWW/SpecialTools/interface/Defs.hh"

//
// ROOT includes
//
#include "TObject.h"
#include "TLorentzVector.h"
#include "TMath.h"

//
// Standard Library Includes
//
#include <vector>

//
// Namespace
//
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class PhysicsObject : public TLorentzVector {
public:
   //
   // Construction/Destruction
   //
   PhysicsObject(double x=0.0, double y=0.0, double z=0.0, double t=0.0);
   virtual ~PhysicsObject();

   //
   // Special Copy Constructor and Assignment Operator
   //
   PhysicsObject(const TLorentzVector& rhs);
   virtual PhysicsObject & operator=(const TLorentzVector& rhs) {return *this;}

   TLorentzVector refLV, rawLV;
   int refpdgId;

   ClassDef(PhysicsObject,1)
};

#endif
