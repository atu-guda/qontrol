/***************************************************************************
                          tlogic.cpp  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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

#include "tlogic.h"

const char* TLogic::helpstr = "<H1>TLogic</H1>\n"
 "<p>Simple logic element: <br>\n"
 "Parameters: <br>\n"
 " - <b>type</b> determinated type of logic: ANR, OR, XOR ... <br>\n"
 " - <b>level</b> -- level of logical 1 on input; <br>\n"
 " - <b>useNInp0 - useNInp3</b> -- inverse inputs; <br>\n"
 " - <b>useNOut</b> -- inverse output; <br>\n"
 " - <b>useMinus</b> -- negative output == -1, not 0.  <br>\n"
 "For AND unused inputs link to ':one' .</p>";

STD_CLASSINFO(TLogic,clpElem|clpCalcAtStart);

CTOR(TLogic,TMiso)
{
}


double TLogic::f() noexcept
{
  int ou;
  int iu0 = ( in_0 > level );
  int iu1 = ( in_1 > level );
  int iu2 = ( in_2 > level );
  int iu3 = ( in_3 > level );
  if( useNInp0 ) { iu0 = ! iu0; }
  if( useNInp1 ) { iu1 = ! iu1; }
  if( useNInp2 ) { iu2 = ! iu2; }
  if( useNInp3 ) { iu3 = ! iu3; }
  ou = 0;
  switch( (int)type ) {
    case logAnd:
      if( iu0 > 0 && iu1 > 0 && iu2 > 0 && iu3 > 0 ) {
        ou = 1;
      }
      break;
    case logOr:
      if( iu0 > 0 || iu1 > 0 || iu2 > 0 || iu3 > 0 ) {
        ou = 1;
      }
      break;
    case logXor:
      if( iu0 ) {
          ou = !ou;
      }
      if( iu1 ) {
          ou = !ou;
      }
      if( iu2 ) {
          ou = !ou;
      }
      if( iu3 ) {
          ou = !ou;
      }
      break;
    case logU0:
      ou = iu0; break;
    case logZero:
      ou = 0; break;
    case logOne:
      ou = 1; break;
    default: ;
  };
  if( useNOut ) {
    ou = !ou;
  }
  if( ou ) {
    return 1;
  }
  return useMinus ? -1 : 0;
}

DEFAULT_FUNCS_REG(TLogic)


// end of tlogic.cpp



