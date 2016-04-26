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

int TSearcher::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  // out0 = p_c = (double)out0_init;
  return 1;
}


double TSearcher::f() noexcept
{
  // QuadExtrIn in { p_l, p_ce, p_r, F_l, F_ce, F_r, lim_s, p_min, p_max, limitX, limitG };
  // QuadExtrOut out;
  // // fallback values
  // a_1 = 0; a_2 = 0; p_cnt = 0; p_cn = p_ce;  F_cn = F_ce; dy = 0; dF_sx = 0; f_c = 0;
  //
  // if( calcQuadExtr( in, out ) ) {
  //   p_cn = out.p_e; p_cnt = out.p_et; F_cn = out.F_e;
  //   p_lt = out.p_lt; p_rt = out.p_rt; F_lt = out.p_lt; F_rt = out.F_rt;
  //   a_1 = out.a_1; a_2 = out.a_2;
  //   dy = F_cn - F_c;
  //   dF_dx = dy / p_cnt;
  //   dF_sx = dy * sign( p_cnt );
  //   f_c = 1.0 / pow2( p_lt ) - 1.0 / pow2( p_rt ); // TODO: remove, not here
  // }
  //
  // return p_cn;
  return 0;
}


DEFAULT_FUNCS_REG(TSearcher)


// end of tsearcher.cpp


