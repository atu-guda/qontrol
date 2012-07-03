/***************************************************************************
                          tadjgen.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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
#include "tadjgen.h"

const char* TAdjGen::helpstr = "<H1>TAdjGen</H1>\n"
 "Generator of 0/1, controlled on input frequency u[0]: <br>\n"
 "Have 9 parameters: <b>type, useReset, useLock, outStrobe</b>,<br>\n"
 " <b> useSignStrobe, usePlusStrobe, useF, omega_0, k_omega </b>,<br>\n"
 "None of the can be changed at run time.";

TClassInfo TAdjGen::class_info = {
  CLASS_ID_TAdjGen, "TAdjGen", TAdjGen::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TAdjGen::tadjgen_d_i[27] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 360, 0,  0.0, 0.0, 0, 0, "adjgen_dial", "", "Dialog for TAdjGen"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   3,   20,  70, 120,  20, 2,  0.0, 0.0, 0, 0, "type", "type", "Default\nIllich\nDual(u0,u3)"},
 { dtpInt,   dtpsSw,   0,  150,  70, 200,  20, 2,  0.0, 0.0, 0, 0, "useReset", "", "Use u[1] as Reset"},
 { dtpInt,   dtpsSw,   0,  150, 100, 200,  20, 2,  0.0, 0.0, 0, 0, "useLock", "", "Use u[2] as Lock"},
 { dtpInt,   dtpsSw,   0,  150, 130, 200,  20, 2,  0.0, 0.0, 0, 0, "outStrobe", "", "Output Strobe"},
 { dtpInt,   dtpsSw,   0,  150, 160, 200,  20, 2,  0.0, 0.0, 0, 0, "useSignStrobe", "", "Signed strobe"},
 { dtpInt,   dtpsSw,   0,  150, 190, 200,  20, 2,  0.0, 0.0, 0, 0, "usePlusStrobe", "", "Only \x22+\x22 strobe"},
 { dtpInt,   dtpsSw,   0,  150, 220, 200,  20, 2,  0.0, 0.0, 0, 0, "useZero", "", "use 0 as negative out"},
 { dtpInt,   dtpsSw,   0,  150, 250, 200,  20, 2,  0.0, 0.0, 0, 0, "useF", "", "input is F, not omega"},
 { dtpLabel,      0,   0,   30, 100, 100,  20, 0,  0.0, 0.0, 0, 0, "l_omega_0",   "", "\\omega_0"},
 { dtpDou,        0,   0,   20, 120, 110,  20, 0,  0.0, 1e300, 0, 0, "omega_0", "omega_0", ""},
 { dtpLabel,      0,   0,   30, 150, 100,  20, 0,  0.0, 0.0, 0, 0, "k_omega",   "", "k_\\omega"},
 { dtpDou,        0,   0,   20, 170, 110,  20, 0,  0.0, 1e300, 0, 0, "k_omega", "k_omega", ""},
 { dtpButton,     0,   0,   20, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpInt,        0,   0,    0,   0,   0,   0,52,  0.0, 0.0, 0, 0, "tick", "", "1 means generator ticks now"},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ig", "ig", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ig2", "ig2", ""},
 { dtpDou,        0,   0,    0,   0,   0,   0,52,  -1e300, 1e300, 0, 0, "ctt", "ctt", ""},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TAdjGen::TAdjGen( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( type, "Type" ),
	PRM_INIT( useReset,  "Use u[1] as Reset" ),
	PRM_INIT( useLock, "Use u[2] as Lock" ),
	PRM_INIT( outStrobe, "Output Strobe" ),
	PRM_INIT( useZero, "use 0 as negative out" ),
	PRM_INIT( useSignStrobe, "Signed strobe" ),
	PRM_INIT( usePlusStrobe, "Only + strobe" ),
	PRM_INIT( useF, "input is F, not omega" ),
	PRM_INIT( currOut, "current output" ),
	PRM_INIT( tick, "1 means generator ticks now" ),
	PRM_INIT( omega_0, "\\omega_0" ),
	PRM_INIT( k_omega, "k_\\omega" ),
	PRM_INIT( ctt, "ctt" ),
	PRM_INIT( ig, "ig" ),
	PRM_INIT( ig2, "ig2" )
{
  int i;
  type = useReset = useLock = outStrobe = useZero 
       = useSignStrobe = usePlusStrobe = useF = 0; 
  omega_0 = 1.2; k_omega = 1.0;
  ctt = ig = ig2 = 0; currOut = 0; tick = 0;
  d_i = tadjgen_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; ptrs[5] = &useReset; ptrs[6] = &useLock;
  ptrs[7] = &outStrobe; ptrs[8] = &useSignStrobe; ptrs[9] = &usePlusStrobe; 
  ptrs[10] = &useZero;
  ptrs[11] = &useF;
  ptrs[13] = &omega_0;
  ptrs[15] = &k_omega;

  ptrs[19] = &tick;
  ptrs[20] = &ig; ptrs[21] = &ig2; ptrs[22] = &ctt;
  // from TMiso 
  ptrs[23] = links;
  ptrs[24] = &vis_x; ptrs[25] = &vis_y;

  PRMI(type).setDescr( "Type of generator" );
  PRMI(type).setElems( "Default\nMAI\nDual(u0,u3)" );
  PRMI(useF).setParm( "sep", "col" );

}

TAdjGen::~TAdjGen()
{
}

TDataSet* TAdjGen::create( TDataSet* apar )
{
  return new TAdjGen( apar );
}

int TAdjGen::getClassId(void) const 
{
  return CLASS_ID_TAdjGen;
}


const TClassInfo* TAdjGen::getClassInfo(void) const
{
  return &class_info;
}

const char *TAdjGen::getHelp(void) const
{
  return helpstr;
}

const char** TAdjGen::getIcon(void) const
{
  return icon;
}

int TAdjGen::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  ctt = ig = ig2 = 0; currOut = 0; tick = 0;
  return rc;
}

double TAdjGen::f( const double* u, double /* t */ )
{
  double om, om2, df, ff, v, diff_out = 0;
  int g1, g2;
  om = u[0]; om2 = u[3];
  if( useF ) {
    om  = omega_0 * ( 1 + om*k_omega );
    om2 = omega_0 * ( 1 + om2*k_omega );
  };
  tick = 0;
  switch( type ) {
    case 0: ig += om * tdt;                 // Pi=int_0^T(om*dt);
	    if( useReset && u[1] > 0.01 ) {
	      tick = 1; break;
	    };
	    if( useLock && u[2] > 0.01 ) break;
            if( ig > M_PI ) {
	      tick = 1;
            };
            break;
    case 1: if( om < 1.0e-100 ) om = 1.0e-50; 
            ctt += tdt; ff = 1.0 / om;  df = tdt * ff;
            ig += df * M_PI;
	    if( useReset && u[1] > 0.01 ) {
	      tick = 1; break;
	    }; 
	    if( useLock && u[2] > 0.01 ) break;
            if( ctt*ctt > ig ) {
	      tick = 1;
            };
            break;
    case 2: ig += om * tdt; ig2 += om2 * tdt;      // dual
	    g1 = ( ig  > M_PI );
	    g2 = ( ig2 > M_PI );
	    if( useReset && u[1] > 0.01 ) {
	      tick = 1; break;
	    };
	    if( useLock && u[2] > 0.01 ) break;
            if( g1 && g2 ) {
	      tick = 1;
            };
	    diff_out = g1 - g2;
            break;
    default: v = 0;
  };
  if( tick ) {
    ctt = ig = ig2 = 0;
    currOut = !currOut;
  };
  if( type == 2 ) { // dual
    return diff_out;
  } 
  v = currOut ? 1.0 : ( useZero ? 0.0 : -1.0 );
  if( !outStrobe )
    return v;
  if( !tick ) 
    return 0;
  if( usePlusStrobe && v < 1 )
    return 0;
  return ( useSignStrobe ) ? v : 1;
}

int TAdjGen::registered = reg();

int TAdjGen::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TAdjGen::icon[] = {
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
"e c #c0c0c0",
"f c #808080",
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jiiiiiijjjjjjiiiiiijjiiiiiiijjjj",
"jiiiiiijjjjjjiiiiiijjiiiiiiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jeeeeiieeeeeeiieeiieeiieeeiieeej",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiijjjjjjiijjiijjiijjjiijjjj",
"jjjjjiiiiiiiiiijjiiiiiijjjiiiijj",
"jjjjjiiiiiiiiiijjiiiiiijjjiiiijj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};
  
// end of tadjgen.cpp


