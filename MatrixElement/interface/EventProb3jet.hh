#ifndef EVENTPROB3JET_HH
#define EVENTPROB3JET_HH

#include <string>
#include <vector>

#include <TLorentzVector.h>

#include "EventProb.hh"

class EventProb3Jet : public EventProb
{
   public:
  EventProb3Jet(DEFS::EP::Type ept, Integrator& integrator, unsigned nVars,
		unsigned nLoop, const TransferFunction& bTF,
		const TransferFunction& gluonTF);
      virtual ~EventProb3Jet() {}

      virtual void getPeaks(VecVecDouble& answer, const double bounds[]) const;

      virtual void setDynamicBounds();
      virtual unsigned getProbMax() const;

   protected:
      virtual void changeVars(const std::vector<double>&);
      virtual double totalTF() const;
      virtual bool onSwitch();
      virtual void makeFortranArray(double array[][4]) const;

      const TransferFunction& getBTF() const {return m_bTF;}
      const TransferFunction& getLightTF() const {return m_gluonTF;}

   private:
      const TransferFunction& m_bTF;
      const TransferFunction& m_gluonTF;
};

#endif
