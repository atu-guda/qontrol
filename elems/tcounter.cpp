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

#include "miscfun.h"
#include "tcounter.h"

const char* TCounter::helpstr = "<H1>TCounter</H1>\n"
 "Counts input pulses up to <b>n</b>, and changes output by type.<br>\n"
 "Parameters:<br>\n"
 " - <b>type</b> -- output type: level, pulse{+, +-}.. ;<br>\n"
 " - <b>n</b> -- up to count;<br>\n"
 " - <b>cn</b> -- counter [0;n-1] (ro).<br>\n";

STD_CLASSINFO(TCounter,clpElem );

CTOR(TCounter,TMiso)
{
}


int TCounter::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  cn = (int)(out0_init);
  return 1;
}

double TCounter::f() noexcept
{
  if( in_p ) {
    ++cn;
  }
  if( in_m ) {
    --cn;
  }
  if( in_rst ) {
    cn = (int)(out0_init);
  }
  if( modn ) {
    cn %= n;
  } else {
    cn = vBound( 0, (int)(cn), (int)(n) );
  }
  return a * cn + b;
}

DEFAULT_FUNCS_REG(TCounter)


// end of tcounter.cpp

