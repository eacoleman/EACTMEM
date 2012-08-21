////////////////////////////////////////////////////////////////////////////////
//
// Lepton
// ------
//
//                         08/02/2012 Alexx Perloff  <aperloff@physics.tamu.edu>
////////////////////////////////////////////////////////////////////////////////
// This class is a generalized container for a lepton  and all of its associated
// information. It inherets from TLorentzVector, as that is the most basic
// and required information for a lepton.
////////////////////////////////////////////////////////////////////////////////

#ifndef LEPTON_HH
#define LEPTON_HH

//
// User Defined Includes
//
#include "TAMUWW/MEPATNtuple/interface/PhysicsObject.hh"

////////////////////////////////////////////////////////////////////////////////
// class definition
////////////////////////////////////////////////////////////////////////////////
class Lepton : public PhysicsObject {
public:
   //
   // Construction/Destruction
   //
   Lepton(double x=0.0, double y=0.0, double z=0.0, double t=0.0);
   ~Lepton();

   //
   // Special Copy Constructor and Assignment Operator
   //
   Lepton(const TLorentzVector& rhs);
   virtual PhysicsObject & operator=(TLorentzVector& rhs);
   Lepton & operator=(TLorentzVector& rhs) const;

   int lQ, ldetComp;
   DEFS::LeptonCat leptonCat, leptonCat_passAll;
   double lTotIso, lecalIso, lhcalIso, ltrkIso, lrelIso, Thetalj1pj2;

   // Other Lepton Selection Variables
   double ldz, ldB;

   // Other Muon Selection Variables
   bool mIsGlobal, mIsPF, mIsTracker;
   double mNormalizedChi2, mNumberOfValidMuonHits, mNumberValidHits, mNumberOfPixelLayersWithMeasurement, mNumberOfMatches;

   // Other Electron Selection Variables
   bool eIsEB, eIsEE;
   double eSuperClusterEta, eEcalEnergy, eSigmaIetaIeta, eDeltaPhi, eDeltaEta, eHadronicOverEm, eConvDist, eConvDcot, eNumberOfHits;
   
   ClassDef(Lepton,1)
};

#endif
