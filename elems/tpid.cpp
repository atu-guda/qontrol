/***************************************************************************
                          tpid.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2014 by atu
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

#include "tpid.h"

const char* TPid::helpstr = "<H1>TPid</H1>\n"
 "<p>Integrators, differenciators and proportional element: <br>\n"
 "Have 6 parameters: <b>kd2, kd1, kp, ki1, ki1, aver</b>.\n"
 "Coefficients can be changed at any time, aver - no.</p>";

STD_CLASSINFO(TPid,clpElem );

CTOR(TPid,TMiso)
{
}


int TPid::do_preRun( int /*run_tp*/, int /*an*/,
                     int /*anx*/, int /*any*/, double /*adt*/ )
{
  if( tdt < 1e-100 )
    return 1;
  tdt2 = tdt * tdt;
  return 0;
}

int TPid::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  vi1 = vi2 = u_old = u_old2 = 0;
  start = 1;
  return 0;
}

double TPid::f( double t )
{
  double v = 0, d1, d2, uc = in_u;

  if( start == 1 ) {
    start = 2; u_old = uc; vi1 += uc * tdt; vi2 += vi1 * tdt;
    v = kp * uc + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  if( start == 2 ) {
    start = 0; d1 = ( uc - u_old ) / tdt;
    vi1 += uc * tdt; vi2 += vi1 * tdt;
    u_old2 = u_old; u_old = uc;
    v = kd1 * d1 + kp * uc + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  d1 = ( uc - u_old ) / tdt;
  d2 = ( uc - 2*u_old + u_old2 ) / tdt2;
  vi1 += uc * tdt;
  vi2 += vi1 * tdt;
  v = kd2 * d2 + kd1 * d1 + kp * uc + ki1 * vi1 + ki2 * vi2;
  if( aver )
    v /= t;
  u_old2 = u_old; u_old = uc;
  return v;
}

DEFAULT_FUNCS_REG(TPid)


// end of tpid.cpp


