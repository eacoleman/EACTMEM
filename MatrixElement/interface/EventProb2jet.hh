#ifndef EVENTPROB2JET_HH
#define EVENTPROB2JET_HH

#include <complex>
#include <string>
#include <vector>

#include <TLorentzVector.h>

#include "EventProb.hh"

class TransferFunction;

class EventProb2Jet : public EventProb
{
   public:
      EventProb2Jet(std::string name, Integrator& integrator, unsigned nVars,
                    unsigned nLoop, const TransferFunction& tf);
      virtual ~EventProb2Jet() {}

      virtual void getPeaks(VecVecDouble& answer, const double bounds[]) const;

      virtual void setDynamicBounds();

   protected:
      virtual void changeVars(const std::vector<double>&);
      virtual double totalTF() const;
      void makeFortranArray(double array[][4]) const;

      const TransferFunction& getBTF() const {return m_tf;}

      virtual bool onSwitch();

   private:
      const TransferFunction& m_tf;
};

#endif
