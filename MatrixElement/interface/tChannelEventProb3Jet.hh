#ifndef TCHANNELEVENTPROB3JET_HH
#define TCHANNELEVENTPROB3JET_HH

#include "EventProb3jet.hh"
#include "topEventProb.hh"

class tChannelEventProb3Jet : public EventProb3Jet, public topEventProb
{
   public:
      tChannelEventProb3Jet(Integrator& integrator, 
                            const TransferFunction& bTF,
                            const TransferFunction& lightTF);

   protected:
      tChannelEventProb3Jet(Integrator& integrator, 
                            const TransferFunction& bTF,
                            const TransferFunction& lightTF,
                            const DEFS::EP::Type & ept);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual void makeFortranArray(double array[][4]) const;
      virtual void setTopMassAndWidth(double mTop);
};

class tChannelEventProb3JetAlt : public tChannelEventProb3Jet
{
   public:
      tChannelEventProb3JetAlt(Integrator& integrator, 
                               const TransferFunction& bTF,
                               const TransferFunction& lightTF);

   protected:
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
};

#endif
