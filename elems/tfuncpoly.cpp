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

const char* TFuncPoly::helpstr = "<H1>TFuncPoly</H1>\n"
 "Polinomial functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

TClassInfo TFuncPoly::class_info = {
  CLASS_ID_TFuncPoly, "TFuncPoly", TFuncPoly::create,
  &TMiso::class_info, helpstr };

TDataInfo TFuncPoly::tfuncpoly_d_i[26] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 500, 350, 0,  0.0, 0.0, 0, 0, "funcpoly_dial", "", "Dialog for TFuncPoly"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,  12,   20,  70, 330,  20, 2,  0.0, 0.0, 0, 0, "type", "func type", 
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
 },
 { dtpLabel,      0,   0,  360,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a",   "", "a"},
 { dtpDou,        0,   0,  350,  70, 120,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,  360,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_b",   "", "b"},
 { dtpDou,        0,   0,  350, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpLabel,      0,   0,  360, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "l_c",   "", "c"},
 { dtpDou,        0,   0,  350, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "c", "c", ""},
 { dtpLabel,      0,   0,  360, 170,  50,  20, 0,  0.0, 0.0, 0, 0, "l_d",   "", "d"},
 { dtpDou,        0,   0,  350, 190, 120,  20, 0,  -1e300, 1e300, 0, 0, "d", "d", ""},
 { dtpLabel,      0,   0,  360, 210,  50,  20, 0,  0.0, 0.0, 0, 0, "l_e",   "", "e"},
 { dtpDou,        0,   0,  350, 230, 120,  20, 0,  -1e300, 1e300, 0, 0, "e", "e", ""},
 { dtpLabel,      0,   0,  360, 250,  50,  20, 0,  0.0, 0.0, 0, 0, "l_g",   "", "g"},
 { dtpDou,        0,   0,  350, 270, 120,  20, 0,  -1e300, 1e300, 0, 0, "g", "g", ""},
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_x0",   "", "x0"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "x0", "x0", ""},
 { dtpButton,     0,   0,   20, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  190, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  350, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TFuncPoly::TFuncPoly( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 
  d_i = tfuncpoly_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; 
  ptrs[6] = &a; ptrs[8] = &b; ptrs[10] = &c;
  ptrs[12] = &d; ptrs[14] = &e; ptrs[16] = &g; ptrs[18] = &x0;
  // from TMiso 
  ptrs[22] = links;
  ptrs[23] = &vis_x; ptrs[24] = &vis_y;
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

const char* TFuncPoly::getClassName(void) const 
{
  return "TFuncPoly";
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

double TFuncPoly::f( const double* u, double /* t */ )
{
  double v, y, y2, t1, t2;
  y = u[0] - u[1] - x0; y2 = y*y;
  switch( type ) {
    case 0: v = a * y; break;
    case 1: v = a * y2 + b * y; break;
    case 2: v = a * y2 * y + b * y2 + c * y; break;
    case 3: v = a * u[0]*u[0] + b * u[0]*u[1] + c * u[1]*u[1]; break;
    case 4: v = ( y > 0 ) ? a * sqrt( y ) : 0; break;
    case 5: v = hypot( a*u[0], b*u[1] ); break;
    case 6: v = a*u[0]*u[0] + b*u[1]*u[1] + c*u[2]*u[2] + d*u[3]*u[3]; break;
    case 7: t1 = b*b - y2; t1 *= t1; 
	    v = 1 / sqrt( t1 + a * a *y2 ); break;
    case 8: t1 = ( 1- u[0] ); t1 *= t1; t2 = ( u[1] - u[0]*u[0] ); t2 *= t2;
	    v = a * t2 + b * t1; break;
    case 9: t1 = u[0]*u[0] + u[1]*u[1] - 1; t1 *= t1;
	    v = 1 - exp( -a * ( t1 - b*u[0] -c*u[1]) ); break;
    case 10: v = a * ( 1 + b*y ) ; break;
    case 11: v = a * ( b * y + c * fabs(y) ) ; break;
    default: v = 0;
  };
  v += g;
  return v; 
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


