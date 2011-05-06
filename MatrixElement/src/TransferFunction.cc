
// Our code libraries
#include "TAMUWW/MatrixElement/interface/TransferFunction.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/DataFile.hh"
#include "TAMUWW/AuxFunctions/interface/AuxFunctions.hh" 
#include "TAMUWW/SpecialTools/interface/Table.hh" 
#include "TAMUWW/SpecialTools/interface/TableCellVal.hh" 


// C++ libraries
#include <iostream>
#include <cmath>
#include <stdexcept>

// ROOT libraries
#include "TMath.h"
#include "TF1.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;




// --------------------------------------------------------------------
// The single gaussian function used by the transfer functions.
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
   // 
   using AuxFunctions::Math::square;

   double difference = measuredE - partonE;
   double p01 = m_parameters[0] + m_parameters[1] * partonE;
   double p23 = m_parameters[2] + m_parameters[3] * partonE;

   double fxy = std::exp(-.5 * square((difference - p01) / p23));
   fxy /= p23 ;
   fxy /= std::sqrt(TMath::TwoPi());

   return fxy;
}

// --------------------------------------------------------------------
// The double gaussian function used by the transfer functions.
DoubleGaussian::DoubleGaussian(const string& dataFileName, bool table )
{
  if(table) {
    
    cout << "DoubleGaussian::Reading in transfer function parameters from table." << endl;
    Table* table = new Table(dataFileName);
    if(table->parseFromFile(dataFileName,"TableCellVal")){
      
      // Copy the first 10 parameters from the table
      for (int i=0; i<10; i++){
	string row = AuxFunctions::concatString("Par",i);
	TableCellVal * param = (TableCellVal *) table->getCellRowColumn(row,"Parameters");
	if (param){
	  m_parameters.push_back(param->val.value);
	  m_parErrors.push_back(param->val.error);
	}else{
	  cout << "ERROR::DoubleGaussian::Unable to find row="<<row<<" in table. Exiting now. "<<endl;
	  exit (1);
	}
      }//for 
      
    }else{
      cout << "ERROR::DoubleGaussian::Unable to parse table from file " <<dataFileName<<" Exiting now."<< endl;
      exit (1);
    }

  }else{
    
    cout << "DoubleGaussian::Reading in transfer function parameters from InputDataFile." << endl;
    InputDataFile data(dataFileName);
    cout<<"Input TFs "<<dataFileName<<endl;
    for (unsigned int i = 0; i < 10; ++i){
      string param = AuxFunctions::concatString("transfer function parameter ",i);
      m_parameters.push_back(data.readFloat(param));
      m_parErrors.push_back(0);
    }//for

  }

  // Perform the normalization for Pt cut of 30.
  computeNormalizationFactors(30);

  //Print the parameters
  for (unsigned int i = 0; i < 10; ++i)
    cout<<"\t Parameter "<<i<<"\t"<<m_parameters[i]<<" +/- "<< m_parErrors[i]<<endl;
  
} // DoubleGaussian


double DoubleGaussian::operator()(double partonE, double measuredE) const
{
   using AuxFunctions::Math::square;

   double difference = measuredE - partonE;
   double p01 = m_parameters[0] + m_parameters[1] * partonE;
   double p23 = m_parameters[2] + m_parameters[3] * partonE;
   double p45 = m_parameters[4] + m_parameters[5] * partonE;
   double p67 = m_parameters[6] + m_parameters[7] * partonE;
   double p89 = m_parameters[8] + m_parameters[9] * partonE;

   double fxy = std::exp(-.5 * square((difference - p01) / p23));
   fxy += p45 * std::exp(-.5 * square((difference - p67) / p89));
   fxy /= p23 + p45 * p89;
   fxy /= std::sqrt(TMath::TwoPi());

   if (partonE < 0 || partonE >= m_norm.size()){
     cout<<"ERROR DoubleGaussian::operator() called with partonE="<<partonE<<endl;
     return 0;
   }
   return m_norm[(int)partonE] * fxy;
}

// For the given Ep compute the intergral of the double-gaussian
// from minEj to the max possible energy.
double DoubleGaussian::computeIntegral(double Ep, double minEj){
 
  double inte = 0;
  for (int Ej = minEj; Ej < MEConstants::beamEnergy; Ej++)
    inte += operator()(Ep,Ej);
  
  return inte;

}//computeIntegral

// compute the normalization factors given the minEj cut used in the analysis
void  DoubleGaussian::computeNormalizationFactors(double minEj){

  // clear the normalization vector
  m_norm.clear();
  
  // Create the norm vector with norm=1
  for (int Ep = 0; Ep <= MEConstants::beamEnergy;  Ep ++)
    m_norm.push_back(1);


  // Loop over all the parton energies
  for (unsigned int Ep = 0; Ep <  m_norm.size(); Ep ++){
						
    double inte = computeIntegral(0.5+Ep, minEj); // +0.5 to avoid the singularity at zero
    if (inte == 0) inte = 1;
    m_norm[Ep] = 1.0/inte;
    
  }// for Ep

}// computeNormalizationFactors


// --------------------------------------------------------------------
// This is the core of the TransferFunction abstract class
TransferFunction::TransferFunction(const string& name) :
   m_name(name)
{}

