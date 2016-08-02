/***************************************************************************
     tmovingcorr.cpp - Moving correlation analisys definition
                             -------------------
    begin                : 2016.08.02
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

#include <cmath>
#include "miscfun.h"
#include "tmovingcorr.h"


const char* TMovingCorr::helpstr = "<H1>TMovingCorr</H1>\n"
 "<p>Moving correlational analysis for x_in, y_in  <br>\n"
 "</p>\n";

STD_CLASSINFO(TMovingCorr,clpElem );

CTOR(TMovingCorr,TMiso)
{
}

void TMovingCorr::reset_data()
{
  ok = 0;
  corr = cov = a = b = dis_x = dis_y = a = b = corr = cov = 0;
  sigma_x = sigma_y = 0;
  ave_x = ave_y = ave_x2 = ave_y2 = 0;
  s_x = s_x2 = s_y = s_y2 = s_xy = 0;
  x_old = y_old = 0; on_start = true;
  slpt = slp * tw;
  for( auto a : bufs ) {
   a->reset();
  }
}


int TMovingCorr::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  reset_data();
  return 1;
}


int TMovingCorr::do_preRun()
{
  auto imd = size_t( ceil( tw / ctdt ) );
  for( auto a : bufs ) {
   a->resize( imd, 0.0 );
  }
  return 1;
}

int TMovingCorr::do_postRun( int /*good*/ )
{
  for( auto a : bufs ) {
    a->clear();   a->shrink_to_fit();
  }
  return 1;
}

double TMovingCorr::f() noexcept
{
  double x = in_x, y = in_y;
  if( on_start ) {
    x_old = x; y_old = y;
    on_start = false;
  }
  if( diff_x ) {
    x -= x_old; x /= ctdt;
  }
  if( diff_y ) {
    y -= y_old; y /= ctdt;
  }
  x_old = in_x; y_old = in_y; // old, not diff value

  a_x.push_back(    x );
  a_x2.push_back( x*x );
  a_y.push_back(    y );
  a_y2.push_back( y*y );
  a_xy.push_back( x*y );
  calc();
  return a;
}


int TMovingCorr::calc()
{
  a = out0_init.cval(); b = corr = 0; ok = 0;
  auto n = a_x.getN();
  if( n < 1 ) {
    return 0;
  };
  const double dn = (double)(n);
  const double dn2 = dn * dn;

  s_x = a_x.sum();  s_x2 = a_x2.sum();  s_y = a_y.sum(); s_y2 = a_y2.sum(); s_xy = a_xy.sum();

  ave_x  = a_x.aver();  ave_y  = a_y.aver();
  ave_x2 = a_x2.aver(); ave_y2 = a_y2.aver();
  dis_x = ( s_x2 * dn - pow2( s_x ) ) / dn2;
  dis_y = ( s_y2 * dn - pow2( s_y ) ) / dn2;
  sigma_x = sqrt0( dis_x );
  sigma_y = sqrt0( dis_y );

  const double dd = n * s_x2 - s_x * s_x;
  const double t1 = n * s_xy - s_x * s_y;
  cov = t1 / dn2;
  if( dd > 0  &&  ct > slpt ) {
    a = t1 / dd;
    b = ( s_y * s_x2 - s_x * s_xy ) / dd;
    const double dz = ( n*s_x2 - s_x*s_x ) * ( n*s_y2 - s_y*s_y );
    if( dz > 0 ) {
      corr = t1 / sqrt( dz );
      ok = 1;
    }
  }
  return ok;
}


DEFAULT_FUNCS_REG(TMovingCorr)


// end of tmovingcorr.cpp

