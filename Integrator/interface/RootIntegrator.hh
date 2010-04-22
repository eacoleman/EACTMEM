#ifndef ROOTINTEGRATOR_HH
#define ROOTINTEGRATOR_HH

#include "Integrator.hh"

//#include <Rtypes.h>

class RootIntegrator : public Integrator
{
   public:
      RootIntegrator();
      virtual ~RootIntegrator() {}

      virtual void doIntegral(double returnVal[], double error[], int* fail,
                              int* neval, double prob[]) const;

//      virtual void setNdim(int input);
//      virtual void setIntegrand(Cuba::integrand_t input);
      
   private:
      
//      static Double_t sm_myFunction(Double_t* x, Double_t* par);
//      static Cuba::integrand_t sm_integrand;
//      static int sm_dimension;
};

#endif
