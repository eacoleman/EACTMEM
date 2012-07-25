#ifndef TCHANNELEVENTPROB2JET_HH
#define TCHANNELEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"

class tChannelEventProb2Jet : public EventProb2Jet, public topEventProb
{

   public:
      tChannelEventProb2Jet(Integrator& integrator, 
                            const TransferFunction& btf,
			    const TransferFunction& lighttf);

      virtual void setDynamicBounds();
  //  virtual unsigned getProbMax() const;

   protected:
      virtual void changeVars(const std::vector<double>&);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
      virtual double totalTF() const;
      virtual bool onSwitch();
      virtual void setTopMassAndWidth(double mTop);
      virtual void setupIntegral();

   private:
      const TransferFunction& m_lightTF;
      // Mutable because of stupid FORTRAN requirements
      mutable std::complex<double> m_factor[2];
  //      const bool m_rh;
      bool swapPartonMom;
      double alphas_process;

};

#endif
