#ifndef CUBAINTEGRATOR_HH
#define CUBAINTEGRATOR_HH

#include <iostream>

#include <vector>

#include "Integrator.hh"

namespace CubaInt
{
   class Vegas : public Integrator
   {
      public:
         Vegas();
         virtual ~Vegas() {}

         virtual void doIntegral(double returnVal[], double error[], int* fail,
                                 int* neval, double prob[]) const;

         void setParams(int start, int increase)
         {m_start = start; m_increase = increase;}

      private:
         int m_start;
         int m_increase;
   };


   class Suave : public Integrator
   {
      public:
         Suave();
         virtual ~Suave() {}

         virtual void doIntegral(double returnVal[], double error[], int* fail,
                                 int* neval, double prob[]) const;

         void setNew(int input) {m_new = input;}
         void setFlatness(double input) {m_flatness = input;}

      private:
         int m_new;
         double m_flatness;
   };


   class Divonne : public Integrator
   {
      public:
         Divonne();
         virtual ~Divonne() {}

         virtual void doIntegral(double returnVal[], double error[], int* fail,
                                 int* neval, double prob[]) const;

#ifndef __CINT__
         typedef void(*PeakFinder)(const int*, const double[], int*, double[]);
#endif

         void setPartitioningRule(int input) {m_key1 = input;}
         void setIntegrationRule(int input) {m_key2 = input;}
         void setRefinementRule(int input) {m_key3 = input;}
         void setMaxPass(int input) {m_maxpass = input;}
         void setBorder(double input) {m_border = input;}
         void setChi2(double input) {m_chi2 = input;}
         void setMinDev(double input) {m_minDev = input;}
         void setMaxPeaks(int input) {m_maxPeaks = input;}
#ifndef __CINT__
         void setPeakFinder(PeakFinder input) {m_peakFinder = input;}
#endif

         void addPoint(std::vector<double> point);

      private:
         int m_key1;
         int m_key2;
         int m_key3;
         int m_maxpass;
         double m_border;
         double m_chi2;
         double m_minDev;

         typedef std::vector<double> VecDouble;
         std::vector<VecDouble>* m_points;

         int m_maxPeaks;
#ifndef __CINT__
         PeakFinder m_peakFinder;
#endif
         // Why a pointer? Vectors are designed so you don't have to
         // muck around with pointers.
         // I'll tell you why: g++ sucks in some subtle fashion.
         // If I attempt to call the destructor on this vector,
         // the program will abort.
         // The only solution is to initialize the pointer in the constructor,
         // and then LET THE MEMORY LEAK because I can't call the stupid
         // destructor.
         // Tell me again, why are we using this compiler?
         // Can't we find one that at least implements std::vector right?
         // (Probably this is all my mistake.  But I can't figure out how.
         // I couldn't make that error message if I tried.)
   };


   class Cuhre : public Integrator
   {
      public:
         Cuhre();
         virtual ~Cuhre() {}

         virtual void doIntegral(double returnVal[], double error[], int* fail,
                                 int* neval, double prob[]) const;

         void setCubatureRule(int input) {m_key = input;}

      private:
         int m_key;
   };
}

#endif
