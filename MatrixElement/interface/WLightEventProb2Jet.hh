#ifndef WLIGHTEVENTPROB2JET_HH
#define WLIGHTEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WLightEventProb2Jet : public EventProb2Jet
{
   public:
      WLightEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      bool swapPartonMom;

};

#endif
