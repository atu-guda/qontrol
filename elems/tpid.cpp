/***************************************************************************
                          tpid.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#include "tpid.h"

const char* TPid::helpstr = "<H1>TPid</H1>\n"
 "Integrators, differenciators and proportional element: <br>\n"
 "Have 6 parameters: <b>kd2, kd1, kp, ki1, ki1, aver</b>.\n"
 "Coefficients can be changed at any time, aver - no.";

TClassInfo TPid::class_info = {
  "TPid", TPid::create,
  &TMiso::class_info, helpstr, clpElem };


TPid::TPid( TDataSet* aparent )
        :TMiso( aparent )
{
  kd2 = kd1 = ki2 = ki1 = 0; kp = 1; aver = 0;
  tdt2 = 1; vi1 = vi2 = u_old = u_old2 = 0;
}

TPid::~TPid()
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
  double v, d1, d2;
  v = 0;
  if( start == 1 ) {
    start = 2; u_old = *in_so[0]; vi1 += *in_so[0] * tdt; vi2 += vi1 * tdt;
    v = kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  if( start == 2 ) {
    start = 0; d1 = ( *in_so[0] - u_old ) / tdt;
    vi1 += *in_so[0] * tdt; vi2 += vi1 * tdt;
    u_old2 = u_old; u_old = *in_so[0];
    v = kd1 * d1 + kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  d1 = ( *in_so[0] - u_old ) / tdt;
  d2 = ( *in_so[0] - 2*u_old + u_old2 ) / tdt2;
  vi1 += *in_so[0] * tdt;
  vi2 += vi1 * tdt;
  v = kd2 * d2 + kd1 * d1 + kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
  if( aver )
    v /= t;
  u_old2 = u_old; u_old = *in_so[0];
  return v; 
}

DEFAULT_FUNCS_REG(TPid)


// end of tpid.cpp


