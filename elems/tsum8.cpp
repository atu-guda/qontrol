/***************************************************************************
                          tsum8.cpp  - 8-in summator implementation
                             -------------------
    begin                : 2015.02.22
    copyright            : (C) 2015-2016 by atu
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

#include "tsum8.h"

const char* TSum8::helpstr = "<H1>TSum8</H1>\n"
 "8-input Linear summator: <br>\n"
 "<b>x = a_0*u_0 + a_1*u_1 + ... + a_7*u_7 + b</b><br>\n";

STD_CLASSINFO(TSum8,clpElem);

CTOR(TSum8,TMiso)
{
}

double TSum8::f( double /* t */ )
{
  return u_0*a_0 + u_1*a_1 + u_2*a_2 + u_3*a_3 +
         u_4*a_4 + u_5*a_5 + u_6*a_6 + u_7*a_7 + b;
}

DEFAULT_FUNCS_REG(TSum8)


// end of tsum8.cpp

