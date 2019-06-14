/***************************************************************************
                          tintegrator.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2019 by atu
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
  last_rst =  (double)out0_init;
  v_old = v = v_a = last_rst;
  t_rst = 0;
  return 1;
}


double TIntegrator::f() noexcept
{
  double in = in_u - in_u1 + in_u2;
  if( useSqIn ) {
    in *= in;
  }

  if( in_rst ) {
    t_rst = 0;
    last_rst = (double)( v_rst );
    v   = last_rst;
    v_a = last_rst;
  };

  v_old = v;
  double eki = invKi ? ( 1.0/ki ) : ki;
  v += in * ctdt * eki; // TODO: different methods
  if( useMin  &&  v < vmin ) {
    v = (double)vmin;
  }
  if( useMax  &&  v > vmax ) {
    v = (double)vmax;
  }


  t_rst += ctdt;
  v_a = v / t_rst;
  double v_ret = (useAver) ? v_a : v;

  if( useSqrOut ) {
    v_ret = sqrt0( v_ret );
  }
  x2 = v_ret * v_ret;
  return v_ret;
}

DEFAULT_FUNCS_REG(TIntegrator)


// end of tintegrator.cpp

