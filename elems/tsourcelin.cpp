/***************************************************************************
                          tsourcelin.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003-2022 by atu
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
  double v;
  t_r = u;

  if( (double)(t_r) < 0 ) {
    if( cycle_before )  {
      int nc = floor( t_r / T_c );
      t_r -= nc * T_c;
    } else {
      t_r = 0;
    }
  }

  if( (double)(t_r) > (double)(T_c) ) {
    if( cycle_after ) {
      int nc = floor( t_r / T_c );
      t_r -= nc * T_c;
    } else {
      t_r = T_c;
    }
  }

  auto clp = upper_bound( tsta.begin(), tsta.end(), (double)(t_r) );
  // if( clp == tsta.end() ) { // failsafe, unlikely
  //   qWarning() << " t_r override: t_r= " << t_r << " back: " << tsta.back() << NWHE;
  // }
  if( clp != tsta.begin() ) { // failsafe, likely, as started from 0
    --clp;
  }

  int curr_lin = clp - tsta.begin();
  n_rr = curr_lin;

  t_r0 = *clp;
  t_rr = t_r - t_r0; // reative from current slopy start
  v = vs[curr_lin] + slopes[curr_lin] * t_rr;
  v *= a; v += b;
  return v;
}

void TSourceLin::do_post_set()
{
  recalc();
}


int TSourceLin::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  t_r = u; t_rr = t_r; t_r0 = 0; n_rr = 0;
  recalc();
  out0 = vs[0];
  return 1;
}

void TSourceLin::recalc(void)
{
  int n = t_int.arrSize();
  slopes.assign( n, 0 ); tsta.assign( n, 1e100 );
  T_c = 0; freq = 1e10; omega = 2 * M_PI * freq;

  // fail-safe
  if( t_int[0] <= 0 ) {
    t_int[0] = 1;
  }

  for( int i=0; i<n; ++i ) {
    if( t_int[i] <= 0 ) {
      n_lin = i;
      break;
    };
    slopes[i] = ( ve[i] - vs[i] ) / t_int[i];
    tsta[i] = T_c;
    T_c += t_int[i];
  };
  tsta[n_lin] = T_c;
  freq = 1.0 / T_c; omega = 2 * M_PI * freq;

  // for( int j=0; j<n_lin; ++j ) {
  //   qWarning() << "tsta[" << j << "]= " << tsta[j] << " a = " << slopes[j];
  // }
}


DEFAULT_FUNCS_REG(TSourceLin)


// end of tsourcelin.cpp

