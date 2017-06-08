/***************************************************************************
  tqsearcher.cpp - q-based searcher element declaration
                             -------------------
    begin                : 2016.12.30
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
  f_c = 0; f_n = 0; f_e = 0;
  double p_ct0 = out0 - out0_init;
  pr_l = p_l - out0; pr_r = p_r - out0;
  qr_l = q_l - q_o;  qr_c = q_c - q_o; qr_r = q_r - q_o; // not full: need /qr_c
  p_e = out0; p_e0 = out0; pr_e = 0.0; F_c = 0.0; S_e = 0.0;
  brIdx = 0;
  double sure_coeff = 0.0;

  F_c = qF_fun( qr_c, q_gamma, F_type, limitF );

  double pr_e0 = 0.0, pr_b = 1e7; // pr_b - active bound relative
  do { // calculate p_e, S_e with local exit via break
    if( F_c > 0.999999 ) { // precise
      S_e = 1.0; sure_coeff = 1.0; brIdx = 1;
      break;
    }
    qr_l /= qr_c; qr_r /= qr_c; qr_c = 1; // final relative q calculation, qr_c !=0 as F_c < 1
    if( pr_l > -1e-12 || pr_r < 1e-12 ) { // BEWARE: dimension!
      brIdx = 2;
      break;
    }

    // relative coordinates of crosses
    double pr_el = pr_l / ( 1.0 - qr_l ); // zero?
    double pr_er = pr_r / ( 1.0 - qr_r );

    if( qr_l > 0 && qr_r > 0 ) {  // ----------- one side from q_o ----------------
      if( qr_l > 1.0 ) {
        if( qr_r > 1.0 ) {    // :.: central point is better, but not good
          pr_e0 = 0.1 * pr_el + 0.1 * pr_er;
          pr_b = 0.5 * ( pr_l + pr_l );
          sure_coeff = 0.2;
          brIdx = 3;
        } else {            // ::.
          pr_e0 = pr_er; pr_b = pr_r; sure_coeff = 0.7;
          brIdx = 4;
        }
      } else {
        if( qr_r > 1 ) {    // .::
          pr_e0 = pr_el;  pr_b = pr_l ; sure_coeff = 0.7;
          brIdx = 5;
        } else {            // .:.
          if(  qr_l < qr_r ) {
            pr_e0 = pr_l; pr_b = pr_l;
          } else {
            pr_e0 = pr_r; pr_b = pr_r;
          }
          brIdx = 6;
          sure_coeff = 0.1;
        }
      }
      break;
    }

    if( qr_l < 0 && qr_r < 0 ) { // ------------- double cross ---------------
      pr_e0 = 0.1 * pr_el + 0.1 * pr_er;
      pr_b = 0.5 * ( pr_l + pr_l );
      // mix?
      sure_coeff = 0.5;
      brIdx = 10;
      break;
    }

    if( qr_l < 0  ) { // 2L
      pr_e0 = pr_el;  pr_b = pr_l; sure_coeff = 1.0;
      brIdx = 100;
    } else {          // 2R
      pr_e0 = pr_er;  pr_b = pr_r; sure_coeff = 1.0;
      brIdx = 101;
    }
    break;
  } while( false );

  pr_e = pr_e0;
  if( limitPe ) {
    pr_e = vBound( pr_l.cval(), pr_e0, pr_r.cval() );
  }

  p_e0 = pr_e + out0;
  p_e  = pr_e + out0;
  S_e  = sure_coeff * exp( -pow2( pr_e0 / pr_b ) );
  FS_e = F_c * S_e;

  switch( (FeType)(int)(fe_type) ) {
    case fe_lin:
       f_e = S_e * k_e * ( p_e - out0 ); break;
    case fe_sign:
       f_e = S_e * k_e * sign( p_e - out0 ); break;
    case fe_lim:
       f_e = S_e * k_e * limitLine( p_e - out0, ( p_max - p_min ) * lin_rlim ); break;
    default:
       f_e = 0;
  }

  f_c = - k_cl  * ( p_ct0 )
        - k_ch  * barrierHypUp(    out0, p_max )
        + k_ch  * barrierHypDown(  out0, p_min );
  f_n =   k_nl  * ( p_r - 2*out0 + p_l )
        - k_nh  * barrierHypUp(    out0, p_r )
        + k_nh  * barrierHypDown(  out0, p_l );


  f_t = f_c + f_n + f_e;
  double p_cn = out0 + (double)f_t * v_f * ctdt;
  if( limitP ) {
    p_cn = vBound( p_min.cval(), p_cn, p_max.cval() );
  }

  return p_cn;
}


DEFAULT_FUNCS_REG(TQSearcher)


// end of tqsearcher.cpp


