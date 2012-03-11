/***************************************************************************
                          textrlatch.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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
#include "textrlatch.h"

const char* TExtrLatch::helpstr = "<H1>TExtrLatch</H1>\n"
 "Extremum latch: <br>\n"
 "Latches value or time of given extremum type<br>\n"
 "-<B>type</B> - type of extremum; <br>\n"
 "-<B>useFirst </B> - latch only first extremum; <br>\n"
 "-<B>useLocal </B> - latch local extremums; <br>\n"
 "-<B>outT </B> - output time of latching; <br>\n"
 "-<B>useReset </B> - reset on u[1] strobe; <br>\n"
 "-<B>tStart </B> - start time; <br>\n"
 "-<B>fuzzy </B> - fuzzy level; <br>\n"
 "";

TClassInfo TExtrLatch::class_info = {
  CLASS_ID_TExtrLatch, "TExtrLatch", TExtrLatch::create,
  &TMiso::class_info, helpstr };

TDataInfo TExtrLatch::textrlatch_d_i[20] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "ExtrLatchDlg", "", "Dialog for TExtrLatch"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   5,   20,  70, 120,  20, 0,  0.0, 0.0, 0, 0, "type", "Extr. type", "Max\nMin\nMax||\nAverage\nAmplitude" },
 { dtpInt,   dtpsSw,   0,   20, 100, 120,  20, 2,  0.0, 0.0, 0, 0, "useFirst", "Only first extremun", "&First only"},
 { dtpInt,   dtpsSw,   0,   20, 130, 120,  20, 2,  0.0, 0.0, 0, 0, "useLocal", "Latch local extremums", "&Local"},
 { dtpInt,   dtpsSw,   0,   20, 160, 120,  20, 2,  0.0, 0.0, 0, 0, "outT", "Output time of extremum", "out &T"},
 { dtpInt,   dtpsSw,   0,   20, 190, 120,  20, 2,  0.0, 0.0, 0, 0, "useReset", "Reset if u[1] strobe", "Reset on u[1]"},
 { dtpLabel,      0,   0,  180,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_tStart", "", "start Time"},
 { dtpDou,        0,   0,  180,  70, 120,  20, 2,  0.0, 1e300, 0, 0, "tStart", "start Time", ""},
 { dtpLabel,      0,   0,  180, 110, 100,  20, 0,  0.0, 0.0, 0, 0, "l_fuzzy", "", "fuzzy level"},
 { dtpDou,        0,   0,  180, 130, 120,  20, 0,  0.0, 1e300, 0, 0, "fuzzy", "fuzzy level for local extremums", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TExtrLatch::TExtrLatch( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  type = 0; useLocal = useFirst = outT = useReset = 0; tStart = fuzzy = 0;
  u_min = u_max = t_min = t_max = 0; isStart = wasExtr = 0; 
  d_i = textrlatch_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; ptrs[5] = &useFirst; ptrs[6] = &useLocal;
  ptrs[7] = &outT; 
  ptrs[8] = &useReset; 
  ptrs[10] = &tStart; ptrs[12] = &fuzzy;
  // from TMiso 
  ptrs[16] = links;
  ptrs[17] = &vis_x; ptrs[18] = &vis_y;
}

TExtrLatch::~TExtrLatch()
{
}

TDataSet* TExtrLatch::create( TDataSet* apar )
{
  return new TExtrLatch( apar );
}

int TExtrLatch::getClassId(void) const 
{
  return CLASS_ID_TExtrLatch;
}

const char* TExtrLatch::getClassName(void) const 
{
  return "TExtrLatch";
}

const TClassInfo* TExtrLatch::getClassInfo(void) const
{
  return &class_info;
}

const char *TExtrLatch::getHelp(void) const
{
  return helpstr;
}

const char** TExtrLatch::getIcon(void) const
{
  return icon;
}

double TExtrLatch::f( const double* uu, double t )
{
  double v, u, ua;
  int k;
  static const int mybits[6] = { 1, 2, 4, 7, 7, 0 };
  if( useLocal && fuzzy > 0 && isStart == 0 ) { // TODO: more robust need
    u = ( uu[0] + fuzzy * u_old + 0.1 * fuzzy * u_old2 ) / ( 1 + 1.1 * fuzzy );
  } else {
    u = uu[0]; 
  };
  ua = fabs( u );
  if( t < tStart )                 // time before work
     return u;
  if( useReset && uu[1] > 0.1 ) {   // reset on u[1] signal
    isStart = 1; wasExtr = 0; // for first only
    u_max = u_min = u_abs = u_ex = t_max = t_min = t_abs = t_ex = 0;
    u_old = u_old2 = t_old = 0;
  }
  if( useFirst && wasExtr )        // keep first extremum
    return outT ? t_ex : u_ex;
  if( isStart == 1 ) {
    isStart = 2;
    u_min = u_max = u; u_abs = ua; t_ex = t_min = t_max = t_abs = t;
    if( outT ) return t;
    switch( type ) {
      case 0: u_ex = u_max; break; 
      case 1: u_ex = u_min; break;
      case 2: u_ex = u_abs; break;
      case 3: u_ex = u_max; break;
      case 4: u_ex = 0; break;
      default: u_ex = 0;
    };
    u_old = uu[0];
    return outT ? t : u;
  };
  if( isStart == 2 ) {
    isStart = 0;
    if( useLocal ) {
      u_old2 = u_old; u_old = uu[0]; 
      return u;
    };
  };
  if( !useLocal ) {
    k = 0;
    if( u > u_max ) 
      { u_max = u; t_max = t; k  = 1; };
    if( u < u_min ) 
      { u_min = u; t_min = t; k |= 2; };
    if( ua > u_abs )
      { u_abs = ua; t_abs = t; k |= 4; };
    if( k & mybits[ type ] ) {
      switch( type ) {
	case 0: u_ex = u_max; t_ex = t_max; break;
	case 1: u_ex = u_min; t_ex = t_min; break;
	case 2: u_ex = u_abs; t_ex = t_abs; break;
	case 3: u_ex = 0.5 * ( u_min + u_max ); 
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	case 4: u_ex = 0.5 * ( u_max - u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	default: u_ex = 0; t_ex = t;
      };
    } else {
      if( useFirst )
	wasExtr = 1;
    };
  } else {  // -------------------------- useLocal
    k = 0;
    if( u < u_old && u_old >= u_old2 ) {
      u_max = u_old; t_max = t_old; k = 1;
      if( u_max > u_abs ) {
	u_abs = u_max; t_abs = t_max; k |= 4;
      };
    }; 
    if( u > u_old && u_old <= u_old2 ) {
      u_min = u_old; t_min = t_old; k |= 2;
      if( fabs( u_min ) > u_abs ) {
	u_abs = fabs( u_min ); k |= 4;
      };
    }; 
    if( k & mybits[ type ] ) {
      switch( type ) {
	case 0: u_ex = u_max; t_ex = t_max; break;
	case 1: u_ex = u_min; t_ex = t_min; break;
	case 2: u_ex = u_abs; t_ex = t_abs; break;
	case 3: u_ex = 0.5 * ( u_max + u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	case 4: u_ex = 0.5 * ( u_max - u_min );
		t_ex = ( t_min > t_max ) ? t_min : t_max; break;
	default: u_ex = 0; t_ex = t; break;
      };
    };
  };

  v = outT ? t_ex : u_ex; 
  u_old2 = u_old; u_old = u; t_old = t;
  return v;
}

int TExtrLatch::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  isStart = 1; wasExtr = 0; // for first only
  u_max = u_min = u_abs = u_ex = t_max = t_min = t_abs = t_ex = 0;
  u_old = u_old2 = t_old = 0;
  return rc;
}

/* XPM */
const char* TExtrLatch::icon[] = {
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjbbbb",
"jjjjjjjjjjjjjjjjjjjjiiiggggggggb",
"jjjjjjjjjjjjjjjjjjiiijjgjiibbbbb",
"jjjjjjjjjjjjjjjjjiijjjjgjjiibbbb",
"jjjjjjjjjjjjjjjjiijjjjjgjjjiibbb",
"jjjjjjjjjjjjjjjjijjjjjjgjjjjibbb",
"jjjjjjjjjjjjjjjijjjjjjjgjjjjbibb",
"jjjjjjjjjjjjjjijjjjjjjjgjjjjbiib",
"bbbbbbbbbbbbbibbbbbbbbbgbbbbbbib",
"bbbbbbbbbbbbiibbbbbbbbbgbbbbbbbb",
"bbbbbbbbbbbiibbbbbbbbbbgbbbbbbbb",
"bbbbbbbbbbbibbbbbbbbbbbgbbbbbbbb",
"ffffffffffiifffffffffffffffffffb",
"bbbbbbbbbiibbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbibbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbiibbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbibbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbibbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbiibbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbibbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"biibbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
};

// end of textrlatch.cpp

