/***************************************************************************
                          tlinp1q0.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2013 by atu
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

#include "tlinp1q0.h"

const char* TLinP1Q0::helpstr = "<H1>TLinP1Q0</H1>\n"
 "Integrate (linear if f(x)==x) diffefentional equation: <br>\n"
 "<b> dx/dt = a * f( ku * u(t) - x ) </b><br>\n"
 "Parameter <b>ku</b> -- amplification, can be changed at any time.<br>\n"
 "Parameter <b>a</b> -- frequency, can be changed at any time.<br>\n"
 " <b>a = 1 / tau; </B> "
 "where <B> tau </B> -- time constant.<br>\n"
 "Linear transfer function: <br>\n"
 "<B>W = ku / ( tau * p + 1 ) </B>.<br>\n"
 "If u(t) = theta(t) than x(t) = ku * ( 1 - exp(-t/tau) ), and<br>\n"
 " x(tau) = 0.6321 * ku \n"
 ;

STD_CLASSINFO(TLinP1Q0,clpElem );

CTOR(TLinP1Q0,TMiso)
{
}


int TLinP1Q0::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x_old = 0;
  return 0;
}

double TLinP1Q0::f( double /*t*/ )
{
  double x, f;
  if( use_u1 ) {
    f = in_f;
  } else {
    f = x_old;
  };
  // TODO: check this for stability
  x = x_old + a * tdt * ( ku * in_u - f );
  x_old = x;
  return x;
}

DEFAULT_FUNCS_REG(TLinP1Q0)


// end of tlinp1q0.cpp

