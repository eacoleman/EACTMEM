#ifndef DATAFILE_HH
#define DATAFILE_HH

#include <fstream>
#include <map>
#include <string>

class InputDataFile
{
   public:
      explicit InputDataFile(std::string filename);

      int readInt(std::string name) const;
      double readFloat(std::string name) const;
      std::string readString(std::string name) const;

   private:
      std::map<std::string, int> m_intMap;
      std::map<std::string, float> m_floatMap;
      std::map<std::string, std::string> m_stringMap;
};

class OutputDataFile
{
   public:
      explicit OutputDataFile(std::string filename);

      void writeInt(std::string name, int value);
      void writeFloat(std::string name, double value);
      void writeString(std::string name, std::string value);
      void writeComment(std::string comment);

   private:
      std::ofstream m_file;
      void m_check();
};

#endif
