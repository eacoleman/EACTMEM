#include "TAMUWW/MatrixElement/interface/DataFile.hh"
#include "TAMUWW/MatrixElement/interface/PeterFunctions.hh"

#include <iostream>
#include <iomanip>
#include <stdexcept>

using std::string;
using std::getline;

InputDataFile::InputDataFile(string filename)
{
   std::ifstream input(filename.c_str(), std::ios::in);

   if (!input)
   {
     throw std::runtime_error("InputDataFile::InputDataFile File "+filename+" not found!\n");
   }

   while (!input.eof())
   {
      input.clear(); 

      char type;
      string name;
      int intVal;
      double floatVal;
      string stringVal;

      input >> type;
      if (input.eof())
      {
         break;
      }
      if (type == '#')
      {
         getline(input, name, '\n');
         continue;
      }

      input.ignore();
      getline(input, name, '\t');
//      std::cerr << '\"' << name << '\"' << std::endl;

      switch (type)
      {
         case 'I':
            input >> intVal;
            m_intMap.insert(std::make_pair(name, intVal));
            break;

         case 'F':
            input >> floatVal;
            m_floatMap.insert(std::make_pair(name, floatVal));
            break;

         case 'S':
            getline(input, stringVal);
            m_stringMap.insert(std::make_pair(name, stringVal));
            break;

         default:
            throw std::runtime_error("Invalid type!\n");
      }
   }
}

int InputDataFile::readInt(string name) const
{
   try
   {
      return PeterFunctions::checkedMapGet(m_intMap, name);
   }
   catch (std::runtime_error)
   {
      std::cerr << name << " not found!\n";
      throw;
   }
}

double InputDataFile::readFloat(string name) const
{
   try
   {
      return PeterFunctions::checkedMapGet(m_floatMap, name);
   }
   catch (std::runtime_error)
   {
      std::cerr << name << " not found!\n";
      throw;
   }
}

string InputDataFile::readString(string name) const
{
   try
   {
      return PeterFunctions::checkedMapGet(m_stringMap, name);
   }
   catch (std::runtime_error)
   {
      std::cerr << name << " not found!\n";
      throw;
   }
}

OutputDataFile::OutputDataFile(string filename) :
   m_file(filename.c_str(), std::ios::out)
{
   if (!m_file)
   {
      throw std::runtime_error("OutputDataFile: Unable to create file!\n");
   }

   m_file << std::setprecision(10);
}

void OutputDataFile::writeInt(string name, int value)
{
   m_check();
   m_file << "I\t" << name << '\t' << value << std::endl;
}

void OutputDataFile::writeFloat(string name, double value)
{
   m_check();
   m_file << "F\t" << name << '\t' << value << std::endl;
}

void OutputDataFile::writeString(string name, string value)
{
   m_check();
   m_file << "S\t" << name << '\t' << value << std::endl;
}

void OutputDataFile::writeComment(string comment)
{
   m_check();
   m_file << "#" << comment << '\n';
}

void OutputDataFile::m_check()
{
   if (!m_file.good())
      throw std::runtime_error("OutputDataFile: File is no longer valid!\n");
}
