// TODO:
//  -add booleans to insure that last step was successfully performed before performing next step

// Title:         PDFCreator.hh
// Author:        Travis Lamb
// Started:       21st of June, 2012
// Last Edit:     22nd of June, 2012

// Outputs tables to a LaTeX document.

// Our Libraries
#include "TAMUWW/SpecialTools/interface/Table.hh"

// C++ Libraries
#include <iostream>
#include <iomanip>
#include <string>

class PDFCreator
{

private:
   std::string templateFilepath;
   std::string outputFilepath;
   ifstream templateFile;
   ofstream outputFile;
      
public:
   PDFCreator();
   PDFCreator(std::string templateLocation = "TEMPLATE.tex",
              std::string outputLocation = "Tables.tex");
   ~PDFCreator();
   
   // Writes the header/package information along with the title.
   void beginDocument(std::string title = "Monte Carlo Acceptances and Yields",
                      std::string author = "Ricardo Eusebi, Travis Lamb, Alexx Perloff",
                      std::string email = "eusebi@physics.tamu.edu, travis.t.lamb@gmail.com, aperloff@physics.tamu.edu");
   
   // Writes the introduction which displays the data used to form the tables
   void writeIntroduction(double muonLuminosity,
                          double electronLuminosity,
                          Table &crossSectionsTable,
                          Table &eventsTable);
   
   void newSection(std::string sectionName);
   void newSubSection(std::string subSectionName);
   void startFrame(std::string frameName);
   void endFrame();
   
   void writeTable(Table &table);
   
   void endDocument();
   
private:
   void changeSpecialCharacters();
   
};
