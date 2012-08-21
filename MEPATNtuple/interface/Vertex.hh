////////////////////////////////////////////////////////////////////////////////
//
// Vertex
// ------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is a generalized container for a vertex  and all of its associated
// information. It inherets from TLorentzVector, only as a storage container for
// the z position (t = x^2+y^2+z^2 ==> mass = 0.0 GeV)
////////////////////////////////////////////////////////////////////////////////

#ifndef VERTEX_HH
#define VERTEX_HH

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class Vertex : public PhysicsObject {
public:
   //
   // Construction/Destruction
   //
   Vertex(double x=0.0, double y=0.0, double z=0.0, double t=0.0);
   ~Vertex();

   //
   // Special Copy Constructor and Assignment Operator
   //
   Vertex(const TLorentzVector& rhs);
   virtual PhysicsObject & operator=(TLorentzVector& rhs);
   Vertex & operator=(TLorentzVector& rhs) const;

   double vNDOF, npv;
   vector < float > tnpus;
   vector < int > npus;

   ClassDef(Vertex,1)
};

#endif
