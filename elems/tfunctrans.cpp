/***************************************************************************
                          tfunctrans.cpp  -  description
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
#include "tfunctrans.h"

using namespace std;


const char* TFuncTrans::helpstr = "<H1>TFuncTrans</H1>\n"
 "<p>Harmonic-alike functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = in_0 - in_1 - x0</b><br>\n"
 "<b>ae = a/ar,  b = b /br</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, ra, rb, x0</b> can be changed at any time\n";

STD_CLASSINFO(TFuncTrans,clpElem|clpCalcAtStart);

CTOR(TFuncTrans,TMiso)
{
}


double TFuncTrans::f() noexcept
{
  double v, by, tm;
  ae = a / ra; be = b / rb;
  y = in_0 - in_1 - x0; by = y * be;
  double byc = by + c;

  switch( (int)type ) {
    case ft_sin:
      v = ae * sin( byc ); break;
    case ft_signSin:
      v = ae * sign( sin( byc ) + d ); break;
    case ft_tanh:
      v = ae * tanh( byc ); break;
    case ft_atan2:
      v = ae * atan2( in_0 * be, in_1 ); break;
    case ft_exp:
      v = ae * exp( byc ); break;
    case ft_expM2:
      v = ae * exp( -by * y + c ); break;
    case ft_wave:
      v = ae * waveWave( byc ); break;
    case ft_mhat:
      v = ae * waveMhat( byc ); break;
    case ft_ln:
      v = ( byc > 0 ) ? ( ae * log( byc ) ) : -1e100; break;
    case ft_yExp:
      v = ae * y * exp( -byc ); break;
    case ft_yExp2:
      v = ae * y * exp( -by * y + c ); break;
    case ft_sin2:
      tm = sin( byc ); v = ae * tm * tm; break;
    case ft_cos:
      v = ae * cos( byc ); break;
    case ft_cos2:
      tm = cos( byc ); v = ae * tm * tm; break;
    case ft_tan:
      v = ae * tan( byc ); break;
    case ft_exp1Msin:
      tm = sin( d * y );  tm = 1 - c * tm*tm;
      v = ae * exp( -by*y ) * tm; break;
    case ft_1Mexp1Msin2:
      tm = sin( d * y );
      tm = 1 - c * tm*tm;
      v = ae * ( 1 - exp( -by*y ) ) * tm; break;
    case ft_expM2d:
      tm = y/be; v = ae * exp( - tm * tm + c ); break;
    case ft_divMod:
      v = ae / ( fabs(y)/be + 1 + c ); break;
    case ft_expMod:
      v = ae * exp( - fabs(y)/ be + c ); break;
    case ft_sinh:
      v = ae * sinh( byc ); break;
    case ft_cosh:
      v = ae * cosh( byc ); break;
    case ft_expm1:
      v = ae * expm1( byc ); break;
    default:  v = 0;
  };
  v += g;
  v = clamp( v, vmin.cval(), vmax.cval() );
  x2 = v * v;
  return v;
}

double TFuncTrans::f_d( double arg0, double arg1, double /*arg2*/, double /*arg3*/ )
{
  in_0.setInput( arg0 );
  in_1.setInput( arg1 );
  return f();
}

DEFAULT_FUNCS_REG(TFuncTrans)


// end of tfunctrans.cpp