// --------------------------------------------------------------------
// For a given jet energy this method provides the energy of the partons
// necessary for the transfer function to the be lowPercent and highPercent
// This method is called before running the ME to determine what the limits
// of the integration on the parton jet energy should be for each measured jet.
// For example getBounds(jet, 0.01, 0.03, lowBound, highBound) will return
// -the parton energy at which the TF is maximum, maxE.
// -by reference in  lowBound the minimum parton energy such that  
//       TF(jet,lowBound)/TF(jet,maxE)  > 1% 
// -by reference in highBound the maximum parton energy such that 
//       TF(jet,highBound)/TF(jet,maxE) > 3% 
double TransferFunction::getBounds(const PartonColl::Jet& measuredJet,
                                   float lowPercent, float highPercent,
                                   double& lowBound, double& highBound) const
{
   using MEConstants::beamEnergy;

   double max = 0., maxE = 0.;

   // Find the maximum value that the TF will take by scanning over all 
   // possible energy values that the partons could take.
   for (int ienergy = 0; ienergy < beamEnergy; ++ienergy)
   {
      PartonColl::Jet partonJet(measuredJet);
      partonJet.lv.SetE(ienergy);
      partonJet.energy07 = ienergy;
      double value = getTF(partonJet, measuredJet);
      if (value > max){
         max = value;
         maxE = ienergy;
      }
   }

   // Find the lower parton energy at which the TF/max is larger than lowPercent.  
   // As before by scanning over the parton energy from zero to the beam's energy
   // in bins of 1 GeV.
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

   // Change the lowbound minimum energy to at least one GeV, using 0 
   // results in cases of jets with zero energy and momentum, which in 
   // turn results in nan from the ME calculations.
   if (lowBound == 0) 
     lowBound = 1;

   // Find the higher parton energy at which the TF/max is higher than highPercent  
   // As before by scanning over the parton energy but this time going down from 
   // the beam's energy to zero energy in bins of 1 GeV.
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

// --------------------------------------------------------------------
// The identity transfer function always returns 1.
IdentityTransferFunction::IdentityTransferFunction() :
   TransferFunction("No transfer function")
{}

double IdentityTransferFunction::getTF(const PartonColl::Jet& partonJet,
				       const PartonColl::Jet& measuredJet) const
{
   return 1;
} // IdentityTransferFunction


// --------------------------------------------------------------------
// The double gaussian transfer function.
DGTransferFunction::DGTransferFunction(const string& paramFile) :
  TransferFunction("Double Gaussian transfer function"),
  m_params(paramFile, true){} // true to use Table to read it


double DGTransferFunction::getTF(const PartonColl::Jet& partonJet,
                                  const PartonColl::Jet& measuredJet) const
{
   return m_params(partonJet.lv.E(), measuredJet.lv.E());
} 

// --------------------------------------------------------------------
// The Neural Net transfer function takes advantage of the full jet
// information for the determination of the TF probability.
NNTransferFunction::NNTransferFunction(const string& paramFile_NN_TF,const string&  sample) :
  TransferFunction("NN transfer function "),
  m_NNTF(paramFile_NN_TF),
  m_sample(sample)
{}


double NNTransferFunction::getTF(const PartonColl::Jet& partonJet,
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
    cout << "Wrong sample" << endl;
    return 0;	
  }
  */
  
  return m_NNTF(partonJet.lv.E(), output);

} // NNTransferFunction 



// --------------------------------------------------------------------
// The RelWidthTransferFunction utilizes a Gaussian transfer function
// with a width given by the input "sigma" value.
// 
// ** In priciple we could use the formula TF = N * exp(-0.5*(Em-Ep)^2/sigma^2)
// but this will give a fixed width for all parton jet energies (Ep).
// 
// ** Instead, it is better to define this TF to be 
//     TF = N * exp(-0.5*(Em-Ep)^2/(Ep*sigma)^2)
// so that the sigma is representative of the *percentage* of energy at 1 sigma.
// A smear of 0.1 will produce a width of 10%.

// ** For the normalization we should take into account that the gaussian is truncated
// as there are no measured jets below minEj, which is the min cut requirement on the jets.
// The proper normalization can be shown to be :
//      N = 2 / erfc( (Ejmin-Ep) / (sqrt(2)*sigma*Ep) ) 
// Where erfc(z) = 2/sqrt(pi) int_z^infinite exp(-t^2) dt (see http://mathworld.wolfram.com/Erfc.html ;-))
// The fact that the cut is usually applied on Pt rather than E does not the change the 
// fact that the min possible Ej is also be the Pt cut. 
RelWidthTransferFunction::RelWidthTransferFunction(double sigma) :
  TransferFunction("Simple smearing"), m_sigma(sigma){
  
  // Compute the normalization here
  m_norm = 2 / TMath::Erfc( -1.0 / (sqrt(2)*m_sigma) );
  
}

double RelWidthTransferFunction::getTF(const PartonColl::Jet& partonJet,
				       const PartonColl::Jet& measuredJet) const {

  return m_norm*std::exp(-.5 * AuxFunctions::Math::square((measuredJet.lv.E()
							   - partonJet.lv.E()) 
							  / (measuredJet.lv.E()
							     * m_sigma         )));  
  

} // RelWidthTransferFunction

// --------------------------------------------------------------------
// The NewerTransferFunction 
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
    cout<<"  NewerTransferFunction::getTF "<<f<<"   partonJet"<< partonJet<<endl;
    cout<<"  NewerTransferFunction::getTF "<<f<<" measuredJet"<< measuredJet<<endl; 
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
