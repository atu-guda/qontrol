/***************************************************************************
  terrmeasure.cpp -- error measure element implementation
                             -------------------
    begin                : 2016.05.23
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

#include <cmath>

#include "miscfun.h"
#include "terrmeasure.h"

using namespace std;

const char* TErrMeasure::helpstr = "<H1>TErrMeasure</H1>\n"
 "<p>Misc error measures\n";

STD_CLASSINFO(TErrMeasure,clpElem );

CTOR(TErrMeasure,TMiso)
{
}

int TErrMeasure::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  reset_vals();
  return 1;
}

void TErrMeasure::reset_vals()
{
  t_rst = 0;
  s2 = sp = 0;
  mi1 = mi2 = mip = mmax = 0;
}

double TErrMeasure::f() noexcept
{
  if( rst > 0.1 ) {
    reset_vals();
  };

  er = in_x - in_y;
  er1 = fabs( er );
  er2 = pow2( er );
  erp = pow( er1, p );

  t_rst += ctdt;
  mi1 += er1 * ctdt;  m1 = mi1 / t_rst;
  s2  += er2 * ctdt;  m2 = sqrt( s2 / t_rst );          mi2 = t_rst * m2; // horrorfull but works.
  sp  += erp * ctdt;  mp = pow(  sp / t_rst, (1.0/p) ); mip = mp * t_rst;

  if( mmax < er1 ) {
    mmax = er1;
  }

  return m2;
}

DEFAULT_FUNCS_REG(TErrMeasure)

// end of terrmeasure.cpp

