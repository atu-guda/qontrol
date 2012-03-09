/***************************************************************************
                          tlatch.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000 by atu
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
  &TMiso::class_info, helpstr };

TDataInfo TLatch::tlatch_d_i[19] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "dial", "", "Dialog for TLatch"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  60,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,   2,   20,  80, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "", "Time\nSignal u[1]"},
 { dtpLabel,      0,   0,   30, 120,  50,  20, 0,  0.0, 0.0, 0, 0, "l_t0", "", "t0"},
 { dtpDou,        0,   0,   20, 140, 120,  20, 2,  0.0, 1e300, 0, 0, "t0", "t0", ""},
 { dtpLabel,      0,   0,   30, 180, 100,  20, 0,  0.0, 0.0, 0, 0, "l_v_st", "", "Start value"},
 { dtpDou,        0,   0,   20, 200, 120,  20, 2,  -1e300, 1e300, 0, 0, "v_st", "start value", ""},
 { dtpInt,   dtpsSw,   0,  180,  50, 150,  20, 2,  0.0, 0.0, 0, 0, "usePulse", "", "use Pulse"},
 { dtpInt,   dtpsSw,   0,  180,  80, 150,  20, 2,  0.0, 0.0, 0, 0, "useFirst", "", "only First"},
 { dtpInt,   dtpsSw,   0,  180, 110, 150,  20, 2,  0.0, 0.0, 0, 0, "useAdd", "", "Add to value"},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TLatch::TLatch( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  t0 = u_old = 0; type = 0; usePulse = useFirst = useAdd = 0; wasLatch = -1;
  v = v_st = 0;
  d_i = tlatch_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &t0; ptrs[8] = &v_st;
  ptrs[9] = &usePulse; ptrs[10] = &useFirst; ptrs[11] = &useAdd;
  // from TMiso 
  ptrs[15] = links;
  ptrs[16] = &vis_x; ptrs[17] = &vis_y;
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

const char* TLatch::getClassName(void) const 
{
  return "TLatch";
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

double TLatch::f( const double* u, double t )
{
  double dv, bv; 
  int ok;
  dv = u[1] - u_old; u_old = u[1];
  if( wasLatch == -1 ) { 
    dv = 0; wasLatch = 0; // first step
  }; 
  bv = useAdd ? v : 0;
  switch( type ) {
    case 0: if( t >= t0 ) {
              if( wasLatch ) break;
	      wasLatch = 1; v = bv + u[0];
	    };
            break;
    case 1: if( useFirst && (wasLatch > 0) ) 
	      break;
            if( usePulse ) {
	      ok = ( dv > 0.5 );
	    } else {
	      ok = ( u[1] > 0.1 );
	    };
	    if( ok ) { 
	      wasLatch = 1; v = bv + u[0];
	    };
	    break;
    default: ;
  };
  return v;
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

