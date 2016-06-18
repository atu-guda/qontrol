/***************************************************************************
                          tvibro.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#include "miscfun.h"
#include "tvibro.h"

const char* TVibro::helpstr = "<H1>TVibro</H1>\n"
 "Vibrational element <b> d2x/dt^2 + c0*dx/dt + Omega^2*f(x) = u</b>: <br>\n"
 "Parameters <b>c0</b> and <b>Omega</b> can be changed at any time.<br>\n";

STD_CLASSINFO(TVibro,clpElem ); // no calc at start

CTOR(TVibro,TMiso)
{
}


int TVibro::do_preRun()
{
  tdt2 = ctdt * ctdt;
  return 1;
}

int TVibro::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  v = (double)v0;
  x_old = out0_init;
  x_old2 = out0_init - v0 * ctdt;
  x2 = pow2( x_old );
  a = 0;
  return 1;
}

double TVibro::f() noexcept
{
  double ctau = ctdt * c0 / 2; // c0 can be changed at any time, so here, TODO: use mod_prm
  double f = use_u1 ? in_f : out0;

  double x = ( 2*x_old - x_old2 * (1-ctau) + tdt2 * ( in_u - Omega * Omega * f ) )
          / ( 1 + ctau );
  v = ( x - x_old2 ) / ( 2 * ctdt );
  a = ( x - 2 * x_old + x_old2 ) / tdt2;
  x2 = pow2( x );

  x_old2 = x_old; x_old = x;
  return x;
}

DEFAULT_FUNCS_REG(TVibro)

// end of tvibro.cpp


