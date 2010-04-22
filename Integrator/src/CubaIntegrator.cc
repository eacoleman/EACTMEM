#include "TAMUWW/Integrator/interface/CubaIntegrator.hh"

#include <iostream>

#include <stdexcept>

using std::vector;

namespace CubaInt
{
   Vegas::Vegas() :
      Integrator("VEGAS integrator from CUBA"),
      m_start(10),
      m_increase(10)
   {
//      Cuba::vegasgridno_ = 1;
   }

   void Vegas::doIntegral(double returnVal[], double error[], int* fail,
                          int* neval, double prob[]) const
   {
      Cuba::Vegas(getNdim(), getNcomp(), getIntegrand(), getEpsilonRel(),
                  getEpsilonAbs(), getFlags(), getMinEval(), getMaxEval(),
                  m_start, m_increase, neval, fail, returnVal, error, prob);
   }

   Suave::Suave() :
      Integrator("SUAVE integrator from CUBA"),
      m_new(10),
      m_flatness(1)
   {}

   void Suave::doIntegral(double returnVal[], double error[], int* fail,
                          int* neval, double prob[]) const
   {
      int dummy = 0;
      int* dummyPtr = &dummy;
      Cuba::Suave(getNdim(), getNcomp(), getIntegrand(), getEpsilonRel(),
                  getEpsilonAbs(), getFlags(), getMinEval(), getMaxEval(), m_new,
                  m_flatness, dummyPtr, neval, fail, returnVal, error, prob);
   }

   Divonne::Divonne() :
      Integrator("DIVONNE integrator from CUBA"),
      m_key1(10),
      m_key2(10),
      m_key3(1),
      m_maxpass(100),
      m_border(0),
      m_chi2(10),
      m_minDev(.1),
      m_peakFinder(0)
   {
      m_points = new std::vector<VecDouble>();  
   }

   void Divonne::doIntegral(double returnVal[], double error[], int* fail,
                            int* neval, double prob[]) const
   {
//      double dummy[1] = {0};
      int dummy2 = 0;
      int* dummy2Ptr = &dummy2;

      unsigned size = m_points->size() * getNdim();
//      unsigned size = 0;
      double* pointList = size ? new double[size] : 0;
      unsigned counter = 0;
      for (vector<vector<double> >::const_iterator it = m_points->begin();
           it != m_points->end(); ++it)
      {
         for (vector<double>::const_iterator it2 = it->begin();
              it2 != it->end(); ++it2)
         {
            pointList[counter++] = *it2;
         }
      }

      int maxPeaks = m_peakFinder ? m_maxPeaks : 0;
//      std::cerr << "maxPeaks: " << maxPeaks << std::endl;

      Cuba::Divonne(getNdim(), getNcomp(), getIntegrand(), getEpsilonRel(),
                    getEpsilonAbs(), getFlags(), getMinEval(), getMaxEval(),
                    m_key1, m_key2, m_key3, m_maxpass, m_border, m_chi2,
                    m_minDev, m_points->size(), getNdim(), pointList,
                    maxPeaks, m_peakFinder,
//                    m_minDev, 0, getNdim(), pointList, 0, 0,
                    dummy2Ptr, neval, fail, returnVal, error, prob);

      delete[] pointList;
   }

   void Divonne::addPoint(vector<double> point)
   {
      if (point.size() != static_cast<unsigned>(getNdim()))
         throw std::invalid_argument("CubaInt::Divonne::addPoint: Size of vector does not match dimension!");

      m_points->push_back(point);
   }

   Cuhre::Cuhre() :
      Integrator("CUHRE integrator from CUBA"),
      m_key(9)
   {}

   void Cuhre::doIntegral(double returnVal[], double error[], int* fail,
                          int* neval, double prob[]) const
   {
      int dummy = 0;
      int* dummyPtr = &dummy;
               
      Cuba::Cuhre(getNdim(), getNcomp(), getIntegrand(), getEpsilonRel(),
                  getEpsilonAbs(), getFlags(), getMinEval(), getMaxEval(),
                  m_key, dummyPtr, neval, fail, returnVal, error, prob);
   }
}
