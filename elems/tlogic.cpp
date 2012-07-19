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
  level = 0.1; type = 1; useNInp0 = useNInp1 = useNInp2 = useNInp3 = 0;
  useNOut = useMinus = 0;

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

double TLogic::f( double /* t */ )
{
  int i, iu[4], ou;
  for( i=0; i<4; i++ ) {
    iu[i] = ( *in_so[i] > level );
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



