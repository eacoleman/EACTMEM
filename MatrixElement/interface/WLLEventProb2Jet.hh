#ifndef WLLEVENTPROB2JET_HH
#define WLLEVENTPROB2JET_HH

#include "EventProb2jet.hh"
#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"

class WLLEventProb2Jet : public EventProb2Jet
{
   public:
      WLLEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      int initialStateConfiguration;
      bool swapPartonMom;
      double alphas_process;
      ///Needed for reordering
      static const int NVec=4;//Number of *size 2* initial vectors
      static const int VSize=16;
      enum {vecSize = VSize};
      static const int NOutputP1=9;
      virtual void AssignOrdering(int o_vec[NOutputP1][NVec], int output_vecidx, const char* order) const;
      virtual void Determine_switchedlabels(int sl[NOutputP1][NVec], int o_target[NVec], int o_output[NOutputP1][NVec], int output_vecidx) const;
      virtual void Determine_switchedindex(int idxvec_reordered[NOutputP1][VSize], int switchedlabel[NOutputP1][NVec], int output_vecidx) const;
      typedef SimpleArray<doublecomplex, VSize> OutputType;
      virtual void Reorder(OutputType& output, const int idxvec_reordered[NOutputP1][VSize], int output_vecidx) const;

      int idxvec_reordered_ggp[NOutputP1][VSize];
      int idxvec_reordered_ggm[NOutputP1][VSize];

      int idxvec_reordered_uup[NOutputP1][VSize];
      int idxvec_reordered_ddm[NOutputP1][VSize];

      int idxvec_reordered_ddp[NOutputP1][VSize];
      int idxvec_reordered_uum[NOutputP1][VSize];


};

#endif
