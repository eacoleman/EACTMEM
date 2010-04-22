/*
	common.c
		includes most of the modules
		this file is part of Cuhre
		last modified 14 Feb 05 th
*/


#include "TAMUWW/CubaCuhre/interface/ChiSquare.c"
#include "TAMUWW/CubaCuhre/interface/Rule.c"
#include "TAMUWW/CubaCuhre/interface/Integrate.c"


static inline bool BadDimension(ccount ndim)
{
#if NDIM > 0
  if( ndim > NDIM ) return true;
#endif
  return ndim < 2;
}


static inline bool BadComponent(cint ncomp)
{
#if NCOMP > 0
  if( ncomp > NCOMP ) return true;
#endif
  return ncomp < 1;
}

