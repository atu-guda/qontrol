/***************************************************************************
  tbjt.cpp - Simple BJT transistor model
                             -------------------
    begin                : 2016.08.12
    copyright            : (C) 2016-2016 by atu
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

#include "tbjt.h"
#include "gsl/gsl_const_mksa.h"

const char* TBjt::helpstr = "<h1>TBjt</h1>\n"
 "<p>Simple (Ebers-Moll alike) BJT transistor simulation <br>\n"
 "</p>\n";

STD_CLASSINFO(TBjt,clpElem|clpCalcAtStart);

CTOR(TBjt,TMiso)
{
}

double TBjt::f() noexcept
{
  const constexpr double k_b = GSL_CONST_MKSA_BOLTZMANN;       // 1.3806504e-23)  kg m^2 / K s^2
  const constexpr double q_e = GSL_CONST_MKSA_ELECTRON_CHARGE; // 1.602176487e-19)  A s
  const constexpr double k1 = k_b / q_e;

  V_t = k1 * Temp;
  V_be = V_b - V_e;
  V_ce = V_c - V_e;

  // TODO: IF(mode)

  I_c = I_s * expm1( V_be / ( V_t * N_f ) )  * ( 1 + V_ce / V_af );
  I_b = I_c / h_FE;
  I_e = I_c + I_b;

  return I_c;
}


DEFAULT_FUNCS_REG(TBjt)


// end of tbjt.cpp

