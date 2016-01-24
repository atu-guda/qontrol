/***************************************************************************
                          tswitch.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tswitch.h"

const char* TSwitch::helpstr = "<H1>TSwitch</H1>\n"
 "<p>Connects output to one of inputs, accrding to values of u_s: <br>\n</p>";

STD_CLASSINFO(TSwitch,clpElem|clpCalcAtStart);

CTOR(TSwitch,TMiso)
{
}


double TSwitch::f() noexcept
{
  if( fabs(step) < D_AZERO ) { step = D_AZERO; }
  double vn = qBound( 0.0, ( in_s - level_0 ) / step, double(n_max) );
  int n = int( vn );
  n_cur = n;

  switch( n ) {
    case 0: return in_0;
    case 1: return in_1;
    case 2: return in_2;
    case 3: return in_3;
    case 4: return in_4;
    case 5: return in_5;
    case 6: return in_6;
    case 7: return in_7;
  }
  return 0;
}

DEFAULT_FUNCS_REG(TSwitch)


// end of tswitch.cpp

