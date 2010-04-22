#include "TAMUWW/MatrixElement/interface/topEventProb.hh"
#include "TAMUWW/MatrixElement/interface/MEConstants.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"

// This method returns the theoretical Top Width, 
// without any multiplicative scale factor
double topEventProb::calcTopWidth(double topMass){

   using std::atan;
   using PeterFunctions::Math::cube;
   using PeterFunctions::Math::square;
   using MEConstants::wMass;
   using MEConstants::wWidth;

   const double factor = 1.88392789543834161e-06;
   double frontFactor = factor * cube(topMass);
   double alpha2 = square(wMass / topMass);
   double alpha4 = square(alpha2);
   const double wMassWidth = wMass * wWidth;
   double massFactor = (1 - 3 * alpha4 + 2 * alpha4 * alpha2)
      / wMassWidth;
   double piFactor = atan((square(topMass - 5) - square(wMass)) / wMassWidth)
      - atan(-wMass / wWidth);

   return  9.11 * frontFactor * piFactor * massFactor;

}//calcTopWidth

