/***************************************************************************
  tdyn3d.cpp - TDyn3D class definition
                             -------------------
    begin                : 2016.04.12
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

#include <ctime>
#include <cmath>
#include <gsl/gsl_math.h>
#include "miscfun.h"
#include "tdyn3d.h"

using namespace std;

const char* TDyn3D::helpstr = "<h1>TDyn3D</h1>\n"
 "<p>Simple 3D integrator <br/>\n"
 " \\dot{x} = c_x * v_x; <br/>\n"
 " \\dot{y} = c_y * v_y; <br/>\n"
 " \\dot{z} = c_z * v_z; <br/>\n"
 " <br>\n"
 " Inputs 0-2: v_x, v_y, v_z <br/>\n"
 " <br/>\n"
 " Main outputs out0= x variable.</p>\n";

STD_CLASSINFO(TDyn3D,clpElem );

CTOR(TDyn3D,TMiso)
{
}

double TDyn3D::f() noexcept
{
  v = gsl_hypot3( v_x, v_y, v_z );

  x += ctdt * v_x * c_x;
  y += ctdt * v_y * c_y;
  z += ctdt * v_z * c_z;
  return x;
}


int TDyn3D::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x = (double)x_0; y = (double)y_0 ; z = (double)z_0;
  out0 = x; // ????
  return 1;
}

DEFAULT_FUNCS_REG(TDyn3D)


// end of tdyn3d.cpp

