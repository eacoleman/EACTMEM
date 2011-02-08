#ifndef VALUE_HH
#define VALUE_HH

#include <cmath>
#include "TObject.h"
#include <iostream>

// ---- Value ----
// Simple value + error pair with operations defined
class Value : public TObject
{
public:

  Value();// : value(0), error(0) {}
   Value(double val, double err);// : value(val), error(err) {}

  ~Value();//  {}

 
  Value(const Value & p); //cpy constructor
  Value & operator= (const Value & nv); // assignement operator



      double value;
      double error;

      inline Value& operator+= (double);
      inline Value operator+ (double) const;
      inline Value& operator-= (double);
      inline Value operator- (double) const;
      inline Value& operator*= (double);
      inline Value operator* (double) const;
      inline Value& operator/= (double);
      inline Value operator/ (double) const;

      inline Value& operator+= (const Value&);
      inline Value operator+ (const Value&) const;
      inline Value& operator-= (const Value&);
      inline Value operator- (const Value&) const;
      inline Value& operator*= (const Value);
      inline Value operator* (const Value&) const;
      inline Value& operator/= (const Value&);
      inline Value operator/ (const Value&) const;

      inline bool operator< (const Value&) const;

      ClassDef(Value, 1)
};


std::ostream& operator<< (std::ostream&, const Value&);

inline Value& Value::operator+= (double input)
{
   value += input;
   return *this;
}
   
inline Value Value::operator+ (double input) const
{
   Value response(value, error);
   return response += input;
}

inline Value& Value::operator-= (double input)
{
   value -= input;
   return *this;
}
   
inline Value Value::operator- (double input) const
{
   Value response(value, error);
   return response -= input;
}

inline Value& Value::operator*= (double input)
{
   value *= input;
   error *= input;
   return *this;
}
   
inline Value Value::operator* (double input) const
{
   Value response(value, error);
   return response *= input;
}

inline Value& Value::operator/= (double input)
{
   value /= input;
   error /= input;
   return *this;
}
   
inline Value Value::operator/ (double input) const
{
   Value response(value, error);
   return response /= input;
}

inline Value& Value::operator+= (const Value& input)
{
   error = std::sqrt(std::pow(error, 2) + std::pow(input.error, 2));
   value += input.value;
   return *this;
}
   
inline Value Value::operator+ (const Value& input) const
{
   Value response(value, error);
   return response += input;
}

inline Value& Value::operator-= (const Value& input)
{
   error = std::sqrt(std::pow(error, 2) + std::pow(input.error, 2));
   value -= input.value;
   return *this;
}
   
inline Value Value::operator- (const Value& input) const
{
   Value response(value, error);
   return response -= input;
}

inline Value& Value::operator*= (const Value input)
{
   error = std::sqrt(std::pow(input.value * error, 2)
                     + std::pow(input.error * value, 2));
   value *= input.value;
   return *this;
}
   
inline Value Value::operator* (const Value& input) const
{
   Value response(value, error);
   return response *= input;
}

inline Value& Value::operator/= (const Value& input)
{
   error = std::sqrt(std::pow(error / input.value, 2)
                     + std::pow(value * input.error /
                                std::pow(input.value, 2), 2));
   value /= input.value;
   return *this;
}
   
inline Value Value::operator/ (const Value& input) const
{
   Value response(value, error);
   return response /= input;
}

inline bool Value::operator< (const Value& input) const
{
   return value < input.value;
}

#endif
