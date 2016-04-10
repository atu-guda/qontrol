/***************************************************************************
                          trelaxgen.cpp  -  description
                             -------------------
    begin                : 2014.10.28
    copyright            : (C) 2014-2016 by atu
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
 "Simulates element, in which capacitor <b>c</b> is charging <br/>"
 "from external voltage source <b>v<sub>in</sub></b> <br/>\n"
 "via resistor <b>r<sub>1</sub></b> up to <b>v<sub>1</sub></b>,<br/>"
 "than discharges to ground down to <b>v<sub>2</sub></b> via <b>r<sub>2</sub></b>.\n"
 "Charging or discharging may be continuous. \n"
 "External elements may be used as voltage-current conversions.<br/>\n";

STD_CLASSINFO(TRelaxGen,clpElem );

CTOR(TRelaxGen,TMiso)
{
}


int TRelaxGen::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  v = (double)out0_init;
  isDis = I =  dv_ch = dv_dis = 0;
  return 1;
}

double TRelaxGen::f() noexcept
{
  if( prm_mod ) {
    if( c <= 0 )   { c   = 1e-16; };
    if( r_1 <= 0 ) { r_1 = 1e-6;  };
    if( r_2 <= 0 ) { r_2 = 1e-6;  };
    prm_mod = 0;
  }
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
    v += I * ctdt / c;
    if( v <= v_2 ) {
      isDis = 0;
    }
  } else { // -------- charging
    if( useCharge ) {
      I = i_ch;
    } else {
      I = dv_ch / r_1;
    }
    if( useContDischarge ) {
      I -= dv_dis / r_2;
    }
    v += I * ctdt / c;
    if( v >= v_1 || ( useTrig && trig > 0.1 ) ) {
      isDis = 1;
    }
  }

  return v;
}

DEFAULT_FUNCS_REG(TRelaxGen)


// end of trelaxgen.cpp


