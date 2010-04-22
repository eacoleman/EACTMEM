#ifndef TOPEVENTPROB_HH
#define TOPEVENTPROB_HH

class topEventProb {

 public:
  // Require a method to set the Mass and Width 
  virtual void setTopMassAndWidth(double mTop) = 0;

  // Static method to calculate widths from masses
  static double calcTopWidth(double);

 protected:
  double m_massTop;
  double m_widthTop;

};

#endif
