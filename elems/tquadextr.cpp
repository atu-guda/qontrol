/***************************************************************************
                          tquadexrt.cpp - find top extranum in quadatic approx
                             -------------------
    begin                : 2015.02.27
    copyright            : (C) 2015-2015 by atu
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

#include "tquadextr.h"

const char* TQuadExtr::helpstr = "<H1>TQuadExtr</H1>\n"
 "Find upper extremum in quadratic approximation by given 3 points \n"
 "(may by with limits)";

STD_CLASSINFO(TQuadExtr,clpElem);

CTOR(TQuadExtr,TMiso)
{
}


double TQuadExtr::f( double /*t*/ )
{
  double x_ce, y_ce;
  if( emulC ) {
    x_ce = ( x_r + x_l ) / 2;
    y_ce = ( y_r + y_l ) / 2;
  } else {
    x_ce = x_c;
    y_ce = y_c;
  }
  // fallback values
  a_1 = 0; a_2 = 0; x_cnt = 0; x_cn = x_ce;

  x_lt = x_l  - x_ce;
  x_rt = x_r  - x_ce;
  y_lt = y_l  - y_ce;
  y_rt = y_r  - y_ce;

  // limit values
  double lim_x_lt = 0.5 * x_lt, lim_x_rt = 0.5 * x_rt;
  if( limitG ) {
    lim_x_lt = x_min - x_ce; lim_x_rt = x_max - x_ce;
  }

  double denom = x_lt * x_lt * x_rt - x_lt * x_rt * x_rt;
  if( fabs( denom ) < D_AZERO ) {
    return x_cn;
  }

  a_1 = ( y_rt * x_lt * x_lt - y_lt * x_rt * x_rt ) / denom;
  a_2 = - ( y_rt * x_lt - y_lt * x_rt ) / denom;

  x_cnt = - 0.5 * a_1 / a_2;

  if( limitX ) {
    x_cnt = qBound( lim_x_lt, (double)(x_cnt), lim_x_rt );
  }

  x_cn = x_ce + x_cnt;
  return x_cn;

}


DEFAULT_FUNCS_REG(TQuadExtr)


// end of tquadextr.cpp


