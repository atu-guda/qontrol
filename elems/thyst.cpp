/***************************************************************************
                          thyst.cpp  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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
#include "miscfun.h"
#include "thyst.h"

using namespace std;


const char* THyst::helpstr = "<H1>THyst</H1>\n"
 "<p>Can simulate some simple models of hysteresis: <br>\n"
 "Have 5 parameters: <b>x0, alpha, a, b, type</b>\n"
 " <b>x0, alpha </b> - parameers of hysteresys\n"
 " <b>a, b </b> - output scale\n"
 "Can be changed at any time: <b>x0, alpha</b>.</p>";

STD_CLASSINFO(THyst,clpElem);

CTOR(THyst,TMiso)
{
}


double THyst::f() noexcept
{
  double ud, ts , v, u_old;
  if( prm_mod ) {
    fixState();
  }
  // atu test
  u_old = d + s;
  //
  ud = in_u - u_old;
  ts = s + ud;
  if( ts >= x0 ) {
    s = (double)x0;  d += ts - x0;
  } else {
    if( ts <= -x0 ) {
      s = -x0;  d += ts + x0;
    }  else {
      s = ts;
    };
  };
  switch( (int)type ) {
    case ht_sAlphaD:
      v = s + alpha * d; break;
    case ht_alphaD:
      v = alpha * d; break;
    case ht_thetaD:
      v = thetta( d ); break;
    case ht_signD:
      v = sign( d ); break;
    case ht_tanhAlphaD:
      v = tanh( alpha * d ); break;
    case ht_tanhSAlphaD:
      v = tanh( s + alpha * d ); break;
    default: v = 0;
  };
  v = a * v + b;
  return v;
}

void THyst::fixState(void)
{
  if( x0 < 0 ) { x0 = 0; };
  if( s > x0 ) {
    d += s - x0;  s = (double)x0;
  };
  if( s < -x0 ) {
    d += s + x0;  s = -x0;
  };
  prm_mod = 0;
}

int THyst::do_startLoop( int acnx, int acny )
{
  TMiso::do_startLoop( acnx, acny );
  s = d = 0;
  return 1;
}

DEFAULT_FUNCS_REG(THyst)


// end of thyst.cpp


