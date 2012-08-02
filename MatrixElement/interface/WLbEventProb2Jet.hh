#ifndef WLBEVENTPROB2JET_HH
#define WLBEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"

class WLbEventProb2Jet : public EventProb2Jet, public topEventProb
{
   public:
      WLbEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf, const TransferFunction& btf);
      virtual void setDynamicBounds();

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
      virtual double totalTF() const;
      virtual void setTopMassAndWidth(double mTop);
      virtual void setupIntegral();

   private:
      const TransferFunction& m_bTF;
      bool swapPartonMom;
      double alphas_process;

};

#endif
