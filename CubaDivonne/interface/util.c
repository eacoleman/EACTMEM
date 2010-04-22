/*
	util.c
		Utility functions
		this file is part of Divonne
		last modified 9 Apr 09 th
*/


#include "TAMUWW/CubaDivonne/interface/decl.h"

static count ndim_, ncomp_, selectedcomp_, nregions_;
static number neval_, neval_opt_, neval_cut_;
static int sign_, phase_;

static Bounds border_;

static Samples samples_[3];
static Rule rule7_, rule9_, rule11_, rule13_;
static real *xgiven_, *fgiven_, *xextra_, *fextra_;
static count ldxgiven_;
static number ngiven_, nextra_;

static Totals *totals_;

static void *voidregion_;
#define region_ ((Region *)voidregion_)
static count size_;

#ifdef DEBUG
#include "debug.c"
#endif

