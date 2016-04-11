/***************************************************************************
   tmatrix34.c - definition of TMatrix34 class
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
#include "tmatrix34.h"

using namespace std;

const char* TMatrix34::helpstr = "<H1>TMatrix34</H1>\n"
 "<p>This element applies matrix 3x4 trasform to 3D vector<br>\n"
 " (x,y,z)=A*(x_in,y_in,z_in)\n</p>";

STD_CLASSINFO(TMatrix34,clpElem );

CTOR(TMatrix34,TMiso)
{
}

double TMatrix34::f() noexcept
{
  x = cx_x * x_in  +  cx_y * y_in  + cx_z * z_in + cx_0;
  y = cy_x * x_in  +  cy_y * y_in  + cy_z * z_in + cy_0;
  z = cz_x * x_in  +  cz_y * y_in  + cz_z * z_in + cz_0;
  return x;
}


DEFAULT_FUNCS_REG(TMatrix34)


// end of tmatrix34.cpp

