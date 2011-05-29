#ifndef FIGUREOFMERIT_HH
#define FIGUREOFMERIT_HH

// Root libraries
#include "TH1.h"

class FigureOfMerit{

public:

  FigureOfMerit();
  ~FigureOfMerit();

  // Use this when finding limits on cross sections, as with the WH analysis
  static double usingShapeFromTemplates(TH1* signal, TH1* background, double relativeBinErrorLimit = 3);

  // This computes the sqrt[sum_bin S^2/(S+B+deltaS^2+deltaB^2)]
  static double usingChi2(TH1* signal, TH1* background, double relativeBinErrorLimit = 3);

  // This computes the sqrt[sum_bin S^2/B]
  static double usingS2OverB(TH1* signal, TH1* background);

  // This returns false if the Histogram has an empty bin
  static bool checkBackgroundHistogram(TH1* back, double relativeBinErrorLimit = 3);

private:

  //These are used in usingShapeFromTemplates for the fitting procedure
  static TH1* signal;
  static TH1* background;
  static void minimizationFunction(Int_t &npar, Double_t *gin, Double_t &f, 
			      Double_t *par, Int_t iflag);


};

#endif
