/***************************************************************************
                          tfuncmisc.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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
#include "tfuncmisc.h"

using namespace std;

const char* TFuncMisc::helpstr = "<H1>TFuncMisc</H1>\n"
 "Miscelenios nonlinear functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = in_0 - in_1 - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";


STD_CLASSINFO(TFuncMisc,clpElem|clpCalcAtStart);

CTOR(TFuncMisc,TMiso)
{
}


double TFuncMisc::f() noexcept
{
  double v;
  y = in_0 - in_1 - x0;
  ax = fabs( in_0 ); ay = fabs( y );
  double cy = c * y;
  switch( (FuncType)(int)type ) {
    case ft_deadLine     : v = a * deadLine( y, b ) + cy; break;
    case ft_limitLine    : v = a * limitLine( y, b ) + cy; break;
    case ft_dealLimitLine: v = a * deadLimitLine( y, b, c, d ); break;
    case ft_sign         : v = a * sign( y ) + cy; break;
    case ft_theta        : v = a * thetta( y ) + cy; break;
    case ft_diod         : v = a * posval( y ) + cy; break;
    case ft_threeState   : v = a * threeStateLine( y, b ) + cy; break;
    case ft_triangle     : v = a * triangleLine( y , b ) + cy; break;
    case ft_reactLine    : v = a * rectLine( y, b, c ) + cy; break;
    case ft_2slope       : v = ( y >= e ) ? ( a * y ) : ( b * y ); break;
    case ft_div          : v = a * in_0 / ( in_1 +  b ) + cy;  break;
    case ft_int          : v = a * int( y * b ) + cy;  break;
    case ft_frac         : v = a * ( ( y * b ) - int( y * b ) ) + cy;  break;
    case ft_aasin        : v = a * asin( fabs( sin( y * b) ) );  break;
    case ft_erf          : v = a * erf( y*b );  break;
    case ft_min          : v = a * fmin( in_0, in_1 );  break;
    case ft_max          : v = a * fmax( in_0, in_1 );  break;
    case ft_2slopeEx     : v = a * (( y >= e ) ? ( b * y ) : ( c * y )); break;
    case ft_copysign     : v = a * copysign( in_0, in_1 ); break;
    default: v = 0;
  };
  v += g;
  return v;
}

double TFuncMisc::f_d( double arg0, double arg1, double /*arg2*/, double /*arg3*/ )
{
  in_0.setInput( arg0 );
  in_1.setInput( arg1 );
  return f();
}

DEFAULT_FUNCS_REG(TFuncMisc)


// end of tfuncmisc.cpp


