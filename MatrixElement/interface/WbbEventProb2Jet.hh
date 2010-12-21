#ifndef WBBEVENTPROB2JET_HH
#define WBBEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WbbEventProb2Jet : public EventProb2Jet
{
   public:
      WbbEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
};

#endif
