#include "TAMUWW/MatrixElement/interface/HiggsEventProb.hh"

// This method returns the theoretical Higgs Width, 
// without any multiplicative scale factor
double HiggsEventProb::calcHiggsWidth(double higgsMass){

  return 3.0 * 1.16639e-5 * 3.7 * 3.7 * higgsMass
     / (4 * 1.41421 * 3.1415927); 

}
