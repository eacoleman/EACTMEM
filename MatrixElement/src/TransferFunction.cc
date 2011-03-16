

#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"

#include <iostream>
using namespace std;



#include <cmath>
#include <stdexcept>

#include "TMath.h"

#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/DataFile.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh"

// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH100GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH105GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH110GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH115GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH120GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH125GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH130GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH135GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH140GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH145GeV_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WH150GeV_TF/n_i7_h12_o1.C"
// 
// 
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WCC_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/WBB_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/schan_TF/n_i7_h12_o1.C"
// //#include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/ttbar_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/ttbar_TF_improved/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/tchan_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/tchan_light_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/Wcg_charm_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/Wcg_gluon_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/diboson_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/Wgg_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/Wjg_light_TF/n_i7_h12_o1.C"
// #include "TAMUWW/MatrixElement/interface/../../build_TFs/neuralnetwork/code/rootsnns_v3.0/results/Wjg_gluon_TF/n_i7_h12_o1.C"
// 

using std::string;
using std::vector;

DoubleGaussian::DoubleGaussian(const string& dataFileName)
{
   InputDataFile data(dataFileName);
   std::cout<<"***MatrixElement/src/TransferFunction.cc (DoubleGaussian::DoubleGaussian(const string& dataFileName))***"<<std::endl;
   std::cout<<"Input TFs "<<dataFileName<<std::endl;
   for (unsigned i = 0; i < 10; ++i)
   {
      string param
         = AuxFunctions::concatString("transfer function parameter ",
                                        i);
      m_parameters.push_back(data.readFloat(param));
      std::cout<<" "<<param<<"  "<<m_parameters[i]<<std::endl;
   }
}

double DoubleGaussian::operator()(double partonE, double measuredE) const
{
   using AuxFunctions::Math::square;

   double difference = measuredE - partonE;
   double p01 = m_parameters[0] + m_parameters[1] * partonE;
   double p23 = m_parameters[2] + m_parameters[3] * partonE;
   double p45 = m_parameters[4] + m_parameters[5] * partonE;
   double p67 = m_parameters[6] + m_parameters[7] * partonE;
   double p89 = m_parameters[8] + m_parameters[9] * partonE;

//   std::cerr << "Parameters: ";
//   for (int i = 0; i < 10; ++i)
//      std::cerr << m_parameters[i] << " ";
//   std::cerr << std::endl;

   double fxy = std::exp(-.5 * square((difference - p01) / p23));
   fxy += p45 * std::exp(-.5 * square((difference - p67) / p89));
   fxy /= p23 + p45 * p89;
   fxy /= std::sqrt(TMath::TwoPi());

//   std::cerr << "TF result: " << fxy << std::endl;
//   exit(1);
   return fxy;
}

SingleGaussian::SingleGaussian(const string& dataFileName)
{
   InputDataFile data(dataFileName);
   for (unsigned i = 0; i < 4; ++i)
   {
      string param
         = AuxFunctions::concatString("transfer function parameter ",
                                        i);
      m_parameters.push_back(data.readFloat(param));
   }
}

double SingleGaussian::operator()(double partonE, double measuredE) const
{
   // Barbara will rewrite this part
   using AuxFunctions::Math::square;

   double difference = measuredE - partonE;
   double p01 = m_parameters[0] + m_parameters[1] * partonE;
   double p23 = m_parameters[2] + m_parameters[3] * partonE;

   double fxy = std::exp(-.5 * square((difference - p01) / p23));
   fxy /= p23 ;
   fxy /= std::sqrt(TMath::TwoPi());

   return fxy;
}


TransferFunction::TransferFunction(const string& name) :
   m_name(name)
{}

