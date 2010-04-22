#include "TAMUWW/Integrator/interface/Integrator.hh"

#include <iostream>

#include <stdexcept>
#include <string>

Integrator::Integrator(std::string name) :
   m_name(name),
   m_ndim(0),
   m_ncomp(1),
   m_integrand(0),
   m_epsrel(0),
   m_epsabs(0),
   m_flags(13),
   m_mineval(0),
   m_maxeval(100000)
{}

NullIntegrator::NullIntegrator() :
   Integrator("Null integrator")
{}

void NullIntegrator::doIntegral(double returnVal[], double[], int*, int*,
                                double[]) const
{
   int dim = getNdim();
   int comp = getNcomp();
   int* dimPtr = &dim;
   int* compPtr = &comp;
   double* params = new double[dim];

   if (static_cast<int>(m_params.size()) != getNdim())
      throw std::runtime_error("Parameters not set in NullIntegrator!");
   for (int i = 0; i < dim; ++i)
   {
      params[i] = m_params[i];
   }

   std::cerr << "Params[0] : " << params[0] << std::endl;
   getIntegrand()(dimPtr, params, compPtr, returnVal);
}

void NullIntegrator::setParam(unsigned param, double value)
{
   if (static_cast<int>(m_params.size()) != getNdim())
      m_params.resize(getNdim());

   if (static_cast<int>(param) >= getNdim())
      throw std::runtime_error("Invalid setting in NullIntegrator::setParam!");

   m_params[param] = value;
}
