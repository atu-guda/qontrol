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

