/***************************************************************************
                          contout.cpp  - implementation of ContOut,
                          containter for TOutArr
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2019 by atu
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

STD_CLASSINFO(ContOut,clpSpecial);

CTOR(ContOut,LinkedObj)
{
  allowed_types = "TOutArr,+SPECIAL";
  vo.reserve( 64 );
}


int ContOut::do_preRun()
{
  vo.clear();

  // fill array for pure TOutArr entries: for fast access later
  for_type<TOutArr>( [&vo=vo]( auto arr )
    {
      if( arr->getDataD( QSL("type"), 0 ) == TOutArr::OutArrType::outSimple  )
      vo.push_back( arr );
    }
  );

  return 1;
}

int ContOut::do_postRun( int /*good*/ )
{
  vo.clear();
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
    qWarning() << QSL("input ptr[s] is null in ") << getFullName() << WHE;
    return 0;
  }

  TOutArr *in_x = getObjT<TOutArr*>( nm_x );
  if( !in_x ) {
    qWarning() << "not found X array " << nm_x << " in " << getFullName() << WHE;
    return 0;
  }
  const dvector *d_x  = in_x->getArray();

  int nx = in_x->getDataD( QSL("n"), 0 );

  TOutArr *in_y = getObjT<TOutArr*>( nm_y );
  if( !in_y ) {
    qWarning() << "not found output array " << nm_y << " in " << getFullName() << WHE;
    return 0;
  }
  const dvector *d_y  = in_y->getArray();
  int ny = in_y->getDataD( QSL("n"), 0 );
  int n = min( nx, ny );
  if( n < 2 ) {
    qWarning() << "operation required at least 2 elements, given " << n << " in " << getFullName() << WHE;
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

int  ContOut::transLin( const QString &nm_in, const QString &nm_out,
                              double a, double b )
{
  TOutArr *in_in = getObjT<TOutArr*>( nm_in );
  if( !in_in ) {
    qWarning() << "not found input array " << nm_in << " in " << getFullName() << WHE;
    return 0;
  }
  const dvector *d_in  = in_in->getArray();
  int n = in_in->getDataD( QSL("n"), 0 );
  if( n < 1 ) {
    return 0;
  }

  TOutArr *in_out = getArrWithType( nm_out, TOutArr::OutArrType::outSpec );
  if( !in_out ) {
    qWarning() << "not found output array " << nm_out << " in " << getFullName() << WHE;
    return 0;
  }

  in_out->reset();
  in_out->alloc( n );

  for( int i=0; i<n; ++i ) {
    in_out->add( (*d_in)[i] * a + b );
  }

  return n;
}

TOutArr* ContOut::getArrWithType( const QString &nm, int rq_tp )
{
  TOutArr *r = getObjT<TOutArr*>( nm );
  if( !r ) {
    return nullptr;
  }
  if( rq_tp == -1 ) { // any type
    return r;
  }
  int arr_tp = r->getDataD( QSL("type"), 0 );
  if( arr_tp != rq_tp ) {
      return nullptr;
  }
  return r;
}

// ---------------------- Fourier --------------------------

int ContOut::fft( const QString &nm_in, const QString &nm_omega,
                const QString &nm_a, const QString &nm_phi, double ome_max, bool angular_freq )
{
  return fftx( nm_in, nm_omega, nm_a, nm_phi, ome_max, false, angular_freq );
}

int ContOut::fftc( const QString &nm_in, const QString &nm_omega,
                const QString &nm_re, const QString &nm_im, double ome_max, bool angular_freq )
{
  return fftx( nm_in, nm_omega, nm_re, nm_im, ome_max, true, angular_freq );
}


int ContOut::fftx( const QString &nm_in, const QString &nm_omega,
                const QString &nm_a, const QString &nm_phi, double ome_max, bool cmpl, bool angular_freq )
{
  const double freq_scale = angular_freq ? ( 2*M_PI ) : 1.0;
  TOutArr *in = getObjT<TOutArr*>( nm_in );
  if( !in ) {
    qWarning() << "not found input array " << nm_in << " in " << getFullName() << WHE;
    return 0;
  }
  const int n = in->getDataD( QSL("n"), 0 );
  if( n < 16 ) {
    qWarning() << "not enough data for FFT " << n <<  " in " << getFullName() << WHE;
    return 0;
  }

  TModel *model = getAncestorT<TModel>();
  if( !model ) {
    qWarning() << "not found model in" << getFullName() << WHE;
    return 0;
  }
  const double tdt = model->getDataD( QSL("tdt"), 1.0 );
  const double tdt_n = tdt * n;

  // output arrays may not exist, but must be special
  TOutArr *o_omega = getArrWithType( nm_omega, TOutArr::OutArrType::outSpec );
  TOutArr *o_a     = getArrWithType( nm_a,     TOutArr::OutArrType::outSpec );
  TOutArr *o_phi   = getArrWithType( nm_phi,   TOutArr::OutArrType::outSpec );
  vector<TOutArr*> arrs { o_omega, o_a, o_phi };

  if( ome_max <= 0.0 ) {
    ome_max = DMAX;
  }

  dvector v = *(in->getArray()); // copy, as fft may disturb data
  double *pv = v.data();



  const int o_n = 1 + n/2;
  fftw_complex *out = (fftw_complex*) fftw_malloc( (2+o_n) * sizeof( fftw_complex ) ); // TODO: hide in class
  fftw_plan plan = fftw_plan_dft_r2c_1d( n, pv, out, FFTW_ESTIMATE );

  fftw_execute( plan );
  fftw_destroy_plan( plan );

  int i_m = (int)( n * tdt * ome_max / ( freq_scale ) );
  if( i_m > o_n ) {
    i_m = o_n;
  }

  for( auto arr : arrs ) {
    if( arr ) {
      arr->reset();
      arr->alloc( i_m );
    }
  }


  double scl = 2.0 / double(n); // common scale
  for( int i=0; i<i_m ; ++i ) {
    double ome = freq_scale*i / tdt_n;
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

