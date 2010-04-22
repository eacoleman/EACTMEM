#ifndef INTEGRATOR_HH
#define INTEGRATOR_HH

#include <string>
#include <vector>

namespace Cuba
{
#include "cuba.h"
}

class Integrator
{
   public:
      Integrator(std::string name);
      virtual ~Integrator() {}

      virtual void doIntegral(double returnVal[], double error[], int* fail,
                              int* neval, double prob[]) const = 0;

      void setNdim(int input) {m_ndim = input;}
      void setNcomp(int input) {m_ncomp = input;}
      void setIntegrand(Cuba::integrand_t input) {m_integrand = input;}
      void setEpsilon(double relative, double absolute)
      {m_epsrel = relative; m_epsabs = absolute;}
      inline void setVerbose(int input);
      inline void setSampleSet(bool input);
      inline void setPseudoRandom(bool input);
      void setNeval(int min, int max) {m_mineval = min; m_maxeval = max;}

      std::string getName() const {return m_name;}
      int getNdim() const {return m_ndim;}
      int getNcomp() const {return m_ncomp;}
      Cuba::integrand_t getIntegrand() const {return m_integrand;}
      double getEpsilonRel() const {return m_epsrel;}
      double getEpsilonAbs() const {return m_epsabs;}
      int getMinEval() const {return m_mineval;}
      int getMaxEval() const {return m_maxeval;}

//   protected:
      int getFlags() const {return m_flags;}

   private:
      std::string m_name;

      int m_ndim;
      int m_ncomp;
      Cuba::integrand_t m_integrand;
      double m_epsrel;
      double m_epsabs;
      int m_flags;
      int m_mineval;
      int m_maxeval;
};

void Integrator::setVerbose(int input) // 0011 mask
{
   m_flags &= 8;
   m_flags |= input & 3;
}

void Integrator::setSampleSet(bool input) // 0100 mask
{
   m_flags &= 11;
   m_flags |= (input << 2) & 4;
}

void Integrator::setPseudoRandom(bool input)  // 1000 mask
{
   m_flags &= 7;
   m_flags |= (input << 3) & 8;
}


class NullIntegrator : public Integrator
{
   public:
      NullIntegrator();

      virtual ~NullIntegrator() {}

      virtual void doIntegral(double returnVal[], double error[], int* fail,
                              int* neval, double prob[]) const;

      void setParam(unsigned param, double value);

   private:
      std::vector<double> m_params;
};

#endif
