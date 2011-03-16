#ifndef MEJOB_HH
#define MEJOB_HH

#include <map>
#include <string>
#include <vector>

class TBenchmark;
class TFile;
class TTree;

class EventFile;
class EventProb;
class Integrator;
class METree;
class PartonColl;

class MEJob{

public:
      
  MEJob(EventFile& inputFile, const std::string& outputFilename);

  ~MEJob();

  void addEventProb(EventProb&, unsigned jetBin);

  void loopOverEvents();
  void massScan(float low, float high, unsigned steps, 
		std::string process = "Top");

  void setDynamicBounds(bool input) {m_useDynamicBounds = input;}

  static void sm_peakFinder(const int*, const double[], int*, double[]);


private:
  EventFile& m_inputFile;
  TFile* m_outputFile;
  METree* m_output;

  typedef std::vector<EventProb*> EventProbVec;
  typedef std::map<unsigned, EventProbVec> EventProbMap;
  EventProbMap m_probs;

  TBenchmark* m_benchmark;

  bool m_useDynamicBounds;

  void m_doEvent(PartonColl& partons);

  static void sm_integrand(const int*, const double[], const int*,
			   double[]);
  static EventProb* sm_eventProb;

  // Method used when doing Higgs mass scans
  void m_setHiggsMass(double mass);

  // Method used when doing Top mass scans
  void m_setTopMass(double mass);

  // Report the input values of the parton collection
  void  m_reportInputValues(const PartonColl & partons);

};

#endif
