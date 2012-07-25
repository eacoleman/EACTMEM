#ifndef SCHANNELEVENTPROB2JET_HH
#define SCHANNELEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"
#include "EPDEventProb2Jet.hh"

class sChannelEventProb2Jet : public EventProb2Jet, public topEventProb
{
   public:
      sChannelEventProb2Jet(Integrator& integrator,
                            const TransferFunction& tf);
  //, bool rh = false);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
      virtual void setTopMassAndWidth(double mTop);

   private:
      // Mutable because of stupid FORTRAN requirements
      mutable std::complex<double> m_factor[2];
  //      const bool m_rh;
      bool swapPartonMom;
      double alphas_process;

};

#endif
