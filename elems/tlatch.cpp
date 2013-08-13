/***************************************************************************
                          tlatch.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000-2013 by atu
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
 "Latch: can latch (or add) input value u[0], depend on <b>type</b>: <br>\n"
 "a) at given time <b>t0</b> (type=0); <br>\n"
 "b) by the signal of u[1]. (type=1).<br>\b"
 "Parameters: <br>\n"
 "- <b>type</b>. - latch at given time t0 or by u[1] signal;<br>\n"
 "- <b>t0</b>. - time to latch (if type=0);<br>\n"
 "- <b>v_st</b>. - start value;<br>\n"
 "- <b>usePulse</b>. - latch on jump of u[1] (>0.5), not level;<br>\n"
 "- <b>useFirst</b>. - count only first signal to latch, ignore all other;<br>\n"
 "- <b>useAdd</b>. - add current u[0] to value, not set.<br>\n";
 
STD_CLASSINFO(TLatch,clpElem);

CTOR(TLatch,TMiso)
{
  t0 = u_old = 0; type = 0; usePulse = useFirst = useAdd = 0; wasLatch = -1;
  v = v_st = 0;
}


int TLatch::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  v = (double)v_st; u_old = 0;  wasLatch = -1;
  return 0;
}

double TLatch::f( double t )
{
  double dv, bv; 
  int ok;
  dv = *in_so[1] - u_old; u_old = *in_so[1];
  if( wasLatch == -1 ) { 
    dv = 0; wasLatch = 0; // first step
  }; 
  bv = useAdd ? v : 0;
  switch( (int)type ) {
    case 0: if( t >= t0 ) {
              if( wasLatch ) break;
	      wasLatch = 1; v = bv + *in_so[0];
	    };
            break;
    case 1: if( useFirst && (wasLatch > 0) ) 
	      break;
            if( usePulse ) {
	      ok = ( dv > 0.5 );
	    } else {
	      ok = ( *in_so[1] > 0.1 );
	    };
	    if( ok ) { 
	      wasLatch = 1; v = bv + *in_so[0];
	    };
	    break;
    default: ;
  };
  return v;
}

DEFAULT_FUNCS_REG(TLatch)


// end of tlatch.cpp

