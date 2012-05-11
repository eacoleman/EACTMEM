#ifndef STOPTWEVENTPROB2JET_HH
#define STOPTWEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "topEventProb.hh"

class STopTWEventProb2Jet : public EventProb2Jet, public topEventProb
{
   public:
      STopTWEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf);
  //virtual void setDynamicBounds();

   protected:
      virtual void changeVars(const std::vector<double>& parameters);
      virtual void met();
      virtual void getTotalLV(TLorentzVector& vec) const;
      virtual double matrixElement() const;
      virtual double phaseSpace() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual bool onSwitch();
  //virtual double totalTF() const;
      virtual void setTopMassAndWidth(double mTop);
      virtual void setupIntegral();

   private:
  //      const TransferFunction& m_bTF;
      bool decTWjj;
      bool swapPartonMom;
      double alphas_process;
      TLorentzVector m_b;

};

#endif
