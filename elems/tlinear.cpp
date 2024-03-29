/***************************************************************************
                          tlinear.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2022 by atu
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

#include "tlinear.h"

const char* TLinear::helpstr = "<H1>TLinear</H1>\n"
 "Linear summator: <br>\n"
 "<b>x = ( a / d ) * (a0*u0 + a1*u1 + a2*u2 + a3*u3 + b)</b><br>\n"
 "Have 6 parameters: <b>a, a0 .. a3, b</b>, each can be changed at any time.";

STD_CLASSINFO(TLinear,clpElem|clpCalcAtStart);

CTOR(TLinear,TMiso)
{
}

double TLinear::f() noexcept
{
  double v = ( a / d ) * ( in_0*a0 + in_1*a1 + in_2*a2 + in_3*a3 + b );
  x2 = v * v;
  return v;
}

double TLinear::f_d( double arg0, double arg1, double arg2, double arg3 )
{
  // setInput not used: too simple
  double v = ( a / d ) * ( arg0*a0 + arg1*a1 + arg2*a2 + arg3*a3 + b );
  x2 = v * v;
  return v;
}

DEFAULT_FUNCS_REG(TLinear)


// end of tlinear.cpp

