/***************************************************************************
    tsinsum.cpp -- sum of sinuses
                             -------------------
    begin                : 2016.12.26
    copyright            : (C) 2016-2019 by atu
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
 "Sum of sinuses of gives amplitudes, frequencies and phases. <br/>\n"
 "And linear part too<br/>\n"
 "<b>p = ( u_0 - x_0 ) * b / d</b><br>\n"
 "x = c_0 + c_lin * (u_0 - x_0)[?b/d]  + \\sum a_i * sin( p * f_i + p_i )\n";


STD_CLASSINFO(TSinSum,clpElem|clpCalcAtStart);

CTOR(TSinSum,TMiso)
{
}


double TSinSum::f() noexcept
{
  double u = u_0 - x0;
  p = u * b / d;
  double v = c_0 + c_lin * ( use_bd ? p : u );
  double mult = mul_pi ? M_PI : 1.0;

  v += a_1 * sin( mult * ( p * f_1 + p_1 ) );
  v += a_2 * sin( mult * ( p * f_2 + p_2 ) );
  v += a_3 * sin( mult * ( p * f_3 + p_3 ) );
  v += a_4 * sin( mult * ( p * f_4 + p_4 ) );
  v += a_5 * sin( mult * ( p * f_5 + p_5 ) );
  x2 = v * v;
  return v;
}

double TSinSum::f_d( double arg0, double /*arg1*/, double /*arg2*/, double /*arg3*/ )
{
  u_0.setInput( arg0 );
  return f();
}

DEFAULT_FUNCS_REG(TSinSum)


// end of tsinsum.cpp


