/***************************************************************************
                          ../elems/tthrum.cpp  -  description
                             -------------------
    begin                : 2013.08.17
    copyright            : (C) 2013-2022 by atu
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

#include "tthrum.h"

const char* TThrum::helpstr = "<H1>TThrum</H1>\n"
 "Dummy element: to test interface and holders <br>\n";

STD_CLASSINFO(TThrum,clpElem);

CTOR(TThrum,TMiso)
{
}

double TThrum::f() noexcept
{
  if( prm_mod ) {
    fixparm();
  }
  return in0 * p_dou + doup;
}

void TThrum::fixparm()
{
  p_int = 17;
  if( doup > 42 ) { doup = 42; }
}

DEFAULT_FUNCS_REG(TThrum)


// end of tlinear.cpp

