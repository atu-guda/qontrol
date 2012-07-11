/***************************************************************************
                          ttrigger.cpp  -  description
                             -------------------
    begin                : Sun Sep 3 2000
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

#include "ttrigger.h"

const char* TTrigger::helpstr = "<H1>TTrigger</H1>\n"
 "Different kinds of triggers: <br>\n"
 "Inputs: u0 - Reset, u1 - Set, u2 - special, u3 - ?enable.<br>\n"
 "Ordinary input assumed 1 if more then <b>level1</b><br>\n"
 "Shmitt trigger resetted by signal u2 < <b>level0</b>.<br>\n"
 "If enabled (<b>useT0</b>), trigger performs autoreset after <b>t0</b>.<br>\n"
 "Have 5 switches: <b>useInit1, useEnable, usePulse, useMinus, useT0</b>.";

TClassInfo TTrigger::class_info = {
  CLASS_ID_TTrigger, "TTrigger", TTrigger::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TTrigger::ttrigger_d_i[23] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "trigg_dia", "", "Dialog for TTrigger"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   5,   20,  70, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", 
   "RS"               // 0
   "\nShmitt(u2)\n"   // 1
   "CountRise(u2)\n"  // 2
   "CountDown(u2)\n"  // 3
   "CountLevel(u2)"}, // 4 
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_level0", "", "Level of 0"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "level0", "0 - level", ""},
 { dtpLabel,      0,   0,   30, 140,  50,  20, 0,  0.0, 0.0, 0, 0, "l_level1", "", "Level of 1"},
 { dtpDou,        0,   0,   20, 160, 120,  20, 0,  -1e300, 1e300, 0, 0, "level1", "1 - level", ""},
 { dtpLabel,      0,   0,   30, 190,  50,  20, 0,  0.0, 0.0, 0, 0, "l_t0", "", "t0 - autoreset"},
 { dtpDou,        0,   0,   20, 210, 120,  20, 0,  0.0, 1e300, 0, 0, "t0", "autoreset time", ""},
 { dtpInt,   dtpsSw,   0,  180,  50, 150,  20, 2,  0.0, 0.0, 0, 0, "useInit1", "", "Set to 1 on start"},
 { dtpInt,   dtpsSw,   0,  180,  70, 150,  20, 2,  0.0, 0.0, 0, 0, "useEnable", "", "Use u3 as enable"},
 { dtpInt,   dtpsSw,   0,  180,  90, 150,  20, 2,  0.0, 0.0, 0, 0, "usePulse", "", "Pulse output"},
 { dtpInt,   dtpsSw,   0,  180, 110, 150,  20, 2,  0.0, 0.0, 0, 0, "useMinus", "", "Use -1 as neg. out"},
 { dtpInt,   dtpsSw,   0,  180, 130, 150,  20, 2,  0.0, 0.0, 0, 0, "useT0", "", "Autoreset after t0"},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TTrigger::TTrigger( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( type, "Type" ),
	PRM_INIT( cst, "Current state" ),
	PRM_INIT( level0, "Level of 0 " ),
	PRM_INIT( level1, "Level of 1" ),
	PRM_INIT( t0, "Autoreset time" ),
	PRM_INIT( et, "Elapsed time" ),
	PRM_INIT( useInit1, "Set on start" ),
	PRM_INIT( useEnable, "Use u3 as enable" ),
	PRM_INIT( usePulse, "Pulse output" ),
	PRM_INIT( useMinus, "Negative pulse" ),
	PRM_INIT( useT0, "Autoreset (t0)" )

{
  int i;
  type = 0; level0 = 0.1; level1 = 0.5; t0 = 2; et = 0; u2_old = 0;
  useInit1 = useEnable = usePulse = useMinus = useT0 = 0;
  d_i = ttrigger_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &level0; ptrs[8] = &level1;  ptrs[10] = &t0; 
  ptrs[11] = &useInit1; ptrs[12] = &useEnable; ptrs[13] = &usePulse; 
  ptrs[14] = &useMinus; ptrs[15] = &useT0; 
  // from TMiso 
  ptrs[19] = links;
  ptrs[20] = &vis_x; ptrs[21] = &vis_y;
  PRMI(type).setDescr("Type of trigger");
  PRMI(type).setElems(
   "RS\n"             // 0
   "Shmitt(u2)\n"     // 1
   "CountRise(u2)\n"  // 2
   "CountDown(u2)\n"  // 3
   "CountLevel(u2)"   // 4 
  );
  PRMI(level0).setDescr("Level of '0' signal");
  PRMI(level1).setDescr("Level of '1' signal (sor shmidt)");
  PRMI(t0).setDescr("Time of autoreset (if enabled))");
  PRMI(useInit1).setDescr("Set triggers value to 1 of start");
  PRMI(useInit1).setParm( "sep", "col" );
  PRMI(useEnable).setDescr("Use u[3] as enable signal");
  PRMI(usePulse).setDescr("Output is pulse, no level");
  PRMI(useMinus).setDescr("Drop pulse is negative, not 0");
  PRMI(useT0).setDescr("Use autoreset after t0 time elapsed");
}

TTrigger::~TTrigger()
{
}

TDataSet* TTrigger::create( TDataSet* apar )
{
  return new TTrigger( apar );
}

int TTrigger::getClassId(void) const 
{
  return CLASS_ID_TTrigger;
}

const TClassInfo* TTrigger::getClassInfo(void) const
{
  return &class_info;
}

const char *TTrigger::getHelp(void) const
{
  return helpstr;
}

const char** TTrigger::getIcon(void) const
{
  return icon;
}

int TTrigger::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  cst = useInit1; u2_old = et = 0;
  return rc;
}

double TTrigger::f( double t )
{
  int i, ui[4], sw, act;
  double du;
  if( t > tdt/2 ) {
    du = *in_so[2] - u2_old;
  } else {
    du = 0;
  };
  u2_old = *in_so[2];
  for( i=0; i<4; i++ )
    ui[i] = ( *in_so[i]>level1 );
  if( useEnable && ui[3] < 1 ) {
    if( usePulse ) return 0;
    if( cst ) return 1;
    return useMinus ? -1 : 0;
  };
  sw = 0;
  if( ui[1] ) { // SET/RESET inputs 
    sw = 1; 
  } else {
    if( ui[0] ) { 
      sw = -1; 
    };
  };
  if( sw == 0 ) {
    switch( type ) {
      case 0: break;
      case 1: if( *in_so[2] > level1 ) { sw = 1; break; };
	      if( *in_so[2] < level0 ) { sw = -1; break; };
	      break;
      case 2: if( du > 0.5 ) sw = 2; 
	      break;  
      case 3: if( du < -0.5 ) sw = 2;
	      break;
      case 4: sw = ui[2] ? 2 : 0; break;
      default: break;
    };
  };
  if( useT0 && sw == 0 && cst == 1 ) {
    if( et >= t0 ) {
      sw = -1; et = 0;
    } else {
      et += tdt;
    };
  };
  act = 0;
  switch( sw ) {
    case 0:  break;
    case 1:  if( cst == 0 ) act = 1;
	     cst = 1; et = 0; break;
    case -1: if( cst == 1 ) act = -1;
	     cst = 0; break;
    case 2:  cst = !cst;  act = cst ? 1 : -1;
	     if( cst ) et = 0; break;
    default: break;
  };
  if( usePulse ) 
    return act;
  if( cst ) return 1;
  return useMinus ? -1 : 0;
}


int TTrigger::registered = reg();

int TTrigger::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TTrigger::icon[] = {
/* width height num_colors chars_per_pixel */
"32 32 5 1",
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
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
".....bbbbbbbbbbbbbbb............",
".....b.............b............",
".....b.............b............",
".....b.............b............",
".bbbbb.............bbbbbbbbbbb..",
"................................",
"...................aa...........",
"...................aa...........",
"...................aa...........",
".aaaaaaaaaaaaaaaaaaaaaaaaaaaaa..",
"................................",
".....##.........................",
".....##.........................",
".....##.........................",
".#############################..",
"................................"
};


// end of ttrigger.cpp

