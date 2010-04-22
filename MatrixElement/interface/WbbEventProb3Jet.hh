#ifndef WBBEVENTPROB3JET_HH
#define WBBEVENTPROB3JET_HH

#include "EventProb3jet.hh"

class WbbEventProb3Jet : public EventProb3Jet
{
   public:
      WbbEventProb3Jet(Integrator& integrator, const TransferFunction& bTF,
                       const TransferFunction& gluonTF);
      virtual unsigned getProbMax() const;

   protected:
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual bool onSwitch();
};

#endif
