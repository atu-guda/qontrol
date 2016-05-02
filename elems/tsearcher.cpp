/***************************************************************************
  tsearcher.cpp - extremum searcher element definition
                             -------------------
    begin                : 2016.04.26
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

#include "tsearcher.h"
#include "miscfun.h"

const char* TSearcher::helpstr = "<H1>TSearcher</H1>\n"
 "Realizes some tactics in extremum search "
 "in non-linear dynamic system identification task.";

STD_CLASSINFO(TSearcher,clpElem|clpCalcAtStart);

CTOR(TSearcher,TMiso)
{
}

// out0 is p_c;

int TSearcher::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  L_l0 = out0_init - p_l;
  L_r0 = p_r  - out0_init;
  if( autocalc_minmax ) {
    p_min = out0_init - c_mm * L_l0;
    p_max = out0_init + c_mm * L_r0;
  }
  return 1;
}


double TSearcher::f() noexcept
{
  QuadExtrIn in { p_l, p_c, p_r, F_l, F_c, F_r, lim_s, 0, 0, true, false };
  QuadExtrOut out;
  // fallback values
  a_1 = 0; a_2 = 0; p_et = 0; p_e = p_c;  dF = 0; dF_dp = 0;
  f_c = 0; f_n = 0; f_g = 0;
  double p_ct0 = p_c - out0_init;

  if( calcQuadExtr( in, out ) ) {
    p_e = out.x_e; p_et = out.x_et; F_e = out.y_e;
    p_lt = out.x_lt; p_rt = out.x_rt; F_lt = out.y_lt; F_rt = out.y_rt;
    a_1 = out.a_1; a_2 = out.a_2;
    dF = F_e - F_c;
    dF_dp = dF / p_et;
  }

  f_c = - k_cl  * ( p_ct0 )
        - k_ch  * barrierHypUp(    p_c, p_max )
        + k_ch  * barrierHypDown(  p_c, p_min )
        - k_ch2 * barrierHyp2Up(   p_c, p_max )
        + k_ch2 * barrierHyp2Down( p_c, p_min );
  f_n =   k_nl  * ( p_r - 2*p_c + p_l )
        - k_nh  * barrierHypUp(    p_c, p_r )
        + k_nh  * barrierHypDown(  p_c, p_l )
        - k_nh2 * barrierHyp2Up(   p_c, p_r )
        + k_nh2 * barrierHyp2Down( p_c, p_l ); // TODO: constant shift

  switch( (F_g_Type)(int)(f_g_type) ) {
    case fg_p_e:
      f_g = ( p_e - p_c );
      break;
    case fg_diff:
      f_g = ( F_r - F_l ) / ( p_r - p_l );
      break;
    default:
      f_g = 0;
      break;
  }
  f_g *= k_g;

  f_t = f_c + f_n + f_g;
  double p_cn = out0 + (double)f_t * v_f * ctdt;

  return p_cn;
}


DEFAULT_FUNCS_REG(TSearcher)


// end of tsearcher.cpp


