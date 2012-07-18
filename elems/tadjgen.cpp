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


TAdjGen::TAdjGen( TDataSet* aparent )
        :TMiso( aparent )
{
  type = useReset = useLock = outStrobe = useZero 
       = useSignStrobe = usePlusStrobe = useF = 0; 
  omega_0 = 1.2; k_omega = 1.0;
  ctt = ig = ig2 = 0; currOut = 0; real_tick = 0;
  tick = 0;
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
  ctt = ig = ig2 = 0; currOut = 0; real_tick = 0; tick = 0;
  return rc;
}

double TAdjGen::f( double /* t */ )
{
  double om, om2, df, ff, v, diff_out = 0;
  int g1, g2;
  om = *in_so[0]; om2 = *in_so[3];
  if( useF ) {
    om  = omega_0 * ( 1 + om*k_omega );
    om2 = omega_0 * ( 1 + om2*k_omega );
  };
  real_tick = 0; tick = 0;
  switch( type ) {
    case 0: ig += om * tdt;                 // Pi=int_0^T(om*dt);
	    if( useReset && *in_so[1] > 0.01 ) {
	      real_tick = 1; break;
	    };
	    if( useLock && *in_so[2] > 0.01 ) break;
            if( ig > M_PI ) {
	      real_tick = 1;
            };
            break;
    case 1: if( om < 1.0e-100 ) om = 1.0e-50; 
            ctt += tdt; ff = 1.0 / om;  df = tdt * ff;
            ig += df * M_PI;
	    if( useReset && *in_so[1] > 0.01 ) {
	      real_tick = 1; break;
	    }; 
	    if( useLock && *in_so[2] > 0.01 ) break;
            if( ctt*ctt > ig ) {
	      real_tick = 1;
            };
            break;
    case 2: ig += om * tdt; ig2 += om2 * tdt;      // dual
	    g1 = ( ig  > M_PI );
	    g2 = ( ig2 > M_PI );
	    if( useReset && *in_so[1] > 0.01 ) {
	      real_tick = 1; break;
	    };
	    if( useLock && *in_so[2] > 0.01 ) break;
            if( g1 && g2 ) {
	      real_tick = 1;
            };
	    diff_out = g1 - g2;
            break;
    default: v = 0;
  };
  tick = real_tick;
  if( real_tick ) {
    ctt = ig = ig2 = 0;
    currOut = !currOut;
  };
  if( type == 2 ) { // dual
    return diff_out;
  } 
  v = currOut ? 1.0 : ( useZero ? 0.0 : -1.0 );
  if( !outStrobe )
    return v;
  if( !real_tick ) 
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


