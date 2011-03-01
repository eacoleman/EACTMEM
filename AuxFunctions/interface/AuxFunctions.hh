#ifndef AUXFUNCTIONS_HH
#define AUXFUNCTIONS_HH

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "TH1.h"

namespace AuxFunctions
{
  std::string removeFunnyChar(const std::string& input);

  void copyHistStyle(const TH1* from, TH1* to);

// ---- checkedMapGet ----
// Checked version for getting stuff from const maps
   template <class K, class V>
   V checkedMapGet(const std::map<K, V>& map, K key)
   {
      typename std::map<K, V>::const_iterator it = map.find(key);
      
      if (it == map.end())
      {
         throw std::runtime_error("Invalid key in checkedGet!");
      }
      return it->second;
   }

// ---- fillVec ----
// Fills a vector from an array
// This is silly and pointless and should not be used
// The proper way to do it is to call a vector with (array, array + size)
// as the constructor arguments.
   template <class T>
   std::vector<T> fillVec(T array[], unsigned size)
   {
      std::vector<T> response;
      response.reserve(size);

      for (unsigned i = 0; i < size; ++i)
      {
         response.push_back(array[i]);
      }

      return response;
   }

// ---- stringSub ----
// Substitute part of a string for another part
// Returns whether a substitution was made
   bool stringSub(std::string& input, std::string replace, std::string with);

// ---- concatString ----
// Put two things together and make them a string
// Must be a streamable object
   template <class T, class U>
   std::string concatString(const T& obj1, const U& obj2)
   {
      std::ostringstream output;
      output << obj1 << obj2;
      return output.str();
   }

   namespace Math
   {
      template <class T>
      inline T pow(T base, unsigned operand)
      {
         T response = 1;

         for (unsigned i = 0; i < operand; ++i)
         {
            response *= base;
         }

         return response;
      }

      template <class T>
      inline T square(T base)
      {
         return base * base;
      }

      template <class T>
      inline T cube(T base)
      {
         return base * base * base;
      }
   }
   
  std::vector<std::string> splitLineIntoWords(std::string line,  const std::string & match);

  void trimSpaces(std::string& str);


}
#endif
