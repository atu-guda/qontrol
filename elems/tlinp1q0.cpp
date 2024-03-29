/***************************************************************************
                          tlinp1q0.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

#include "miscfun.h"
#include "tlinp1q0.h"

using namespace std;

const char* TLinP1Q0::helpstr = "<h1>TLinP1Q0</h1>\n"
 "<p>Integrate (linear if f(x)==x) differential equation: <br/>\n"
 "<b> dx/dt = a * ( ku * u(t) - x ) </b><br/>\n"
 "or<br/>\n"
 "<b> dx/dt = a * ( ku * u(t) - f() ) </b><br/>\n"
 "Parameter <b>ku</b> -- amplification, can be changed at any time.<br/>\n"
 "Parameter <b>a</b> -- frequency, can be changed at any time.<br/>\n"
 " <b>a = 1 / tau; </b> "
 "where <b> tau </b> -- time constant.<br/>\n"
 "Linear transfer function: <br/>\n"
 "<b>W = ku / ( tau * p + 1 ) </b>.<br/>\n"
 "If u(t) = theta(t), than x(t) = ku * ( 1 - exp(-t/tau) ), and<br/>\n"
 " x(tau) = 0.6321 * ku </p>\n"
 ;

STD_CLASSINFO(TLinP1Q0,clpElem );

CTOR(TLinP1Q0,TMiso)
{
}


int TLinP1Q0::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x_old = ( use_sqrt0 ) ? pow2( out0_init ) : out0_init;
  tau = 1.0 / a;
  double u00 = in_u - in_n;
  u0 = u00;
  x2 = pow2( x_old );
  return 1;
}

void TLinP1Q0::preCalc()
{
  readAllInputs();
  pre_f();
}

void TLinP1Q0::pre_f() noexcept
{
  u0 = in_u - in_n;
}

double TLinP1Q0::f() noexcept
{
  pre_f();
  const double f = ( use_u1 ) ? in_f : x_old;

  const double u = ( use_u2 ) ? pow2( u0 ) : u0;
  tau = 1.0 / a;
  const double x = x_old + a * ctdt * ( ku * u - f );
  x_old = x;
  double v =  ( use_sqrt0 ) ? sqrt0( x ) : x;
  v = clamp( v, vmin.cval(), vmax.cval() );
  x2 = v * v;
  return v;
}

DEFAULT_FUNCS_REG(TLinP1Q0)


// end of tlinp1q0.cpp

