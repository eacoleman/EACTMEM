#ifndef HWWEVENTPROB2JET_HH
#define HWWEVENTPROB2JET_HH

//This code libraries
#include "TAMUWW/MatrixElement/interface/EventProb2jet.hh"
#include "TAMUWW/MatrixElement/interface/topEventProb.hh"
#include "TAMUWW/MatrixElement/interface/HiggsEventProb.hh"
#include "TAMUWW/MEPATNtuple/interface/MicroNtuple.hh"

// C++ libraries
#include <vector>

class EPDEventProb2Jet : public EventProb2Jet, 
			 public topEventProb, 
			 public HiggsEventProb{
   public:
      EPDEventProb2Jet(Integrator & integrator,
		       std::vector<EventProb*> eventProbs,
		       const TransferFunction  & tf,
		       double topM, double higgsM);

      virtual double execute(const double parameters[]);

      virtual void setupIntegral();
      virtual void setTopMassAndWidth(double mTop) ;
      virtual void setHiggsMassAndWidth(double mHiggs) ;


   protected:
      virtual void changeVars(const std::vector<double>& parameters);
      double  computeEPD(const double parameters[]) const;
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual bool onSwitch();
      double evaluateEPDforthisME(EventProb * evtP) const ;

   private:
      // The vector containing all the probabilities the computation
      // of the EPD requires 
      std::vector<EventProb*> eventProbs;

      // A copy of the actual measured collection, to be 
      // distributed to all ME's.
      static PartonColl m_measuredColl;

      double bProb[2];
     
      // This returns the volume in units of GeV, properly corrected for 
      // the jacobian of the transformation in changeVars
      double getCorrectedVolume();

      // This is an auxiliary array that is used in the computeEPD method
      static double evtMEProb[MicroNtuple::nEventProb];

};

#endif
