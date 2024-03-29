/***************************************************************************
                          tcorralalysis.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000-2022 by atu
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
#include "miscfun.h"
#include "tcorralalysis.h"


const char* TCorrAnalysis::helpstr = "<H1>TCorrAnalysis</H1>\n"
 "Correlational analysis for x=u0; y = u1: <br>\n"
 "Unstable: Information about parameters see in info.";

STD_CLASSINFO(TCorrAnalysis,clpElem );

CTOR(TCorrAnalysis,TMiso)
{
}



void TCorrAnalysis::reset_data()
{
  ok = n = ii = 0;
  corr = cov = a = b = dis_x = dis_y = a = b = corr = cov = 0;
  sigma_x = sigma_y = 0;
  ave_x = ave_y = ave_x2 = ave_y2 = 0;
  reset_counters();
}

void TCorrAnalysis::reset_counters()
{
  s_x = s_x2 = s_y = s_y2 = s_xy = 0;
  n = 0;
}

int TCorrAnalysis::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  reset_data();
  return 1;
}


double TCorrAnalysis::f() noexcept
{
  bool add = true;
  double x = in_x, y = in_y;
  ++ii;
  switch( (int)type ) {
    case call_all:
      break;
    case call_time:
      add = isInBounds( t0, ct, t1 ); break;
    case call_u2:
      add = in_add.lval(); break;
    default:
      add = false;
  };
  if( add ) {
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y;
    ++n;
  };
  if( ( ii >= (int)(rinf->N-1) || ( useCalc && in_calc ))   ) {
    calc();
  };
  if( useReset && in_rst ) {
    reset_counters();
  };
  return s_x;
}


int TCorrAnalysis::calc()
{
  a = b = corr = 0; ok = 0;
  if( n < 1 ) {
    return 0;
  };
  const double dn = (double)(n); // const here and down: error catch + optim?
  const double dn2 = dn * dn;

  ave_x  = s_x  / dn; ave_y  = s_y  / dn;
  ave_x2 = s_x2 / dn; ave_y2 = s_y2 / dn;
  dis_x = ( s_x2 * dn - s_x * s_x ) / dn2;
  dis_y = ( s_y2 * dn - s_y * s_y ) / dn2;
  sigma_x = sqrt0( dis_x );
  sigma_y = sqrt0( dis_y );

  const double dd = n * s_x2 - s_x * s_x;
  const double t1 = n * s_xy - s_x * s_y;
  cov = t1 / dn2;
  if( dd > 0 ) {
    a = t1 / dd;
    b = ( s_y * s_x2 - s_x * s_xy ) / dd;
    const double dz = ( n*s_x2 - s_x*s_x ) * ( n*s_y2 - s_y*s_y );
    if( dz > 0 ) {
      corr = t1 / sqrt( dz );
      ok = 1;
    }
  }
  return ok;
}


DEFAULT_FUNCS_REG(TCorrAnalysis)


// end of tcorranalysis.cpp

