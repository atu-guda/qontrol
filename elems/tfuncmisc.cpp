/***************************************************************************
                          tfuncmisc.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#include <cmath>
#include "miscfun.h"
#include "tfuncmisc.h"

using namespace std;

const char* TFuncMisc::helpstr = "<H1>TFuncMisc</H1>\n"
 "Miscelenios nonlinear functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = in_0 - in_1 - x0</b><br>\n"
 "<b>ae = a/ar,  b = b /br</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, ra, rb, x0</b> can be changed at any time\n";


STD_CLASSINFO(TFuncMisc,clpElem|clpCalcAtStart);

CTOR(TFuncMisc,TMiso)
{
}


double TFuncMisc::f() noexcept
{
  double v;
  ae = a / ra; be = b / rb;
  y = in_0 - in_1 - x0;
  ax = fabs( in_0 ); ay = fabs( y );
  double cy = c * y;

  switch( (FuncType)(int)type ) {
    case ft_deadLine     : v = ae * deadLine( y, be ) + cy; break;
    case ft_limitLine    : v = ae * limitLine( y, be ) + cy; break;
    case ft_dealLimitLine: v = ae * deadLimitLine( y, be, c, d ); break;
    case ft_sign         : v = ae * sign( y ) + cy; break;
    case ft_theta        : v = ae * thetta( y ) + cy; break;
    case ft_diod         : v = ae * posval( y ) + cy; break;
    case ft_threeState   : v = ae * threeStateLine( y, be ) + cy; break;
    case ft_triangle     : v = ae * triangleLine( y , be ) + cy; break;
    case ft_reactLine    : v = ae * rectLine( y, be, d ) + cy; break;
    case ft_2slope       : v = ( y >= e ) ? ( ae * y ) : ( be * y ); break;
    case ft_div          : v = ae * in_0 / ( in_1 +  be ) + cy;  break;
    case ft_int          : v = ae * int( y * be ) + cy;  break;
    case ft_frac         : v = ae * ( ( y * be ) - int( y * be ) ) + cy;  break;
    case ft_aasin        : v = ae * asin( fabs( sin( y * be ) ) );  break;
    case ft_erf          : v = ae * erf( y * be );  break;
    case ft_min          : v = ae * fmin( in_0, in_1 );  break;
    case ft_max          : v = ae * fmax( in_0, in_1 );  break;
    case ft_2slopeEx     : v = ae * (( y >= e ) ? ( be * y ) : ( c * y )); break;
    case ft_copysign     : v = ae * copysign( in_0, in_1 ); break;
    case ft_clamp        : v = ae * clamp( (double)y, (double)be, (double)d ) + cy; break;
    case ft_switch       : v = ae * ( ( (double)in_2 > d ) ? ( be * in_0 ) : ( c * in_1 ) ) ; break;
    default: v = 0;
  };
  v += g;
  v = clamp( v, vmin.cval(), vmax.cval() );
  ao = fabs( v );
  x2 = v * v;
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


