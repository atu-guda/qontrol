/***************************************************************************
  tbjt.cpp - Simple BJT transistor model
                             -------------------
    begin                : 2016.08.12
    copyright            : (C) 2016-2017 by atu
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

#include <gsl/gsl_const_mksa.h>
#include "tbjt.h"
#include "miscfun.h"

using namespace std;

const char* TBjt::helpstr = "<h1>TBjt</h1>\n"
 "<p>Simple (Ebers-Moll alike) BJT transistor simulation <br>\n"
 "</p>\n";

STD_CLASSINFO(TBjt,clpElem|clpCalcAtStart);

CTOR(TBjt,TMiso)
{
}


double TBjt::f() noexcept
{
  const constexpr double k1 =  GSL_CONST_MKSA_BOLTZMANN / GSL_CONST_MKSA_ELECTRON_CHARGE;
  // GSL_CONST_MKSA_BOLTZMANN;       // 1.3806504e-23  kg m^2 / K s^2
  // GSL_CONST_MKSA_ELECTRON_CHARGE; // 1.602176487e-19  A s

  V_t = k1 * Temp;
  V_be = V_b - V_e;
  int n_iter = 1;

  if( useRc ) {
    V_co = 0.5 * V_cc + 1.5 * V_e; // ( V_cc - V_e ) / 2 + V_e;
    n_iter = 3;
  } else {
    V_co = V_c;
  }

  for( int i=0; i<n_iter; ++i ) {

    V_ce = V_co - V_e;
    double I_c0 = I_s * expm1( V_be / ( V_t * N_f ) )  * ( 1 + V_ce / V_af );
    I_c0 = clamp( I_c0, I_c_min, I_c_max.cval() );
    I_b = I_c0 / h_FE;
    double R_ce = V_ce / I_c0 + R_ce0;

    if( useRc ) {
      I_c = ( V_cc - V_e ) / ( R_ce + R_c );
      V_co = V_e + I_c * R_ce;
    } else {
      I_c = V_ce / R_ce;
    }

  }

  I_e = I_c + I_b;
  V_cb = V_co - V_b;

  return I_c;
}


DEFAULT_FUNCS_REG(TBjt)


// end of tbjt.cpp

