// TAMUWW libraries
#include "TAMUWW/Tools/interface/TTbarreweight.hh"

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::make_pair;

//------------------------------------------------------------------------
TTbarreweight::TTbarreweight(){

}

//------------------------------------------------------------------------
double TTbarreweight::getWeight(const EventNtuple * ntuple, const PhysicsProcess* proc){

   if(proc) {
      TString auxName = proc->name;
      auxName.ToUpper();
      if(!auxName.Contains("TTBAR")) {
         cout << "WARNING::plotter_x::ttbarWeight::Cannot apply TTbar reweighting to process " << proc->name << endl
              << "\tReturning a weight of 1" << endl;
         return 1.0;
      }
   }

   const double topFactor_A = 0.159;
   const double topFactor_B = -0.00141;
   //Calculate SF and weight
   pair<double,double> ttbarPtPair = ttbarPt(ntuple);
   double hadronicSF = exp(topFactor_A+topFactor_B*ttbarPtPair.second);
   double leptonicSF = exp(topFactor_A+topFactor_B*ttbarPtPair.first);
   //Top Event Weight
   return sqrt(hadronicSF*leptonicSF);
    
}//getWeight

//-----------------------------------------------------------------------
pair<double,double> TTbarreweight::ttbarPt(const EventNtuple * ntuple) {
   if (ntuple->genParticleCollection.size()==0) {
      cout << "WARNING::plotter_x::ttbarPt::No genParticleCollection present in this sample" << endl;
      return make_pair(-1.0,-1.0);
   }
   
   vector<int> Tpositions;
   for (unsigned int i=0; i<ntuple->genParticleCollection.size(); i++) {
      if (abs(ntuple->genParticleCollection[i].pdgId)==6) {
         Tpositions.push_back(i);
      }
   }
   if(Tpositions.size()!=2) {
      cout << "WARNING::plotter_x::ttbarPt::There are " << Tpositions.size() << "top quarks in the TTbar sample, not 2" << endl;
      return make_pair(-1.0,-1.0);
   }

   return make_pair(ntuple->genParticleCollection[Tpositions[0]].p4.Pt(),ntuple->genParticleCollection[Tpositions[1]].p4.Pt());

}//ttbarPt