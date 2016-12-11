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
  I =  I_in = I_out = dv_ch = dv_dis = 0;
  isDis = ( v >= v_1 ) ? 1 : 0;
  return 1;
}

double TRelaxGen::f() noexcept
{
  if( prm_mod ) {
    if( c <= 0 )   { c   = 1e-16; };
    if( r_1 <= 0 ) { r_1 = 1e-6;  };
    if( r_2 <= 0 ) { r_2 = 1e-6;  };
    if( r_lc <= 0 ) { r_lc = 1e-6;  };
    if( r_ld <= 0 ) { r_ld = 1e-6;  };
    prm_mod = 0;
  }
  // TODO: external I calc
  dv_ch  = ( v_in - v );
  if( noDischargeSrc && dv_ch < 0 ) {
    dv_ch = 0;
  }
  dv_dis = (double)v;
  double i_ch_local  =    dv_ch / r_lc;
  double i_dis_local =  -dv_dis / r_ld;

  if( isDis || useContDischarge ) {
    i_dis_local += useDischarge ? ( - i_dis ) : ( -dv_dis / r_2 );
  }
  if( !isDis || useContCharge ) {
    i_ch_local += useCharge     ? (    i_ch ) : (  dv_ch  / r_1 );
  }
  I_in = i_ch_local; I_out = i_dis_local; I = i_ch_local + i_dis_local;
  v += I * ctdt / c;

  int new_isDis = isDis;
  if( isDis && v <= v_2 ) {
    new_isDis = 0;
    if( limitDis ) {
      v = min( v_2.cval(), v_in.cval() );
    }
  }
  if( !isDis && v >= v_1 ) {
    new_isDis = 1;
    if( limitCh ) {
      v = v_1;
    }
  }
  if( trig ) {
    new_isDis = 1;
  }
  isDis = new_isDis;

  return v;
}

DEFAULT_FUNCS_REG(TRelaxGen)


// end of trelaxgen.cpp


