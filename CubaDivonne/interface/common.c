/*
	common.c
		includes most of the modules
		this file is part of Divonne
		last modified 5 May 09 th
*/


static bool Explore(count iregion, cSamples *samples, cint depth, cint flags);

static void Split(count iregion, int depth);

#include "TAMUWW/CubaDivonne/interface/Random.c"
#include "TAMUWW/CubaDivonne/interface/ChiSquare.c"
#include "TAMUWW/CubaDivonne/interface/Rule.c"
#include "TAMUWW/CubaDivonne/interface/Sample.c"
#include "TAMUWW/CubaDivonne/interface/FindMinimum.c"
#include "TAMUWW/CubaDivonne/interface/Explore.c"
#include "TAMUWW/CubaDivonne/interface/Split.c"
#include "TAMUWW/CubaDivonne/interface/Integrate.c"


static inline bool BadDimension(ccount ndim, cint flags, ccount key)
{
#if NDIM > 0
  if( ndim > NDIM ) return true;
#endif
  if( IsSobol(key) ) return
    ndim < SOBOL_MINDIM || (!PSEUDORNG && ndim > SOBOL_MAXDIM);
  if( IsRule(key, ndim) ) return ndim < 1;
  return ndim < KOROBOV_MINDIM || ndim > KOROBOV_MAXDIM;
}


static inline bool BadComponent(cint ncomp)
{
#if NCOMP > 0
  if( ncomp > NCOMP ) return true;
#endif
  return ncomp < 1;
}

