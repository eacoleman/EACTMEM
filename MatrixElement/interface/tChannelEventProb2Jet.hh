#ifndef TCHANNELEVENTPROB2JET_HH
#define TCHANNELEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"

class tChannelEventProb2Jet : public EventProb2Jet, public topEventProb
{
   // make the JetAlt a friend of this class so it can access the private variables 
   friend class tChannelEventProb2JetAlt;

   public:
      tChannelEventProb2Jet(Integrator& integrator, 
                            const TransferFunction& btf,
                            const TransferFunction& lighttf,
                            bool rh = false);

      virtual void setDynamicBounds();
      virtual unsigned getProbMax() const;

   protected:
      tChannelEventProb2Jet(Integrator& integrator, 
                            const TransferFunction& btf,
                            const TransferFunction& lighttf,
                            const std::string& name, bool rh = false);
      virtual void changeVars(const std::vector<double>&);
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual double totalTF() const;
      virtual bool onSwitch();
      virtual void setTopMassAndWidth(double mTop);
      virtual void setupIntegral();

   private:
      const TransferFunction& m_lightTF;
      // Mutable because of stupid FORTRAN requirements
      mutable std::complex<double> m_factor[2];
      const bool m_rh;
};

class tChannelEventProb2JetAlt : public tChannelEventProb2Jet
{
   public:
      tChannelEventProb2JetAlt(Integrator& integrator, 
                               const TransferFunction& btf,
                               const TransferFunction& lighttf,
                               bool rh = false);
      
   protected:
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void setupIntegral();

};

#endif
