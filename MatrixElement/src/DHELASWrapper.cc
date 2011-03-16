#include "TAMUWW/MatrixElement/interface/DHELASWrapper.hh"

std::ostream& operator<<(std::ostream& stream, const DHELAS::HelArray& rhs)
{
   stream << "(" << rhs.p1 << ", " << rhs.p2 << ", " << rhs.p3 << ", "
          << rhs.p4 << ", " << rhs.p5 << ", " << rhs.p6 << ")";
   return stream;
}
