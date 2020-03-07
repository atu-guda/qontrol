/***************************************************************************
   tbarrier.cpp - barrier functions definitions
                             -------------------
    begin                : 2016.04.24
    copyright            : (C) 2016-2020 by atu
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
#include "tbarrier.h"

using namespace std;

const char* TBarrier::helpstr = "<H1>TBarrier</H1>\n"
 "Barrier functions: <br>\n"
 "Argument y calculates as:<br>\n"
 "<b>y = x - x_n - x0</b><br>\n"
 "Double parameters <b> b, scale, shift, x0</b> can be changed at any time\n";


STD_CLASSINFO(TBarrier,clpElem|clpCalcAtStart);

CTOR(TBarrier,TMiso)
{
}


double TBarrier::f() noexcept
{
  double xr = (double)x - (double)(x_n) - x_0;
  double v;
  switch( (BarrierType)(int)type ) {
    case bt_HypUp:
      v = barrierHypUp( xr, b ); break;
    case bt_HypDown:
      v = barrierHypDown( xr, -b ); break;
    case bt_HypUpUp:
      v = barrierHypUpUp( xr, b ); break;
    case bt_HypUpDown:
      v = barrierHypUpDown( xr, b ); break;
    case bt_Hyp2Up:
      v = barrierHyp2Up( xr, b ); break;
    case bt_Hyp2Down:
      v = barrierHyp2Down( xr, -b ); break;
    case bt_Hyp2UpUp:
      v = barrierHyp2UpUp( xr, b ); break;
    case bt_Hyp2UpDown:
      v = barrierHyp2UpDown( xr, b ); break;
    default: v = 0;
  };
  v *= scale;
  v += shift;
  return v;
}

double TBarrier::f_d( double arg0, double arg1, double /*arg2*/, double /*arg3*/ )
{
  x.setInput( arg0 );
  x_n.setInput( arg1 );
  return f();
}

DEFAULT_FUNCS_REG(TBarrier)


// end of tbarrier.cpp

