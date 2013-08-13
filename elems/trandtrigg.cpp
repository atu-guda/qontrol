/***************************************************************************
                          trandtrigg.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include "miscfun.h"
#include "tmodel.h"
#include "trandtrigg.h"


const char* TRandTrigg::helpstr = "<H1>TRandTrigg</H1>\n"
 "Random - deterministic trigger: <br>\n"
 "it sense change or level of input signal u[0],<br>\n"
 "than if forced state or good probability, change output signal.<br>\n"
 "Parametr <b>prob</b> defines probability of change.<br>\n"
 "Parametr <b>nforce</b> defines how often will be forced click. .<br>\n"
 "Parametrs <b>seed, seedType, addBaseSeed </b> controls random ganerator<br>\n"
 "Parameters: <b>useLevel, useForce, useLock, useZero</b>, - misc flags.";

STD_CLASSINFO(TRandTrigg,clpElem);

CTOR(TRandTrigg,TMiso)
{
  prob = 0.5; u_old = 0; nforce = 2;
  useLevel = useForce = useLock = useZero = 0; ns = -1;
  seed = 9561 + (time(0) & 0x1AFFF );
  seedType = 3; addBaseSeed = 1;
  rng = 0;
}

TRandTrigg::~TRandTrigg()
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  }
}


int TRandTrigg::do_preRun( int /*run_tp*/, int /*an*/, 
                           int /*anx*/, int /*any*/, double /*adt*/ )
{
  const gsl_rng_type *t = gsl_rng_default;
  rng = gsl_rng_alloc( t );
  eff_seedType = seedType;
  if( seedType == 3 ) { // as model 
    model->getData( "seedType", &eff_seedType ); 
  };
  bseed = 0;
  if( addBaseSeed ) {
    model->getData( "seed", &bseed );
  };
  return 0;
}

int TRandTrigg::do_postRun( int /*good*/ )
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  }
  return 0;
}

int TRandTrigg::do_startLoop( int acnx, int acny )
{
  ns = -1; u_old = 0; currOut = 0;
  if( (eff_seedType == 0) ||                // need to seed now
      (eff_seedType == 1 && acnx == 0 ) ||
      (acnx == 0 && acny == 0) 
    ) {
    if( seed == -1 || ( addBaseSeed && bseed == -1) ) {
      sseed = int( time(0) + acnx * 1001 + acny );
    } else {
      sseed = seed + bseed;
    }
    gsl_rng_set( rng, sseed );
  };
  return 0;
}


double TRandTrigg::f( double /* t */ )
{
  double du, rv;
  du = *in_so[0] - u_old; u_old = *in_so[0];
  if( ns == -1 ) { du = 0; ns = 0; };
  if( useForce && *in_so[1] > 0.1 ) {
    ns = 0; currOut = !currOut;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( useLock && *in_so[2] > 0.1 ) {
    // ns++;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( ( useLevel && *in_so[0] > 0.1 ) || ( !useLevel && fabs(du) > 0.5 ) ) {
    rv = gsl_ran_flat( rng, 0, 1 ); 
    if( rv <= prob || nforce <= 1 || ns >= nforce-1 ) {
      ns = 0; currOut = !currOut;
    } else {
      ns++;
    };
  };
  return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
}

DEFAULT_FUNCS_REG(TRandTrigg)


// end of trandtrigg.cpp


