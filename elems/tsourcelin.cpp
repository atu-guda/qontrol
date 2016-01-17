/***************************************************************************
                          tsourcelin.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003-2016 by atu
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
#include "tsourcelin.h"

using namespace std;

const char* TSourceLin::helpstr = "<H1>TSourceLin</H1>\n"
 "<p>Source of signal, which consist of some linear segments. \n"
 "Each segment have time: <B>t_int[i]</B>, \n"
 "start value: <B>vs[i]</B>, \n"
 "end value: <B>ve[i]</B>, <BR>\n"
 "List terminated, in t_int[i] &lt=; 0.</p>\n"
;

STD_CLASSINFO(TSourceLin,clpElem );

CTOR(TSourceLin,TMiso),
  slopes( def_n_slopes, 0 )
{
}



double TSourceLin::f() noexcept
{
  double tr, v;
  tr = t - t_start;
  while( tr > t_int[curr_lin] ) { // find next line (may be not the next)
    tr -= t_int[curr_lin]; t_start += t_int[curr_lin];
    curr_lin++;
    if( curr_lin >= n_lin )
      curr_lin = 0;
  };
  v = vs[curr_lin] + slopes[curr_lin] * tr;
  v *= a;
  return v;
}


int TSourceLin::do_startLoop( int acnx, int acny )
{
  TMiso::do_startLoop( acnx, acny );
  recalc();
  t_start = 0; curr_lin = 0;
  return 1;
}

void TSourceLin::recalc(void)
{
  int n = t_int.arrSize();
  slopes.assign( n, 0 );

  // fail-safe
  if( t_int[0] <= 0 )
    t_int[0] = 1;

  for( int i=0; i<n; ++i ) {
    if( t_int[i] <= 0 ) {
      n_lin = i;
      break;
    };
    slopes[i] = ( ve[i] - vs[i] ) / t_int[i];
  };
}


DEFAULT_FUNCS_REG(TSourceLin)


// end of tsourcelin.cpp

