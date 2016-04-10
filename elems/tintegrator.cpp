/***************************************************************************
                          tintegrator.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2016 by atu
    email                : atu@nmetau.edu.ua
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
 "<p>Realistic integrator: <br>\n"
 "Can has limits, discharge, reset, floating base.<br></p>\n";

STD_CLASSINFO(TIntegrator,clpElem );

CTOR(TIntegrator,TMiso)
{
}




int TIntegrator::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  v_old = v = v_a = (double)out0_init;
  t_rst = 0;
  return 1;
}


double TIntegrator::f() noexcept
{
  double v_ret, base = 0, in;
  if( useBase ) {
    base = in_base;
  }
  if( useAdd ) {
    base += out0_init;
  }
  if( useSqIn ) {
    in = in_u * in_u;
  }  else {
    in = in_u;
  }

  if( useReset && in_rst > 0.1 ) {
    t_rst = 0;
  };

  if( t_rst < (ctdt/2) ) {  // start or reset case
    v_ret = v;
    if( useAver ) {
      v = 0;  v_ret = v_a = in;
    } else {
      v = v_ret = v_a = out0_init;
    };
    if( useHold ) {
      v_ret = v_old;
    }
  } else {
    v += in * ctdt * ki;
    if( useDis ) {
      v -= ctdt * ( v - base ) * dis;
    };
    if( useMin  &&  v < vmin ) {
      v = (double)vmin;
    }
    if( useMax  &&  v > vmax ) {
      v = (double)vmax;
    }
    v_ret = v;
    v_a = v_ret  / t_rst;
    if( useAver ) {
       v_ret = v_a;
    };
  };

  t_rst += ctdt; v_old = v_ret;
  if( useSqrOut ) {
    v_ret = ( v_ret > 0 ) ? sqrt( v_ret ) : 0.0;
  }
  return v_ret;
}

DEFAULT_FUNCS_REG(TIntegrator)


// end of tintegrator.cpp

