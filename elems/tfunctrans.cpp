/***************************************************************************
                          tfunctrans.cpp  -  description
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
#include "tfunctrans.h"

using namespace std;


const char* TFuncTrans::helpstr = "<H1>TFuncTrans</H1>\n"
 "<p>Harmonic-alike functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = in_0 - in_1 - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n</p>";

STD_CLASSINFO(TFuncTrans,clpElem);

CTOR(TFuncTrans,TMiso)
{
}


double TFuncTrans::f( double /* t */ ) noexcept
{
  double v, by, tm;
  y = in_0 - in_1 - x0; by = y * b;
  switch( (int)type ) {
    case ft_sin:
      v = a * sin( by ); break;
    case ft_signSin:
      v = a * sign( sin( by ) + c ); break;
    case ft_tanh:
      v = a * tanh( by ); break;
    case ft_atan2:
      v = a * atan2( in_0 * b, in_1 ); break;
    case ft_exp:
      v = a * exp( by ); break;
    case ft_expM2:
      v = a * exp( -by * y ); break;
    case ft_wave:
      v = a * waveWave( by ); break;
    case ft_mhat:
      v = a * waveMhat( by ); break;
    case ft_ln:
      v = ( by > 0 ) ? ( a * log( by ) ) : 0; break;
    case ft_yExp:
      v = a * y * exp( -by ); break;
    case ft_yExp2:
      v = a * y * exp( -by * y ); break;
    case ft_sin2:
      tm = sin( by ); v = a * tm * tm; break;
    case ft_cos:
      v = a * cos( by ); break;
    case ft_cos2:
      tm = cos( by ); v = a * tm * tm; break;
    case ft_tan:
      v = a * tan( by ); break;
    case ft_exp1Msin:
      tm = sin( d * y );  tm = 1 - c * tm*tm;
      v = a * exp( -by*y ) * tm; break;
    case ft_1Mexp1Msin2:
      tm = sin( d * y );
      tm = 1 - c * tm*tm;
      v = a * (1-exp( -by*y )) * tm; break;
    case ft_expM2d:
      tm = y/b; v = a * exp( - tm * tm ); break;
    case ft_divMod:
      v = a / ( fabs(y)/b + 1 ); break;
    case ft_expMod:
      v = a * exp( - fabs(y)/ b ); break;
    case ft_sinh:
      v = a * sinh( y * b ); break;
    case ft_cosh:
      v = a * cosh( y * b ); break;
    default:  v = 0;
  };
  v += g;
  return v;
}

DEFAULT_FUNCS_REG(TFuncTrans)


// end of tfunctrans.cpp


