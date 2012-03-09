/***************************************************************************
                          thyst.cpp  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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
#include "thyst.h"

const char* THyst::helpstr = "<H1>THyst</H1>\n"
 "Can simulate some simple models of hysteresis: <br>\n"
 "Have 5 parameters: <b>x0, alpha, a, b, type</b>\n"
 " <b>x0, alpha </b> - parameers of hysteresys\n"
 " <b>a, b </b> - output scale\n"
 "Can be changed at any time: <b>x0, alpha</b>.";

TClassInfo THyst::class_info = {
  CLASS_ID_THyst, "THyst", THyst::create,
  &TMiso::class_info, helpstr };

TDataInfo THyst::thyst_d_i[20] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "hyst_dial", "", "Dialog for THyst"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",   ""},
 { dtpLabel,      0,   0,   30,  40,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,   6,   20,  60, 140,  20, 2,  0.0, 0.0, 0, 0, "type", "Hysteresis type", "s+alpha*d\n"
     "alpha*d\n"       // 1
     "Thetta(d)\n"     // 2 
     "sign(d)\n"       // 3 
     "tanh(alpha*d)\n"       // 4
     "tanh(s+alpha*d)" // 5
    },
 { dtpLabel,      0,   0,  190,  40,  50,  20, 0,  0.0, 0.0, 0, 0, "l_x0", "", "x0"},
 { dtpDou,        0,   0,  180,  60, 100,  20, 0,  0.0, 1e300, 0, 0, "x0", "Hyst. width", ""},
 { dtpLabel,      0,   0,  190,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_alpha", "", "alpha"},
 { dtpDou,        0,   0,  180, 110, 100,  20, 0,  -1e300, 1e300, 0, 0, "alpha", "Slope", ""},
 { dtpLabel,      0,   0,  190, 140,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a", "", "a"},
 { dtpDou,        0,   0,  180, 160, 100,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,  190, 190,  50,  20, 0,  0.0, 0.0, 0, 0, "l_b", "", "b"},
 { dtpDou,        0,   0,  180, 210, 100,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



THyst::THyst( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  type = 0; x0 = 1; alpha = 0.2; a = 1; b = 0; s = d = 0;
  d_i = thyst_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &x0; ptrs[8] = &alpha;
  ptrs[10] = &a; ptrs[12] = &b;
  // from TMiso 
  ptrs[16] = links;
  ptrs[17] = &vis_x; ptrs[18] = &vis_y;
}

THyst::~THyst()
{
}

TDataSet* THyst::create( TDataSet* apar )
{
  return new THyst( apar );
}

int THyst::getClassId(void) const 
{
  return CLASS_ID_THyst;
}

const char* THyst::getClassName(void) const 
{
  return "THyst";
}

const TClassInfo* THyst::getClassInfo(void) const
{
  return &class_info;
}

const char *THyst::getHelp(void) const
{
  return helpstr;
}

const char** THyst::getIcon(void) const
{
  return icon;
}

double THyst::f( const double* u, double /* t */ )
{
  double ud, ts , v, u_old;
  // atu test
  u_old = d + s;
  // 
  ud = u[0] - u_old; 
  ts = s + ud;
  if( ts >= x0 ) { 
    s = x0;  d += ts - x0;  
  } else { 
    if( ts <= -x0 ) { 
      s = -x0;  d += ts + x0; 
    }  else { 
      s = ts; 
    };
  };
  switch( type ) {
    case 0: v = s + alpha * d; break;
    case 1: v = alpha * d; break;   
    case 2: v = thetta( d ); break;
    case 3: v = sign( d ); break;
    case 4: v = tanh( alpha * d ); break;
    case 5: v = tanh( s + alpha * d ); break;
    default: v = 0;
  };
  v = a * v + b;
  return v;
}

void THyst::fixState(void) 
{
  if( s > x0 )  
    { d += s - x0;  s = x0;  };
  if( s < -x0 ) 
    { d += s + x0;  s = -x0; };
}

int THyst::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  s = d = 0;
  return rc;
}

int THyst::setDataID( int ni, double da, int allowConv )
{
  int k;
  k = TDataSet::setDataID( ni, da, allowConv );
  if( k == 0 && ( ni == 6 || ni == 8 ) )
    fixState();
  return k;
}

/* XPM */
const char* THyst::icon[] = {
"32 32 3 1",
"# c #c0c0c0",
"a c #ffff00",
". c #0000ff",
"................................",
"................................",
"................................",
"................................",
"................................",
".....................a..........",
"..............#.................",
"..............#....a..........a.",
"..............#.................",
"..............#..a..........a...",
"..............#.................",
"..............#a..........a.....",
".............aaaaaaaaaaaaa......",
"............aaaaaaaaaaaaa.......",
"...........aa.a.......aa........",
"..........aa..#......aa.........",
".........aa...#.....aa..........",
".#######aa####aaaaaaa#######....",
".......aa.....#...aa............",
"......aa......#..aa.............",
".....aaaaaaaaa#aaa..............",
"....aa........#aa...............",
"...aa.........aa................",
"..aaaaaaaaaaaa#.................",
".aaaaaaaaaaaa.#.................",
".a..........a.#.................",
"..............#.................",
"..........a...#.................",
"..............#.................",
"........a.....#.................",
"..............#.................",
"................................"
};


// end of thyst.cpp

 
