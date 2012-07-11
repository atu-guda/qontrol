/***************************************************************************
                          tcounter.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tcounter.h"

const char* TCounter::helpstr = "<H1>TCounter</H1>\n"
 "Counts input pulses up to <b>n</b>, and changes output by type.<br>\n"
 "Parameters:<br>\n"
 " - <b>type</b> -- output type: level, pulse{+, +-}.. ;<br>\n"
 " - <b>n</b> -- up to count;<br>\n"
 " - <b>useReset</b> -- use u[1] as reset counter signal;<br>\n"
 " - <b>cn</b> -- counter [0;n-1] (ro).<br>\n";

TClassInfo TCounter::class_info = {
  CLASS_ID_TCounter, "TCounter", TCounter::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TCounter::tcounter_d_i[16] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "dial", "", "Dialog for TCounter"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Output"},
 { dtpInt, dtpsList,   4,   20,  70, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", "level\npulse+\npulse+-\nn"},
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_n", "", "n"},
 { dtpInt,        0,   0,   20, 110, 100,  20, 2,  1.0, 1e9, 0, 0, "n", "n", ""},
 { dtpInt,   dtpsSw,   0,  170,  70, 150,  20, 2,  0.0, 0.0, 0, 0, "useReset", "", "Use u[1] as reset"},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpInt,        0,   0,    0,   0,   0,   0, 52, 0.0, 0.0, 0, 0, "cn", "Current n", ""},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,  0, 0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TCounter::TCounter( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( type, "Type" ),
	 PRM_INIT( n, "n" ),
	 PRM_INIT( cn, "cn" ),
	 PRM_INIT( useReset, "use Reset" )
{
  int i;
  cn = type = useReset = flip = 0; u_old = 9e300; n = 2;
  d_i = tcounter_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &n; ptrs[7] = &useReset;
  ptrs[11] = &cn;
  // from TMiso 
  ptrs[12] = links;
  ptrs[13] = &vis_x; ptrs[14] = &vis_y;
  PRMI(type).setDescr( "Type of counters output" );
  PRMI(type).setElems( "level\npulse+\npulse+-\nn" );
  PRMI(n).setDescr( "Number to count" );
  PRMI(cn).setDescr( "Current counter value" );
  PRMI(useReset).setDescr( "Use u[1] as Reset signal" );

}

TCounter::~TCounter()
{
}

TDataSet* TCounter::create( TDataSet* apar )
{
  return new TCounter( apar );
}

int TCounter::getClassId(void) const 
{
  return CLASS_ID_TCounter;
}


const TClassInfo* TCounter::getClassInfo(void) const
{
  return &class_info;
}

const char *TCounter::getHelp(void) const
{
  return helpstr;
}

const char** TCounter::getIcon(void) const
{
  return icon;
}


int TCounter::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  cn = flip = 0; u_old = 9e300;
  return rc;
}

double TCounter::f( double /* t */ )
{
  double v, du;
  int tick;
  du = *in_so[0] - u_old; u_old  = *in_so[0]; tick = 0;
  if( useReset && *in_so[1] > 0.1 ) {
    cn = flip = 0; 
  } else {
    if( du > 0.1 ) {
      cn++;
      if( cn >= n ) {
	cn = 0; tick = 1; flip = ! flip;
      };
    };
  };
  switch( type ) {
    case 0: v = flip; break; // level
    case 1: v = tick; break; // pulse+
    case 2: v = tick ? ( -1 + 2*flip ) : 0; break; // pulse+-
    case 3: v = cn; break; // count;
    default: v = 0;
  };
  return v;
}

int TCounter::registered = reg();

int TCounter::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TCounter::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjijiiijjjjj",
"jjjjjjjjjjjjjjjjjjjjjjiijjjijjjj",
"jjjjjjjjjjjjjjjjjjjjjjijjjjijjjj",
"jjjjjjjjjjjjjjjjjjjjjjijjjjijjjj",
"jjjjjjjjjjjjjjjjjjjjjjijjjjijjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjjjjiijjjjjiijjjjiijjjjjjjjjjjj",
"jjiiiiiiiiiiiiiiiiiiiiiiiiiiijjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjgggggggggggjjj",
"jjjjjjjjjjjjjjjjjjgggggggggggjjj",
"jjjjjjjjjjjjjjjjjjggjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjggjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjggjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjggjjjjjjjjjjjj",
"jjggggggggggggggggggjjjjjjjjjjjj",
"jjggggggggggggggggggjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};


// end of tcounter.cpp

