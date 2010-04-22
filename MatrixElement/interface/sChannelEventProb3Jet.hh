#ifndef SCHANNELEVENTPROB3JET_HH
#define SCHANNELEVENTPROB3JET_HH

#include "EventProb3jet.hh"
#include "topEventProb.hh"

class sChannelEventProb3Jet : public EventProb3Jet, public topEventProb
{
   public:
      sChannelEventProb3Jet(Integrator& integrator, 
                            const TransferFunction& bTF,
                            const TransferFunction& gluonTF);

   protected:
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual void setTopMassAndWidth(double mTop);
};

#endif
