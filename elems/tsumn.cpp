/***************************************************************************
                          tsumn.h  - N-input linear summator
                             -------------------
    begin                : 2015.10.31
    copyright            : (C) 2015-2019 by atu
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

#include "tsumn.h"

const char* TSumN::helpstr = "<H1>TSumN</H1>\n"
 "N-input Linear summator: <br>\n"
 "<b>x = b + a * \\sum_i u_i </b><br>\n";

STD_CLASSINFO(TSumN,clpElem|clpCalcAtStart);

CTOR(TSumN,TMiso)
{
  allowed_types = "InputSimple,+SPECIAL";
}

double TSumN::f() noexcept
{
  double r = 0;
  for( auto i : qAsConst( inps_s ) ) {
    r += static_cast<double>(*i);
  }

  r *= a;
  r += b;
  x2 = r * r;
  return r;
}


DEFAULT_FUNCS_REG(TSumN)


// end of tsumn.cpp

