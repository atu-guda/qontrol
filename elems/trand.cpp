/***************************************************************************
                          trand.cpp  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2014 by atu
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
#include <cmath>
#include <miscfun.h>
#include "tmodel.h"
#include "trand.h"

const char* TRand::helpstr = "<H1>TRand</H1>\n"
 "<p>Random values signal (noise) generator: <br>\n"
 "Integer parameter <b>type</b> selects type of distribution (see GSL docs).<br>\n"
 "Argument <b>tau</b> defined recalculation time of generator<br>.\n"
 "Argument <b>ampl</b> - scale of output.<br>\n"
 "Argument <b>zval</b> - zero level of output.<br>\n"
 "Flag <b>InputTime</b> - if set, uses time is input, not in_t (def).<br>\n"
 "Arguments <b>sigma, a, b, c</b> -- may me parameters of distribution.<br>\n"
 "Integer parameter <b>seed</b> -- seed of generator, if -1 -- new seed every seeding.<br>\n"
 "Integer <b>seedType</b> -- type of seedeng\n"
 "Flag <b>addBaseSeed</b> -- add value of base seed to current\n"
 "Double parameters <b>ampl, zval, sigma, a, b, c</b> can be changed at any time.<br>\n"
 "About distributions and parameters see GSL documentation (info:gsl-ref).<br></p>\n"
;


STD_CLASSINFO(TRand,clpElem);

CTOR(TRand,TMiso)
{
}



// =========================== meat ================================

int TRand::do_preRun( int /*run_tp*/, int /*an*/,
                     int /*anx*/, int /*any*/, double /*adt*/ )
{
  eff_seedType = seedType;
  if( seedType == 3 ) { // as model
    par->getData( "seedType", &eff_seedType );
  };
  bseed = 0;
  if( addBaseSeed ) {
    par->getData( "seed", &bseed );
  };
  return 0;
}


int TRand::do_startLoop( int acnx, int acny )
{
  old_val = 0; sp_time = DMAX; old_in = 0;
  if( (eff_seedType == 0) ||                // need to seed now
      (eff_seedType == 1 && acnx == 0 ) ||
      (acnx == 0 && acny == 0)
    ) {
    if( seed == -1 || ( addBaseSeed && bseed == -1) ) {
      sseed = int( time(0) + acnx * 1001 + acny );
    } else {
      sseed = seed + bseed;
    }
    rng.set( sseed );
  };
  return 0;
}

double TRand::f( double  t  )
{
  double v, du, in;
  if( inputTime ) {
    in = t;
  } else {
    in = in_t;
  }

  if( sp_time >= tau || tau <=0  ) {
    sp_time -= tau;
    if( sp_time >= tau )
      sp_time = 0;
    switch( (int)type ) {
      case 0:  v = rng.flat( -sigma, sigma ); break;
      case 1:  v = rng.gaussian( sigma ); break;
      case 2:  v = rng.gaussian_tail( a, sigma ); break;
      case 3:  v = rng.exponential( sigma ); break;
      case 4:  v = rng.laplace( a ); break;
      case 5:  v = rng.exppow(  a, b ); break;
      case 6:  v = rng.beta(  a, b ); break;
      case 7:  v = rng.cauchy( a ); break;
      case 8:  v = rng.chisq( a ); break;
      case 9:  v = rng.erlang( a, b ); break;
      case 10: v = rng.fdist( a, b ); break;
      case 11: v = rng.gamma( a, b ); break;
      case 12: v = rng.levy( a, b ); break;
      case 13: v = rng.logistic( a ); break;
      case 14: v = rng.lognormal( a, sigma ); break;
      case 15: v = rng.pareto( a, b ); break;
      case 16: v = rng.rayleigh( sigma ); break;
      case 17: v = rng.weibull( a, b ); break;
      default: v = 0;
    };
    old_val = zval + ampl * v;
  };
  du = in - old_in; old_in = in;
  sp_time += du;
  return old_val;
}

DEFAULT_FUNCS_REG(TRand)


// end of trand.cpp


