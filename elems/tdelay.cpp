/***************************************************************************
                          tdelay.cpp  -  description
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

#include "tdelay.h"
#include "miscfun.h" 
#include "tdelay.h"

const char* TDelay::helpstr = "<H1>TDelay</H1>\n"
 "Delay signal for given time: <br>\n"
 "Have 2 parameters: <b>mdelay</b> - maximum delay, <br>\n"
 "<b>cdelay</b> - current delay, can changed at any time, but <= <b>mdelay</b>.";

TClassInfo TDelay::class_info = {
  CLASS_ID_TDelay, "TDelay", TDelay::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TDelay::tdelay_d_i[14] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial, 0, 0, 0, 0, 480, 320, 0, 0.0, 0.0, 0, 0, "dia_delay", "", "Dialog for TDelay" },
 { dtpInt, 0, 0, 20, 10, 60, 20, 8, 0.0, 0.0, 0, 0, "ord", "Order", "" }, 
 { dtpStr, 0, 60, 100, 10, 380, 20, 2, 0.0, 0.0, 0, 0, "descr", "Description", "" },
 { dtpLabel, 0, 0, 30, 50, 100, 20, 0, 0.0, 0.0, 0, 0, "l_mdelay", "", "Max delay" }, 
 { dtpDbl, 0, 0, 30, 70, 100, 20, 0, 0.0, 1e4, 2, 0, "mdelay", "Max. delay", "" }, 
 { dtpLabel, 0, 0, 160, 50, 90, 20, 0, 0.0, 0.0, 0, 0, "l_cdelay", "", "Delay" },
 { dtpDbl, 0, 0, 150, 70, 100, 20, 0, 0.0, 1e4, 0, 0, "cdelay", "Delay", "" },
 { dtpButton,     0,   0,   20, 200,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 200,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  250, 200,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TDelay::TDelay( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( mdelay, "Max. delay" ),
	PRM_INIT( cdelay, "Current delay" )
{
  int i;
  mdelay = 1.5; cdelay = 1; v1 = v2 = u00 = 0; buf = 0; 
  d_i = tdelay_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr; 
  ptrs[4] = &mdelay;  ptrs[6] = &cdelay;
  // from TMiso
  ptrs[10] = links;
  ptrs[11] = &vis_x; ptrs[12] = &vis_y;
  PRMI(mdelay).setDescr( "Maximum delay" );
  PRMI(mdelay).setMinMax( 0, 1e10 );
  PRMI(cdelay).setDescr( "Current delay" );
  PRMI(cdelay).setMinMax( 0, 1e10 );
}

TDelay::~TDelay()
{
  delete buf; buf = 0; // in case of postRun missing
}

TDataSet* TDelay::create( TDataSet* apar )
{
  return new TDelay( apar );
}

int TDelay::getClassId(void) const
{
  return CLASS_ID_TDelay;
}

const char* TDelay::getClassName(void) const
{
  return "TDelay";
}

const TClassInfo* TDelay::getClassInfo(void) const
{
  return &class_info;
}

const char *TDelay::getHelp(void) const
{
  return helpstr;
}

const char** TDelay::getIcon(void) const
{
  return icon;
}

double TDelay::f( const double* u, double t )
{
  double a1, a2;
  if( buf == 0 ) return 0;
  if( t < 1.3 * tdt )
    u00 = u[0];
  buf->add( u[0] );
  a1 = (*buf)[icd]; a2 = (*buf)[icd+1];
  if( t < cdelay )
    return u00;
  return v1 * a1 + v2 * a2;
}

int TDelay::preRun( int run_tp, int an, int anx, int any, double atdt )
{
  TMiso::preRun( run_tp, an, anx, any, atdt );
  imd = int( mdelay / tdt );
  buf = new TCircBuf( imd );
  return 0;
}

int TDelay::postRun( int good )
{
  delete buf; buf = 0;
  TMiso::postRun( good );
  return 0;
}

int TDelay::startLoop( int acnx, int acny )
{
  double v;
  int rc = TMiso::startLoop( acnx, acny );
  buf->reset(); u00 = 0; 
  v = cdelay / tdt;
  icd = int( v );
  v2 = v - icd; v1 = 1.0 - v2;
  return rc;
}

int TDelay::setDataID( int ni, double da, int allowConv  )
{
  int k; double v;
  k = TMiso::setDataID( ni, da, allowConv );
  if( k == 0 && ( ni == 4 || ni == 6 ) ) {
    if( mdelay < cdelay ) 
      cdelay = mdelay;
    if( buf != 0 ) {
      v = cdelay / tdt;
      icd = int( v );
      v2 = v - icd; v1 = 1.0 - v2;
    };
  };
  return k;
}

/* XPM */
const char* TDelay::icon[] = {
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjiiijjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjijjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjiiijjjjjjjjjjjijijjjjjjjj",
"jjjjjjjiiijjjjjjjjjjjjijjjjjjjjj",
"jjjjjjjijiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiiiiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiijiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiijiijjjjjjjjjjjjjjjjjjjjj",
"jjiiiiiijiiiiiiiiiiiiiiiiiiiijjj",
"jjiiiiijjjiiiiiiiiiiiiiiiiiiijjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjjgjjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjgggjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjgggjjjjjjjjjjjjjj",
"jjjjj.j.j.jjjjggjggjjjjjjjjjjjjj",
"jjjjjj...jjjjjggjggjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjggjggjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjggjggjjjjjjjjjjjjj",
"jjgggggggggggggjjjgggggggggggjjj",
"jjgggggggggggggjjjgggggggggggjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};

// end of tdelay.cpp

 
