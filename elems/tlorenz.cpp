/***************************************************************************
                          tlorenz.cpp  -  description
                             -------------------
    begin                : Fri Mar 09 2012
    copyright            : (C) 2012-2016 by atu
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
#include "miscfun.h"
#include "tlorenz.h"

using namespace std;

const char* TLorenz::helpstr = "<H1>TLorenz</H1>\n"
 "<p>Simulator element for Lorenz system <br>\n"
 " \\dot{x} = \\sigma ( y-x ); + in_x<br>\n"
 " \\dot{y} = x (r-z ) - y + in_y; <br>\n"
 " \\dot{z} = xy - bz.<br> + in_z\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Main output out0= x variable<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>sigma, r ,b </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.</p>";

STD_CLASSINFO(TLorenz,clpElem );

CTOR(TLorenz,TMiso)
{
}

double TLorenz::f() noexcept
{
  vx = sigma * (y-x);
  vy = x * r - xz - y;
  vz = xy - b*z;

  x += ctdt * vx + in_x;
  y += ctdt * vy + in_y;
  z += ctdt * vz + in_z;
  calcAux();
  return x;
}

void TLorenz::calcAux() noexcept
{
  x2 = pow2( x ); y2 = pow2( y ); z2 = pow2( z );
  xy = x * y; xz = x * z; yz = y * z;
  v = sqrt( vx*vx + vy*vy + vz*vz );
}


int TLorenz::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  x = (double)x_0; y = (double)y_0 ; z = (double)z_0;
  calcAux();
  out0 = x; // ????
  return 1;
}

DEFAULT_FUNCS_REG(TLorenz)


// end of tlorenz.cpp

