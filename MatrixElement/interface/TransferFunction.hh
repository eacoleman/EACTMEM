#ifndef TRANSFERFUNCTION_HH
#define TRANSFERFUNCTION_HH

// C++ libraries
#include <string>
#include <vector>

// This code libraries
#include "PartonColl.hh"

// --------------------------------------------------------------------
// The transfer functions described in this file could have easily be done
// by using ROOT's TFormula class. However performance is of key importance
// for the calculation of the Matrix Element as the TF's are called hundred 
// of thousands of times per Matrix Element per event per permutation.
// The implementation used here allows for excellent performance with
// minimal overhead. Ricardo.
class SingleGaussian
{
   public:
      SingleGaussian(const std::string& dataFileName);

      double operator()(double partonE, double measuredE) const;

   private:
      std::vector<double> m_parameters;
};

class DoubleGaussian
{
   public:
  DoubleGaussian(const std::string& dataFileName, bool useTable = false);

      double operator()(double partonE, double measuredE) const;

   private:
       // the normalization factors for the minEj used in the analysis
       void computeNormalizationFactors(double minEj);
       double computeIntegral(double Ep, double minEj);

       std::vector<double> m_parameters;
       std::vector<double> m_parErrors;
       std::vector<double> m_norm;
  
};


// --------------------------------------------------------------------
// This is the abstract base class from which every Tranfer function 
// derives. The pure virtual function getTF(partonJet, measuredjet) 
// is the core of it. 
// The TransferFunctions satisfy the normalization condition that 
// for a given parton energy the integral over all measured energies is one.
// This stems from P(x)=Int P(y)P(x|y) dy where x is the set of measured
// quantities, y is the set of parton-level quantities and P(x|y) is the 
// tranfer function. 
// It is the user's responsibility to ensure that the integration of the 
// tranfer function over mesured energies x adds up to exactly 1 for all cases 
// of parton jet energy y.
class TransferFunction
{
   public:
      TransferFunction(const std::string& name);
      virtual ~TransferFunction() {}

      std::string getName() const {return m_name;}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const = 0;

      // This returns the maximum point of the TF and fills the bounds
      double getBounds(const PartonColl::Jet& measuredJet, float lowPercent,
                       float highPercent, double& lowBound,
                       double& highBound) const;

   private:
      const std::string m_name;
};

// --------------------------------------------------------------------
// The identity transfer function always returns 1.
class IdentityTransferFunction : public TransferFunction
{
   public:
      IdentityTransferFunction();
      virtual ~IdentityTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;
};

// --------------------------------------------------------------------
// The double gaussian transfer function. A single DG for all 
// jet types in all eta ranges. 
class DGTransferFunction : public TransferFunction
{
   public:
      DGTransferFunction(const std::string& paramFile);
      virtual ~DGTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

  // test 
  DoubleGaussian getDG(){ return m_params;}

   private:
      const DoubleGaussian m_params;
};


// --------------------------------------------------------------------
// The Neural Net transfer function takes advantage of the full jet
// information for the determination of the TF probability.
class NNTransferFunction : public TransferFunction
{
   public:
      NNTransferFunction(const std::string& paramFile_NN_TF, const std::string& sample);
      virtual ~NNTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
			   const PartonColl::Jet& measuredJet) const;

   private:
      const DoubleGaussian m_NNTF;
      const std::string m_sample;
};

// --------------------------------------------------------------------
// The RelWidthTransferFunction transfer function provides a 
// gaussian transfer function with a width relative to the parton energy 
// of "sigma". For example a value sigma=0.1 means that parton jets
// of 100 GeV will have a gaussian distribution of measured energies around 100 GeV 
// with a sigma of 10 Gev. The function is normalized taking into account the
// minimum cut energy imposed to the measured jet.
class RelWidthTransferFunction : public TransferFunction
{
   public:
      RelWidthTransferFunction(double sigma);
      virtual ~RelWidthTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

   private:
  double m_sigma;
  double m_norm;

};

class NewerTransferFunction : public TransferFunction
{
   public:
      struct ParamFiles
      {
            std::string central07SLT;
            std::string central07noSLT;
            std::string central04SLT;
            std::string central04noSLT;
            std::string middle07;
            std::string middle04;
            std::string forward07;
            std::string forward04;
	//std::string sumecentral;
      };

      NewerTransferFunction(const ParamFiles& files, float centralEtaBorder,
                            float forwardEtaBorder);
      virtual ~NewerTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

  // test 
  DoubleGaussian getDG(){ return m_central04noSLT;}

   private:
      const DoubleGaussian m_central07SLT;
      const DoubleGaussian m_central07noSLT;
      const DoubleGaussian m_central04SLT;
      const DoubleGaussian m_central04noSLT;
      const DoubleGaussian m_middle07;
      const DoubleGaussian m_middle04;
      const DoubleGaussian m_forward07;
      const DoubleGaussian m_forward04;

      const float m_centralEtaBorder;
      const float m_forwardEtaBorder;
};

#endif
