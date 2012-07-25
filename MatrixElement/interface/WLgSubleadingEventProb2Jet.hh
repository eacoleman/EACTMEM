#ifndef WLGSUBLEADINGEVENTPROB2JET_HH
#define WLGSUBLEADINGEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WLgSubleadingEventProb2Jet : public EventProb2Jet
{
   public:
      WLgSubleadingEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf, const TransferFunction& gluontf);
      virtual void setDynamicBounds();

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
      virtual double totalTF() const;

   private:
      const TransferFunction& m_gluonTF;
      bool swapPartonMom;
      double alphas_process;

};

#endif
