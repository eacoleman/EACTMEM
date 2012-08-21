////////////////////////////////////////////////////////////////////////////////
//
// Vertex
// ------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/Vertex.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Vertex::Vertex(double x, double y, double z, double t) 
   : PhysicsObject(x,y,z,TMath::Power(x,2)+TMath::Power(y,2)+TMath::Power(z,2)) {
   vNDOF = 0;
   npv = 0;
}

//______________________________________________________________________________
Vertex::~Vertex() {}

////////////////////////////////////////////////////////////////////////////////
// copy/assignment
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Vertex::Vertex(const TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),
                              TMath::Power(rhs.Px(),2)+TMath::Power(rhs.Py(),2)+TMath::Power(rhs.Pz(),2));
}

//______________________________________________________________________________
PhysicsObject & Vertex::operator=(TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),
                              TMath::Power(rhs.Px(),2)+TMath::Power(rhs.Py(),2)+TMath::Power(rhs.Pz(),2));
   return *this;
}

//______________________________________________________________________________
Vertex & Vertex::operator=(TLorentzVector& rhs) const {
   Vertex res = *this;
   res = rhs;
   return res;
}

ClassImp(Vertex)
