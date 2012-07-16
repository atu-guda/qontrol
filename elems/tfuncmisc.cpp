/***************************************************************************
                          tfuncmisc.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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
#include "miscfun.h"
#include "tfuncmisc.h"

const char* TFuncMisc::helpstr = "<H1>TFuncMisc</H1>\n"
 "Miscelenios nonlinear functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

static const char* const tfuncmisc_list = 
     "a*deadLine(y,b)+g\n"           // 0
     "a*limitLine(y,b)+g\n"          // 1 
     "a*deadLimitLine(y,b,c,d)+g\n"  // 2
     "a*sign(y)+g\n"                 // 3
     "a*Thetta(y)+g\n"               // 4
     "a*diod(y)+g\n"                 // 5
     "a*threeState(y,b)+g\n"         // 6
     "a*triangle(y,b)+g\n"           // 7
     "a*rectLine(y,b,c)+g\n"         // 8
     "((y>0)?a:b)*y+g\n"             // 9
     "a*u0/(u1+b)+g\n"  	     // 10
     "a*int(y*b)+g\n" 		     // 11
     "a*frac(y*b)+g\n" 		     // 12
     "a*asin(abs(sin(y*b)))+g\n"     // 13
     "a*erf(y*b)+g\n"                // 14
     "a*min(u0,u1)+g\n"              // 15
     "a*max(u0,u1)+g"                // 16
;

TClassInfo TFuncMisc::class_info = {
  CLASS_ID_TFuncMisc, "TFuncMisc", TFuncMisc::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TFuncMisc::TFuncMisc( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( type, "Type" ),
	PRM_INIT( a,  "a" ),
	PRM_INIT( b,  "b" ),
	PRM_INIT( c,  "c" ),
	PRM_INIT( d,  "d" ),
	PRM_INIT( e,  "e" ),
	PRM_INIT( g,  "g" ),
	PRM_INIT( x0, "x0" )
{
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 

  PRMI(type).setDescr( "Functon type" );
  PRMI(type).setElems( tfuncmisc_list );
  PRMI(a).setDescr(  "Coefficient a" );
  PRMI(b).setDescr(  "Coefficient b" );
  PRMI(c).setDescr(  "Coefficient c" );
  PRMI(d).setDescr(  "Coefficient d" );
  PRMI(e).setDescr(  "Coefficient e" );
  PRMI(g).setDescr(  "Coefficient g" );
  PRMI(x0).setDescr( "Input shift: y = u[0] - u[1] - x0;" );
}

TFuncMisc::~TFuncMisc()
{
}

TDataSet* TFuncMisc::create( TDataSet* apar )
{
  return new TFuncMisc( apar );
}

int TFuncMisc::getClassId(void) const 
{
  return CLASS_ID_TFuncMisc;
}


const TClassInfo* TFuncMisc::getClassInfo(void) const
{
  return &class_info;
}

const char *TFuncMisc::getHelp(void) const
{
  return helpstr;
}

const char** TFuncMisc::getIcon(void) const
{
  return icon;
}

double TFuncMisc::f( double /* t */ )
{
  double v, y;
  y = *in_so[0] - *in_so[1] - x0; 
  switch( type ) {
    case 0:  v = a * deadLine( y, b ); break;
    case 1:  v = a * limitLine( y, b ); break;
    case 2:  v = a * deadLimitLine( y, b, c, d ); break;
    case 3:  v = a * sign( y ); break;
    case 4:  v = a * thetta( y ); break;
    case 5:  v = ( y > 0 ) ? y : 0; v *= a; break;
    case 6:  v = a * threeStateLine( y, b ); break;
    case 7:  v = a * triangleLine( y , b ); break;
    case 8:  v = a * rectLine( y, b, c ); break;
    case 9:  v = ( y >= 0 ) ? (a * y) : ( b * y ); break;
    case 10: v = a * *in_so[0] / ( *in_so[1] +  b );  break;
    case 11: v = a * int( y * b );  break;
    case 12: v = a * ( ( y * b ) - int( y * b ) );  break;
    case 13: v = a * asin( fabs( sin( y * b) ) );  break;
    case 14: v = a * erf( y*b );  break;
    case 15: v = a * fmin( *in_so[0], *in_so[1] );  break;
    case 16: v = a * fmax( *in_so[0], *in_so[1] );  break;
    default: v = 0;
  };
  v += g;
  return v; 
}

int TFuncMisc::registered = reg();

int TFuncMisc::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TFuncMisc::icon[] = {
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
"..............###...............",
"............##.##...............",
"............#..##...............",
"...........##...................",
"...........##...................",
"..........###...................",
"..........##....................",
".......#########................",
"..........##....................",
".........###....................",
".........###....................",
".........##.....................",
".........##....a................",
".........##....a.......bbbbbbb..",
"........###...a.a......b........",
"........###...a.a......b........",
"........##...a...a.....b........",
"........##..cacccabbbbbbccccccc.",
"........##........b.............",
".......##.........b.............",
".......##.........b.............",
"...##.##.....bbbbbb.............",
"...#####........................",
"....###.........................",
"................................"
};
  
// end of tfuncmisc.cpp


