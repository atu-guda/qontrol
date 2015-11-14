/***************************************************************************
                          tfuncpoly.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2015 by atu
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
#include "tfuncpoly.h"

using namespace std;

const char* TFuncPoly::helpstr = "<H1>TFuncPoly</H1>\n"
 "Polinomial functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = in_0 - in_1 - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

STD_CLASSINFO(TFuncPoly,clpElem);

CTOR(TFuncPoly,TMiso)
{
}


double TFuncPoly::f( double /* t */ )
{
  double v, t1, t2;
  y = in_0 - in_1 - x0; y2 = y*y;
  switch( (int)type ) {
    case  ft_lin:
      v = a * y; break;
    case  ft_square:
      v = a * y2 + b * y; break;
    case  ft_cube:
      v = a * y2 * y + b * y2 + c * y; break;
    case  ft_biSquare:
      v = a * in_0 * in_0 + b * in_0 * in_1 + c * in_1 * in_1; break;
    case  ft_sqrt:
      t1 = b * y + c;
      v = a * sqrt( posval(t1) ); break;
    case  ft_hypot:
      v = hypot( a * in_0, b * in_1 ); break;
    case  ft_4square:
      v = a * in_0* in_0  +  b * in_1 * in_1
        + c * in_2* in_2  +  d * in_3 * in_3; break;
    case  ft_vibro:
      t1 = b*b - y2; t1 *= t1;
      v = 1 / sqrt( t1 + a * a *y2 ); break;
    case  ft_msquare:
      t1 = ( 1- in_0 ); t1 *= t1;
      t2 = ( in_1 - in_0 * in_0 );
      t2 *= t2;
      v = a * t2 + b * t1; break;
    case  ft_xExp:
      t1 = in_0 * in_0 + in_1 * in_1 - 1;
      t1 *= t1;
      v = 1 - exp( -a * ( t1 - b * in_0 - c * in_1 ) ); break;
    case  ft_lin2:
      v = a * ( 1 + b*y ) ; break;
    case  ft_linAbs:
      v = a * ( b * y + c * fabs(y) ) ; break;
    case  ft_pow4:
      v = a*y2*y2 + b*y*y2 + c*y2 + d*y; break;
    case  ft_pow5:
      v = a*y2*y2*y + b*y2*y2 + c*y2*y + d*y2 + e*y; break;
    case  ft_pow:
      v = a * pow( b * posval(y), posval(in_2) ); break;
    case  ft_pows:
      v = a * pow( b * fabs(y), posval(in_2) ) * sign( y ); break;
    default: v = 0;
  };
  v += g;
  return v;
}

DEFAULT_FUNCS_REG(TFuncPoly)


// end of tfuncpoly.cpp


