#ifndef QCDEVENTPROB2JET_HH
#define QCDEVENTPROB2JET_HH

#include "EventProb2jet.hh"

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"


class QCDEventProb2Jet : public EventProb2Jet
{
   public:
      QCDEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      double alphas_process;
      static const int VSize=64;
      enum {vecSize = VSize};
      typedef SimpleArray<doublecomplex, VSize> OutputType;

};

#endif
