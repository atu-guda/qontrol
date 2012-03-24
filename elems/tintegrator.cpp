/***************************************************************************
                          tintegrator.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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
#include "tintegrator.h"

const char* TIntegrator::helpstr = "<H1>TIntegrator</H1>\n"
 "Realistic integrator: <br>\n"
 "Can has limits, discharge, reset, floating base.<br>\n"
 "- <b>ki</b> - coefficient of integration;<br>\n"
 "- <b>s_val</b> - initial value, may be added to base;<br>\n"
 "- <b>vmin, vmax</b> - limits. Used if checked flags <b>useMin, UseMax</b>;<br>\n"
 "- <b>dis</b> - coefficient of discharge.Used if checked flag <b>useDis</b>;<br>\n"
 "- <b>useReset</b> - set value to <b>base</b> if u[1] > 0.1;<br>\n"  
 "- <b>useBase</b> - use u[3] as base for reset and discharge, def=0;<br>\n"  
 "- <b>useAdd</b> - add <b>s_val</b> to base;<br>\n"  
 "- <b>useHold</b> - on reset, hold old value this step;<br>\n"  
 "- <b>useAver</b> - calculate average value;<br>\n"  
 "- <b>useSqIn</b> - calculate u^2 before processing;<br>\n"  
 "- <b>useSqrOut</b> - calculate sqrt(out);<br>\n"  
 "- <b>useMin, useMax</b> - use limits to bound value."; 

TClassInfo TIntegrator::class_info = {
  CLASS_ID_TIntegrator, "TIntegrator", TIntegrator::create,
  &TMiso::class_info, helpstr };

TDataInfo TIntegrator::tintegrator_d_i[29] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 400, 410, 0,  0.0, 0.0, 0, 0, "int_dial", "", "Dialog for TIntegrator"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  40,  50,  20, 0,  0.0, 0.0, 0, 0, "l_ki", "", "Ki"},
 { dtpDou,        0,   0,   20,  60, 120,  20, 0,  -1e300, 1e300, 0, 0, "ki", "ki", ""},
 { dtpLabel,      0,   0,   30,  90, 100,  20, 0,  0.0, 0.0, 0, 0, "l_lim",   "", "Limits:"},
 { dtpInt,   dtpsSw,   0,   20, 110,  50,  20, 0,  0.0, 0.0, 0, 0, "useMin", "useMin", "Min:"},
 { dtpDou,        0,   0,   90, 110, 100,  20, 0,  -1e300, 1e300, 0, 0, "vmin", "min value", ""},
 { dtpInt,   dtpsSw,   0,   20, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "useMax", "useMax", "Max:"},
 { dtpDou,        0,   0,   90, 130, 100,  20, 0,  -1e300, 1e300, 0, 0, "vmax", "max value", ""},
 { dtpLabel,      0,   0,   30, 160, 100,  20, 0,  0.0, 0.0, 0, 0, "l_s_val", "", "Start value"},
 { dtpDou,        0,   0,   20, 180, 100,  20, 2,  -1e300, 1e300, 0, 0, "s_val", "Start value", ""},
 { dtpInt,   dtpsSw,   0,  220,  50, 150,  20, 2,  0.0, 0.0, 0, 0, "useReset", "use reset", "Use u[1] as Reset"},
 { dtpInt,   dtpsSw,   0,  220,  80, 150,  20, 2,  0.0, 0.0, 0, 0, "useBase", "use base", "Use u[3] as base"},
 { dtpInt,   dtpsSw,   0,  220, 110, 150,  20, 2,  0.0, 0.0, 0, 0, "useAdd", "add start to base", "Add start to base"},
 { dtpInt,   dtpsSw,   0,  220, 140, 150,  20, 0,  0.0, 0.0, 0, 0, "useDis", "use discarge", "Use discharge"},
 { dtpLabel,      0,   0,  230, 160, 100,  20, 0,  0.0, 0.0, 0, 0, "l_dis", "", "Discharge coeff."},
 { dtpDou,        0,   0,  220, 180, 100,  20, 0,  0.0, 1e300, 0, 0, "dis", "Discharge coeff", ""},
 { dtpInt,   dtpsSw,   0,  220, 210, 150,  20, 0,  0.0, 0.0, 0, 0, "useHold", "", "Hold value 1-step"},
 { dtpInt,   dtpsSw,   0,  220, 240, 150,  20, 0,  0.0, 0.0, 0, 0, "useAver", "", "calculate Average"},
 { dtpInt,   dtpsSw,   0,  220, 270, 150,  20, 0,  0.0, 0.0, 0, 0, "useSqIn", "", "calculete in^2"},
 { dtpInt,   dtpsSw,   0,  220, 300, 150,  20, 0,  0.0, 0.0, 0, 0, "useSqrOut", "", "calculate sqrt(out)"},
 { dtpButton,     0,   0,   20, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  150, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  280, 360,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TIntegrator::TIntegrator( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  ki = 1; vmin = -1000;  vmax = 1000; s_val = 0; dis = 1e-3; t_rst = 0;
  useMin = useMax = useReset = useBase = useAdd = useDis 
         = useAver = useHold = useSqIn = useSqrOut = 0;
  d_i = tintegrator_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &ki;  ptrs[6] = &useMin; ptrs[7] = &vmin;
  ptrs[8] = &useMax; ptrs[9] = &vmax;
  ptrs[11] = &s_val; ptrs[12] = &useReset; ptrs[13] = &useBase;
  ptrs[14] = &useAdd; ptrs[15] = &useDis;
  ptrs[17] = &dis;
  ptrs[18] = &useHold;
  ptrs[19] = &useAver;
  ptrs[20] = &useSqIn;
  ptrs[21] = &useSqrOut;
  // from TMiso 
  ptrs[25] = links;
  ptrs[26] = &vis_x; ptrs[27] = &vis_y;
}

TIntegrator::~TIntegrator()
{
}

TDataSet* TIntegrator::create( TDataSet* apar )
{
  return new TIntegrator( apar );
}

int TIntegrator::getClassId(void) const 
{
  return CLASS_ID_TIntegrator;
}

const char* TIntegrator::getClassName(void) const 
{
  return "TIntegrator";
}

const TClassInfo* TIntegrator::getClassInfo(void) const
{
  return &class_info;
}

const char *TIntegrator::getHelp(void) const
{
  return helpstr;
}

const char** TIntegrator::getIcon(void) const
{
  return icon;
}


int TIntegrator::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  v_old = v = s_val; t_rst = 0;
  return rc;
}


double TIntegrator::f( const double* u, double /* t */ )
{
  double v_ret, base = 0, in;
  if( useBase ) base = u[3];
  if( useAdd )  base += s_val;
  if( useSqIn )
    in = u[0] * u[0];
  else
    in = u[0];

  if( useReset && u[1] > 0.1 ) {
    t_rst = 0;
  };

  if( t_rst < (tdt/2) ) {  // start or reset case
    v_ret = v;
    if( useAver ) {
      v = 0;
      v_ret = in;
      if( useHold )
	v_ret = v_old ;
    } else {
      v = v_ret = s_val;
      if( useHold ) 
	v_ret = v_old;
    };
  } else {
    v += in * tdt * ki;
    if( useDis ) {
      v -= tdt * ( v - base ) * dis;
    };
    if( useMin  &&  v < vmin )
      v = vmin;
    if( useMax  &&  v > vmax )
      v = vmax;
    v_ret = v;
    if( useAver ) {
       v_ret = v_ret  / t_rst;
    };  
  };
  t_rst += tdt; v_old = v_ret;
  if( useSqrOut ) {
    if( v_ret > 0 )
      v_ret = sqrt( v_ret );
    else
      v_ret = 0;
  };
  return v_ret;
}

/* XPM */
const char* TIntegrator::icon[] = {
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
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllggggllllllllllllllllllllll",
"lllllgggggglllllllllllllllllllll",
"llllggllllggllllllllllllllllllll",
"llllggllllggllllllllllllllllllll",
"llllggllllglllllllllllllllllllll",
"lllllgglllllllllllllllllllllllll",
"lllllgglllllllllllllllllllllllll",
"llllllggglllllllllllllllllllllll",
"lllllllgglllllllllllllllllllllll",
"lllllllgglllllllllllllllllllllll",
"llllllllggllllllllllllllllllllll",
"llllllllggllllllllllllllllllllll",
"lllllllllgglllllllllllllllllllll",
"lllllllllgglllllllllllllllllllll",
"lllllllllgglllllllllllllllllllll",
"lllllllllgglllllllllllllllllllll",
"lllllglllgglllllllllllllllllllll",
"llllgglllgglllllllllllllllllllll",
"llllgggllgglllllllllllllllllllll",
"lllllgggggllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll"
};

// end of tintegrator.cpp

