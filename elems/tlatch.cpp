/***************************************************************************
                          tlatch.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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

static const char* const tlatch_list = "Time\nSignal u[1]";

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
 
TClassInfo TLatch::class_info = {
  CLASS_ID_TLatch, "TLatch", TLatch::create,
  &TMiso::class_info, helpstr, clpElem };


TLatch::TLatch( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( type, "Type" ),
	 PRM_INIT( t0, "Time" ),
	 PRM_INIT( v_st, "Start value" ),
	 PRM_INIT( v, "v" )
{
  t0 = u_old = 0; type = 0; usePulse = useFirst = useAdd = 0; wasLatch = -1;
  v = v_st = 0;

  PRMI(type).setDescr( "Type" );
  PRMI(type).setElems( tlatch_list );
  PRMI(t0).setDescr( "Time" );
  PRMI(v_st).setDescr( "Start value" );
  PRMI(v).setDescr( "v" );
}

TLatch::~TLatch()
{
}

TDataSet* TLatch::create( TDataSet* apar )
{
  return new TLatch( apar );
}

int TLatch::getClassId(void) const 
{
  return CLASS_ID_TLatch;
}


const TClassInfo* TLatch::getClassInfo(void) const
{
  return &class_info;
}

const char *TLatch::getHelp(void) const
{
  return helpstr;
}

const char** TLatch::getIcon(void) const
{
  return icon;
}

int TLatch::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  v = v_st; u_old = 0;  wasLatch = -1;
  return rc;
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
  switch( type ) {
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

int TLatch::registered = reg();

int TLatch::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TLatch::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32        5            1",
/* colors */
"c c #808080",
"b c #000000",
"a c #0000ff",
". c #dcdcdc",
"# c #ff0000",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"............................bb..",
".......................bbbbb....",
"....................bbb.........",
".................bbb............",
"..............bbb...............",
"............bbaaaaaaaaaaaaaaaaa.",
"..........bbaaaaaaaaaaaaaaaaaaa.",
".........bb.....................",
"........bb......................",
".......bb.......................",
"......bb........................",
".....bb.........................",
"....bb..........................",
"....bb..........................",
".ccbbcccccccccccccccccccccccccc.",
"................................",
"............##..................",
"............##..................",
"............##..................",
"............##..................",
"............##..................",
"............##..................",
".###########cc#################.",
"................................"
};

// end of tlatch.cpp

