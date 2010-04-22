#ifndef WHEVENTPROB3JET_HH
#define WHEVENTPROB3JET_HH

#include "EventProb3jet.hh"
#include "HiggsEventProb.hh"

class WHEventProb3Jet : public EventProb3Jet, public HiggsEventProb 
{
   public:
      WHEventProb3Jet(Integrator& integrator, const TransferFunction& bTF,
                      const TransferFunction& lightTF, double higgsMass);


      ~WHEventProb3Jet() {}
      virtual unsigned getProbMax() const;
      virtual void setupIntegral();
      virtual void setHiggsMassAndWidth(double mHiggs);

   protected:
      virtual double matrixElement() const;
      virtual void   setQuarkIDs() const;
      virtual void   getScale(double& scale1, double& scale2) const;


};

#endif
