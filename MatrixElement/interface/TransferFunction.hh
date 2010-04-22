#ifndef TRANSFERFUNCTION_HH
#define TRANSFERFUNCTION_HH

#include <string>
#include <vector>

#include "PartonColl.hh"

class DoubleGaussian
{
   public:
      DoubleGaussian(const std::string& dataFileName);

      double operator()(double partonE, double measuredE) const;

   private:
      std::vector<double> m_parameters;
};

class SingleGaussian
{
   public:
      SingleGaussian(const std::string& dataFileName);

      double operator()(double partonE, double measuredE) const;

   private:
      std::vector<double> m_parameters;
};

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


class NullTransferFunction : public TransferFunction
{
   public:
      NullTransferFunction();
      virtual ~NullTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;
};


class OldTransferFunction : public TransferFunction
{
   public:
      OldTransferFunction(const std::string& paramFile);
      virtual ~OldTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

   private:
      const DoubleGaussian m_params;
};


class NewTransferFunction : public TransferFunction
{
   public:
      NewTransferFunction(const std::string& paramFileCentral,
                          const std::string& paramFileCentralSumE,
                          const std::string& paramFileForward, float etaSplit);
      virtual ~NewTransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

   private:
      const DoubleGaussian m_centralE;
      const SingleGaussian m_centralSumE;
      const DoubleGaussian m_forward;

      const float m_etaSplit;
};
//ALL NN TF
class NN_TF_TransferFunction : public TransferFunction
{
   public:
      NN_TF_TransferFunction(const std::string& paramFile_NN_TF, const std::string& sample);
      virtual ~NN_TF_TransferFunction() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
			   const PartonColl::Jet& measuredJet) const;

   private:
      const DoubleGaussian m_NN_TF;
      const std::string m_sample;
};



class SimpleSmearing : public TransferFunction
{
   public:
      SimpleSmearing(double smear);
      virtual ~SimpleSmearing() {}

      virtual double getTF(const PartonColl::Jet& partonJet,
                           const PartonColl::Jet& measuredJet) const;

   private:
      double m_smear;
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

   private:
      const DoubleGaussian m_central07SLT;
      const DoubleGaussian m_central07noSLT;
      const DoubleGaussian m_central04SLT;
      const DoubleGaussian m_central04noSLT;
      const DoubleGaussian m_middle07;
      const DoubleGaussian m_middle04;
      const DoubleGaussian m_forward07;
      const DoubleGaussian m_forward04;
  //const SingleGaussian m_sumecentral;

      const float m_centralEtaBorder;
      const float m_forwardEtaBorder;
};

#endif