double TransferFunction::getBounds(const PartonColl::Jet& measuredJet,
                                   float lowPercent, float highPercent,
                                   double& lowBound, double& highBound) const
{
   using MEConstants::beamEnergy;

   double max = 0., maxE = 0.;

   for (int ienergy = 0; ienergy < beamEnergy; ++ienergy)
   {
      PartonColl::Jet partonJet(measuredJet);
      partonJet.lv.SetE(ienergy);
      partonJet.energy07 = ienergy;
      double value = getTF(partonJet, measuredJet);
//      std::cerr << ienergy << " " << value << std::endl;
      if (value > max)
      {
         max = value;
         maxE = ienergy;
//         std::cerr << "New max at " << max << std::endl;
      }
   }

   lowBound = 0 ;
   highBound = beamEnergy;

   for (int ienergy = 0; ienergy < maxE; ++ienergy)
   {
      PartonColl::Jet partonJet(measuredJet);
      partonJet.lv.SetE(ienergy);
      partonJet.energy07 = ienergy;
      double value = getTF(partonJet, measuredJet);

      if (value > lowPercent * max)
      {
         lowBound = ienergy == 0 ? 0 : ienergy - 1;
         break;
      }

   }

   // RE Change the lowbound minimum energy to at least one GeV, using 0 
   // results in cases of jets with zero energy and momentum, which in 
   // turn results in nan from the ME calculations.
   if (lowBound == 0) 
     lowBound = 1;

   for (int ienergy = static_cast<int>(beamEnergy);
        ienergy > maxE; --ienergy)
   {
      PartonColl::Jet partonJet(measuredJet);
      partonJet.lv.SetE(ienergy);
      partonJet.energy07 = ienergy;
      double value = getTF(partonJet, measuredJet);

         if (value > highPercent * max)
         {
            highBound = ienergy == beamEnergy ? beamEnergy : ienergy + 1;
            break;
         }
   }

   return maxE;
}

NullTransferFunction::NullTransferFunction() :
   TransferFunction("No transfer function")
{}

double NullTransferFunction::getTF(const PartonColl::Jet& partonJet,
                                   const PartonColl::Jet& measuredJet) const
{
   return 1;
}

OldTransferFunction::OldTransferFunction(const string& paramFile) :
   TransferFunction("Old-style transfer function"),
   m_params(paramFile)
{}

double OldTransferFunction::getTF(const PartonColl::Jet& partonJet,
                                  const PartonColl::Jet& measuredJet) const
{
   return m_params(partonJet.lv.E(), measuredJet.lv.E());
}


NewTransferFunction::NewTransferFunction(const std::string& paramFileCentral,
                                         const std::string& paramFileCentralSumE,
                                         const std::string& paramFileForward,
                                         float etaSplit) :
   TransferFunction("New transfer function"),
   m_centralE(paramFileCentral),
   m_centralSumE(paramFileCentralSumE),
   m_forward(paramFileForward),
   m_etaSplit(etaSplit)
{}

double NewTransferFunction::getTF(const PartonColl::Jet& partonJet,
                                  const PartonColl::Jet& measuredJet) const
{
   if (std::abs(measuredJet.lv.Eta()) > m_etaSplit)
   {
      return m_forward(partonJet.lv.E(), measuredJet.lv.E());
   }
   else
   {
      return m_centralE(partonJet.lv.E(), measuredJet.lv.E())
         * m_centralSumE(partonJet.lv.E(), measuredJet.sumE);
   }
}

//ALL NN TF

NN_TF_TransferFunction::NN_TF_TransferFunction(const std::string& paramFile_NN_TF,const std::string&  sample) :
  TransferFunction("NN transfer function "),
  m_NN_TF(paramFile_NN_TF),
  m_sample(sample)
{}


double NN_TF_TransferFunction::getTF(const PartonColl::Jet& partonJet,
				     const PartonColl::Jet& measuredJet) const
{
  double x[7] = {0.,0.,0.,0.,0.,0.,0.};
  double output = 0.;
  double energy07 = measuredJet.energy07;

  if(energy07 == -999.)  energy07 = measuredJet.lv.E() ;
  
  x[0] = measuredJet.lv.E();
  x[1] = measuredJet.lv.Eta();
  x[2] = measuredJet.sumE;
  x[3] = measuredJet.ptjet;
  x[4] = measuredJet.lv.Phi();
  x[5] = measuredJet.rawe;     
  x[6] = energy07;
  /*    
  if(m_sample == "WH100GeV")     snns_wh100(x, &output);	
  else if(m_sample == "WH105GeV")snns_wh105(x, &output);	
  else if(m_sample == "WH110GeV")snns_wh110(x, &output);	
  else if(m_sample == "WH115GeV")snns_wh115(x, &output);	
  else if(m_sample == "WH120GeV")snns_wh120(x, &output);	
  else if(m_sample == "WH125GeV")snns_wh125(x, &output);	
  else if(m_sample == "WH130GeV")snns_wh130(x, &output);	
  else if(m_sample == "WH135GeV")snns_wh135(x, &output);	
  else if(m_sample == "WH140GeV")snns_wh140(x, &output);	
  else if(m_sample == "WH145GeV")snns_wh145(x, &output);	
  else if(m_sample == "WH150GeV")snns_wh150(x, &output);	

  else if(m_sample == "WBB")        snns_wbb(x, &output);	
  else if(m_sample == "schan")      snns_schan(x, &output);	
  else if(m_sample == "WCC")        snns_wcc(x, &output);	
  else if(m_sample == "Wcg_charm")  snns_Wcg_charm(x, &output);
  else if(m_sample == "Wcg_gluon")  snns_Wcg_gluon(x, &output);
  else if(m_sample == "ttbar")      snns_ttbar_improved(x, &output);
  else if(m_sample == "tchan")      snns_tchan(x, &output);
  else if(m_sample == "tchan_light")snns_tchan_light(x, &output);
  else if(m_sample == "diboson")    snns_diboson(x, &output);
  else if(m_sample == "Wgg")        snns_Wgg(x, &output);
  else if(m_sample == "Wjg_light")  snns_Wjg_light(x, &output);
  else if(m_sample == "Wjg_gluon")  snns_Wjg_gluon(x, &output);


  else {
    std::cout << "Wrong sample" << std::endl;
    return 0;	
  }
  */
  
  return m_NN_TF(partonJet.lv.E(), output);
}




