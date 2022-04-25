/***************************************************************************
                          tintegrator.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2022 by atu
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
#include "miscfun.h"

using namespace std;

const char* TIntegrator::helpstr = "<H1>TIntegrator</H1>\n"
 "<p>Realistic integrator: <br>\n"
 "Can has 3 inputs (sum), limits, discharge, reset.<br></p>\n";

STD_CLASSINFO(TIntegrator,clpElem );

CTOR(TIntegrator,TMiso)
{
}




int TIntegrator::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  useAver_c   = (bool)useAver; // TODO:? preRun
  useMin_c    = (bool)useMin;
  useMax_c    = (bool)useMax;
  useSqIn_c   = (bool)useSqIn;
  useSqrOut_c = (bool)useSqrOut;
  invKi_c     = (bool)invKi;

  last_rst =  (double)out0_init;
  v_old = v = v_a = last_rst;
  t_rst = 0;
  x2 = v_old * v_old;

  return 1;
}

void TIntegrator::pre_f() noexcept
{
  in = in_u - in_u1 + in_u2;
  if( useSqIn_c ) {
    in *= in;
  }
  double eki = invKi_c ? ( 1.0/ki ) : ki;
  in_k = in * eki;
}

double TIntegrator::f() noexcept
{
  pre_f();

  if( in_rst ) {
    t_rst = 0;
    last_rst = (double)( v_rst );
    v   = last_rst;
    v_a = last_rst;
  };

  v_old = v;
  v += in_k * ctdt; // TODO: different methods

  if( useMin_c  &&  v < vmin ) {
    v = (double)vmin;
  }
  if( useMax_c  &&  v > vmax ) {
    v = (double)vmax;
  }


  t_rst += ctdt;
  v_a = v / t_rst;
  double v_ret = (useAver_c) ? v_a : v;

  if( useSqrOut_c ) {
    v_ret = sqrt0( v_ret );
  }
  x2 = v_ret * v_ret;
  return v_ret;
}

void TIntegrator::preCalc()
{
  readAllInputs();
  pre_f();
}

DEFAULT_FUNCS_REG(TIntegrator)


// end of tintegrator.cpp

