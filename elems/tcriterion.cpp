/***************************************************************************
                          tcriterion.cpp  -  description
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

#include <math.h>
#include "tcriterion.h"


const char* TCriterion::helpstr = "<H1>TCriterion</H1>\n"
 "Miscellaneous criterions: <br>\n"
 "Parameter <b>type</b> -- type of criterion. <br>\n"
 "Parameters <b>a,b</b> -- to with compare (x=u[0]-u[1]). <br>\n"
 "Parameter <b>t0</b> -- start time (if enabled). <br>\n"
 "5 flags: <b>useA, useT0, useEnable, useLock, usePulse</b>.";

TClassInfo TCriterion::class_info = {
  CLASS_ID_TCriterion, "TCriterion", TCriterion::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TCriterion::tcriterion_d_i[23] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "crit_dia", "", "Dialog for TCriterion"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   5,   20,  70, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", "|x|<a\n|x|>a\nx<a\nx>a\na<x<b"},
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a", "", "a"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,   30, 140,  50,  20, 0,  0.0, 0.0, 0, 0, "l_b", "", "b"},
 { dtpDou,        0,   0,   20, 160, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpLabel,      0,   0,   30, 190,  50,  20, 0,  0.0, 0.0, 0, 0, "l_t0", "", "t0"},
 { dtpDou,        0,   0,   20, 210, 120,  20, 0,  0.0, 1e300, 0, 0, "t0", "start time", ""},
 { dtpInt,   dtpsSw,   0,  170,  50, 180,  20, 2,  0.0, 0.0, 0, 0, "useA", "", "Use u[3] as 'a'"},
 { dtpInt,   dtpsSw,   0,  170,  80, 180,  20, 2,  0.0, 0.0, 0, 0, "useT0", "", "Work after t0"},
 { dtpInt,   dtpsSw,   0,  170, 110, 180,  20, 2,  0.0, 0.0, 0, 0, "useEnable", "", "Use u[2] as enable"},
 { dtpInt,   dtpsSw,   0,  170, 140, 180,  20, 2,  0.0, 0.0, 0, 0, "useLock", "", "Lock after first"},
 { dtpInt,   dtpsSw,   0,  170, 170, 180,  20, 2,  0.0, 0.0, 0, 0, "usePulse", "", "Pulse out(+-1,0)"},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TCriterion::TCriterion( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( type, "Type" ),
	 PRM_INIT( a, "a" ),
	 PRM_INIT( b, "b" ),
	 PRM_INIT( t0, "t0" ),
	 PRM_INIT( useA, "u[3] is a" ),
	 PRM_INIT( useT0, "t0" ),
	 PRM_INIT( useEnable, "u[2] is Enable" ),
	 PRM_INIT( useLock, "Lock" ),
	 PRM_INIT( usePulse, "Pulse Output" ),
	 PRM_INIT( st, "state" )
{
  int i;
  a = 0.5; b = 1.0; type = 0; t0 = 0; 
  useA = useT0 = useEnable = useLock = usePulse = 0;
  d_i = tcriterion_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &a; ptrs[8] = &b; ptrs[10] = &t0;
  ptrs[11] = &useA; ptrs[12] = &useT0; ptrs[13] = &useEnable; 
  ptrs[14] = &useLock; ptrs[15] = &usePulse;
  // from TMiso 
  ptrs[19] = links;
  ptrs[20] = &vis_x; ptrs[21] = &vis_y;
  PRMI(type).setDescr( "Type of criterion" );
  PRMI(type).setElems( "|x|<a\n|x|>a\nx<a\nx>a\na<x<b" );
  PRMI(a).setDescr( "level 'a'" );
  PRMI(b).setDescr( "level 'b'" );
  PRMI(t0).setDescr( "Start time" );
  PRMI(useA).setDescr( "Use u[3] as 'a' value" );
  PRMI(useA).setParm( "sep", "col" );
  PRMI(useT0).setDescr( "Dont work before given time (t0)" );
  PRMI(useEnable).setDescr( "Use u[2] signal as Enable" );
  PRMI(useLock).setDescr( "Lock after first front" );
  PRMI(usePulse).setDescr( "output is pulse +1, 0, -1" );
  PRMI(st).setDescr( "current state" );
}

TCriterion::~TCriterion()
{
}

TDataSet* TCriterion::create( TDataSet* apar )
{
  return new TCriterion( apar );
}

int TCriterion::getClassId(void) const 
{
  return CLASS_ID_TCriterion;
}


const TClassInfo* TCriterion::getClassInfo(void) const
{
  return &class_info;
}

const char *TCriterion::getHelp(void) const
{
  return helpstr;
}

const char** TCriterion::getIcon(void) const
{
  return icon;
}

int TCriterion::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  st = 0;
  return rc;
}

double TCriterion::f( double t )
{
  int ok, rv; 
  double aa, x;
  if( useT0 && t < t0 ) return 0;
  x = *in_so[0] - *in_so[1];
  if( useLock && st > 0 )
    return usePulse ? 0 : 1;
  if( useEnable && *in_so[2] < 0.1 ) {
    if( !st ) return 0;
    return usePulse ? 0 : 1;
  };
  aa = a;
  if( useA )
    aa = *in_so[3];
  switch( type ) {
    case 0: ok = ( fabs( x ) < aa ); break;
    case 1: ok = ( fabs( x ) > aa ); break;
    case 2: ok = ( x < aa ); break;
    case 3: ok = ( x > aa ); break;
    case 4: ok = ( (x > aa) && (x < b) ); break;
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
  return rv;
}

int TCriterion::registered = reg();

int TCriterion::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TCriterion::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32      5            1  ",
"c c #808080",
"b c #000000",
"a c #0000ff",
". c #dcdcdc",
"# c #ff0000",
/* pixels */
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"............aaaaaa..aaaa........",
"............a....a..a..a........",
"............a....a..a..a........",
"............a....a..a..a........",
"..aaaaaaaaaaa....aaaa..aaaaaaa..",
"................................",
"...............b................",
"..............b.b...............",
"..............b.b...............",
"..###########b##b############...",
".............b...b..............",
"......b......b...b..............",
".....b.b....b.....b.............",
".....b.b....b.....b....bbbb.....",
"....b...b...b.....b....b..b.....",
"....b...b..b.......b...b..bbb...",
"...b.....b.b.......b...b........",
"...b.....b.b.......b...b........",
"..........b.........b..b........",
"..##################b##b######..",
"....................b..b........",
".....................bb.........",
"................................"
};

// end of tcriterion.cpp


