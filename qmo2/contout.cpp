/***************************************************************************
                          contout.cpp  - inplementation of ContOut,
                          containter for TOutArr
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2015 by atu
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
#include <fftw3.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

#include "toutarr.h"
#include "tmodel.h"
#include "contout.h"

using namespace std;


const char* ContOut::helpstr = "<H1>ContOut</H1>\n"
 "Container of output arrays";

STD_CLASSINFO(ContOut,clpSpecial | clpContainer);

CTOR(ContOut,TDataContainer)
{
  allowed_types = "TOutArr,+SPECIAL";
  vo.reserve( 64 );
}

ContOut::~ContOut()
{
}

int ContOut::preRun( int run_tp, int an, int anx, int any, double adt )
{
  vo.clear();

  // fill array for pure TOutArr entries: for fast access later
  for( auto o : children() ) {
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) { // unlikely
      continue;
    }
    vo.push_back( arr );
    if( ! arr->preRun( run_tp, an, anx, any, adt ) ) {
      vo.clear();
      return 0;
    }
  }
  return 1;
}

int ContOut::postRun( int good )
{
  for( auto arr : vo ) {
    arr->postRun( good );
  }
  vo.clear();
  return 1;
}

int ContOut::startLoop( int acnx, int acny )
{
  for( auto arr : vo ) {
    if( ! arr->startLoop( acnx, acny ) ) {
      return 0;
    }
  }

  return 1;
}

int ContOut::endLoop( int acnx, int acny )
{
  for( auto arr : vo ) {
    arr->endLoop( acnx, acny );
  }

  return 1;
}


void ContOut::takeAllVals()
{
  for( auto arr : vo ) {
    arr->take_val();
  }
}

int ContOut::prep_2in( const QString &nm_x, const QString &nm_y,
                 const dvector **p_d_x, const dvector **p_d_y ) const 
{
  if( !p_d_x && !p_d_y ) {
    return 0;
  }

  TOutArr *in_x = getElemT<TOutArr*>( nm_x );
  if( !in_x ) {
    return 0;
  }
  const dvector *d_x  = in_x->getArray();
  if( ! d_x ) {
    return 0;
  }

  int nx = in_x->getDataD( "n", 0 );

  TOutArr *in_y = getElemT<TOutArr*>( nm_y );
  if( !in_y ) {
    return 0;
  }
  const dvector *d_y  = in_y->getArray();
  if( ! d_y ) {
    return 0;
  }
  int ny = in_y->getDataD( "n", 0 );
  int n = min( nx, ny );
  if( n < 2 ) {
    return 0;
  }
  *p_d_x = d_x;
  *p_d_y = d_y;
  return n;
}

TripleF ContOut::regre( const QString &nm_x, const QString &nm_y, bool need_corr ) const
{
  TripleF r {0.0, 0.0, 0.0};
  const dvector *d_x = nullptr, *d_y = nullptr;

  int n = prep_2in( nm_x, nm_y, &d_x, &d_y );

  if( n < 2 ) {
    return r;
  }
  double cov00, cov01, cov11, resid;

  gsl_fit_linear( d_x->data(), 1, d_y->data(), 1, n, &r.b, &r.a,
      &cov00, &cov01, &cov11, &resid );

  if( need_corr ) {
    r.c =  gsl_stats_correlation( d_x->data(), 1, d_y->data(), 1, n );
  }

  return r;
}

double  ContOut::corr( const QString &nm_x, const QString &nm_y ) const
{
  const dvector *d_x = nullptr, *d_y = nullptr;

  int n = prep_2in( nm_x, nm_y, &d_x, &d_y );

  if( n < 2 ) {
    return 0;
  }
  return gsl_stats_correlation( d_x->data(), 1, d_y->data(), 1, n );
}

double  ContOut::covar( const QString &nm_x, const QString &nm_y ) const
{
  const dvector *d_x = nullptr, *d_y = nullptr;

  int n = prep_2in( nm_x, nm_y, &d_x, &d_y );

  if( n < 2 ) {
    return 0;
  }
  return gsl_stats_covariance( d_x->data(), 1, d_y->data(), 1, n );
}

int ContOut::fft( const QString &nm_in, const QString &nm_omega,
                const QString &nm_a, const QString &nm_phi, double ome_max )
{
  return fftx( nm_in, nm_omega, nm_a, nm_phi, ome_max, false );
}

int ContOut::fftc( const QString &nm_in, const QString &nm_omega,
                const QString &nm_re, const QString &nm_im, double ome_max )
{
  return fftx( nm_in, nm_omega, nm_re, nm_im, ome_max, true );
}


int ContOut::fftx( const QString &nm_in, const QString &nm_omega,
                const QString &nm_a, const QString &nm_phi, double ome_max, bool cmpl )
{
  int n = 0;
  TOutArr *in = getElemT<TOutArr*>( nm_in );
  if( !in ) {
    return 0;
  }
  n = in->getDataD( "n", 0 );
  if( n < 2 ) {
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    DBGx( "warn: not found model in \"%s\"", qP(getFullName()) );
    return 0;
  }
  double tdt = model->getDataD( "tdt", 1.0 );

  // output arrays may not exist, but must be special
  TOutArr *o_omega = getElemT<TOutArr*>( nm_omega );
  if( o_omega ) {
    int tp = o_omega->getDataD( "type", 0 );
    if( tp != TOutArr::OutArrType::outSpec ) {
      o_omega = nullptr;
    }
  }

  TOutArr *o_a = getElemT<TOutArr*>( nm_a );
  if( o_a ) {
    int tp = o_a->getDataD( "type", 0 );
    if( tp != TOutArr::OutArrType::outSpec ) {
      o_a = nullptr;
    }
  }

  TOutArr *o_phi = getElemT<TOutArr*>( nm_phi );
  if( o_phi ) {
    int tp = o_phi->getDataD( "type", 0 );
    if( tp != TOutArr::OutArrType::outSpec ) {
      o_phi = nullptr;
    }
  }

  if( ome_max <= 0.0 ) {
    ome_max = DMAX;
  }

  dvector v = *(in->getArray()); // copy, as fft may disturn data
  double *pv = v.data();



  int o_n = 1 + n/2;
  fftw_complex *out = (fftw_complex*) fftw_malloc( (2+o_n) * sizeof( fftw_complex ) );
  fftw_plan plan = fftw_plan_dft_r2c_1d( n, pv, out, FFTW_ESTIMATE );

  fftw_execute( plan );
  fftw_destroy_plan( plan );

  int i_m = (int)( n * tdt * ome_max / ( 2*M_PI ) );
  if( i_m > o_n ) {
    i_m = o_n;
  }

  if( o_omega ) {
    o_omega->reset();
    o_omega->alloc( i_m );
  }

  if( o_a ) {
    o_a->reset();
    o_a->alloc( i_m );
  }

  if( o_phi ) {
    o_phi->reset();
    o_phi->alloc( i_m );
  }

  double scl = 2.0 / double(n); // common scale
  for( int i=0; i<i_m ; ++i ) {
    double ome = 2*M_PI*i/(tdt*n);
    if( o_omega ) {
      o_omega->add( ome );
    }
    if( o_a ) {
      if( cmpl ) {
        o_a->add( scl * out[i][0] );
      } else {
        o_a->add( scl * hypot( out[i][0], out[i][1] ) );
      }
    }
    if( o_phi ) {
      if( cmpl ) {
        o_phi->add( scl * out[i][1] );
      } else {
        o_phi->add( atan2( out[i][1], out[i][0] ) );
      }
    }
  }

  fftw_free( out ); out = nullptr;
  return i_m;
}


DEFAULT_FUNCS_REG(ContOut)

// end of contout.cpp

