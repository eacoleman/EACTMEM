#ifndef HWWEVENTPROB2JET_HH
#define HWWEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "HiggsEventProb.hh"

class HWWEventProb2Jet : public EventProb2Jet, public HiggsEventProb
{
   public:
      HWWEventProb2Jet(Integrator& integrator, const TransferFunction& tf,
		       double higgsMass);

      virtual void setupIntegral();
      virtual void setHiggsMassAndWidth(double mHiggs) ;

   protected:
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
};

#endif
