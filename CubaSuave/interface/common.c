/*
	common.c
		includes most of the modules
		this file is part of Suave
		last modified 14 Feb 05 th
*/


#include "TAMUWW/CubaSuave/interface/Random.c"
#include "TAMUWW/CubaSuave/interface/ChiSquare.c"
#include "TAMUWW/CubaSuave/interface/Grid.c"
#include "TAMUWW/CubaSuave/interface/Sample.c"
#include "TAMUWW/CubaSuave/interface/Fluct.c"
#include "TAMUWW/CubaSuave/interface/Integrate.c"


static inline bool BadDimension(cint ndim, cint flags)
{
#if NDIM > 0
  if( ndim > NDIM ) return true;
#endif
  return ndim < SOBOL_MINDIM || (!PSEUDORNG && ndim > SOBOL_MAXDIM);
}


static inline bool BadComponent(cint ncomp)
{
#if NCOMP > 0
  if( ncomp > NCOMP ) return true;
#endif
  return ncomp < 1;
}

