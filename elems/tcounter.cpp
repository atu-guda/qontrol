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


TCounter::TCounter( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( type, "Type" ),
	 PRM_INIT( n, "n" ),
	 PRM_INIT( cn, "cn" ),
	 PRM_INIT( useReset, "use Reset" )
{
  cn = type = useReset = flip = 0; u_old = 9e300; n = 2;
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

