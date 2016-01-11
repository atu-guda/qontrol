/***************************************************************************
                          tcounter.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tcounter.h"

const char* TCounter::helpstr = "<H1>TCounter</H1>\n"
 "Counts input pulses up to <b>n</b>, and changes output by type.<br>\n"
 "Parameters:<br>\n"
 " - <b>type</b> -- output type: level, pulse{+, +-}.. ;<br>\n"
 " - <b>n</b> -- up to count;<br>\n"
 " - <b>useReset</b> -- use in_rst as reset counter signal;<br>\n"
 " - <b>cn</b> -- counter [0;n-1] (ro).<br>\n";

STD_CLASSINFO(TCounter,clpElem );

CTOR(TCounter,TMiso)
{
}


int TCounter::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  cn = flip = 0; u_old = DMAX;
  return 1;
}

double TCounter::f() noexcept
{
  double v, du;
  int tick;
  du = in_x - u_old; u_old  = in_x; tick = 0;
  if( useReset && in_rst > 0.1 ) {
    cn = flip = 0;
  } else {
    if( du > 0.1 ) {
      ++cn;
      if( cn >= n ) {
        cn = 0; tick = 1; flip = ! flip;
      };
    };
  };
  switch( (int)type ) {
    case co_level:
      v = flip; break;
    case co_plus:
      v = tick; break;
    case co_pm:
      v = tick ? ( -1 + 2*flip ) : 0; break;
    case co_n:
      v = cn; break; // count;
    default: v = 0;
  };
  return v;
}

DEFAULT_FUNCS_REG(TCounter)


// end of tcounter.cpp

