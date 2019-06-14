/***************************************************************************
                          tswitch.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "miscfun.h"
#include "tswitch.h"

using namespace std;

const char* TSwitch::helpstr = "<H1>TSwitch</H1>\n"
 "<p>Connects output to one of inputs, according to value of (in_s+in_s1+in_s2): <br/>\n</p>";

STD_CLASSINFO(TSwitch,clpElem|clpCalcAtStart);

CTOR(TSwitch,TMiso)
{
}


double TSwitch::f() noexcept
{
  if( fabs(step) < D_AZERO ) { step = D_AZERO; }
  in_all = in_s + in_s1  + in_s2 - level_0;
  double vn = clamp( in_all / step, 0.0, double(n_max) );
  int n = int( vn );
  n_cur = n;

  double v;
  switch( n ) {
    case 0: v = in_0; break;
    case 1: v = in_1; break;
    case 2: v = in_2; break;
    case 3: v = in_3; break;
    case 4: v = in_4; break;
    case 5: v = in_5; break;
    case 6: v = in_6; break;
    case 7: v = in_7; break;
    default: v = 0; break;
  }
  x2 = v * v;
  return v;
}

DEFAULT_FUNCS_REG(TSwitch)


// end of tswitch.cpp

