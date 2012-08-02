#ifndef ZLIGHTEVENTPROB2JET_HH
#define ZLIGHTEVENTPROB2JET_HH

#include "EventProb2jet.hh"

#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"


class ZLightEventProb2Jet : public EventProb2Jet
{
   public:
      ZLightEventProb2Jet(Integrator& integrator, const TransferFunction& tf);

   protected:
      virtual bool onSwitch();
      virtual void changeVars(const std::vector<double>& parameters);
      virtual double matrixElement() const;
      virtual void setPartonTypes() const;
      virtual void setJetTypes();
      virtual void getScale(double& scale1, double& scale2) const;
   private:
      bool swapPartonMom;
      bool swapQuarkFlavor;
      double alphas_process;
  //      const bool m_rh;
      static const int NVec=6;
      static const int VSize=64;
      enum {vecSize = VSize};
      static const int NOutputP1=9;
      int idxvec_reordered[NOutputP1][VSize];
      virtual void AssignOrdering(int o_vec[NOutputP1][NVec], int output_vecidx, const char* order) const;
      virtual void Determine_switchedlabels(int sl[NOutputP1][NVec], int o_target[NVec], int o_output[NOutputP1][NVec], int output_vecidx) const;
      virtual void Determine_switchedindex(int idxvec_reordered[NOutputP1][VSize], int switchedlabel[NOutputP1][NVec], int output_vecidx) const;
      typedef SimpleArray<doublecomplex, VSize> OutputType;
      virtual void Reorder(OutputType& output, const int idxvec_reordered[NOutputP1][VSize], int output_vecidx) const;

};

#endif
