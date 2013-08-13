/***************************************************************************
                          trand.cpp  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2013 by atu
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

#include <time.h>
#include <math.h>
#include <miscfun.h>
#include "tmodel.h"
#include "trand.h"

const char* TRand::helpstr = "<H1>TRand</H1>\n"
 "Random values signal (noise) generator: <br>\n"
 "Integer parameter <b>type</b> selects type of distribution (see GSL docs).<br>\n"
 "Argument <b>tau</b> defined recalculation time of generator<br>.\n"
 "Argument <b>ampl</b> - scale of output.<br>\n"
 "Argument <b>zval</b> - zero level of output.<br>\n"
 "Arguments <b>sigma, a, b, c</b> -- may me parameters of distribution.<br>\n"
 "Integer parameter <b>seed</b> -- seed of generator, if -1 -- new seed every seeding.<br>\n"
 "Integer <b>seedType</b> -- type of seedeng\n"
 "Flag <b>addBaseSeed</b> -- add value of base seed to current\n"
 "Double parameters <b>ampl, zval, sigma, a, b, c</b> can be changed at any time.<br>\n"
 "About distributions and parameters see GSL documentation.<br>\n"
 "Input is u[0], <b>not time</b>. If need time, link first input to 't'.<br>\n"
;


STD_CLASSINFO(TRand,clpElem);

CTOR(TRand,TMiso)
{
  type = 0;
  tau = 0.05; ampl = 1; zval = 0;
  sigma = a = b = c = 1;
  seed = 2942 + ( time(0) & 0x07FF );
  seedType = 3; addBaseSeed = 1;
  old_val = 0; sp_time = 1e300; old_in = 0; sseed = 0; bseed = 0;
  rng = 0;
}

TRand::~TRand()
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  };
}


// =========================== meat ================================

int TRand::do_preRun( int /*run_tp*/, int /*an*/, 
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

int TRand::do_postRun( int /*good*/ )
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  };
  return 0;
}

int TRand::do_startLoop( int acnx, int acny )
{
  old_val = 0; sp_time = 1e300; old_in = 0;
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

double TRand::f( double  /*t*/  )
{
  double v, du;
  if( sp_time >= tau || tau <=0  ) {
    sp_time -= tau;
    if( sp_time >= tau )
      sp_time = 0;
    switch( (int)type ) {
      case 0:  v = gsl_ran_flat( rng, -sigma, sigma ); break;
      case 1:  v = gsl_ran_gaussian( rng, sigma ); break;
      case 2:  v = gsl_ran_gaussian_tail( rng, a, sigma ); break;
      case 3:  v = gsl_ran_exponential( rng, sigma ); break;
      case 4:  v = gsl_ran_laplace( rng, a ); break;
      case 5:  v = gsl_ran_exppow( rng, a, b ); break;
      default: v = 0;
    };
    old_val = zval + ampl * v;
  };
  du = *in_so[0] - old_in; old_in = *in_so[0];
  sp_time += du;
  return old_val; 
}

DEFAULT_FUNCS_REG(TRand)


// end of trand.cpp


