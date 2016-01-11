/***************************************************************************
                          tcorralalysis.cpp  -  description
                             -------------------
    begin                : Fri Sep 8 2000
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
#include "tmodel.h"
#include "toutarr.h"
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
  s_x = s_x2 = s_y = s_y2 = s_xy = corr = cov = a = b = dis_x = dis_y = 0;
  sigma_x = sigma_y = 0;
  ave_x = ave_y = ave_x2 = ave_y2 = 0;
}

int TCorrAnalysis::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  reset_data();
  return 1;
}


double TCorrAnalysis::f() noexcept
{
  int add;
  double x = in_x, y = in_y;
  ++ii;
  if( useReset && in_rst > 0.1 ) { // history was < 0.1
    reset_data();
  };
  switch( (int)type ) {
    case call_all:
      add = 1; break;
    case call_time:
      add = ( t >= t0 ) && ( t <= t1 ); break;
    case call_u2:
      add = ( in_add > 0.1 ); break;
    default:
      add = 0;
  };
  if( add ) {
    s_x += x; s_x2 += x*x; s_y += y; s_y2 += y*y;
    s_xy += x * y;
    ++n;
  };
  if( ( ii >= model_nn-1 || ( useCalc && in_calc > 0.1 ))   ) {
    calc();
  };
  return s_x;
}


int TCorrAnalysis::calc()
{
  double dd;
  if( n < 1 ) {
    ok = 0;
    return 0;
  };
  double dn = (double)(n);
  double dn2 = dn * dn;

  ave_x  = s_x  / dn; ave_y  = s_y  / dn;
  ave_x2 = s_x2 / dn; ave_y2 = s_y2 / dn;
  dis_x = ( s_x2 * dn - s_x * s_x ) / dn2;
  if( dis_x < 0 ) { // may be due to estimation error
    dis_x = 0;
  }
  dis_y = ( s_y2 * dn - s_y * s_y ) / dn2;
  if( dis_y < 0 ) {
    dis_y = 0;
  }
  sigma_x = sqrt( dis_x );
  sigma_y = sqrt( dis_y );

  dd = n * s_x2 - s_x * s_x;
  cov = ( n * s_xy - s_x * s_y ) / ( dn2 );
  a = b = corr = 0; ok = 0;
  if( dd > 0 ) {
    a = ( n * s_xy - s_x * s_y ) / dd;
    b = ( s_y * s_x2 - s_x * s_xy ) / dd;
    double dz = ( n*s_x2 - s_x*s_x ) * ( n*s_y2 - s_y*s_y );
    if( dz > 0 ) {
      corr = ( n * s_xy - s_x * s_y )  / sqrt( dz );
      ok = 1;
    }
  }
  return ok;
}


DEFAULT_FUNCS_REG(TCorrAnalysis)


// end of tcorranalysis.cpp

