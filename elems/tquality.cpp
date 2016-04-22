/***************************************************************************
     tquality.cpp -- TQuality element definition
                             -------------------
    begin                : 2016.02.29
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

#include "tquality.h"

using namespace std;

const char* TQuality::helpstr = "<h1>TQuality</h1>\n"
 "Miscelenios quality functions: <br/>\n"
 "<b>Vars</b>: q = q_o - q_m; q_r = q / q_gamma;<br/>\n"
 " sq_r = |q_r|; q_r2 = q_r^2<br/>\n"
 "<b>Functions:</b> <br/>\n"
 " Gauss: exp( -q_r2 ); <br/>\n"
 " Parabola: 1 - q_r2 * (1-1/e)<br/>\n"
 " Linear:   1 - aq_r * (1-1/e)<br/>\n"
 " Hyperbolic: 1 / ( 1.0 + |q_r} * (e-1) )<br/>\n"
 " Logariphmic: 1 - log( 1 + |q_r| ) * (1-1/e)/ln(2)<br/>\n";


STD_CLASSINFO(TQuality,clpElem|clpCalcAtStart);

CTOR(TQuality,TMiso)
{
}


double TQuality::f() noexcept
{
  double v;
  q = q_o - q_m;
  q_r = q / q_gamma;
  aq_r = fabs( q_r );
  q_r2 = q_r * q_r;

  switch( (int)type ) {
    case qa_Gauss  : v = exp( - q_r2 ); break;
    case qa_Para   : v = 1.0 - q_r2 * oneMinusEm1; break;
    case qa_Lin    : v = 1.0 - aq_r * oneMinusEm1; break;
    case qa_Hyper  : v = 1.0 / ( 1.0 + aq_r * eMinus1 ); break;
    case qa_Log    : v = 1.0 - log( 1.0 + aq_r ) * logQaScale; break;
    default        : v = 0.0; break;
  };

  // v = qRange( (double)F_min, v, (double)(F_max) ); // TODO: inline func
  if( v > (double)F_max ) { v = (double)F_max; };
  if( v < (double)F_min ) { v = (double)F_min; };
  return v;
}

double TQuality::f_d( double arg0, double arg1, double /*arg2*/, double /*arg3*/ )
{
  q_o.setInput( arg0 );
  q_m.setInput( arg1 );
  return f();
}

DEFAULT_FUNCS_REG(TQuality)


// end of tquality.cpp


