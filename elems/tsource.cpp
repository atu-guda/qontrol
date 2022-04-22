/***************************************************************************
                          tsource.cpp  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000-2019 by atu
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
#include "miscfun.h"
#include "scheme.h"
#include "tsource.h"

using namespace std;

const char* TSource::helpstr = "<H1>TSource</H1>\n"
 "<p>Source of different kind of signals: <br>\n"
 "Have N parameters: <b>type, U, omega, .. b</b>,"
 "each can be changed at any time .</p>";

STD_CLASSINFO(TSource,clpElem|clpCalcAtStart);

CTOR(TSource,TMiso)
{
}



double TSource::f() noexcept
{
  double omega_x = omega;
  double tau_x   = omega_x;
  if( use_rfreq ) {
    omega_x *= 2 * M_PI;
  }
  T_0 = 2 * M_PI / omega_x;
  T_1 = T_0 * ( dc - dc0 );
  double omet = ct * omega_x + phi;

  double u_ch = 1;
  if( use_u_ch ) { // chaos on U
    if( ct < u_ch_te ) {
      u_ch = u_ch_vs + ( ct - u_ch_ts ) * u_ch_k;
    } else {
      u_ch = u_ch_vs = u_ch_ve;
      u_ch_ve = rng_u.flat( u_ch_v0, u_ch_vm );
      u_ch_ts = ct;
      u_ch_te = ct + rng_u.flat( u_ch_t0, u_ch_tm );
      u_ch_k = ( u_ch_ve - u_ch_vs ) / ( u_ch_te - u_ch_ts );
    };
  }; // end if( u_chaos )

  double f_ch = 0;
  if( use_f_ch ) { // chaos on phase
    if( ct < f_ch_te ) {
      f_ch = f_ch_vs + ( ct - f_ch_ts ) * f_ch_k;
    } else {
      f_ch = f_ch_vs = f_ch_ve;
      f_ch_ve = rng_p.flat( f_ch_v0, f_ch_vm );
      f_ch_ts = ct;
      f_ch_te = ct + rng_p.flat( f_ch_t0, f_ch_tm );
      f_ch_k = ( f_ch_ve - f_ch_vs ) / ( f_ch_te - f_ch_ts );
    };
  }; // end if( f_chaos )

  double uu_s = uu * u_ch;
  double omet_s = omet + f_ch; // note u: * fi +
  pha = omet_s * M_1_PI * 0.5;
  double old_pha_0 = pha_0;
  pha_0 = fmod( pha, 1 ); // phase in range [ 0; 1 )
  bool pha_flip = pha_0 < old_pha_0;

  double v, cv1;
  switch( (int)type ) {
    case so_sin:
      v = uu_s * sin( omet_s ); cv1 = -v; break;
    case so_sign:
      v = uu_s * sign( sin( omet_s ) ); cv1 = -v; break;
    case so_sin_raise:
      v = uu_s * sin( omet_s ) * ct / tt; cv1 = -v; break;
    case so_sign_raise:
      v = uu_s * sign( sin( omet_s ) ) * ct / tt; cv1 = -v; break;
    case so_dirac:
      v = 0;
      if( was_pulse == 0 && ct >= tau_x ) {
        v = uu_s / ctdt; was_pulse = 1;
      };
      cv1 = -v;
      break;
    case so_theta:
      v = ( ct > tau_x ) ? uu_s : 0;  cv1 = uu_s - v; break;
    case so_raise:
      v = uu_s * ( ct + f_ch ) / tt;  cv1 = uu_s - v; break;
    case so_saw:
      v   = uu_s * pha_0;
      cv1 = uu_s - v;
      break;
    case so_saw2:
      v = 2 * pha_0;
      if( v > 1 ) {
        v = 2 - v;
      }
      v *= uu_s;
      cv1 = uu_s - v;
      break;
    case so_chaos_wave:
      v = uu_s * f_ch;  cv1 = uu_s - v; break;
    case so_triangle:
      if( pha_0 <= 0.25 )
        v = uu_s * 4 * pha_0;
      else if ( pha_0 <= 0.75 )
        v = uu_s * (  1 - 4 * ( pha_0 - 0.25 ) );
      else
        v = uu_s * ( -1 + 4 * ( pha_0 - 0.75 ) );
      cv1 = - v;
      break;
    case so_phase:
      v = pha_0; cv1 = 1 - v;
      break;
    case so_pulse:
      v = pha_flip ? uu_s : 0;   cv1 = uu_s - v;
      break;
    case so_pwm:
      v = ( pha_0 >= dc0 && pha_0 < dc ) ? uu_s : 0;  cv1 = uu_s - v;
      break;
    case so_ladder:
      v = uu_s * int( pha );
      cv1 = - v;
      break;
    default: v = 0; cv1 = uu_s;
  };

  if( !isInBounds( tmin.cval(), ct, tmax.cval() ) ) {
    v = 0; cv1 = uu_s;
  }

  v += cc; cv1 += cc;
  v  = clamp(   v, vmin.cval(), vmax.cval() );
  cv = clamp( cv1, vmin.cval(), vmax.cval() );
  u2 = pow2( v );
  return v;
}

int TSource::do_preRun()
{
  tt = rinf->T;
  if( use_u_ch ) {
    eff_seedType_u = seedType_u;
    if( seedType_u == SeedType::asModel ) {
      getUpData( QSL("seedType"), &eff_seedType_u );
    };
    bseed_u = 0;
    if( addBaseSeed_u ) {
      getUpData( QSL("seed"), &bseed_u );
    };
    hseed_u = 0;
    if( addHash_u ) {
      hseed_u = qHash( getFullName() );
    }
  };
  // Phi
  if( use_f_ch ) {
    eff_seedType_p = seedType_p;
    if( seedType_p == SeedType::asModel ) {
      getUpData( QSL("seedType"), &eff_seedType_p );
    };
    bseed_p = 0;
    if( addBaseSeed_p ) {
      getUpData( QSL("seed"), &bseed_p );
    };
    hseed_p = 0;
    if( addHash_p ) {
      hseed_p = qHash( getFullName() );
    }
  };

  return 1;
}


int TSource::miso_startLoop( long acnx, long acny )
{
  was_pulse = 0;
  pha = phi * M_1_PI * 0.5;
  pha_0 = fmod( pha, 1 );
  u2 = pow2( cc );
  cv = 0;
  T_0 = T_1 = 1; // FAKE

  // TODO: common fun
  // U rnd init
  if( use_u_ch ) {
    if( (eff_seedType_u == 0) ||                // need to seed now
        (eff_seedType_u == 1 && acnx == 0 ) ||
        (acnx == 0 && acny == 0)
      ) {
      if( seed_u == -1 || ( addBaseSeed_u && bseed_u == -1) ) {
        sseed_u = int( time(0) + acnx * 101 + acny*17 + hseed_u );
      } else {
        sseed_u = seed_u + bseed_u + hseed_u;
      }
      rng_u.set( sseed_u );
    };

    u_ch_vs = rng_u.flat( u_ch_v0, u_ch_vm );
    u_ch_ve = rng_u.flat( u_ch_v0, u_ch_vm );
    u_ch_ts = 0;
    u_ch_te = rng_u.flat( u_ch_t0, u_ch_tm );
    u_ch_k = ( u_ch_ve - u_ch_vs ) / ( u_ch_te - u_ch_ts );
  };

  // Phi rnd init
  if( use_f_ch ) {
    if( (eff_seedType_p == 0) ||                // need to seed now
        (eff_seedType_p == 1 && acnx == 0 ) ||
        (acnx == 0 && acny == 0)
      ) {
      if( seed_p == -1 || ( addBaseSeed_p && bseed_p == -1) ) {
        sseed_p = int( time(0) + acnx * 171 + acny*23 + 123 + hseed_p );
      } else {
        sseed_p = seed_p + bseed_p + hseed_p;
      }
      rng_p.set( sseed_p );
    };

    f_ch_vs = rng_p.flat( f_ch_v0, f_ch_vm );
    f_ch_ve = rng_p.flat( f_ch_v0, f_ch_vm );
    f_ch_ts = 0;
    f_ch_te = rng_p.flat( f_ch_t0, f_ch_tm );
    f_ch_k = ( f_ch_ve - f_ch_vs ) / ( f_ch_te - f_ch_ts );
  };

  return 1;
}

DEFAULT_FUNCS_REG(TSource)


// end of tsource.cpp

