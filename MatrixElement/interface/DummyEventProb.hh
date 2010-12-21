#ifndef DUMMYEVENTPROB_HH
#define DUMMYEVENTPROB_HH

#include <complex>
#include <string>
#include <vector>

#include <TLorentzVector.h>

#include "EventProb.hh"

class DummyEventProb : public EventProb
{
   public:
      DummyEventProb(std::string name, Integrator& integrator);

      virtual ~DummyEventProb() {}

      virtual void getPeaks(VecVecDouble& , const double []) const{};

      virtual void setDynamicBounds() {};

   protected:
      virtual void changeVars(const std::vector<double>&){};

      virtual double matrixElement() const { return 0;};

      virtual void setPartonTypes() const {}

      virtual void getScale(double& scale1, double& scale2) const {scale1 = scale2 = 0 ;}

      virtual double totalTF() const {return 0;}

      virtual bool onSwitch();


};

#endif
