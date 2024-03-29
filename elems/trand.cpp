/***************************************************************************
                          trand.cpp  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2022 by atu
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
 "Integer <b>seedType</b> -- type of seeding\n"
 "Flag <b>addBaseSeed</b> -- add value of base seed to current\n"
 "Double parameters <b>ampl, zval, sigma, a, b, c</b> can be changed at any time.<br>\n"
 "About distributions and parameters see GSL documentation (info:gsl-ref).<br></p>\n"
;


STD_CLASSINFO(TRand,clpElem);

CTOR(TRand,TMiso)
{
}



// =========================== meat ================================

int TRand::do_preRun()
{
  eff_seedType = seedType;
  if( seedType == asModel ) {
    getUpData( "seedType", &eff_seedType );
  };
  bseed = 0;
  if( addBaseSeed ) {
    getUpData( "seed", &bseed );
  };
  hseed = 0;
  if( addHash ) {
    hseed = qHash( getFullName() );
  };
  return 1;
}


int TRand::miso_startLoop( long acnx, long acny )
{
  old_val = 0; sp_time = DMAX; old_in = 0;
  if( (eff_seedType == everyRun ) ||                // need to seed now
      (eff_seedType == startLoop && acnx == 0 ) ||
      (acnx == 0 && acny == 0)
    ) {
    if( seed == -1 || ( addBaseSeed && (bseed == -1) ) ) {
      sseed = int( time(0) + acnx * 1001 + acny + hseed );
    } else {
      sseed = seed + bseed + hseed;
    }
    rng.set( sseed );
  };
  x2 = out0 * out0;
  return 1;
}

double TRand::f() noexcept
{
  double du, in;
  if( inputTime ) {
    in = ct;
  } else {
    in = in_t;
  }

  if( sp_time >= tau || tau <=0  ) {
    sp_time -= tau;
    if( sp_time >= tau ) {
      sp_time = 0;
    }
    double v;
    switch( (int)type ) {
      case dt_flat     :  v = rng.flat( -sigma, sigma ); break;
      case dt_gauss    :  v = rng.gaussian( sigma ); break;
      case dt_gausstail:  v = rng.gaussian_tail( a, sigma ); break;
      case dt_exp      :  v = rng.exponential( sigma ); break;
      case dt_laplace  :  v = rng.laplace( a ); break;
      case dt_exppow   :  v = rng.exppow(  a, b ); break;
      case dt_beta     :  v = rng.beta(  a, b ); break;
      case dt_cauchy   :  v = rng.cauchy( a ); break;
      case dt_chisq    :  v = rng.chisq( a ); break;
      case dt_erlang   :  v = rng.erlang( a, b ); break;
      case dt_fdist    :  v = rng.fdist( a, b ); break;
      case dt_gamma    :  v = rng.gamma( a, b ); break;
      case dt_levy     :  v = rng.levy( a, b ); break;
      case dt_logistic :  v = rng.logistic( a ); break;
      case dt_lognorm  :  v = rng.lognormal( a, sigma ); break;
      case dt_pareto   :  v = rng.pareto( a, b ); break;
      case dt_rayleigh :  v = rng.rayleigh( sigma ); break;
      case dt_weibull  :  v = rng.weibull( a, b ); break;
      case dt_none     :  v = 0;  break;
      default          :  v = 0;  break;
    };
    old_val = zval + ampl * v;
  };
  du = in - old_in; old_in = in;
  sp_time += du;
  double v =  old_val + in_v.cval();

  if( useDiscr ) {
    v -= d_base;
    long k = (long)round( ( v ) / d_h );
    v = k * d_h;
    v += d_base;
  }
  x2 = pow2( v );
  return v;
}

DEFAULT_FUNCS_REG(TRand)


// end of trand.cpp


