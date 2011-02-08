#include "TAMUWW/SpecialTools/interface/Value.hh"

#include <ostream>
#include <iomanip>
#include <iostream>

Value::Value() : value(0), error(0) {}
Value::Value(double val, double err) : value(val), error(err) {}

Value::~Value() {}

Value::Value(const Value & nv){
  value = nv.value;
  error = nv.error;
}
Value & Value::operator= (const Value & nv) {
  value = nv.value;
  error = nv.error;
  return *this;
}

std::ostream& operator<< (std::ostream& output, const Value& val)
{
   output << val.value << " &plusmn; " << val.error;

   return output;
}

ClassImp(Value)
