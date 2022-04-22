/***************************************************************************
  tqsearcher.cpp - q-based searcher element declaration
                             -------------------
    begin                : 2016.12.30
    copyright            : (C) 2016-2022 by atu
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

#include <algorithm>

#include "tqsearcher.h"
#include "miscfun.h"

using namespace std;

const char* TQSearcher::helpstr = "<h1>TQSearcher</h1>\n"
 "<p>Realizes some q and F based tactics in search "
 "in non-linear dynamic system identification task.<br/>"
 "Main output: p_e - new central point position.</p>\n"
 "<p> brIdx: 0: unknown, 1: precise, "
 "2: too close w/o precision, "
 "3: center better, not good :.: ,"
 "4: right better ::. ,"
 "5: left better  .:: ,"
 "6: center bad  .:. ,"
 "10: double cross,"
 "100: left,"
 "101: right cross."
 "</p>\n";

STD_CLASSINFO(TQSearcher,clpElem|clpCalcAtStart);

CTOR(TQSearcher,TMiso)
{
}

// out0 is p_c;

int TQSearcher::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  // TODO: more init
  return 1;
}


double TQSearcher::f() noexcept
{
  // fallback values
  f_c = 0; f_n = 0; f_e = 0;
  p_e = out0; p_e0 = out0; pr_e = 0.0; S_e = 0.0; S_e3 = 0.0;
  W = 0.0; FS_e = 0.0;
  brIdx = 0;

  sure_coeff = 0.0; dist_coeff = 1000.0;  pr_b = 0.5 * ( p_r - p_l ); k_l = 1.0;
  pr_l = p_l - out0; pr_r = p_r - out0;
  qr_l = q_l - q_o;  qr_c = q_c - q_o; qr_r = q_r - q_o; // not full: need /qr_c

  F_c = qF_fun( qr_c, q_gamma, F_type, limitF );
  F_l = qF_fun( qr_l, q_gamma, F_type, limitF );
  F_r = qF_fun( qr_r, q_gamma, F_type, limitF );

  if( fabs( qr_c ) > 1e-20 ) {
    qr_l /= qr_c; qr_r /= qr_c; qr_c = 1; // final relative q calculation, qr_c !=0 as F_c < 1
  } else {
    qr_l = -1000; qr_r = 1000; qr_c = 1; // fake values
  }

  /// if( pr_l > -1e-12 || pr_r < 1e-12 ) { // Too near

  pr_e0 = 0.0;

  switch( (PeType)(int)(pe_type) ) {
    case pe_q3p:    calc_pe_q3p();   break;
    case pe_Fquad:  calc_pe_Fquad(); break;
    case pe_FCog:   calc_pe_FCog();  break;
    case pe_qquad:  calc_pe_qquad(); break;
    default: break;
  }

  if( limitPe ) {
    pr_e = clamp( pr_e0.cval(), pr_l.cval() * lPeScale, pr_r.cval() * lPeScale );
  } else {
    pr_e = pr_e0;
  }

  p_e0 = pr_e0 + out0;
  p_e  = pr_e  + out0;
  double pr_e0_eff = pr_e0;
  double dp_min = dist_coeff * min3( fabs( pr_e0 - pr_l ), fabs( pr_e0 ), fabs( pr_e0 - pr_r )  );
  if( use_k_l ) {
    pr_e0_eff *= k_l;
    dp_min    *= k_l;
  }
  S_e   = sure_coeff * exp( -pow2( pr_e0_eff / pr_b ) );
  S_e3  = sure_coeff * exp( -pow2(    dp_min / pr_b ) );
  FS_e0 = F_c * S_e;

  switch( (FSOutType)(int)(FS_type) ) { // really W
    case fso_FcSe:
      W = FS_e0; break;
    case fso_FcSe3:
      W = F_c * S_e3; break;
    case fso_Se3:
      W = S_e3; break;
    case fso_Fc:
      W = F_c; break;
    default:
      W = 0;
  }
  FS_e = W; // alias

  // scale factor for f_e
  double fef_val = 0.0;
  switch( (FeFactorType)(int)(fef_type) ) {
    case fef_Se:  fef_val = S_e;   break;
    case fef_one: fef_val = 1.0;   break;
    case fef_F:   fef_val = F_c;   break;
    case fef_Se3: fef_val = S_e3;  break;
    case fef_W:   fef_val = W;     break;
    default: break;
  }

  // f_e( p_e, ... )
  switch( (FeType)(int)(fe_type) ) {
    case fe_lin:
       f_e = pr_e; break;
    case fe_sign:
       f_e = sign( pr_e ); break;
    case fe_lim:
       f_e = limitLine( pr_e, ( p_max - p_min ) * lin_rlim ); break;
    default:
       f_e = 0;
  }
  f_e *= fef_val * k_e;

  // TODO: f_c types
  double p_ct0 = out0 - out0_init;
  f_c = - k_cl  * ( p_ct0 )
        - k_ch  * barrierHypUp(    out0, p_max )
        + k_ch  * barrierHypDown(  out0, p_min );

  // f_n calculation
  switch( (FnType)(int)(fn_type) ) {
    case fn_lin:
       f_n = ( p_r - 2*out0 + p_l ); break;
    case fn_log:
       f_n = (  log( ( p_r - out0 ) / nl_d ) - log( ( out0 - p_l ) / nl_d ) ); break;
    default:
       f_n = 0;
  }
  f_n *= k_nl;
  f_n += k_nh  * ( barrierHypDown( out0, p_l ) -  barrierHypUp( out0, p_r ) );


  f_t = f_c + f_n + f_e;
  double p_cn = out0 + (double)f_t * v_f * ctdt;
  if( limitP ) {
    p_cn = clamp( p_cn, p_min.cval(), p_max.cval() );
  }

  return p_cn;
}

void TQSearcher::calc_pe_q3p()
{
  do { // calculate p_e, sure_coeff with local exit via break
    if( F_c > 0.9999999 ) { // precise
      dist_coeff = 1.0; sure_coeff = 1.0; brIdx = 1;
      break;
    }
    if( pr_l > -1e-12 || pr_r < 1e-12 ) { // BEWARE: dimension! TODO: eps
      brIdx = 2; k_l = 10;
      break;
    }

    double qr_cl = ( qr_l * pr_r - qr_r * pr_l ) / ( pr_r - pr_l ); // use pr_b?
    double eqrcl = fabs( qr_cl - qr_c );
    double adq = fabs( qr_r - qr_l );
    if( adq > 1e-12 ) {
      k_l = 1.0 + eqrcl / adq;
    } else {
      k_l = 100;
    }
    k_l = clamp( k_l.cval(), 1.0, 100.0 );

    // relative coordinates of crosses
    double pr_el = 100.0 * pr_l; // fallback
    if( fabs( 1.0 - qr_l ) > 1e-12 ) {
      pr_el = pr_l / ( 1.0 - qr_l );
    }
    double pr_er = 100.0 * pr_r;
    if( fabs( 1.0 - qr_r ) > 1e-12 ) {
      pr_er = pr_r / ( 1.0 - qr_r );
    }

    if( qr_l > 0 && qr_r > 0 ) {  // ----------- one side from q_o ----------------
      if( qr_l > 1.0 ) {
        if( qr_r > 1.0 ) {    // :.: central point is better, but not good
          pr_e0 = 0.1 * pr_el + 0.1 * pr_er;
          sure_coeff = 0.5; dist_coeff = 2.0 * c_pen;
          brIdx = 3;
        } else {            // ::.
          pr_e0 = pr_er; pr_b = pr_r;   sure_coeff = 0.9; dist_coeff = c_pen;
          brIdx = 4;
        }
      } else {
        if( qr_r > 1 ) {    // .::
          pr_e0 = pr_el;  pr_b = -pr_l; sure_coeff = 0.9; dist_coeff = c_pen;
          brIdx = 5;
        } else {            // .:.
          if(  qr_l < qr_r ) {
            pr_e0 = pr_l; pr_b = -pr_l;
          } else {
            pr_e0 = pr_r; pr_b = pr_r;
          }
          brIdx = 6;
          sure_coeff = 0.2; dist_coeff = 2.0 * c_pen;
        }
      }
      break;
    }

    if( qr_l < 0 && qr_r < 0 ) { // ------------- double cross ---------------
      pr_e0 = 0.1 * pr_el + 0.1 * pr_er;
      // mix?
      sure_coeff = 0.5; dist_coeff = 0.5 * c_pen;
      brIdx = 10;
      break;
    }

    // near to best: one cross inside
    if( qr_l == 0.0 ) { // sic: precise 0 handling
      qr_l = - qr_r * 1e-10;
    }
    if( qr_r == 0.0 ) {
      qr_r = - qr_l * 1e-10;
    }
    if( qr_l < 0  ) { // 2L
      pr_e0 = pr_el;  pr_b = -pr_l; sure_coeff = 1.0; dist_coeff = 0.5;
      brIdx = 100;
    } else {          // 2R
      pr_e0 = pr_er;  pr_b =  pr_r; sure_coeff = 1.0; dist_coeff = 0.5;
      brIdx = 101;
    }
    break;
  } while( false ); // p_e, sure_coeff calculate

}

void TQSearcher::calc_pe_Fquad()
{
  //               x_l, x_c,  x_r, y_l, y_c, y_r, lim_s, xmin, xmax, limitX, limitG=given[xmin,xmax]
  QuadExtrIn in { pr_l, 0.0, pr_r, F_l, F_c, F_r,  3.00,    0,    0,   true, false };
  QuadExtrOut out;

  if( calcQuadExtr( in, out ) ) {
    pr_e0 = out.x_e;
    sure_coeff = 1.0; dist_coeff = 1.0; // TODO: real values
    if( out.was_limited ) {
      sure_coeff = 0.2; dist_coeff = 10.0;
    }
  }

}

void TQSearcher::calc_pe_FCog()
{
  double den = F_r + F_c + F_l;
  if( fabs( den ) > 1e-12 ) {
    pr_e0 = ( pr_l * F_l /* + pr_c * F_c =0 +*/ + pr_r * F_r ) / den ;
    sure_coeff = 1.0; dist_coeff = 2.0; // TODO: real values
  } else {
    sure_coeff = 0.1; dist_coeff = 50.0;
  }
}

void TQSearcher::calc_pe_qquad()
{
  // TODO: implement
}


DEFAULT_FUNCS_REG(TQSearcher)


// end of tqsearcher.cpp


