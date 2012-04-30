#ifndef WZTOBBEVENTPROB2JET_HH
#define WZTOBBEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WZtobbEventProb2Jet : public EventProb2Jet
{
   public:
      WZtobbEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      bool swapPartonMom;
      double alphas_process;
};

#endif
