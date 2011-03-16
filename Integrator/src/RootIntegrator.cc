#include "TAMUWW/Integrator/interface/RootIntegrator.hh"

//#define USEOLD

#include <iostream>
#include <stdexcept>
#include <vector>

//#include <TF1.h>
#include <TMath.h>

#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

using std::vector;

//Cuba::integrand_t RootIntegrator::sm_integrand;
//int RootIntegrator::sm_dimension(0);

RootIntegrator::RootIntegrator() :
   Integrator("Root integrator")
{}

//void RootIntegrator::doIntegral(double returnVal[], double error[], int* fail,
//                                int* neval, double prob[]) const
//{
////   TF1 function("My function", sm_myFunction, 0, 1, 0);
//   TF1 function("sm_myFunction", sm_myFunction, -TMath::PiOver2(), TMath::PiOver2(), 3);
//   double* lower = new double[getNdim()];
//   double* upper = new double[getNdim()];
//
//   for (int i = 0; i < getNdim(); ++i)
//   {
//      lower[i] = 0;
//      upper[i] = 1;
//   }
//
//   std::cout <<
//
//  std::cout << function.IntegralMultiple(getNdim(), lower, upper,
//                                            getMinEval(), getMaxEval(),
//                                            getEpsilonRel(), error[0], *neval,
//                                     *fail) << std::endl;
//   std::cerr << "PROBABILITY: " << returnVal[0] << std::endl;
//
//   error[0] *= returnVal[0];
//   prob[0] = 0;
//
//   delete [] lower;
//   delete [] upper;
//}

//void RootIntegrator::setNdim(int input)
//{
//   Integrator::setNdim(input);
//   sm_dimension = input;
//}
//
//void RootIntegrator::setIntegrand(Cuba::integrand_t input)
//{
//   Integrator::setIntegrand(input);
//   sm_integrand = input;
//}

//Double_t RootIntegrator::sm_myFunction(Double_t* x, Double_t*)
//{
//   double response[1];
//
//   int dim = sm_dimension;
//   int comp = 1;
//
////   std::cerr << "Dim: " << dim << " comp: " << comp << " x: " << x[0] << " " << x[1] << " " << x[2] << std::endl;
//
//   sm_integrand(&dim, x, &comp, response);
//
//   std::cerr << "Response of function is: " << response[0] << std::endl;
//   return response[0];
//}


