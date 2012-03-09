/***************************************************************************
                          tfuncmisc.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000 by atu
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

TClassInfo TFuncMisc::class_info = {
  CLASS_ID_TFuncMisc, "TFuncMisc", TFuncMisc::create,
  &TMiso::class_info, helpstr };

TDataInfo TFuncMisc::tfuncmisc_d_i[26] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 420, 350, 0,  0.0, 0.0, 0, 0, "funcmisc_dial", "", "Dialog for TFuncMisc"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,  17,   20,  70, 220,  20, 2,  0.0, 0.0, 0, 0, "type", "func type", 
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
 },
 { dtpLabel,      0,   0,  270,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a",   "", "a"},
 { dtpDou,        0,   0,  260,  70, 120,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,  270,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_b",   "", "b"},
 { dtpDou,        0,   0,  260, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpLabel,      0,   0,  270, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "l_c",   "", "c"},
 { dtpDou,        0,   0,  260, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "c", "c", ""},
 { dtpLabel,      0,   0,  270, 170,  50,  20, 0,  0.0, 0.0, 0, 0, "l_d",   "", "d"},
 { dtpDou,        0,   0,  260, 190, 120,  20, 0,  -1e300, 1e300, 0, 0, "d", "d", ""},
 { dtpLabel,      0,   0,  270, 210,  50,  20, 0,  0.0, 0.0, 0, 0, "l_e",   "", "e"},
 { dtpDou,        0,   0,  260, 230, 120,  20, 0,  -1e300, 1e300, 0, 0, "e", "e", ""},
 { dtpLabel,      0,   0,  270, 250,  50,  20, 0,  0.0, 0.0, 0, 0, "l_g",   "", "g"},
 { dtpDou,        0,   0,  260, 270, 120,  20, 0,  -1e300, 1e300, 0, 0, "g", "g", ""},
 { dtpLabel,      0,   0,   30,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_x0",   "", "x0"},
 { dtpDou,        0,   0,   20, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "x0", "x0", ""},
 { dtpButton,     0,   0,   20, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TFuncMisc::TFuncMisc( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 
  d_i = tfuncmisc_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; 
  ptrs[6] = &a; ptrs[8] = &b; ptrs[10] = &c;
  ptrs[12] = &d; ptrs[14] = &e; ptrs[16] = &g; ptrs[18] = &x0;
  // from TMiso 
  ptrs[22] = links;
  ptrs[23] = &vis_x; ptrs[24] = &vis_y;
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

const char* TFuncMisc::getClassName(void) const 
{
  return "TFuncMisc";
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

double TFuncMisc::f( const double* u, double /* t */ )
{
  double v, y;
  y = u[0] - u[1] - x0; 
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
    case 10: v = a * u[0] / ( u[1] +  b );  break;
    case 11: v = a * int( y * b );  break;
    case 12: v = a * ( ( y * b ) - int( y * b ) );  break;
    case 13: v = a * asin( fabs( sin( y * b) ) );  break;
    case 14: v = a * erf( y*b );  break;
    case 15: v = a * fmin( u[0], u[1] );  break;
    case 16: v = a * fmax( u[0], u[1] );  break;
    default: v = 0;
  };
  v += g;
  return v; 
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


