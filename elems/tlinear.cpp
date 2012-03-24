/***************************************************************************
                          tlinear.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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

#include "tlinear.h"

const char* TLinear::helpstr = "<H1>TLinear</H1>\n"
 "Linear summator: <br>\n"
 "<b>x = a0*u0 + a1*u1 + a2*u2 + a3*u3 + b</b><br>\n"
 "Have 5 parameters: <b>a0 .. a3, b</b>, each can be changed at any time.";

TClassInfo TLinear::class_info = {
  CLASS_ID_TLinear, "TLinear", TLinear::create,
  &TMiso::class_info, helpstr };

TDataInfo TLinear::tlinear_d_i[20] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "Linear", "", "Dialog for TLinear"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   90,  60,  50,  20, 0,  0.0, 0.0, 0, 0, "la0",   "", "a0="},
 { dtpDou,        0,   0,  150,  60, 120,  20, 0,  -1e300, 1e300, 0, 0, "a0", "a0", ""},
 { dtpLabel,      0,   0,   90,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "la1",   "", "a1="},
 { dtpDou,        0,   0,  150,  90, 120,  20, 0,  -1e300, 1e300, 0, 0, "a1", "a1", ""},
 { dtpLabel,      0,   0,   90, 120,  50,  20, 0,  0.0, 0.0, 0, 0, "la2",   "", "a2="},
 { dtpDou,        0,   0,  150, 120, 120,  20, 0,  -1e300, 1e300, 0, 0, "a2", "a2", ""},
 { dtpLabel,      0,   0,   90, 150,  50,  20, 0,  0.0, 0.0, 0, 0, "la3",   "", "a3="},
 { dtpDou,        0,   0,  150, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "a3", "a3", ""},
 { dtpLabel,      0,   0,   90, 180,  50,  20, 0,  0.0, 0.0, 0, 0, "lb",   "", "b="},
 { dtpDou,        0,   0,  150, 180, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TLinear::TLinear( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  a[0] = 1; a[1] = -1; a[2] = a[3] = 0; b = 0;
  d_i = tlinear_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &a[0]; ptrs[6] = &a[1]; ptrs[8] = &a[2];
  ptrs[10] = &a[3]; ptrs[12] = &b;
  // from TMiso 
  ptrs[16] = links;
  ptrs[17] = &vis_x; ptrs[18] = &vis_y;
}

TLinear::~TLinear()
{
}

TDataSet* TLinear::create( TDataSet* apar )
{
  return new TLinear( apar );
}

int TLinear::getClassId(void) const 
{
  return CLASS_ID_TLinear;
}

const char* TLinear::getClassName(void) const 
{
  return "TLinear";
}

const TClassInfo* TLinear::getClassInfo(void) const
{
  return &class_info;
}

const char *TLinear::getHelp(void) const
{
  return helpstr;
}

const char** TLinear::getIcon(void) const
{
  return icon;
}

double TLinear::f( const double* u, double /* t */ )
{
  return u[0]*a[0] + u[1]*a[1] + u[2]*a[2] + u[3]*a[3] + b;
}

/* XPM */
const char* TLinear::icon[] = {
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
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeee``eeeeee``eeeeeeeeeeeeee",
"eeeeeee`eeeeeeeeee`eeeeeeeeeeeee",
"eeeeee```eeeeeeee```eeeeeeeeeeee",
"eeeee`e```eeeeee```e`eeeeeeeeeee",
"eeeee`ee```eeee```ee`eeeeeeeeeee",
"eeee`eeee```ee```eeee`eeeeeeeeee",
"eeee`eeeee``````eeeee`ee``eeeeee",
"eeee`eeeeee````eeeeee`eeee``eeee",
"`````eeeeee````eeeeee````````eee",
"eeee`eeeee``````eeeee`eeee``eeee",
"eeee`eeee````````eeee`ee``eeeeee",
"eeeee`ee``````````ee`eeeeeeeeeee",
"eeeee`e````````````e`eeeeeeeeeee",
"eeeeee``````````````eeeeeeeeeeee",
"eeeeeee````````````eeeeeeeeeeeee",
"eeeeeeee``````````eeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
};