#ifndef USEOLD
void RootIntegrator::doIntegral(double returnVal[], double relerr[],
                                int* ifail, int* nfnevl, double prob[]) const
{
   using AuxFunctions::Math::pow;

   Cuba::integrand_t intg = getIntegrand();
   int n = getNdim();
   int comp = getNcomp();
   int* dimPtr = &n;
   int* compPtr = &comp;
   double resultArr[1] = {0};

   int minpts = getMinEval();
   int maxpts = getMaxEval();
   double eps = getEpsilonRel();

   const double xl2 = 0.358568582800318073;
   const double xl4 = 0.948683298050513796;
   const double xl5 = 0.688247201611685289;
   const double w2  = 980./6561;
   const double w4  = 200./19683;
   const double wp2 = 245./486;
   const double wp4 = 25./729;

   const double wn1[14] = {-0.193872885230909911, -0.555606360818980835,
                           -0.876695625666819078, -1.15714067977442459,
                           -1.39694152314179743, -1.59609815576893754,
                           -1.75461057765584494, -1.87247878880251983,
                           -1.94970278920896201, -1.98628257887517146,
                           -1.98221815780114818, -1.93750952598689219,
                           -1.85215668343240347, -1.72615963013768225};

   const double wn3[14] = {0.0518213686937966768, 0.0314992633236803330,
                           0.0111771579535639891, -0.00914494741655235473,
                           -0.0294670527866686986, -0.0497891581567850424,
                           -0.0701112635269013768, -0.0904333688970177241,
                           -0.110755474267134071, -0.131077579637250419,
                           -0.151399685007366752, -0.171721790377483099,
                           -0.192043895747599447, -0.212366001117715794};

   const double wn5[14] = {0.871183254585174982e-01, 0.435591627292587508e-01,
                           0.217795813646293754e-01, 0.108897906823146873e-01,
                           0.544489534115734364e-02, 0.272244767057867193e-02,
                           0.136122383528933596e-02, 0.680611917644667955e-03,
                           0.340305958822333977e-03, 0.170152979411166995e-03,
                           0.850764897055834977e-04, 0.425382448527917472e-04,
                           0.212691224263958736e-04, 0.106345612131979372e-04};

   const double wpn1[14] = {-1.33196159122085045, -2.29218106995884763,
                            -3.11522633744855959, -3.80109739368998611,
                            -4.34979423868312742, -4.76131687242798352,
                            -5.03566529492455417, -5.17283950617283939,
                            -5.17283950617283939, -5.03566529492455417,
                            -4.76131687242798352, -4.34979423868312742,
                            -3.80109739368998611, -3.11522633744855959};

   const double wpn3[14] = {0.0445816186556927292, -0.0240054869684499309,
                            -0.0925925925925925875, -0.161179698216735251,
                            -0.229766803840877915, -0.298353909465020564,
                            -0.366941015089163228, -0.435528120713305891,
                            -0.504115226337448555, -0.572702331961591218,
                            -0.641289437585733882, -0.709876543209876532,
                            -0.778463648834019195, -0.847050754458161859};

   double result = 0;

//   std::cerr << __LINE__ << " " << result << std::endl;
   double abserr = 0;
   *ifail = 3;
   *nfnevl = 0;
   relerr[0] = 0;
   if (n < 2 || n > 15)
      throw std::runtime_error("Improper dimension for RootIntegrator");

   int twondm = pow(2, n);
   int ifncls = 0;
   bool ldv = false;
   int irgnst = 2 * n + 3;
   int irlcls = twondm + 2 * n * (n + 1) + 1;
   int isbrgn = irgnst;
   int isbrgs = irgnst;

   if (minpts < 1) 
      minpts = irlcls;
   if (maxpts < minpts)
      maxpts = 10 * minpts;

// The original agorithm expected a working space array WK of length IWK
// with IWK Length ( >= (2N + 3) * (1 + MAXPTS/(2**N + 2N(N + 1) + 1))/2).
// Here, this array is allocated dynamically

   int iwk = irgnst * (1 + maxpts / irlcls) / 2;
   double* wk = new double[iwk + 10];

   double* ctr = new double[n];
   double* wth = new double[n];
   double* z = new double[n];
   double* wthl = new double[n];

   for (int j = 0; j < n; ++j)
   {
      ctr[j] = 0.5;
      wth[j] = 0.5;
   }

   int idvax0 = 0;

//L20:
   while (*ifail == 3)
   {
      double rgnvol = twondm;

      for (int j = 0; j < n; ++j)
      {
         rgnvol *= wth[j];
         z[j] = ctr[j];
      }

//      std::cerr << __LINE__ << std::endl;
      intg(dimPtr, z, compPtr, resultArr);
//   std::cerr << resultArr[0] << std::endl;
      double sum1 = resultArr[0]; //evaluate function

      double difmax = 0, sum2 = 0, sum3 = 0, f2, f3, dif;
      int idvaxn = 0;

      for (int j = 0; j < n; ++j)
      {
         z[j] = ctr[j] - xl2 * wth[j];
//         std::cerr << "z[" << j << "]: " << z[j] << std::endl;
         intg(dimPtr, z, compPtr, resultArr);
         f2 = resultArr[0];

         z[j] = ctr[j] + xl2 * wth[j];
//      std::cerr << __LINE__ << std::endl;
         intg(dimPtr, z, compPtr, resultArr);
         f2 += resultArr[0];

         wthl[j] = xl4 * wth[j];
         z[j] = ctr[j] - wthl[j];
//      std::cerr << __LINE__ << std::endl;
         intg(dimPtr, z, compPtr, resultArr);
         f3 = resultArr[0];

         z[j] = ctr[j] + wthl[j];
//      std::cerr << __LINE__ << std::endl;
         intg(dimPtr, z, compPtr, resultArr);
         f3 += resultArr[0];

         sum2 += f2;
         sum3 += f3;
         dif = TMath::Abs(7 * f2 - f3 - 12 * sum1);
         if (dif >= difmax)
         {
            difmax = dif;
            idvaxn = j + 1;
         }
         z[j] = ctr[j];
      }

      double sum4 = 0;
      for (int j = 1; j < n; ++j)
      {
         int j1 = j-1;
         for (int k = j; k < n; ++k)
         {
            for (int l = 0; l < 2; ++l)
            {
               wthl[j1] = -wthl[j1];
               z[j1]    = ctr[j1] + wthl[j1];
               for (int m = 0; m < 2; ++m)
               {
                  wthl[k] = -wthl[k];
                  z[k]    = ctr[k] + wthl[k];
                  intg(dimPtr, z, compPtr, resultArr);
                  sum4 += resultArr[0];
               }
            }
            z[k] = ctr[k];
         }
         z[j1] = ctr[j1];
      }

      double sum5 = 0;
      for (int j = 0; j < n; ++j)
      {
         wthl[j] = -xl5 * wth[j];
         z[j] = ctr[j] + wthl[j];
      }
//L90:
      bool repeat = true;
      while (repeat)
      {
         intg(dimPtr, z, compPtr, resultArr);
//   std::cerr << resultArr[0] << std::endl;
         sum5 += resultArr[0];
      
         repeat = false;
         for (int j = 0; j < n; ++j)
         {
            wthl[j] = -wthl[j];
            z[j] = ctr[j] + wthl[j];
            if (wthl[j] > 0)
            {
               repeat = true;
               break;
            }
         }
      }
      
//   std::cerr <<wn1[n-2] << " " << sum1 << " " << w2 << " " << sum2+wn3[n-2] << sum3+w4 << sum4+wn5[n-2] << sum5 << std::endl;
//   std::cerr << __LINE__ << " " << result << std::endl;

      double rgncmp  = rgnvol * (wpn1[n - 2] * sum1 + wp2 * sum2
                                 + wpn3[n - 2] * sum3 + wp4 * sum4);
      double rgnval  = wn1[n - 2] * sum1 + w2 * sum2 + wn3[n - 2] * sum3
         + w4 * sum4 + wn5[n - 2] * sum5;
      rgnval *= rgnvol;
      double rgnerr  = TMath::Abs(rgnval - rgncmp);
      result += rgnval;
//   std::cerr << "rgnval: " << rgnval << " " << result <<std::endl;
      abserr += rgnerr;
      ifncls += irlcls;
      double aresult = TMath::Abs(result);
//   std::cerr << __LINE__ << " " << result << std::endl;
//   exit(1);
      //if (result > 0 && aresult< 1e-100) {
      //   delete [] wk;
      //   ifail = 0;  //function is probably symmetric ==> integral is null: not an error
      //   return result;
      //}

      int isbtmp = 0;

      bool doThisLoop = true;
      if (ldv)
      {
//L110:
         while (1)
         {
            isbtmp = 2 * isbrgn;
            if (isbtmp > isbrgs)
            {
               doThisLoop = false;
               break;
            }
            if (isbtmp < isbrgs)
            {
               int isbtpp = isbtmp + irgnst;
               if (wk[isbtmp - 1] < wk[isbtpp - 1]) isbtmp = isbtpp;
            }
            if (rgnerr >= wk[isbtmp - 1])
            {
               doThisLoop = false;
               break;
            }

            for (int k = 0; k < irgnst; ++k)
            {
               wk[isbrgn - k - 1] = wk[isbtmp - k - 1];
            }
            isbrgn = isbtmp;
         }
      }
//L140:
      if (doThisLoop)
      {
         isbtmp = (isbrgn / (2 * irgnst)) * irgnst;
         while (isbtmp >= irgnst && rgnerr > wk[isbtmp - 1])
         {
            for (int k = 0; k < irgnst; ++k)
            {
               wk[isbrgn - k - 1] = wk[isbtmp - k - 1];
            }
            isbrgn = isbtmp;
            isbtmp = (isbrgn / (2 * irgnst)) * irgnst;
//      goto L140;
         }
      }
//L160:
      wk[isbrgn - 1] = rgnerr;
      wk[isbrgn - 2] = rgnval;
      wk[isbrgn - 3] = static_cast<double>(idvaxn);
      for (int j = 0; j < n; ++j)
      {
         isbtmp = isbrgn - 2 * j - 4;
         wk[isbtmp] = ctr[j];
         wk[isbtmp - 1] = wth[j];
      }
      if (ldv)
      {
         ldv = false;
         ctr[idvax0] += 2 * wth[idvax0];
         isbrgs += irgnst;
         isbrgn = isbrgs;
         continue;
//         goto L20;
      }
      relerr[0] = abserr/aresult;
//      if (relerr[0] < 1e-1 && aresult < 1e-20)
//         *ifail = 0;
//      if (relerr[0] < 1e-3 && aresult < 1e-10)
//         *ifail = 0;
//      if (relerr[0] < 1e-5 && aresult < 1e-5)
//         *ifail = 0;
      if (isbrgs + irgnst > iwk)
         *ifail = 2;
      if (ifncls + 2 * irlcls > maxpts)
      {
         if (sum1 == 0 && sum2 == 0 && sum3 == 0 && sum4 == 0 && sum5 == 0)
         {
            *ifail = 0;
            result = 0;
         }
         else
         {
            *ifail = 1;
         }
      }

//      std::cerr << "Rel error: " << relerr[0] << std::endl;
      if (relerr[0] < eps && ifncls >= minpts)
         *ifail = 0;
      if (*ifail == 3)
      {
         ldv = true;
         isbrgn  = irgnst;
         abserr -= wk[isbrgn - 1];
         result -= wk[isbrgn - 2];
         idvax0  = Int_t(wk[isbrgn - 3] - 1);
         for (int j = 0; j < n; ++j) 
         {
            isbtmp = isbrgn - 2 * j - 4;
            ctr[j] = wk[isbtmp];
            wth[j] = wk[isbtmp - 1];
         }
         wth[idvax0]  = 0.5 * wth[idvax0];
         ctr[idvax0] -= wth[idvax0];
//         goto L20;
      }
   }
//   std::cerr << "Result: " << result
//             << " wk[isbrgn-2] : " << wk[isbrgn - 2] << std::endl;

   delete [] wk;
   delete [] ctr;
   delete [] wth;
   delete [] z;
   delete [] wthl;

   *nfnevl = ifncls;       //number of function evaluations performed.
   returnVal[0] = result;         //an approximate value of the integral
   relerr[0] *= result;
}

