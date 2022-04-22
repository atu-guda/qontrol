/***************************************************************************
                          tquadexrt.cpp - find top extranum in quadatic approx
                             -------------------
    begin                : 2015.02.27
    copyright            : (C) 2015-2022 by atu
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
#include "miscfun.h"

const char* TQuadExtr::helpstr = "<H1>TQuadExtr</H1>\n"
 "Find upper extremum in quadratic approximation by given 3 points \n"
 "(may by with limits)";

STD_CLASSINFO(TQuadExtr,clpElem|clpCalcAtStart);

CTOR(TQuadExtr,TMiso)
{
}


double TQuadExtr::f() noexcept
{
  double x_ce, y_ce; // effective central point (need if emulC)
  if( emulC ) {
    x_ce = ( x_r + x_l ) / 2;
    y_ce = ( y_r + y_l ) / 2;
  } else {
    x_ce = x_c;
    y_ce = y_c;
  }
  QuadExtrIn in { x_l, x_ce, x_r, y_l, y_ce, y_r, lim_s, x_min, x_max, (bool)limitX, (bool)limitG };
  QuadExtrOut out;
  // fallback values
  a_1 = 0; a_2 = 0; x_cnt = 0; x_cn = x_ce;  y_cn = y_ce; dy = 0; dy_sx = 0; f_c = 0;

  if( calcQuadExtr( in, out ) ) {
    x_cn = out.x_e; x_cnt = out.x_et; y_cn = out.y_e;
    x_lt = out.x_lt; x_rt = out.x_rt; y_lt = out.x_lt; y_rt = out.y_rt;
    a_1 = out.a_1; a_2 = out.a_2;
    dy = y_cn - y_c;
    dy_dx = dy / x_cnt;
    dy_sx = dy * sign( x_cnt );
    f_c = 1.0 / pow2( x_lt ) - 1.0 / pow2( x_rt ); // TODO: remove, not here
  }

  return x_cn;
}


DEFAULT_FUNCS_REG(TQuadExtr)


// end of tquadextr.cpp


