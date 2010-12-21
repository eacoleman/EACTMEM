#ifndef EVENTPROB_HH
#define EVENTPROB_HH

#include <string>
#include <utility>
#include <vector>

#include "TransferFunction.hh"
#include "PartonColl.hh"

class TLorentzVector;

class Integrator;

class EventProb
{
   public:
      EventProb(std::string name, Integrator& integrator, unsigned nVars,
                unsigned nLoop);
      virtual ~EventProb() {}

      virtual double execute(const double parameters[]);

      static void setPartonColl(PartonColl& input)
      {m_partonColl = input;}
      static PartonColl* getPartonColl() {return &m_partonColl;}
      static void setMeasuredColl(PartonColl& input)
      {m_measuredColl = input;}
      static PartonColl* getMeasuredColl() {return &m_measuredColl;}

      void setBounds(unsigned param, double lower, double upper);

      std::string getName() const {return m_name;}
      unsigned getDimension() const {return m_nVars;}
      Integrator& getIntegrator() {return m_integrator;}
      const Integrator& getIntegrator() const {return m_integrator;}

      bool incrementLoop();
      unsigned getLoopMax() const {return m_nLoop;}
      virtual unsigned getProbMax() const {return m_nLoop;}

      //Start added by RE
      virtual double getMaxProbNumber() const {return m_maxProbNumber;}
      virtual void   resetMaxProbNumber(){ m_maxProbNumber = 0;}
      // End added by RE

      typedef std::vector<std::vector<double> > VecVecDouble;
      virtual void getPeaks(VecVecDouble& answer,
                            const double bounds[]) const = 0;

      virtual void setDynamicBounds() {}
      void prepareForIntegral();

      //Method to be used in the EPDEventProb object.
      // this the method called when correlating the nuisance parameters
      virtual double matrixElementCorrelated(const std::vector<double> & trueParams);
      double getVolume() const { return m_volume; }

      // This is useful counter to have when debugging  
      int debug;

   protected:

      virtual void setupIntegral() {}

      virtual void changeVars(const std::vector<double>&) = 0;

      virtual void met();

      bool isPossible() const;

      bool getPartonEnergies(double& energy1, double& energy2) const;
      virtual void getTotalLV(TLorentzVector& vec) const;

      virtual double matrixElement() const = 0;

      virtual double phaseSpace() const;

      double pdf(double x1, double x2) const;
      virtual void setPartonTypes() const = 0;
      virtual void getScale(double& scale1, double& scale2) const = 0;

      virtual double totalTF() const = 0;

      virtual bool onSwitch() {return true;} // false means stop the loop

      int getLoop() const {return m_loop;}

      void swapJets(unsigned jet1, unsigned jet2);

      void adjustBounds(std::vector<double>&) const;

      enum PDFTypes
      {
         kGluon = 0,
         kUp = 1,
         kDown = 2,
         kStrange = 3,
         kCharm = 4,
         kBottom = 5,
         kAntiUp = -1,
         kAntiDown = -2,
         kAntiStrange = -3,
         kAntiCharm = -4,
         kAntiBottom = -5
      };

     // Start of variables for Ricardo's test

     double m_maxProbNumber;

     // End of variables for Ricardo's test

   private:
      // Parton level quantities that change during the integration
      static PartonColl m_partonColl;

      // the actual measured collection, which does not changes 
      // during the integration, except for the swapping of the jets
      static PartonColl m_measuredColl;

      std::string m_name;
      Integrator& m_integrator;
      const unsigned m_nLoop;
      const unsigned m_nVars;

      int m_loop;


      // The Bounds type stores the set {a, b - a} for an interval (a, b)
      typedef std::pair<double, double> Bounds;
      std::vector<Bounds> m_bounds;

      void m_changeVars(const double parameters[]);

      float m_volume;

};

#endif
