#ifndef TTEVENTPROB3JET_HH
#define TTEVENTPROB3JET_HH

#include "EventProb3jet.hh"
#include "topEventProb.hh"

class ttEventProb3Jet : public EventProb3Jet, public topEventProb
{
   public:
      ttEventProb3Jet(Integrator& integrator, const TransferFunction& bTF,
                      const TransferFunction& lightTF);
      virtual unsigned getProbMax() const;

   protected:
      virtual void changeVars(const std::vector<double>& parameters);
      virtual void met();
      virtual void getTotalLV(TLorentzVector& vec) const;
      virtual double matrixElement() const;
      virtual double phaseSpace() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual bool onSwitch();
      virtual void setTopMassAndWidth(double mTop);

   private:
      TLorentzVector m_lostJet;
};

#endif
