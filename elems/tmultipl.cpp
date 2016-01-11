/***************************************************************************
                          tmultipl.cpp  -  description
                             -------------------
    begin                : Fri Aug 25 2000
    copyright            : (C) 2000-2016 by atu
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

const char* TMultipl::helpstr = "<H1>TMultipl</H1>\n"
 "<p>Simple multiplicator: <br>\n"
 "it multimplies all enabled inputs and parametr <b>a</b>\n"
 "</p>";

STD_CLASSINFO(TMultipl,clpElem);

CTOR(TMultipl,TMiso)
{
}


double TMultipl::f() noexcept
{
  double v = a;
  v *= in_0;
  v *= in_1;
  v *= in_2;
  v *= in_3;
  return v;
}

DEFAULT_FUNCS_REG(TMultipl)

// end of tmultipl.cpp


