#ifndef HIGGSEVENTPROB_HH
#define HIGGSEVENTPROB_HH

class HiggsEventProb {

 public:
  // Require a method to set the Mass and Width 
  virtual void setHiggsMassAndWidth(double mHiggs) = 0;

  // // Likewise require a method to compute the effective coupling of the higgs to two gluons
  //virtual void setGH(double mHiggs, double mTop, double alphas, double v) = 0;

  // Static method to calculate widths from masses
  static double calcHiggsWidth(double);

 protected:
  double m_massHiggs;
  double m_widthHiggs;
  double m_gh;

};

#endif
