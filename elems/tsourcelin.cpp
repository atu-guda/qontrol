/***************************************************************************
                          tsourcelin.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003-2013 by atu
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

void TSourceLin::post_set()
{
  TDataSet::post_set();
  if( t_int_0 != 31415926 ) { // convert from old data TODO: remove after convert
    DBG1( "inf: convertions from OLD structure" );
    t_int[0]  = t_int_0;  vs[0]  = vs_0;  ve[0]  = ve_0;
    t_int[1]  = t_int_1;  vs[1]  = vs_1;  ve[1]  = ve_1;
    t_int[2]  = t_int_2;  vs[2]  = vs_2;  ve[2]  = ve_2;
    t_int[3]  = t_int_3;  vs[3]  = vs_3;  ve[3]  = ve_3;
    t_int[4]  = t_int_4;  vs[4]  = vs_4;  ve[4]  = ve_4;
    t_int[5]  = t_int_5;  vs[5]  = vs_5;  ve[5]  = ve_5;
    t_int[6]  = t_int_6;  vs[6]  = vs_6;  ve[6]  = ve_6;
    t_int[7]  = t_int_7;  vs[7]  = vs_7;  ve[7]  = ve_7;
    t_int[8]  = t_int_8;  vs[8]  = vs_8;  ve[8]  = ve_8;
    t_int[9]  = t_int_9;  vs[9]  = vs_9;  ve[9]  = ve_9;
    t_int[10] = t_int_10; vs[10] = vs_10; ve[10] = ve_10;
    t_int[11] = t_int_11; vs[11] = vs_11; ve[11] = ve_11;
    t_int[12] = t_int_12; vs[12] = vs_12; ve[12] = ve_12;
    t_int[13] = t_int_13; vs[13] = vs_13; ve[13] = ve_13;
    t_int[14] = t_int_14; vs[14] = vs_14; ve[14] = ve_14;
    t_int[15] = t_int_15; vs[15] = vs_15; ve[15] = ve_15;
  }
  slopes.assign( t_int.size(), 0 );
}


double TSourceLin::f( double t )
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
  return v;
}


int TSourceLin::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  recalc();
  t_start = 0; curr_lin = 0;
  return 0;
}

void TSourceLin::recalc(void)
{
  int n = t_int.size();
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

