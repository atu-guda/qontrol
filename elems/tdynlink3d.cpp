/***************************************************************************
  tdynlink3d.cpp - TDynLink3D class definition
                             -------------------
    begin                : 2016.04.12
    copyright            : (C) 2016-2017 by atu
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

#include <ctime>
#include <cmath>
#include <gsl/gsl_math.h>
#include "miscfun.h"
#include "tdynlink3d.h"

using namespace std;

const char* TDynLink3D::helpstr = "<h1>TDynLink3D</h1>\n"
 "<p>3D integrator with self links: <br/>\n"
 " \\dot{x} = a_x * ( cx_0 + cx_x x + cx_y y ... +  v_x ); <br/>\n"
 " \\dot{y} = a_y * ( cy_0 + cy_x x + cy_y y ... +  v_y ); <br/>\n"
 " \\dot{z} = a_z * ( cz_0 + cz_x x + cz_y y ... +  v_z ); <br/>\n"
 " <br>\n"
 " Inputs 0-2: v_xa, v_ya, v_za <br/>\n"
 " <br/>\n"
 " Main outputs out0= x variable.</p>\n";

STD_CLASSINFO(TDynLink3D,clpElem );

CTOR(TDynLink3D,TMiso)
{
}

void TDynLink3D::calc_misc()
{
  x2 = x*x; y2 = y*y; z2 = z*z;
  xy = x*y; yz = y*z; xz = x*z;
  ax = fabs( x ); ay = fabs( y ); az = fabs( z );
  x_plus_y   =   x + y;    x_plus_z = x + z;  y_plus_z   = y +  z;
  x2_plus_y2 = x2 + y2; x2_plus_z2 = x2 + z2; y2_plus_z2 = y2 + z2;
  x_plus_y_plus_z = x + y + z;  x2_plus_y2_plus_z2 = x2 + y2 + z2;
  sq_xy = sqrt0( xy ); sq_xz = sqrt0( xz ); sq_yz = sqrt0( yz );
}


void TDynLink3D::calc_v()
{
  v_x = cx_0 + v_xa
      + cx_x  * x   + cx_y  *  y  +  cx_z  * z
      + cx_x2 * x2  + cx_y2 * y2  +  cx_z2 * z2
      + cx_xy * xy  + cx_yz * yz  +  cx_xz * xz;
  v_x *= a_x;

  v_y = cy_0 + v_ya
      + cy_x  * x   + cy_y  *  y  +  cy_z  * z
      + cy_x2 * x2  + cy_y2 * y2  +  cy_z2 * z2
      + cy_xy * xy  + cy_yz * yz  +  cy_xz * xz;
  v_y *= a_y;

  v_z = cz_0 + v_za
      + cz_x  * x   + cz_y  *  y  +  cz_z  * z
      + cz_x2 * x2  + cz_y2 * y2  +  cz_z2 * z2
      + cz_xy * xy  + cz_yz * yz  +  cz_xz * xz;
  v_z *= a_z;
  v = gsl_hypot3( v_x, v_y, v_z );
}

double TDynLink3D::f() noexcept
{
  calc_v();

  x += ctdt * v_x;
  y += ctdt * v_y;
  z += ctdt * v_z;
  calc_misc();
  return x;
}


int TDynLink3D::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x = (double)x_0; y = (double)y_0 ; z = (double)z_0;
  out0 = x; // ????

  calc_misc();
  calc_v();
  return 1;
}

DEFAULT_FUNCS_REG(TDynLink3D)


// end of tdynlink3d.cpp