#else
//void RootIntegrator::doIntegral(double returnVal[], double relerr[],
//                                int* ifail, int* nfnevl, double prob[]) const
//{
//   Cuba::integrand_t intg = getIntegrand();
//   int n = getNdim();
//   int comp = getNcomp();
//   int* dimPtr = &n;
//   int* compPtr = &comp;
//   double resultArr[1] = {0};
//
//   int minpts = getMinEval();
//   int maxpts = getMaxEval();
//   double eps = getEpsilonRel();
//
//   Double_t ctr[15], wth[15], wthl[15], z[15];
//
//   const Double_t xl2 = 0.358568582800318073;
//   const Double_t xl4 = 0.948683298050513796;
//   const Double_t xl5 = 0.688247201611685289;
//   const Double_t w2  = 980./6561;
//   const Double_t w4  = 200./19683;
//   const Double_t wp2 = 245./486;
//   const Double_t wp4 = 25./729;
//
//   Double_t wn1[14] = {     -0.193872885230909911, -0.555606360818980835,
//     -0.876695625666819078, -1.15714067977442459,  -1.39694152314179743,
//     -1.59609815576893754,  -1.75461057765584494,  -1.87247878880251983,
//     -1.94970278920896201,  -1.98628257887517146,  -1.98221815780114818,
//     -1.93750952598689219,  -1.85215668343240347,  -1.72615963013768225};
//
//   Double_t wn3[14] = {     0.0518213686937966768,  0.0314992633236803330,
//     0.0111771579535639891,-0.00914494741655235473,-0.0294670527866686986,
//    -0.0497891581567850424,-0.0701112635269013768, -0.0904333688970177241,
//    -0.110755474267134071, -0.131077579637250419,  -0.151399685007366752,
//    -0.171721790377483099, -0.192043895747599447,  -0.212366001117715794};
//
//   Double_t wn5[14] = {         0.871183254585174982e-01,  0.435591627292587508e-01,
//     0.217795813646293754e-01,  0.108897906823146873e-01,  0.544489534115734364e-02,
//     0.272244767057867193e-02,  0.136122383528933596e-02,  0.680611917644667955e-03,
//     0.340305958822333977e-03,  0.170152979411166995e-03,  0.850764897055834977e-04,
//     0.425382448527917472e-04,  0.212691224263958736e-04,  0.106345612131979372e-04};
//
//   Double_t wpn1[14] = {   -1.33196159122085045, -2.29218106995884763,
//     -3.11522633744855959, -3.80109739368998611, -4.34979423868312742,
//     -4.76131687242798352, -5.03566529492455417, -5.17283950617283939,
//     -5.17283950617283939, -5.03566529492455417, -4.76131687242798352,
//     -4.34979423868312742, -3.80109739368998611, -3.11522633744855959};
//
//   Double_t wpn3[14] = {     0.0445816186556927292, -0.0240054869684499309,
//    -0.0925925925925925875, -0.161179698216735251,  -0.229766803840877915,
//    -0.298353909465020564,  -0.366941015089163228,  -0.435528120713305891,
//    -0.504115226337448555,  -0.572702331961591218,  -0.641289437585733882,
//    -0.709876543209876532,  -0.778463648834019195,  -0.847050754458161859};
//
//   Double_t result = 0;
//
////   std::cerr << __LINE__ << " " << result << std::endl;
//   Double_t abserr = 0;
//   *ifail  = 3;
//   *nfnevl = 0;
//   relerr[0] = 0;
//   if (n < 2 || n > 15) return;
//
//   Double_t twondm = TMath::Power(2,n);
//   Int_t ifncls = 0;
//   Bool_t ldv   = kFALSE;
//   Int_t irgnst = 2*n+3;
//   Int_t irlcls = Int_t(twondm) +2*n*(n+1)+1;
//   Int_t isbrgn = irgnst;
//   Int_t isbrgs = irgnst;
//
//   if (minpts < 1)      minpts = irlcls;
//   if (maxpts < minpts) maxpts = 10*minpts;
//
//// The original agorithm expected a working space array WK of length IWK
//// with IWK Length ( >= (2N + 3) * (1 + MAXPTS/(2**N + 2N(N + 1) + 1))/2).
//// Here, this array is allocated dynamically
//
//   Int_t iwk = irgnst*(1 +maxpts/irlcls)/2;
//   Double_t *wk = new Double_t[iwk+10];
//   Int_t j;
//   for (j=0;j<n;j++) {
//      ctr[j] = 0.5;
//      wth[j] = 0.5;
//   }
//
//   Double_t rgnvol, sum1, sum2, sum3, sum4, sum5, difmax, f2, f3, dif, aresult;
//   Double_t rgncmp=0, rgnval, rgnerr;
//   Int_t j1, k, l, m, idvaxn=0, idvax0=0, isbtmp, isbtpp;
//
//L20:
//   rgnvol = twondm;
//   for (j=0;j<n;j++) {
//      rgnvol *= wth[j];
//      z[j]    = ctr[j];
//   }
//
//   intg(dimPtr, z, compPtr, resultArr);
////   std::cerr << resultArr[0] << std::endl;
//   sum1 = resultArr[0]; //evaluate function
//
//   difmax = 0;
//   sum2   = 0;
//   sum3   = 0;
//   for (j=0;j<n;j++) {
//      z[j]    = ctr[j] - xl2*wth[j];
//      intg(dimPtr, z, compPtr, resultArr);
//      f2 = resultArr[0];
//
//      z[j]    = ctr[j] + xl2*wth[j];
//      intg(dimPtr, z, compPtr, resultArr);
//      f2 += resultArr[0];
//
//      wthl[j] = xl4*wth[j];
//      z[j]    = ctr[j] - wthl[j];
//      intg(dimPtr, z, compPtr, resultArr);
//      f3 = resultArr[0];
//
//      z[j]    = ctr[j] + wthl[j];
//      intg(dimPtr, z, compPtr, resultArr);
//      f3 += resultArr[0];
//
//      sum2   += f2;
//      sum3   += f3;
//      dif     = TMath::Abs(7*f2-f3-12*sum1);
//      if (dif >= difmax) {
//         difmax=dif;
//         idvaxn=j+1;
//      }
//      z[j]    = ctr[j];
//   }
//
//   sum4 = 0;
//   for (j=1;j<n;j++) {
//      j1 = j-1;
//      for (k=j;k<n;k++) {
//         for (l=0;l<2;l++) {
//            wthl[j1] = -wthl[j1];
//            z[j1]    = ctr[j1] + wthl[j1];
//            for (m=0;m<2;m++) {
//               wthl[k] = -wthl[k];
//               z[k]    = ctr[k] + wthl[k];
//               intg(dimPtr, z, compPtr, resultArr);
//               sum4 += resultArr[0];
//            }
//         }
//         z[k] = ctr[k];
//      }
//      z[j1] = ctr[j1];
//   }
//
//   sum5 = 0;
//   for (j=0;j<n;j++) {
//      wthl[j] = -xl5*wth[j];
//      z[j] = ctr[j] + wthl[j];
//   }
//L90:
//   intg(dimPtr, z, compPtr, resultArr);
////   std::cerr << resultArr[0] << std::endl;
//   sum5 += resultArr[0];
//
//   for (j=0;j<n;j++) {
//      wthl[j] = -wthl[j];
//      z[j] = ctr[j] + wthl[j];
//      if (wthl[j] > 0) goto L90;
//   }
//
////   std::cerr <<wn1[n-2] << " " << sum1 << " " << w2 << " " << sum2+wn3[n-2] << sum3+w4 << sum4+wn5[n-2] << sum5 << std::endl;
////   std::cerr << __LINE__ << " " << result << std::endl;
//
//   rgncmp  = rgnvol*(wpn1[n-2]*sum1+wp2*sum2+wpn3[n-2]*sum3+wp4*sum4);
//   rgnval  = wn1[n-2]*sum1+w2*sum2+wn3[n-2]*sum3+w4*sum4+wn5[n-2]*sum5;
//   rgnval *= rgnvol;
//   rgnerr  = TMath::Abs(rgnval-rgncmp);
//   result += rgnval;
////   std::cerr << rgnval << " " << result <<std::endl;
//   abserr += rgnerr;
//   ifncls += irlcls;
//   aresult = TMath::Abs(result);
////   std::cerr << __LINE__ << " " << result << std::endl;
////   exit(1);
//   //if (result > 0 && aresult< 1e-100) {
//   //   delete [] wk;
//   //   ifail = 0;  //function is probably symmetric ==> integral is null: not an error
//   //   return result;
//   //}
//
//   if (ldv) {
//L110:
//      isbtmp = 2*isbrgn;
//      if (isbtmp > isbrgs) goto L160;
//      if (isbtmp < isbrgs) {
//         isbtpp = isbtmp + irgnst;
//         if (wk[isbtmp-1] < wk[isbtpp-1]) isbtmp = isbtpp;
//      }
//      if (rgnerr >= wk[isbtmp-1]) goto L160;
//      for (k=0;k<irgnst;k++) {
//         wk[isbrgn-k-1] = wk[isbtmp-k-1];
//      }
//      isbrgn = isbtmp;
//      goto L110;
//   }
//L140:
//   isbtmp = (isbrgn/(2*irgnst))*irgnst;
//   if (isbtmp >= irgnst && rgnerr > wk[isbtmp-1]) {
//      for (k=0;k<irgnst;k++) {
//         wk[isbrgn-k-1] = wk[isbtmp-k-1];
//      }
//      isbrgn = isbtmp;
//      goto L140;
//   }
//
//L160:
//   wk[isbrgn-1] = rgnerr;
//   wk[isbrgn-2] = rgnval;
//   wk[isbrgn-3] = Double_t(idvaxn);
//   for (j=0;j<n;j++) {
//      isbtmp = isbrgn-2*j-4;
//      wk[isbtmp]   = ctr[j];
//      wk[isbtmp-1] = wth[j];
//   }
//   if (ldv) {
//      ldv = kFALSE;
//      ctr[idvax0-1] += 2*wth[idvax0-1];
//      isbrgs += irgnst;
//      isbrgn  = isbrgs;
//      goto L20;
//   }
//   relerr[0] = abserr/aresult;
////   if (relerr[0] < 1e-1 && aresult < 1e-20) ifail = 0;
////   if (relerr[0] < 1e-3 && aresult < 1e-10) ifail = 0;
////   if (relerr[0] < 1e-5 && aresult < 1e-5)  ifail = 0;
//   if (isbrgs+irgnst > iwk) *ifail = 2;
//   if (ifncls+2*irlcls > maxpts) {
//      if (sum1==0 && sum2==0 && sum3==0 && sum4==0 && sum5==0){
//         *ifail = 0;
//         result = 0;
//      }
//      else
//         *ifail = 1;
//   }
//   if (relerr[0] < eps && ifncls >= minpts) *ifail = 0;
//   if (*ifail == 3) {
//      ldv = kTRUE;
//      isbrgn  = irgnst;
//      abserr -= wk[isbrgn-1];
//      result -= wk[isbrgn-2];
//      idvax0  = Int_t(wk[isbrgn-3]);
//      for (j=0;j<n;j++) {
//         isbtmp = isbrgn-2*j-4;
//         ctr[j] = wk[isbtmp];
//         wth[j] = wk[isbtmp-1];
//      }
//      wth[idvax0-1]  = 0.5*wth[idvax0-1];
//      ctr[idvax0-1] -= wth[idvax0-1];
//      goto L20;
//   }
//   std::cerr << "Result: " << result << " aresult: " << aresult
//             << " wk[isbrgn-2] : " << wk[isbrgn - 2] << std::endl;
//
//   delete [] wk;
//   *nfnevl = ifncls;       //number of function evaluations performed.
//   returnVal[0] = result;         //an approximate value of the integral
//   relerr[0] *= result;
//}

#endif
