/***************************************************************************
                          tlatch.cpp  -  description
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

#include "tlatch.h"


const char* TLatch::helpstr = "<H1>TLatch</H1>\n"
 "<p> Latch: can latch (or add) input value in_u, depend on <b>type</b>: <br>\n"
 "a) at given time <b>t0</b> (type=0); <br>\n"
 "b) by the signal of in_latch. (type=1).<br>\n"
 "Parameters: <br>\n"
 "- <b>type</b>. - latch at given time t0 or by in_latch signal;<br>\n"
 "- <b>t0</b>. - time to latch (if type=0);<br>\n"
 "- <b>usePulse</b>. - latch on jump of in_latch (>0.5), not level;<br>\n"
 "- <b>useFirst</b>. - count only first signal to latch, ignore all other;<br>\n"
 "- <b>useAdd</b>. - add current u[0] to value, not set.<br>\n</p>";

STD_CLASSINFO(TLatch,clpElem);

CTOR(TLatch,TMiso)
{
}


int TLatch::do_startLoop( int acnx, int acny )
{
  TMiso::do_startLoop( acnx, acny );
  v = (double)out0_init; u_old = lt = 0;  wasLatch = -1;
  return 1;
}

double TLatch::f() noexcept
{
  double dv, bv;
  int ok;
  dv = in_latch - u_old;
  u_old = in_latch;
  if( wasLatch == -1 ) {
    dv = 0; wasLatch = 0; // first step
  };
  bv = useAdd ? v : 0;

  switch( (int)type ) {
    case latchTime:
      if( ct >= t0 ) {
        if( wasLatch ) {
          break;
        }
        wasLatch = 1; lt = ct; v = bv + in_u;
      };
      break;
    case latchSignal:
      if( useFirst && (wasLatch > 0) ) {
        break;
      }
      if( usePulse ) {
        ok = ( dv > 0.5 );
      } else {
        ok = ( in_latch > 0.1 );
      };
      if( ok ) {
        wasLatch = 1; lt = ct; v = bv + in_u;
      };
      break;
    default: ;
  };
  return v;
}

DEFAULT_FUNCS_REG(TLatch)


// end of tlatch.cpp

