/***************************************************************************
                          tfourier.cpp  -  description
                             -------------------
    begin                : Sat Sep 9 2000
    copyright            : (C) 2000-2014 by atu
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
#include "tmodel.h"
#include "toutarr.h"
#include "tfourier.h"

const char* TFourier::helpstr = "<H1>TFourier</H1>\n"
 "Perform Fourier analisys on input: <br>\n"
 "Parameters see in object info.";

STD_CLASSINFO(TFourier,clpElem);

CTOR(TFourier,TMiso)
{
  tp = 2 * M_PI / omega;
}


int TFourier::do_preRun( int /*run_tp*/, int /*an*/,
                         int /*anx*/, int /*any*/, double /*adt*/ )
{
  aa.resize( ng+2 );  bb.resize( ng+2 );  am.resize( ng+2 );
  return 1;
}

int TFourier::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  for( int i=0; i<=ng; i++ )
    aa[i] = bb[i] = am[i] = 0;
  s_x2 = a0 = a1 = b1 = ampl = ampl1 = phi = qpow = qpow1 = 0;
  ii = 0; n_st = 0; n_en = model_nn;
  initVars();
  return 1;
}

int TFourier::do_endLoop()
{
  return 1;
}

double TFourier::f( double t )
{
  int i;
  double av;
  if( ii < n_st || ii > n_en ) { ii++; return ampl1; };
  double xx = in_u;
  for( i=0; i<=ng; i++ ) {
    aa[i] += xx * cos( i * omega * t );
    bb[i] += xx * sin( i * omega * t );
  };
  s_x2 += xx * xx;
  if( ii == n_en ) {
    aa[0] /= nn; bb[0] = 0; am[0] = fabs( aa[0] ); ampl = am[0] * am[0];
    for( i=1; i<=ng; i++ ) {
      aa[i] /= nn / 2;
      bb[i] /= nn / 2;
      av = aa[i] * aa[i] + bb[i] * bb[i];
      am[i] = sqrt( av );
      ampl += av;
    };
    s_x2 /= nn; ampl = sqrt( ampl );
    a0 = aa[0]; a1 = aa[1];  b1 = bb[1]; ampl1 = am[1];
    phi = atan2( a1, b1 );
    if( s_x2 > 0 ) {
      qpow = ampl * ampl / s_x2;
      qpow1 = ampl1 * ampl1 / s_x2;
    } else {
      qpow = qpow1 = 1; // no source - quality max
    };
  };
  ii++;
  return ampl1;
}

void TFourier::initVars(void)
{
  double t_st, t_en, tt, dt;
  tp = 2 * M_PI / omega;
  np = int( tp / tdt );
  tt = 1; // safe value
  par->getData( "T", &tt );
  t_st = 0; t_en = tt;
  if( t0 > 0 && t0 < tt-tp )
    t_st = t0;
  if( t1 > tp && t1 < tt )
    t_en = t1;
  dt = t_en - t_st; nper = int( dt / tp );
  n_st = int( t_st / tdt );
  if( useOptim ) {
    dt = nper * tp;
  };
  nn = int( 0.5 + dt / tdt );
  n_en = n_st + nn;
  if( n_en >= model_nn ) n_en = model_nn-1;
}

DEFAULT_FUNCS_REG(TFourier)


// end of tfourier.cpp

