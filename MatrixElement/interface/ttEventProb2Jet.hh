#ifndef TTEVENTPROB2JET_HH
#define TTEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"

class ttEventProb2Jet : public EventProb2Jet, public topEventProb
{
   public:
      ttEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:

      virtual void changeVars(const std::vector<double>& parameters);
      virtual void met();
      virtual void getTotalLV(TLorentzVector& vec) const;
      virtual double matrixElement() const;
      virtual double phaseSpace() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual bool onSwitch();
      virtual void setTopMassAndWidth(double mTop);

   private:
      double alphas_process;
      // This is the lost parton which is supposed to be W
      TLorentzVector m_W;

};

#endif
