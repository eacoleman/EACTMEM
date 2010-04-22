#ifndef WHEVENTPROB2JET_HH
#define WHEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "HiggsEventProb.hh"

class WHEventProb2Jet : public EventProb2Jet, public HiggsEventProb
{
   public:
      WHEventProb2Jet(Integrator& integrator, const TransferFunction& tf,
                      double higgsMass);
      virtual void setupIntegral();
      virtual void getPeaks(VecVecDouble& answer, const double bounds[]) const;

      virtual void setHiggsMassAndWidth(double mHiggs);

   protected:
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;

};

#endif
