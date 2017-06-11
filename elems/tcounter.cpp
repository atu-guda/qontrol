/***************************************************************************
                          tcounter.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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
  avT = 1.0; lastT = 1.0; avF = 1.0; lastF = 1.0; // fake values
  tick_t = 0; rst_t = 0;
  return 1;
}

double TCounter::f() noexcept
{
  double old_tick_t = tick_t;
  if( in_p.lval() ) {
    ++cn;
    tick_t = ct;
    lastT = tick_t - old_tick_t;
    lastF = 1 / lastT;
    lastOm = 2 * M_PI * lastF;
    avT = ( tick_t - rst_t ) / cn;
    avF = 1 / avT;
    avOm = 2 * M_PI * avF;

  }
  if( in_m.lval() ) {
    --cn;
  }
  if( in_rst ) {
    cn = (int)(out0_init);
    tick_t = ct;
    rst_t = ct;
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

