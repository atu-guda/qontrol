/***************************************************************************
                          trossler.cpp  -  description
                             -------------------
    begin                : Thu May 03 2012
    copyright            : (C) 2012-2418 by atu
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

#include <time.h>
#include <cmath>
#include <gsl/gsl_math.h>
#include "miscfun.h"
#include "trossler.h"

const char* TRossler::helpstr = "<h1>TRossler</h1>\n"
 "<p>Simulator element for Rossler system <br/>\n"
 " \\dot{x} = -y -z + in_x;   <br/>\n"
 " \\dot{y} = x + a * y + in_y; <br/>\n"
 " \\dot{z} = b + z * (x-c) + in_z<br/>\n"
 " <br/>\n"
 " Inputs in_[xyz]: add to \\dot{x}, \\dot{y}, \\dot{z} <br/>\n"
 " <br/>\n"
 " Outputs <b>x</b> variable<br/>\n"
 " <br/>\n"
 "Have 6 parameters: 3 from system, <b>a, b ,c </b>, <br/>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br/>"
 "First 3 may can be changed at any time. <br/>out0_init is <b>ignored</b>!</p>";

STD_CLASSINFO(TRossler,clpElem );

CTOR(TRossler,TMiso)
{
}


double TRossler::f() noexcept
{
  vx = -y - z;
  vy =  x + a*y;
  vz =  b + xz - z*c;

  x += ctdt * ( vx + in_x );
  y += ctdt * ( vy + in_y );
  z += ctdt * ( vz + in_z );
  calcAux();
  return x;
}

void TRossler::calcAux() noexcept
{
  x2 = pow2( x ); y2 = pow2( y ); z2 = pow2( z );
  xy = x * y; xz = x * z; yz = y * z;
  v = gsl_hypot3( vx, vy, vz );
}

int TRossler::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x = (double)x_0; y = (double)y_0 ; z = (double)z_0;
  calcAux();
  out0 = x;
  return 1;
}

DEFAULT_FUNCS_REG(TRossler)


// end of trossler.cpp

