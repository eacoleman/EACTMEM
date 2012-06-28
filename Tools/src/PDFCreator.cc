// TODO:
//  -add booleans to insure that last step was successfully performed before performing next step

// Title:         PDFCreator.cc
// Author:        Travis Lamb
// Started:       21st of June, 2012
// Last Edit:     22nd of June, 2012

// Outputs tables to a LaTeX document.

// Our libraries
#include "TAMUWW/Tools/interface/PDFCreator.hh"

using namespace std;

PDFCreator::PDFCreator()
{
   PDFCreator("Tables.tex", "TEMPLATE.tex");
}

PDFCreator::PDFCreator(string templateLocation, string outputLocation)
{
   // Save the file locations
   templateFilepath = templateLocation;
   outputFilepath = outputLocation;
   
   // Open the template and output files
   // NOTE that we first write to a temporary location before finalizing
   //      (see PDFCreator::changeSpecialCharacters())
   templateFile.open(templateFilepath);
   outputFile.open(outputFilepath + "TEMP");
   
   // Check that they were succesfully opened
   if(!templateFile.is_open())
   {
      cout << endl << "WARNING: in PDFCreator::PDFCreator(outputLocation, templateLocation): ";
      cout << templateLocation << " was not opened." << endl;
   }
   if(!outputFile.is_open())
   {
      cout << endl << "WARNING: in PDFCreator::PDFCreator(outputLocation, templateLocation): ";
      cout << outputLocation << " was not opened." << endl;
   }
}

PDFCreator::~PDFCreator()
{
   // Close the files
   templateFile.close();
   outputFile.close();
}

void PDFCreator::beginDocument(string title, string author, string email)
{
   string line;
   // We iterate through every line of the templateFileand copy it to outputFile while inserting the title, author, and email
   while(templateFile.good() || !templateFile.eof())
   {
      // Grab the current line
      getline(templateFile, line);
      
      // If the line contains the keyword "CODEGOESHERE", we stop copying
      if(string::npos != line.find("CODEGOESHERE"))
      {
         outputFile << line << endl;
         break;
      }
      // If the line is the title line, we write the title
      else if(string::npos != line.find("SmallTitle"))
      {
         outputFile << "\\title[" << title << "]{" << title << "}" << endl;
      }
      // If the line is the author line, we write the author
      else if(string::npos != line.find("SmallAuthor"))
      {
         outputFile << "\\author[" << author << "]{" << author << "\\inst{1}}" << endl;
      }
      // If the line is the email line, we write the email
      else if(string::npos != line.find("Email"))
      {
         outputFile << "  \\texttt{" << email << "}" << endl;
      }
      // If this is no sort of special line, we simply copy it
      else
      {
         outputFile << line << endl;
      }
   }
}

void PDFCreator::writeIntroduction(double muonLuminosity, double electronLuminosity, Table &crossSectionsTable, Table &eventsFromMCTable)
{
   // Begin the introduction slide 
   outputFile << "\\section{Introduction}" << endl;
   outputFile << "\\subsection{General Information}" << endl;
   outputFile << "\\frame {" << endl;
   outputFile << "\\frametitle{Data Used}" << endl;
   outputFile << "\\tiny{" << endl << endl;
   
   outputFile << "The following data was used to generate the tables:\\\\" << endl << endl;
   
   // The muon and electron luminosities
   outputFile << "Muon Luminosity ($pb^{-1}$): " << muonLuminosity << "\\\\" << endl << endl;
   outputFile << "Electron Luminosity ($pb^{-1}$): " << electronLuminosity << "\\\\" << endl << endl;
   
   // CrossSections for all the procresses
   outputFile << "CrossSection ($pb^{-1}$):\\\\" << endl;
   crossSectionsTable.printTable(outputFile, "Latex");
   
   outputFile << "\\vspace{0.2cm}" << endl;
   
   // Event numbers for all processes
   outputFile << "EventsFromMC:\\\\" << endl;
   eventsFromMCTable.printTable(outputFile, "LatexInt");
   
   outputFile << "\\note {Note if needed}" << endl;
   outputFile << "}}" << endl;
   
   // New slide to describe the table production process
   outputFile << "\\frame {" << endl;
   outputFile << "\\frametitle{Table Production Method}" << endl;
   outputFile << "\\large{" << endl;
   
   // Itemized listing of the processes 
   outputFile << "\\begin{itemize}" << endl;
   outputFile << "\\item The Acceptance tables were produced by taking the raw tables and dividing them by the EventsFromMC." << endl;
   outputFile << "\\item The Yield per pb$^{-1}$ tables were produced by taking the Acceptance tables and multiplying them by the cross section." << endl;
   outputFile << "\\item The Yields for Total Luminosity tables were produced by taking the Yields per pb$^{-1}$ and multiplying them by the luminosity." << endl;
   outputFile << "\\end{itemize}" << endl;
   
   outputFile << "\\note {Note if needed}" << endl;
   outputFile << "}}" << endl;
}

