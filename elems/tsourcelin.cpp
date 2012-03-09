/***************************************************************************
                          tsourcelin.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by atu
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
#include "tsourcelin.h"

const char* TSourceLin::helpstr = "<H1>TSourceLin</H1>\n"
 "Source of signal, which consist of some linear segments. \n"
 "Each segment have time: <B>t_inr[i]</B>, \n"
 "start value: <B>vs[i]</B>, \n"
 "end value: <B>ve[i]</B>, <BR>\n"
 "List terminated, in t_int &lt=; 0.\n"
;

TClassInfo TSourceLin::class_info = {
  CLASS_ID_TSourceLin, "TSourceLin", TSourceLin::create,
  &TMiso::class_info, helpstr };

TDataInfo TSourceLin::tsourcelin_d_i[61] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial, 0, 0, 0, 0, 410, 450, 0, 0.0, 0.0, 0, 0, "dia", "", "" },
 { dtpInt, 0, 0, 20, 10, 60, 20, 8, 0.0, 0.0, 0, 0, "ord", "Order", "" }, 
 { dtpStr, 0, 60, 100, 10, 380, 20, 2, 0.0, 0.0, 0, 0, "descr", "Description", "" },
 { dtpLabel, 0, 0,  20, 40, 50, 20, 0, 0.0, 0.0, 0, 0, "l_T", "", "T" }, 
 { dtpLabel, 0, 0, 160, 40, 50, 20, 0, 0.0, 0.0, 0, 0, "l_vs", "", "Start" }, 
 { dtpLabel, 0, 0, 300, 40, 50, 20, 0, 0.0, 0.0, 0, 0, "l_ve", "", "End" }, 
 // t_int[0:15]: 6-21
 { dtpDbl, 0, 0,  20,  60, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_0", "t_int[0]", "" }, 
 { dtpDbl, 0, 0,  20,  80, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_1", "t_int[1]", "" }, 
 { dtpDbl, 0, 0,  20, 100, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_2", "t_int[2]", "" }, 
 { dtpDbl, 0, 0,  20, 120, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_3", "t_int[3]", "" }, 
 { dtpDbl, 0, 0,  20, 140, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_4", "t_int[4]", "" }, 
 { dtpDbl, 0, 0,  20, 160, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_5", "t_int[5]", "" }, 
 { dtpDbl, 0, 0,  20, 180, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_6", "t_int[6]", "" }, 
 { dtpDbl, 0, 0,  20, 200, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_7", "t_int[7]", "" }, 
 { dtpDbl, 0, 0,  20, 220, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_8", "t_int[8]", "" }, 
 { dtpDbl, 0, 0,  20, 240, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_9", "t_int[9]", "" }, 
 { dtpDbl, 0, 0,  20, 260, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_10", "t_int[10]", "" }, 
 { dtpDbl, 0, 0,  20, 280, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_11", "t_int[11]", "" }, 
 { dtpDbl, 0, 0,  20, 300, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_12", "t_int[12]", "" }, 
 { dtpDbl, 0, 0,  20, 320, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_13", "t_int[13]", "" }, 
 { dtpDbl, 0, 0,  20, 340, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_14", "t_int[14]", "" }, 
 { dtpDbl, 0, 0,  20, 360, 100, 20, 0, 0.0, 1.0e20, 0, 0, "t_int_15", "t_int[16]", "" }, 
 // vs[0:15]: 22-37
 { dtpDbl, 0, 0, 160,  60, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_0", "vs_0", "" }, 
 { dtpDbl, 0, 0, 160,  80, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_1", "vs_1", "" }, 
 { dtpDbl, 0, 0, 160, 100, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_2", "vs_2", "" }, 
 { dtpDbl, 0, 0, 160, 120, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_3", "vs_3", "" }, 
 { dtpDbl, 0, 0, 160, 140, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_4", "vs_4", "" }, 
 { dtpDbl, 0, 0, 160, 160, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_5", "vs_5", "" }, 
 { dtpDbl, 0, 0, 160, 180, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_6", "vs_6", "" }, 
 { dtpDbl, 0, 0, 160, 200, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_7", "vs_7", "" }, 
 { dtpDbl, 0, 0, 160, 220, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_8", "vs_8", "" }, 
 { dtpDbl, 0, 0, 160, 240, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_9", "vs_9", "" }, 
 { dtpDbl, 0, 0, 160, 260, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_10", "vs_10", "" }, 
 { dtpDbl, 0, 0, 160, 280, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_11", "vs_11", "" }, 
 { dtpDbl, 0, 0, 160, 300, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_12", "vs_12", "" }, 
 { dtpDbl, 0, 0, 160, 320, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_13", "vs_13", "" }, 
 { dtpDbl, 0, 0, 160, 340, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_14", "vs_14", "" }, 
 { dtpDbl, 0, 0, 160, 360, 100, 20, 0, 0.0, -1.0, 0, 0, "vs_15", "vs_15", "" }, 
 // ve[0:15]: 28-53
 { dtpDbl, 0, 0, 300,  60, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_0", "ve_0", "" }, 
 { dtpDbl, 0, 0, 300,  80, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_1", "ve_1", "" }, 
 { dtpDbl, 0, 0, 300, 100, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_2", "ve_2", "" }, 
 { dtpDbl, 0, 0, 300, 120, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_3", "ve_3", "" }, 
 { dtpDbl, 0, 0, 300, 140, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_4", "ve_4", "" }, 
 { dtpDbl, 0, 0, 300, 160, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_5", "ve_5", "" }, 
 { dtpDbl, 0, 0, 300, 180, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_6", "ve_6", "" }, 
 { dtpDbl, 0, 0, 300, 200, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_7", "ve_7", "" }, 
 { dtpDbl, 0, 0, 300, 220, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_8", "ve_8", "" }, 
 { dtpDbl, 0, 0, 300, 240, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_9", "ve_9", "" }, 
 { dtpDbl, 0, 0, 300, 260, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_10", "ve_10", "" }, 
 { dtpDbl, 0, 0, 300, 280, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_11", "ve_11", "" }, 
 { dtpDbl, 0, 0, 300, 300, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_12", "ve_12", "" }, 
 { dtpDbl, 0, 0, 300, 320, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_13", "ve_13", "" }, 
 { dtpDbl, 0, 0, 300, 340, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_14", "ve_14", "" }, 
 { dtpDbl, 0, 0, 300, 360, 100, 20, 0, 0.0, -1.0, 0, 0, "ve_15", "ve_15", "" }, 
 //54
 { dtpButton,     0,   0,   20, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  160, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  300, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TSourceLin::TSourceLin( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  for( i=0; i<16; i++ ) t_int[i] = vs[i] = ve[i] = 0;
  t_int[0] = 1; vs[0] = 0; ve[0] = 0;
  t_int[1] = 1; vs[1] = 0; ve[1] = 1;
  t_int[2] = 1; vs[2] = 1; ve[2] = 1;
  t_int[3] = 1; vs[3] = 1; ve[3] = 0;
  t_int[4] = 0; vs[4] = 0; ve[4] = 0;  // end marker
  recalc();

  d_i = tsourcelin_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr; 
  for( i=0; i<16; i++ ) {
    ptrs[i+6] = t_int + i;
    ptrs[i+22] = vs + i;
    ptrs[i+38] = ve + i;
  };

  // from TMiso
  ptrs[57] = links;
  ptrs[58] = &vis_x; ptrs[59] = &vis_y;
}

TSourceLin::~TSourceLin()
{
}

TDataSet* TSourceLin::create( TDataSet* apar )
{
  return new TSourceLin( apar );
}

int TSourceLin::getClassId(void) const
{
  return CLASS_ID_TSourceLin;
}

const char* TSourceLin::getClassName(void) const
{
  return "TSourceLin";
}

const TClassInfo* TSourceLin::getClassInfo(void) const
{
  return &class_info;
}

const char *TSourceLin::getHelp(void) const
{
  return helpstr;
}

const char** TSourceLin::getIcon(void) const
{
  return icon;
}

double TSourceLin::f( const double* /* u */, double t )
{
  double tr, v;
  tr = t - t_start;
  while( tr > t_int[curr_lin] ) { // find next line (may be not the next)
    tr -= t_int[curr_lin]; t_start += t_int[curr_lin];
    curr_lin++;
    if( curr_lin >= n_lin )
      curr_lin = 0;
  };
  v = vs[curr_lin] + slopes[curr_lin] * tr;
  return v; 
}


int TSourceLin::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  recalc();
  t_start = 0; curr_lin = 0;
  return rc;
}

void TSourceLin::recalc(void)
{
  int i;
  // fail-safe
  if( t_int[0] <= 0 )
    t_int[0] = 1;
  for( i=0; i<16; i++ ) {
    if( t_int[i] <= 0 ) {
      n_lin = i; 
      break;
    };
    slopes[i] = ( ve[i] - vs[i] ) / t_int[i];
  };
}


/* XPM */
const char* TSourceLin::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
". c #000000",
"` c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #408080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"lllllll..........lllllllllllllll",
"lllllll..........lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"lllllll..llllll..lllllllllllllll",
"l`lllll..llllll......lllllllllll",
"```llll..lllllllllll..llllllllll",
"l`lllll..llllllllllll..lllllllll",
"l`lllll..llllllllllll..lllllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..lllllllllllll..llllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..llllllllllllll..lllllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..lllllllllllllll..llllll",
"l`lllll..llllllllllllllll..lllll",
"l`......lllllllllllllllll.....ll",
"l`......``````````````````....`l",
"llllllllllllllllllllllllllllllll"
};

// end of tsourcelin.cpp

