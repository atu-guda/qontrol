/***************************************************************************
                          trelaxgen.cpp  -  description
                             -------------------
    begin                : 2014.10.28
    copyright            : (C) 2014-2015 by atu
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
using namespace std;

#include "trelaxgen.h"

const char* TRelaxGen::helpstr = "<h1>TRelaxGen</h1>\n"
 "Relaxation generator: <br/>\n"
 "Simulates element, in which capacitor C is charging from external voltage source v_in <br/>\n"
 "via resistor r1 up to v_1, than discharges to ground down to v_2 via r1. \n"
 "Charging or discharging may be continuous. \n"
 "External elements may be used as voltage-current conversions.<br/>. \n";

STD_CLASSINFO(TRelaxGen,clpElem );

CTOR(TRelaxGen,TMiso)
{
}


int TRelaxGen::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  v = (double)out0_init;
  isDis = I =  dv_ch = dv_dis = 0;
  return 1;
}

double TRelaxGen::f( double  /* t */ )
{
  // TODO: external I calc
  dv_ch  = ( v_in - v );
  dv_dis = (double)v;
  if( isDis ) {
    if( useDischarge ) {
      I = -i_dis;
    } else {
      I = -dv_dis / r_2;
    }
    if( useContCharge ) {
      I += dv_ch / r_1;
    }
    v += I * tdt / c;
    if( v <= v_2 ) {
      isDis = 0;
    }
  } else {
    if( useCharge ) {
      I = i_ch;
    } else {
      I = dv_ch / r_1;
    }
    if( useContDischarge ) {
      I -= dv_dis / r_2;
    }
    v += I * tdt / c;
    if( v >= v_1 || ( useTrig && trig > 0.1 ) ) {
      isDis = 1;
    }
  }

  return v;
}

DEFAULT_FUNCS_REG(TRelaxGen)


// end of trelaxgen.cpp


