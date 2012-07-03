/***************************************************************************
                          tlogic.cpp  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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

#include "tlogic.h"

static const char* const tlogic_list = 
 "AND\n"  // 0 
 "OR\n"   // 1
 "XOR\n"  // 2
 "u[0]\n" // 3
 "0\n"    // 4
 "1"      // 5
;

const char* TLogic::helpstr = "<H1>TLogic</H1>\n"
 "Simple logic element: <br>\n"
 "Parameters: <br>\n"
 " - <b>type</b> determinated type of logic: ANR, OR, XOR ... <br>\n"
 " - <b>level</b> -- level of logical 1 on input; <br>\n"
 " - <b>useNInp0 - useNInp3</b> -- inverse inputs; <br>\n"
 " - <b>useNOut</b> -- inverse output; <br>\n"
 " - <b>useMinus</b> -- negative output == -1, not 0.  <br>\n"
 "For AND unused inputs link to ':one' .";

TClassInfo TLogic::class_info = {
  CLASS_ID_TLogic, "TLogic", TLogic::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };

TDataInfo TLogic::tlogic_d_i[20] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 310, 0,  0.0, 0.0, 0, 0, "Zzz", "", "Dialog for TLogic"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  40,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   6,   20,  60, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", tlogic_list },
 { dtpLabel,      0,   0,   30,  90, 100,  20, 0,  0.0, 0.0, 0, 0, "l_level", "", "Level of 1"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "level", "level of 1", ""},
 { dtpInt,   dtpsSw,   0,   20, 140, 160,  20, 0,  0.0, 0.0, 0, 0, "useNInp0", "", "Inverse u[0]"},
 { dtpInt,   dtpsSw,   0,   20, 170, 160,  20, 0,  0.0, 0.0, 0, 0, "useNInp1", "", "Inverse u[1]"},
 { dtpInt,   dtpsSw,   0,   20, 200, 160,  20, 0,  0.0, 0.0, 0, 0, "useNInp2", "", "Inverse u[2]"},
 { dtpInt,   dtpsSw,   0,   20, 230, 160,  20, 0,  0.0, 0.0, 0, 0, "useNInp3", "", "Inverse u[3]"},
 { dtpInt,   dtpsSw,   0,  180,  60, 160,  20, 0,  0.0, 0.0, 0, 0, "useNOut", "", "Inverse output"},
 { dtpInt,   dtpsSw,   0,  180,  90, 160,  20, 0,  0.0, 0.0, 0, 0, "useMinus", "", "Use -1 as neg. out"},
 { dtpButton,     0,   0,   20, 260,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 260,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 260,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TLogic::TLogic( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( type, "Type" ),
	PRM_INIT( level, "level of 1" ),
	PRM_INIT( useNInp0, "Inverse u[0]" ),
	PRM_INIT( useNInp1, "Inverse u[1]" ),
	PRM_INIT( useNInp2, "Inverse u[2]" ),
	PRM_INIT( useNInp3, "Inverse u[3]" ),
	PRM_INIT( useNOut,  "Inverse output" ),
	PRM_INIT( useMinus, "Negative out" )
{
  int i;
  level = 0.1; type = 1; useNInp0 = useNInp1 = useNInp2 = useNInp3 = 0;
  useNOut = useMinus = 0;
  d_i = tlogic_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &level; 
  ptrs[7] = &useNInp0; ptrs[8] = &useNInp1;  
  ptrs[9] = &useNInp2;  ptrs[10] = &useNInp3;  
  ptrs[11] = &useNOut; ptrs[12] = &useMinus;
  // from TMiso 
  ptrs[16] = links;
  ptrs[17] = &vis_x; ptrs[18] = &vis_y;

  PRMI(type).setDescr( "Type of logic" );
  PRMI(type).setElems( tlogic_list );
  PRMI(level).setDescr( "Minimal level of '1' signal on input" );
  PRMI(useNInp0).setDescr( "Inverse input u[0]" );
  PRMI(useNInp1).setDescr( "Inverse input u[1]" );
  PRMI(useNInp2).setDescr( "Inverse input u[2]" );
  PRMI(useNInp3).setDescr( "Inverse input u[3]" );
  PRMI(useNOut).setDescr(  "Inverse output" );
  PRMI(useMinus).setDescr( "Negative output is -1, not 0" );
}

TLogic::~TLogic()
{
}

TDataSet* TLogic::create( TDataSet* apar )
{
  return new TLogic( apar );
}

int TLogic::getClassId(void) const 
{
  return CLASS_ID_TLogic;
}

const char* TLogic::getClassName(void) const 
{
  return "TLogic";
}

const TClassInfo* TLogic::getClassInfo(void) const
{
  return &class_info;
}

const char *TLogic::getHelp(void) const
{
  return helpstr;
}

const char** TLogic::getIcon(void) const
{
  return icon;
}

double TLogic::f( const double* u, double /* t */ )
{
  int i, iu[4], ou;
  for( i=0; i<4; i++ ) {
    iu[i] = ( u[i] > level );
  };
  if( useNInp0 ) iu[0] = ! iu[0];
  if( useNInp1 ) iu[1] = ! iu[1];
  if( useNInp2 ) iu[2] = ! iu[2];
  if( useNInp3 ) iu[3] = ! iu[3];
  ou = 0;
  switch( type ) {
    case 0: if( iu[0] > 0 && iu[1] > 0 && iu[2] > 0 && iu[3] > 0 ) 
	      ou = 1;
	    break;
    case 1: if( iu[0] > 0 || iu[1] > 0 || iu[2] > 0 || iu[3] > 0 ) 
	      ou = 1;
	    break;
    case 2: for( i=0; i < 4; i++ )
	      if( iu[i] > 0 )
		ou = !ou;
	    break;
    case 3: ou = iu[0]; break;
    case 4: ou = 0; break;
    case 5: ou = 1; break;
    default: ;
  };
  if( useNOut ) ou = !ou;
  if( ou ) return 1;
  return useMinus ? -1 : 0;
}

int TLogic::registered = reg();

int TLogic::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TLogic::icon[] = {
/* width height num_colors chars_per_pixel */
"32 32 8 1",
"c c #808080",
"b c #000000",
"a c #0000ff",
"g c #00ff00",
"m c #ff00ff",
". c #dcdcdc",
"# c #ff0000",
"y c #ffff00",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"......bbbbbbbbbbbbbbbbbbbbb.....",
"......bbbbbbbbbbbbbbbbbbbbb.....",
"......bb.................bb.....",
"......bb.........aa......bb.....",
"......bb.........aa......bb.....",
".bbbbbbb.........aa......bb.....",
"......bb.........aa......bb.....",
"......bb.........aa......bb.....",
"......bb.........aa....a.bb.....",
".bbbbbbb.........aaaaaaa.bb.....",
"......bb.........aaaaaaa.bbbbbb.",
"......bb.................bbbbbb.",
"......bb.................bb.....",
".bbbbbbb.................bb.....",
"......bb.................bb.....",
"......bb.................bb.....",
"......bb.................bb.....",
".bbbbbbb.................bb.....",
"......bb.................bb.....",
"......bb.................bb.....",
"......bbbbbbbbbbbbbbbbbbbbb.....",
"......bbbbbbbbbbbbbbbbbbbbb.....",
"................................"
};


// end of tlogic.cpp



