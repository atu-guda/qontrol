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
  f_c = 0; f_n = 0; f_e = 0;
  double p_ct0 = out0 - out0_init;
  pr_l = p_l - out0; pr_r = p_r - out0;
  qr_l = q_l - q_o;  qr_c = q_c - q_o; qr_r = q_r - q_o; // not full: need /
  p_e = out0; pr_e = 0.0; F_c = 0.0; S_e = 0.0;

  double q_rel_a = fabs( qr_c / q_gamma );
  double q_rel2 = q_rel_a * q_rel_a;

  switch( (int)F_type ) {
    case qa_Gauss  : F_c = exp( - q_rel2 ); break;
    case qa_Para   : F_c = 1.0 - q_rel2 * oneMinusEm1; break;
    case qa_Lin    : F_c = 1.0 - q_rel_a * oneMinusEm1; break;
    case qa_Hyper  : F_c = 1.0 / ( 1.0 + q_rel_a * eMinus1 ); break;
    case qa_Log    : F_c = 1.0 - log( 1.0 + q_rel_a ) * logQaScale; break;
    default        : F_c = 0.0; break;
  };
  if( limitF ) {
    F_c = vBound( 0.0, F_c, 1.0 );
  }

  do { // calclucate p_e, S_e with local exit via break
    if( F_c > 0.999999 ) { // precise
      S_e = 1.0;
      break;
    }
    qr_l /= qr_c; qr_r /= qr_c;
    if( pr_l > -1e-12 || pr_r < 1e-12 ) { // BEWARE: dimension!
      break;
    }

    double pr_el = pr_l / ( 1.0 - qr_l ); // zero?
    double pr_er = pr_r / ( 1.0 - qr_r );

    if( qr_l > 0 && qr_r > 0 ) {  // ----------- one side from q_o ----------------
      if( qr_l > 1.0 ) {
        if( qr_r > 1.0 ) {    // :.: central point is better, but not good
          pr_e = 0.1 * pr_l + 0.1 * pr_e; S_e = exp( -( pr_er*pr_er ) / (pr_r*pr_l) ); // C_cg
          S_e = 0.7;
        } else {            // ::.
          pr_e = 0.99 * pr_r; S_e = exp( -( pr_er*pr_er ) / (pr_r*pr_r) ); // Cr
        }
      } else {
        if( qr_r > 1 ) {    // .::
          pr_e = 0.99 * pr_l;  S_e = exp( -( pr_el*pr_el ) / (pr_l*pr_l) ); // Cl
        } else {            // .:.
          double pr_x = ( qr_l < qr_r ) ? pr_l : pr_r; // C_cb
          S_e = exp( -( pr_el*pr_er ) / (pr_l*pr_r) );
          pr_e = 0.99 * pr_x;
        }
      }
      break;
    }

    if( qr_l < 0 && qr_r < 0 ) { // ------------- double cross ---------------
      // mix?
      S_e = 0.5;
      break;
    }

    if( qr_l < 0  ) { // 2L
      pr_e = pr_l / ( 1 - qr_l );  S_e = exp( -( pr_e*pr_e ) / (pr_l*pr_l) );
    } else {          // 2R
      pr_e = pr_r / ( 1 - qr_r );  S_e = exp( -( pr_e*pr_e ) / (pr_r*pr_r) );
    }
    break;
  } while( false );
  p_e = pr_e + out0;
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

  return p_cn;
}


DEFAULT_FUNCS_REG(TQSearcher)


// end of tqsearcher.cpp


