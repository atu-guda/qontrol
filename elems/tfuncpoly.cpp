/***************************************************************************
                          tfuncpoly.cpp  -  description
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
#include "tfuncpoly.h"

static const char* const tfuncpoly_list = 
     "a*y+g\n"                                 // 0
     "a*y^2+b*y+g\n"                           // 1 
     "a*y^3+b*y^2+c*y+g\n"                     // 2
     "a*u0^2+b*u0*u1+c*u1^2+g\n"               // 3
     "a*sqrt(y)+g\n"                           // 4
     "hypot(a*u0,b*u1)+g\n"                    // 5
     "a*u0^2+..+d*u3^2+g\n"                    // 6
     "Vibro Ampl(omega); A=c0 B=Ome\n"         // 7
     "a*(u1-u0^2)^2+b*(1-u0)^2\n"              // 8
     "1-exp(-a*((u0^2+u1^2-1)^2-b*u0-c*u1))\n" // 9
     "a*(1+b*y)+g\n"                           // 10
     "a*(b*y+c*abs(y))+g"                      // 11
;

const char* TFuncPoly::helpstr = "<H1>TFuncPoly</H1>\n"
 "Polinomial functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

TClassInfo TFuncPoly::class_info = {
  CLASS_ID_TFuncPoly, "TFuncPoly", TFuncPoly::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TFuncPoly::TFuncPoly( TDataSet* aparent )
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
  PRMI(type).setElems( tfuncpoly_list );
  PRMI(a).setDescr(  "Coefficient a" );
  PRMI(b).setDescr(  "Coefficient b" );
  PRMI(c).setDescr(  "Coefficient c" );
  PRMI(d).setDescr(  "Coefficient d" );
  PRMI(e).setDescr(  "Coefficient e" );
  PRMI(g).setDescr(  "Coefficient g" );
  PRMI(x0).setDescr( "Input shift: y = u[0] - u[1] - x0;" );
}

TFuncPoly::~TFuncPoly()
{
}

TDataSet* TFuncPoly::create( TDataSet* apar )
{
  return new TFuncPoly( apar );
}

int TFuncPoly::getClassId(void) const 
{
  return CLASS_ID_TFuncPoly;
}


const TClassInfo* TFuncPoly::getClassInfo(void) const
{
  return &class_info;
}

const char *TFuncPoly::getHelp(void) const
{
  return helpstr;
}

const char** TFuncPoly::getIcon(void) const
{
  return icon;
}

double TFuncPoly::f( double /* t */ )
{
  double v, y, y2, t1, t2;
  y = *in_so[0] - *in_so[1] - x0; y2 = y*y;
  switch( type ) {
    case 0: v = a * y; break;
    case 1: v = a * y2 + b * y; break;
    case 2: v = a * y2 * y + b * y2 + c * y; break;
    case 3: v = a * *in_so[0]* *in_so[0] + b * *in_so[0]* *in_so[1] 
	      + c * *in_so[1]* *in_so[1]; break;
    case 4: v = ( y > 0 ) ? a * sqrt( y ) : 0; break;
    case 5: v = hypot( a* *in_so[0], b* *in_so[1] ); break;
    case 6: v = a* *in_so[0]* *in_so[0] + b* *in_so[1]* *in_so[1] 
	      + c* *in_so[2]* *in_so[2] + d* *in_so[3]* *in_so[3]; break;
    case 7: t1 = b*b - y2; t1 *= t1; 
	    v = 1 / sqrt( t1 + a * a *y2 ); break;
    case 8: t1 = ( 1- *in_so[0] ); t1 *= t1; t2 = ( *in_so[1] - *in_so[0]* *in_so[0] ); t2 *= t2;
	    v = a * t2 + b * t1; break;
    case 9: t1 = *in_so[0]* *in_so[0] + *in_so[1]* *in_so[1] - 1; t1 *= t1;
	    v = 1 - exp( -a * ( t1 - b* *in_so[0] -c* *in_so[1]) ); break;
    case 10: v = a * ( 1 + b*y ) ; break;
    case 11: v = a * ( b * y + c * fabs(y) ) ; break;
    default: v = 0;
  };
  v += g;
  return v; 
}

int TFuncPoly::registered = reg();

int TFuncPoly::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TFuncPoly::icon[] = {
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
".........##............bbb......",
".........##...........b...b.....",
"........###..............b......",
"........###...bb...bb...b.......",
"........##......b.b....b...b....",
"........##......b.b...bbbbb.....",
"........##.......b..............",
".......##........b..............",
".......##........b..............",
"...##.##........b.b.............",
"...#####........b.b.............",
"....###.......bb...bb...........",
"................................"
};

// end of tfuncpoly.cpp


