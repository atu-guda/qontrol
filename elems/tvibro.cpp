/***************************************************************************
                          tvibro.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#include "tmodel.h"
#include "tvibro.h"

const char* TVibro::helpstr = "<H1>TVibro</H1>\n"
 "Vibrational element <b> d2x/dt^2 + c0*dx/dt + Omega^2*f(x) = u</b>: <br>\n"
 "Parameters <b>c0</b> and <b>Omega</b> can be changed at any time.<br>\n"
 "<b>rfe</b> - NONE! name of element, which must input <b>x</b> as parm[0]. <br>\n"
 "If output of element depend of it's state, it must be flagged as '<b>noauto</b>'" 
 "If this name empty or bad, assumed <b>f(x) = x</b>.";

TClassInfo TVibro::class_info = {
  CLASS_ID_TVibro, "TVibro", TVibro::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TVibro::tvibro_d_i[16] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "vibro_dial", "", "Dialog for TVibro"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_c0", "", "c0"},
 { dtpDou,        0,   0,   20,  70, 120,  20, 0,  0.0, 1e300, 0, 0, "c0", "Damping coeff", ""},
 { dtpLabel,      0,   0,  210,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_Omega", "", "Omega"},
 { dtpDou,        0,   0,  200,  70, 120,  20, 0,  -1e300, 1e300, 0, 0, "Omega", "Natural frequency", ""},
 { dtpLabel,      0,   0,   50, 130, 250,  20, 0,  0.0, 0.0, 0, 0, "l_rfe", "", "Return force element name"},
 { dtpStr,        0,  MAX_NAMELEN,   60, 150, 250,  20, 2,  0.0, 0.0, 0, 0, "rfe", "Return force element", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TVibro::TVibro( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  Omega = 1.2; c0 = 0.4; 
  // rfe = QString(); 
  use_u1 = 0;
  isStart = 1; tdt2 = 1; u_old = f_old = x_old = x_old2 = 0;
  d_i = tvibro_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &c0; ptrs[6] = &Omega; ptrs[8] = &rfe;
  // from TMiso 
  ptrs[12] = links;
  ptrs[13] = &vis_x; ptrs[14] = &vis_y;
}

TVibro::~TVibro()
{
}

TDataSet* TVibro::create( TDataSet* apar )
{
  return new TVibro( apar );
}

int TVibro::getClassId(void) const 
{
  return CLASS_ID_TVibro;
}

const TClassInfo* TVibro::getClassInfo(void) const
{
  return &class_info;
}

const char *TVibro::getHelp(void) const
{
  return helpstr;
}

const char** TVibro::getIcon(void) const
{
  return icon;
}

int TVibro::do_preRun( int /*run_tp*/, int /*an*/, 
                       int /*anx*/, int /*any*/, double /*adt*/ )
{
  tdt2 = tdt * tdt;
  return 0;
}

int TVibro::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  x_old = x_old2 = u_old = f_old = 0;
  isStart = 1;
  return rc;
}

double TVibro::f( const double* u, double /*t*/ )
{
  double x, ctau;
  if( isStart == 1 ) {  // first step
    isStart = 2; x = 0;
    if( use_u1 ) {
      f_old = u[1];
    } else {
      f_old = x;
    };
    u_old = u[0];
    return x;
  };
  if( isStart == 2 ) {  // second step
    isStart = 0; x = x_old + 0.5 * tdt2 * u_old;
    if( use_u1 ) {
      f_old = u[1];
    } else {
      f_old = x;
    };
    u_old = u[0]; x_old2 = x_old; x_old = x;
    return x;
  };
  // all other steps
  ctau = tdt * c0 / 2; // c0 can be changed at any time, so here
  x = ( 2*x_old - x_old2 * (1-ctau) + tdt2 * (u_old - Omega * Omega *f_old) ) 
    / ( 1 + ctau );
  if( use_u1 ) {
    f_old = u[1];
  } else {
    f_old = x;
  };
  u_old = u[0]; x_old2 = x_old; x_old = x;
  return x;
}

int TVibro::registered = reg();

int TVibro::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TVibro::icon[] = {
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"jjjjjjjjjjjjjjjjjjjjjjjjjjbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbhhhhhhhhhhhhbb",
"bbbbbibbbbbbibbbbbbb#bblbbbbbhbb",
"bbbbiibbbbbbibbbbbb#bb#bb#blbhbb",
"bbbbibibbbbibibbbbblbbgggbbbbhbb",
"bbbibbibbbbibibbbbbb#gggggb#bhbb",
"bbbibbibbbbibibbbbbbgggggggblhbb",
"biibbbibbbibbibbbiiigggggggbbhbb",
"bbbbbbbibbibbbibbibbgggggggbbhbb",
"bbbbbbbibibbbbibibbbbgggggbbbhbb",
"bbbbbbbibibbbbibibb#bbgggbblbhbb",
"bbbbbbbibibbbbibibbbbbbbbbbb#hbb",
"bbbbbbbbibbbbbbibbbbl#bbb#bbbhbb",
"bbbbbbbbibbbbbbibbbhhhhhhhhhhhbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
};


// end of tvibro.cpp

 
