#ifndef WCEVENTPROB2JET_HH
#define WCEVENTPROB2JET_HH

#include "EventProb2jet.hh"

class WcEventProb2Jet : public EventProb2Jet
{
   public:
      WcEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf,
                      const TransferFunction& gluontf);
      virtual unsigned getProbMax() const;
      virtual void setDynamicBounds();

   protected:
      WcEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf,
                      const TransferFunction& gluontf,
                      const std::string& name, unsigned iterations);

      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setQuarkIDs() const;
      virtual void getScale(double& scale1, double& scale2) const;
      virtual double totalTF() const;
      virtual bool onSwitch();

   private:
      const TransferFunction& m_gluonTF;
      
};

class WjgEventProb2Jet : public WcEventProb2Jet
{
   public:
      WjgEventProb2Jet(Integrator& integrator, const TransferFunction& lighttf,
                       const TransferFunction& gluontf);
      virtual unsigned getProbMax() const;

   protected:
      virtual void changeVars(const std::vector<double>& parameters);
      virtual void setQuarkIDs() const;
};

#endif
