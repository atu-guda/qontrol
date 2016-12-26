/***************************************************************************
    tsinsum.cpp -- sum of sinuses
                             -------------------
    begin                : 2016.1226
    copyright            : (C) 2016-2016 by atu
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
#include "tsinsum.h"

using namespace std;

const char* TSinSum::helpstr = "<h1>TSinSum</h1>\n"
 "Sum of sinuses of gives amplitudes, frequnces and phases. <br/>\n"
 "And linear part too<br/>\n"
 "<b>p = u_0 * b</b><br>\n"
 "x = c_0 + c_lin * u_0 + \\sum a_i * sin( p * f_i + p_i )\n";


STD_CLASSINFO(TSinSum,clpElem|clpCalcAtStart);

CTOR(TSinSum,TMiso)
{
}


double TSinSum::f() noexcept
{
  double v = c_0 + c_lin * u_0;
  p = u_0 * b;

  v += a_1 * sin( p * f_1 + p_1 );
  v += a_2 * sin( p * f_2 + p_2 );
  v += a_3 * sin( p * f_3 + p_3 );
  v += a_4 * sin( p * f_4 + p_4 );
  v += a_5 * sin( p * f_5 + p_5 );
  return v;
}

double TSinSum::f_d( double arg0, double /*arg1*/, double /*arg2*/, double /*arg3*/ )
{
  u_0.setInput( arg0 );
  return f();
}

DEFAULT_FUNCS_REG(TSinSum)


// end of tfuncmisc.cpp


