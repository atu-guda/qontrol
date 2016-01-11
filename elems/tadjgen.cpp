/***************************************************************************
                          tadjgen.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2016 by atu
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

#include <cmath>
using namespace std;

#include "tadjgen.h"

const char* TAdjGen::helpstr = "<H1>TAdjGen</H1>\n"
 "Generator of 0/1, controlled on input frequency u[0]: <br>\n"
 "Have 9 parameters: <b>type, useReset, useLock, outStrobe</b>,<br>\n"
 " <b> useSignStrobe, usePlusStrobe, useF, omega_0, k_omega </b>,<br>\n"
 "None of the can be changed at run time.";

STD_CLASSINFO(TAdjGen,clpElem );

CTOR(TAdjGen,TMiso)
  , real_tick(0), cav(0), cav2(0)
{
}


int TAdjGen::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  ctt = ig = ig2 = dlt = tick_T = tick_old = av = av2 = av_diff = 0;
  currOut = 0; real_tick = 0; tick = 0;
  cav = cav2 = 0;
  return 1;
}

double TAdjGen::f() noexcept
{
  double om, om2, df, ff, v, diff_out = 0;
  int g1, g2;
  om = in_omega; om2 = in_omega2;
  cav += om * tdt;  cav2 += om2 * tdt;
  if( useF ) {
    om  = omega_0 * ( 1 + om*k_omega );
    om2 = omega_0 * ( 1 + om2*k_omega );
  };
  real_tick = 0; tick = 0;
  switch( (int)type ) {
    case gen_def: ig += om * tdt;                 // Pi=int_0^T(om*dt);
            if( useReset && in_rst > 0.01 ) {
              real_tick = 1; break;
            };
            if( useLock && in_lock > 0.01 ) {
              break;
            }
            if( ig > M_PI ) {
              real_tick = 1;
            };
            break;
    case gen_mai:
            if( om < 1.0e-100 )
              om = 1.0e-50;
            ctt += tdt; ff = 1.0 / om;  df = tdt * ff;
            ig += df * M_PI;
            if( useReset && in_rst > 0.01 ) {
              real_tick = 1; break;
            };
            if( useLock && in_lock > 0.01 )
              break;
            if( ctt*ctt > ig ) {
              real_tick = 1;
            };
            break;
    case gen_dual: ig += om * tdt; ig2 += om2 * tdt;
            g1 = ( ig  > M_PI );
            g2 = ( ig2 > M_PI );
            if( useReset && in_rst > 0.01 ) {
              real_tick = 1; break;
            };
            if( useLock && in_lock > 0.01 ) {
              break;
            }
            if( g1 && g2 ) {
              real_tick = 1;
              dlt = ig - ig2;
            };
            diff_out = g1 - g2;
            break;
    default: v = 0;
  };

  tick = real_tick;
  if( real_tick ) {
    ctt = ig = ig2 = 0;
    tick_T = t - tick_old;
    tick_old = t;
    av = cav / tick_T; av2 = cav2 / tick_T; av_diff = av - av2;
    cav = cav2 = 0;
    currOut = !currOut;
  };

  if( type == gen_dual ) {
    return diff_out;
  }

  v = currOut ? 1.0 : ( useZero ? 0.0 : -1.0 );
  if( !outStrobe )
    return v;

  if( !real_tick )
    return 0;

  if( usePlusStrobe && v < 1 )
    return 0;

  return ( useSignStrobe ) ? v : 1;
}

DEFAULT_FUNCS_REG(TAdjGen)


// end of tadjgen.cpp


