/***************************************************************************
                          trossler.cpp  -  description
                             -------------------
    begin                : Thu May 03 2012
    copyright            : (C) 2012 by atu
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
#include <math.h> 
#include "miscfun.h" 
#include "trossler.h"

const char* TRossler::helpstr = "<H1>TRossler</H1>\n"
 "Simulator element for Rossler system <br>\n"
 " \\dot{x} = -y -x );   <br>\n"
 " \\dot{y} = x + a * y; <br>\n"
 " \\dot{z} = b + z * (x-c)<br>\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Outputs <b>x</b> variable - other can be get by TIinputAny<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>a, b ,c </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.";

STD_CLASSINFO(TRossler,clpElem );

CTOR(TRossler,TMiso)
{
}


double TRossler::f( double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( -y - z )      + *in_so[0];
  yn = y + tdt * ( x  + a*y )    + *in_so[1];
  zn = z + tdt * ( b + z*(x-c) ) + *in_so[2];
  x = xn; y = yn; z = zn;
  return x; 
}

int TRossler::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x = (double)x_0; y = (double)y_0 ; z = (double)z_0;
  return 0;
}

DEFAULT_FUNCS_REG(TRossler)


// end of trossler.cpp

