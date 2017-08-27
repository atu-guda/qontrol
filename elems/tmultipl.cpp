/***************************************************************************
                          tmultipl.cpp  -  description
                             -------------------
    begin                : Fri Aug 25 2000
    copyright            : (C) 2000-2017 by atu
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

#include "tmultipl.h"
#include "miscfun.h"

const char* TMultipl::helpstr = "<H1>TMultipl</H1>\n"
 "<p>Simple multiplicator: <br/>\n"
 "it multimplies all enabled inputs and parametr <b>a</b><br/>\n"
 "a ( in_0 * in_1 * in_2 * in_3 ) / c + b</b> <br/>\n"
 " or <br/>\n"
 "a sqrt0( in_0 * in_1 * in_2 * in_3 ) / c + b</b> \n"
 "</p>";

STD_CLASSINFO(TMultipl,clpElem|clpCalcAtStart);

CTOR(TMultipl,TMiso)
{
}


double TMultipl::f() noexcept
{
  double v = 1;
  v *= in_0;
  v *= in_1;
  v *= in_2;
  v *= in_3;
  if( use_sqrt ) {
    v = sqrt0( v );
  }
  v *= a / c;
  v += b;
  return v;
}

DEFAULT_FUNCS_REG(TMultipl)

// end of tmultipl.cpp


