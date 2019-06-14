/***************************************************************************
                          tpid.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#include "tpid.h"

const char* TPid::helpstr = "<H1>TPid</H1>\n"
 "<p>Integrators, differentiators and proportional element: <br>\n"
 "Have 6 parameters: <b>kd2, kd1, kp, ki1, ki1, aver</b>.\n"
 "Coefficients can be changed at any time, aver - no.</p>";

STD_CLASSINFO(TPid,clpElem );

CTOR(TPid,TMiso)
{
}


int TPid::do_preRun()
{
  tdt2 = ctdt * ctdt;
  return 1;
}

int TPid::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  vi1 = vi2 = 0; u_old = u_old2 = out0;
  start = 1;
  return 1;
}

double TPid::f() noexcept
{
  double uc = in_u;

  if( start ) {
    u_old = u_old2 = uc;
    start = 0;
  }

  d1 = ( uc - u_old ) / ctdt;
  d2 = ( uc - 2*u_old + u_old2 ) / tdt2;
  vi1 += uc * ctdt;
  vi2 += vi1 * ctdt;
  double v = kd2 * d2 + kd1 * d1 + kp * uc + ki1 * vi1 + ki2 * vi2 + b;

  if( aver && ct > 0 ) {
    v /= ct;
  }
  u_old2 = u_old; u_old = uc;
  x2 = v * v;
  return v;
}

DEFAULT_FUNCS_REG(TPid)


// end of tpid.cpp


