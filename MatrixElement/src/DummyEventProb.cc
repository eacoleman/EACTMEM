#include "TAMUWW/MatrixElement/interface//DummyEventProb.hh"

DummyEventProb::DummyEventProb(std::string name, Integrator& integrator) :
   EventProb(name, integrator, 3, 1) {
  // at least 3 variables (used for the bounds)
  // 1 permutation loop, but it could be anything as the onSwitch will always kill it before permutation is started
}//C'tor

//This method is defined here to avoid "undefined reference to `vtable " error. At least one pure virtual 
//function must be defined out-of-line (i.e. not inline in the header)
bool DummyEventProb::onSwitch() {
  return false;
}


   
