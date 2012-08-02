#ifndef WZEVENTPROB2JET_HH
#define WZEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WZEventProb2Jet : public EventProb2Jet
{
   public:
      WZEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      bool swapPartonMom;
      bool swapQuarkFlavor;
      double alphas_process;
};

#endif
