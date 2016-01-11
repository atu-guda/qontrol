/***************************************************************************
                          tcriterion.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#include <cmath>
#include "tcriterion.h"


const char* TCriterion::helpstr = "<H1>TCriterion</H1>\n"
 "<p>Miscellaneous criterions in diff signal x=in_pos - in_neg: <br>\n"
 "Parameter <b>type</b> -- type of criterion. <br>\n"
 "Parameters <b>a,b</b> -- to with compare x. <br>\n"
 "Parameter <b>t0</b> -- start time (if enabled). <br>\n"
 "5 flags: <b>useA, useT0, useEnable, useLock, usePulse</b>.</p>";

STD_CLASSINFO(TCriterion,clpElem);

CTOR(TCriterion,TMiso)
{
}


int TCriterion::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  st = 0;
  return 1;
}

double TCriterion::f() noexcept
{
  int ok, rv;
  double aa, x;
  if( useT0 && t < t0 ) {
    return 0;
  }
  x = in_pos - in_neg;
  if( useLock && st > 0 )
    return usePulse ? 0 : 1;
  if( useEnable && in_ena < 0.1 ) { // disabled
    if( !st ) {
      return 0;
    }
    return usePulse ? 0 : 1;
  };

  aa = a;
  if( useA ) {
    aa = in_a;
  }

  switch( (int)type ) {
    case cr_mod_less:
      ok = ( fabs( x ) < aa ); break;
    case cr_mod_greate:
      ok = ( fabs( x ) > aa ); break;
    case cr_less:
      ok = ( x < aa ); break;
    case cr_greate:
      ok = ( x > aa ); break;
    case cr_between:
      ok = ( (x > aa) && (x < b) ); break;
    default: ok = 0;
  };

  if( usePulse ) {
    if( ok == st ) {
      rv = 0;
    } else {
      rv = ok > st ? 1 : -1;
    };
  } else {
    rv = ok;
  };

  st = ok;
  // TODO: break all computation action
  return rv;
}

DEFAULT_FUNCS_REG(TCriterion)


// end of tcriterion.cpp