SimpleSmearing::SimpleSmearing(double smear) :
   TransferFunction("Simple smearing"),
   m_smear(smear)
{}

double SimpleSmearing::getTF(const PartonColl::Jet& partonJet,
                             const PartonColl::Jet& measuredJet) const
{
   return std::exp(-.5 * AuxFunctions::Math::square((measuredJet.lv.E()
                                                       - partonJet.lv.E()) 
                                                      / partonJet.lv.E()
                                                      / m_smear));  
}

NewerTransferFunction::NewerTransferFunction(const ParamFiles& files,
                                             float centralEtaBorder,
                                             float forwardEtaBorder) :
   TransferFunction("Newer transfer function"),
   m_central07SLT(files.central07SLT),
   m_central07noSLT(files.central07noSLT),
   m_central04SLT(files.central04SLT),
   m_central04noSLT(files.central04noSLT),
   m_middle07(files.middle07),
   m_middle04(files.middle04),
   m_forward07(files.forward07),
   m_forward04(files.forward04),
   //m_sumecentral(files.sumecentral),
   m_centralEtaBorder(centralEtaBorder),
   m_forwardEtaBorder(forwardEtaBorder)
{}

double NewerTransferFunction::getTF(const PartonColl::Jet& partonJet,
                                    const PartonColl::Jet& measuredJet) const
{
  /*
  // For debugging purposes
  static int f = 0;
  if (f<1 || f == 330780 || f == 330781) {
    std::cout<<"  NewerTransferFunction::getTF "<<f<<"   partonJet"<< partonJet<<std::endl;
    std::cout<<"  NewerTransferFunction::getTF "<<f<<" measuredJet"<< measuredJet<<std::endl; 
  }
  f++;
  */

   //central
   if (std::abs(measuredJet.lv.Eta()) < m_centralEtaBorder)
   {
      if (measuredJet.energy07 <= 0)
      {
	if (measuredJet.slt)
	  {
	    return m_central04SLT(partonJet.lv.E(), measuredJet.lv.E());	   
	  } 
	else
	  {
	    return m_central04noSLT(partonJet.lv.E(), measuredJet.lv.E()); 	    
	  }
      }
      else if (measuredJet.slt)
      {
	return m_central07SLT(partonJet.lv.E(), measuredJet.energy07);
      }
      else
      {
	return m_central07noSLT(partonJet.lv.E(), measuredJet.energy07);
      }
   }
   //middle
   else if (std::abs(measuredJet.lv.Eta()) < m_forwardEtaBorder)
   {
      if (measuredJet.energy07 <= 0)
      {
	return m_middle04(partonJet.lv.E(), measuredJet.lv.E());
      }
      else
      {
	return m_middle07(partonJet.lv.E(), measuredJet.energy07);
      }
   }
   //forward
   else
   {
      if (measuredJet.energy07 <= 0)
      {
	return m_forward04(partonJet.lv.E(), measuredJet.lv.E());
      }
      else
      {
	return m_forward07(partonJet.lv.E(), measuredJet.energy07);
      }
   }
}
