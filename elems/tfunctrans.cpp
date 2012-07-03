/***************************************************************************
                          tfunctrans.cpp  -  description
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
#include "tfunctrans.h"

static const char* const tfunctrans_list = 
     "a*sin(b*y)+g\n"                // 0
     "a*sign(sin(b*y)+c)+g\n"        // 1 
     "a*tanh(by)+g\n"                // 2
     "a*atan2(u0,u1(x))+g\n"         // 3
     "a*exp(b*y)+g\n"                // 4
     "a*exp(-b*y^2)+g\n"             // 5
     "a*Wave(b*y)+g\n"               // 6
     "a*Mhat(b*y)+g\n"               // 7
     "a*ln(b*y)+g\n"                 // 8
     "a*y*exp(-b*y)+g\n"             // 9
     "a*y*exp(-b*y^2)+g\n"           // 10
     "a*sin^2(b*y)+g\n"              // 11
     "a*cos(b*y)+g\n"                // 12
     "a*cos^2(b*y)+g\n"              // 13
     "a*tan(b*y)+g\n"                // 14
     "a*exp(-b*y*y)*(1-c*sin^2(dy))+g\n" // 15
     "a*(1-exp(-b*y*y))*(1-c*sin^2(dy))+g" // 16
;

const char* TFuncTrans::helpstr = "<H1>TFuncTrans</H1>\n"
 "Harmonic-alike functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";

TClassInfo TFuncTrans::class_info = {
  CLASS_ID_TFuncTrans, "TFuncTrans", TFuncTrans::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };

TDataInfo TFuncTrans::tfunctrans_d_i[26] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 480, 350, 0,  0.0, 0.0, 0, 0, "functrans_dial", "", "Dialog for TFuncTrans"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,  17,   20,  70, 290,  20, 2,  0.0, 0.0, 0, 0, "type", "func type", tfunctrans_list },
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
 { dtpButton,     1,   0,  140, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 300,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TFuncTrans::TFuncTrans( TDataSet* aparent )
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
  int i;
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 
  d_i = tfunctrans_d_i;
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

  PRMI(type).setDescr( "Functon type" );
  PRMI(type).setElems( tfunctrans_list );
  PRMI(a).setDescr(  "Coefficient a" );
  PRMI(b).setDescr(  "Coefficient b" );
  PRMI(c).setDescr(  "Coefficient c" );
  PRMI(d).setDescr(  "Coefficient d" );
  PRMI(e).setDescr(  "Coefficient e" );
  PRMI(g).setDescr(  "Coefficient g" );
  PRMI(x0).setDescr( "Input shift: y = u[0] - u[1] - x0;" );
}

TFuncTrans::~TFuncTrans()
{
}

TDataSet* TFuncTrans::create( TDataSet* apar )
{
  return new TFuncTrans( apar );
}

int TFuncTrans::getClassId(void) const 
{
  return CLASS_ID_TFuncTrans;
}

const char* TFuncTrans::getClassName(void) const 
{
  return "TFuncTrans";
}

const TClassInfo* TFuncTrans::getClassInfo(void) const
{
  return &class_info;
}

const char *TFuncTrans::getHelp(void) const
{
  return helpstr;
}

const char** TFuncTrans::getIcon(void) const
{
  return icon;
}

double TFuncTrans::f( const double* u, double /* t */ )
{
  double v, y, by, tm;
  y = u[0] - u[1] - x0; by = y * b;
  switch( type ) {
    case 0:   v = a * sin( by ); break;
    case 1:   v = a * sign( sin( by ) + c ); break;
    case 2:   v = a * tanh( by ); break;
    case 3:   v = a * atan2( u[0], u[1] ); break;
    case 4:   v = a * exp( by ); break;
    case 5:   v = a * exp( -by * y ); break;
    case 6:   v = a * waveWave( by ); break;
    case 7:   v = a * waveMhat( by ); break;
    case 8:   v = ( by > 0 ) ? ( a * log( by ) ) : 0; break;
    case 9:   v = a * y * exp( -by ); break;
    case 10:  v = a * y * exp( -by * y ); break;
    case 11:  tm = sin( by ); v = a * tm * tm; break;
    case 12:  v = a * cos( by ); break;
    case 13:  tm = cos( by ); v = a * tm * tm; break;
    case 14:  v = a * tan( by ); break;
    case 15:  tm = sin( d * y );  tm = 1 - c * tm*tm;
              v = a * exp( -by*y ) * tm; break;
    case 16:  tm = sin( d * y );  tm = 1 - c * tm*tm;
              v = a * (1-exp( -by*y )) * tm; break;
    default:  v = 0;
  };
  v += g;
  return v; 
}

/* XPM */
const char* TFuncTrans::icon[] = {
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
".........##...........#.........",
".........##..........#.#........",
".........##.........#...#.......",
"........###.........#...#.......",
"........###.........#...#.......",
"........##.....aa...#...#....aa.",
"........##....a..a.#.....#..a...",
"........##...a....#.......#a....",
".......##..cacccc#caccccccac#c..",
".......##...........a....a......",
"...##.##.............a..a.......",
"...#####..............aa........",
"....###.........................",
"................................"};

// end of tfunctrans.cpp


