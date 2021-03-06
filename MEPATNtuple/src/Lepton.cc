////////////////////////////////////////////////////////////////////////////////
//
// Lepton
// ------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/Lepton.hh"

////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Lepton::Lepton(double x, double y, double z, double t) 
   : PhysicsObject(x,y,z,t) {
   lQ = 0;
   ldetComp = 0;
   leptonCat=DEFS::none;
   leptonCat_passAll=DEFS::none;
   lTotIso = 0;
   lecalIso = 0;
   lhcalIso = 0;
   ltrkIso = 0;
   ldetIso = 0;
   lpfIso = 0;
   Thetalj1pj2 = 0;
   ldz = 0;
   ldB = 0;
   mIsGlobal = false;
   mIsPF = false;
   mIsTracker = false;
   mNormalizedChi2 = 0;
   mNumberOfValidMuonHits = 0;
   mNumberValidHits = 0;
   mNumberOfValidPixelHits = 0;
   mNumberOfMatchedStations = 0;
   ePassConversionVeto = false;
   eIsEB = false;
   eIsEE = false;
   emvaTrig = 0;
   emvaNonTrig = 0;
   eSuperClusterEta = 0;
   eEcalEnergy = 0;
   eESuperClusterOverP = 0;
   eSigmaIetaIeta = 0;
   eDeltaPhi = 0;
   eDeltaEta = 0;
   eHadronicOverEm = 0;
}

//______________________________________________________________________________
Lepton::~Lepton() {}

////////////////////////////////////////////////////////////////////////////////
// copy/assignment
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Lepton::Lepton(const TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),rhs.E());
}

//______________________________________________________________________________
PhysicsObject & Lepton::operator=(TLorentzVector& rhs) {
   TLorentzVector::SetPxPyPzE(rhs.Px(),rhs.Py(),rhs.Pz(),rhs.E());
   return *this;
}

//______________________________________________________________________________
Lepton & Lepton::operator=(TLorentzVector& rhs) const {
   Lepton res = *this;
   res = rhs;
   return res;
}

ClassImp(Lepton)