void PDFCreator::newSection(string sectionName)
{
   outputFile << "\\section{" << sectionName << "}" << endl;
}

void PDFCreator::newSubSection(string subSectionName)
{
   outputFile << "\\subsection{" << subSectionName << "}" << endl;
}

void PDFCreator::startFrame(std::string frameName)
{
   // Starts new frame
   outputFile << "\\frame {" << endl;
   outputFile << "\\frametitle{" << frameName << "}" << endl;
   outputFile << "\\tiny{" << endl << endl;
}

void PDFCreator::endFrame()
{
   // Ends current frame
   outputFile << "\\note {Note if needed}" << endl;
   outputFile << "}}" << endl;
}

void PDFCreator::writeTable(Table &table)
{
   outputFile << "\\resizebox{\\textwidth}{!}{" << endl;
   table.printTable(outputFile, "Latex");
   outputFile << "}" << endl;
   outputFile << "\\vspace{0.2cm}" << endl;
}

void PDFCreator::endDocument()
{
   // Copies the rest of the template file, line by line
   string line;
   while(!templateFile.eof())
   {
      getline(templateFile, line);
      outputFile << line << endl;
   }
   
   templateFile.close();
   outputFile.close();
   
   // This function changes "_" and "/" to "\_" and "\/"
   changeSpecialCharacters();
}

void PDFCreator::changeSpecialCharacters()
{
   // First we copy from a temporary location to a final location
   ifstream tempFile(outputFilepath + "TEMP");
   ofstream finalFile(outputFilepath);
   
   string line;
   
   // Copy the header line by line till the tables start
   while(!tempFile.eof())
   {
      getline(tempFile, line);
      if(string::npos != line.find("CODEGOESHERE"))
         break;
      finalFile << line << endl;
   }
   
   // Copies the rest of the file while replacing "_" and "/" with "\_" and "\/"
   while(!tempFile.eof())
   {
      size_t index = 0;
      getline(tempFile, line);
      
      // Replaces "_"
      if(string::npos != line.find("_"))
      {
         while (true)
         {
            // Locate the substring to replace
            index = line.find("_", index);
            if (index == string::npos) break;

            // Make the replacement
            line.replace(index, 1, "\\_");

            // Advance index forward one spot so the next iteration doesn't pick it up as well
            index += 2;
         }
      }
      
      index = 0;
      
      // Replaces "/"
      if(string::npos != line.find("/"))
      {
         while (true)
         {
            // Locate the substring to replace
            index = line.find("/", index);
            if (index == string::npos) break;

            // Make the replacement
            line.replace(index, 1, "\\/");

            // Advance index forward one spot so the next iteration doesn't pick it up as well
            index += 2;
         }
      }
      
      // Finally, copy the line
      finalFile << line << endl;
   }
   
   // Close the files and remove the temporary file
   tempFile.close();
   remove( (outputFilepath + "TEMP").c_str() );
   finalFile.close();
}
