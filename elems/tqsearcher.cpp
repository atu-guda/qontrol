/***************************************************************************
  tqsearcher.cpp - q-based searcher element declaration
                             -------------------
    begin                : 2016.12.30
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

#include "tqsearcher.h"
#include "miscfun.h"

const char* TQSearcher::helpstr = "<H1>TQSearcher</H1>\n"
 "Realizes some Q-based tactics in search "
 "in non-linear dynamic system identification task.<br/>"
 "Main output: p_e - new central point position.";

STD_CLASSINFO(TQSearcher,clpElem|clpCalcAtStart);

CTOR(TQSearcher,TMiso)
{
}

// out0 is p_c;

int TQSearcher::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  return 1;
}


double TQSearcher::f() noexcept
{
  // fallback values
  p_e = out0;
  f_c = 0; f_n = 0; f_e = 0;
  double p_ct0 = out0 - out0_init;

  double qr_c = q_o - q_c;
  double q_r = qr_c / q_gamma;
  double aq_r = fabs( q_r );
  double q_r2 = q_r * q_r;

  switch( (int)F_type ) {
    case qa_Gauss  : F_c = exp( - q_r2 ); break;
    case qa_Para   : F_c = 1.0 - q_r2 * oneMinusEm1; break;
    case qa_Lin    : F_c = 1.0 - aq_r * oneMinusEm1; break;
    case qa_Hyper  : F_c = 1.0 / ( 1.0 + aq_r * eMinus1 ); break;
    case qa_Log    : F_c = 1.0 - log( 1.0 + aq_r ) * logQaScale; break;
    default        : F_c = 0.0; break;
  };

  f_c = - k_cl  * ( p_ct0 )
        - k_ch  * barrierHypUp(    out0, p_max )
        + k_ch  * barrierHypDown(  out0, p_min );
  f_n =   k_nl  * ( p_r - 2*out0 + p_l )
        - k_nh  * barrierHypUp(    out0, p_r )
        + k_nh  * barrierHypDown(  out0, p_l );

  f_e *= k_e;

  f_t = f_c + f_n + f_e;
  double p_cn = out0 + (double)f_t * v_f * ctdt;

  return p_cn;
}


DEFAULT_FUNCS_REG(TQSearcher)


// end of tqsearcher.cpp


