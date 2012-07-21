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

#include <cmath>
#include "tintegrator.h"

using namespace std;

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
  &TMiso::class_info, helpstr, clpElem };


TIntegrator::TIntegrator( TDataSet* aparent )
        :TMiso( aparent )
{
  ki = 1; vmin = -1000;  vmax = 1000; s_val = 0; dis = 1e-3; t_rst = 0;
  useMin = useMax = useReset = useBase = useAdd = useDis 
         = useAver = useHold = useSqIn = useSqrOut = 0;
}

TIntegrator::~TIntegrator()
{
}

TDataSet* TIntegrator::create( TDataSet* apar )
{
  return new TIntegrator( apar );
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


double TIntegrator::f( double /* t */ )
{
  double v_ret, base = 0, in;
  if( useBase ) base = *in_so[3];
  if( useAdd )  base += s_val;
  if( useSqIn )
    in = *in_so[0] * *in_so[0];
  else
    in = *in_so[0];

  if( useReset && *in_so[1] > 0.1 ) {
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

int TIntegrator::registered = reg();

int TIntegrator::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
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

