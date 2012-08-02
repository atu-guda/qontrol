/***************************************************************************
                          tsource.cpp  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <time.h>
#include <math.h> 
#include "miscfun.h" 
#include "tmodel.h"
#include "tsource.h"

const char* TSource::helpstr = "<H1>TSource</H1>\n"
 "Source of different kind of signals: <br>\n"
 "Have N parameters: <b>type, U, omega, .. b</b>,"
 "each can be changed at any time ??.";

TClassInfo TSource::class_info = {
  CLASS_ID_TSource, "TSource", TSource::create,
  &TMiso::class_info, helpstr, clpElem };


TSource::TSource( TDataSet* aparent )
        :TMiso( aparent )
{
  type = use_u_ch = use_f_ch = 0;
  uu = 1; omega = 0.7; cc = 0;
  u_ch_v0 = 0.5; u_ch_vm = 1.5; u_ch_t0 = 2; u_ch_tm = 10;
  f_ch_v0 = -0.2; f_ch_vm = 0.2; f_ch_t0 = 2; f_ch_tm = 10;
  was_pulse = 0;
  u_ch_vs = u_ch_ve = u_ch_ts = u_ch_te = u_ch_k 
          = f_ch_vs = f_ch_ve = f_ch_ts = f_ch_te = f_ch_k = 1; 
  seed_u = 17228 + (time(0) & 0x3AF7F );
  seedType_u = 3; addBaseSeed_u = 1;
  rng_u = 0;
  seed_p = seed_p + (time(0) & 0xFFF7 );
  seedType_p = 3; addBaseSeed_p = 1;
  rng_p = 0;
}

TSource::~TSource()
{
  if( rng_u ) {
    gsl_rng_free( rng_u );
    rng_u = 0;
  }
  if( rng_p ) {
    gsl_rng_free( rng_p );
    rng_p = 0;
  }
}

TDataSet* TSource::create( TDataSet* apar )
{
  return new TSource( apar );
}

const TClassInfo* TSource::getClassInfo(void) const
{
  return &class_info;
}

const char *TSource::getHelp(void) const
{
  return helpstr;
}

double TSource::f( double t )
{
  double v ,omet, uu_s, omet_s, lt, u_ch, f_ch, phi, phi_0;
  int n;
  omet = t * omega; u_ch = 1; f_ch = 0;
  if( use_u_ch ) {
    if( t < u_ch_te ) {
      u_ch = u_ch_vs + ( t - u_ch_ts ) * u_ch_k;
    } else {
      u_ch = u_ch_vs = u_ch_ve;
      u_ch_ve = gsl_ran_flat( rng_u,  u_ch_v0, u_ch_vm );
      u_ch_ts = t;
      u_ch_te = t + gsl_ran_flat( rng_u, u_ch_t0, u_ch_tm );
      u_ch_k = ( u_ch_ve - u_ch_vs ) / ( u_ch_te - u_ch_ts );
    };
  }; // end if( u_chaos )
  if( use_f_ch ) {
    if( t < f_ch_te ) {
      f_ch = f_ch_vs + ( t - f_ch_ts ) * f_ch_k;
    } else {
      f_ch = f_ch_vs = f_ch_ve;
      f_ch_ve = gsl_ran_flat( rng_p, f_ch_v0, f_ch_vm );
      f_ch_ts = t;
      f_ch_te = t + gsl_ran_flat( rng_p, f_ch_t0, f_ch_tm );
      f_ch_k = ( f_ch_ve - f_ch_vs ) / ( f_ch_te - f_ch_ts );
    };
  }; // end if( f_chaos )
  uu_s = uu * u_ch; omet_s = omet + f_ch; // note u: * fi +
  phi = omet_s * M_1_PI * 0.5;
  phi_0 = fmod( phi, 1 ); // phase in range [ 0; 1 )
  switch( type ) {
    case 0: v = uu_s * sin( omet_s ); break;
    case 1: v = uu_s * sign( sin( omet_s ) ); break;
    case 2: v = uu_s * sin( omet_s ) * t / tt; break;
    case 3: v = uu_s * sign( sin( omet_s ) ) * t / tt; break;
    case 4: v = 0;
	    if( was_pulse == 0 && t >= omega ) 
	      { v = uu / tdt; was_pulse = 1; };  
	    break;
    case 5: v = ( t > omega ) ? uu_s : 0; break;
    case 6: v = uu_s * ( t + f_ch ) / tt; break;
    case 7: n = int( t / omega ); lt = t - n * omega;
	    v = uu_s * ( lt + f_ch ) / omega;
	    break;
    case 8: n = int( 2 * t / omega ); lt = 2 * t - n * omega;
	    if( n & 1 ) {
	      v = 0.5 * uu - 2 * ( lt + f_ch ) * uu / omega;
	    } else {
	      v = -0.5 * uu + 2 * ( lt + f_ch ) * uu / omega;
	    };
	    break;
    case 9: v = f_ch; break; 	
    case 10: if( phi_0 <= 0.25 ) 
	       v = uu_s * 4 * phi_0;
	     else if ( phi_0 <= 0.75 )
	       v = uu_s * ( 1 - 4*(phi_0-0.25));
	     else
	       v = uu_s * ( -1 +4*(phi_0-0.75));
	     break; 	
    case 11: v = phi_0; break; 	
    default: v = 0;
  };
  v += cc;
  return v; 
}

int TSource::do_preRun( int /*run_tp*/, int /*an*/, 
                     int /*anx*/, int /*any*/, double /*adt*/ )
{
  const gsl_rng_type *t = gsl_rng_default;
  // U
  if( use_u_ch ) {
    rng_u = gsl_rng_alloc( t );
    eff_seedType_u = seedType_u;
    if( seedType_u == 3 ) { // as model 
      model->getData( "seedType", &eff_seedType_u ); 
    };
    bseed_u = 0;
    if( addBaseSeed_u ) {
      model->getData( "seed", &bseed_u );
    };
  };
  // Phi
  if( use_f_ch ) {
    rng_p = gsl_rng_alloc( t );
    eff_seedType_p = seedType_p;
    if( seedType_p == 3 ) { // as model 
      model->getData( "seedType", &eff_seedType_p ); 
    };
    bseed_p = 0;
    if( addBaseSeed_p ) {
      model->getData( "seed", &bseed_p );
    };
  };

  return 0;
}

int TSource::do_postRun( int /*good*/ )
{
  if( rng_u ) {
    gsl_rng_free( rng_u );
    rng_u = 0;
  };
  if( rng_p ) {
    gsl_rng_free( rng_p );
    rng_p = 0;
  };
  return 0;
}


int TSource::do_startLoop( int acnx, int acny )
{
  was_pulse = 0;
  tt = model_nn * tdt;
  // U rnd init
  if( use_u_ch ) {
    if( (eff_seedType_u == 0) ||                // need to seed now
	(eff_seedType_u == 1 && acnx == 0 ) ||
	(acnx == 0 && acny == 0) 
      ) {
      if( seed_u == -1 || ( addBaseSeed_u && bseed_u == -1) ) {
	sseed_u = int( time(0) + acnx * 101 + acny*17 );
      } else {
	sseed_u = seed_u + bseed_u;
      }
      gsl_rng_set( rng_u, sseed_u );
    };
    
    u_ch_vs = gsl_ran_flat( rng_u, u_ch_v0, u_ch_vm );
    u_ch_ve = gsl_ran_flat( rng_u, u_ch_v0, u_ch_vm );
    u_ch_ts = 0;
    u_ch_te = gsl_ran_flat( rng_u, u_ch_t0, u_ch_tm );
    u_ch_k = ( u_ch_ve - u_ch_vs ) / ( u_ch_te - u_ch_ts );
  };

  // Phi rnd init
  if( use_f_ch ) {
    if( (eff_seedType_p == 0) ||                // need to seed now
	(eff_seedType_p == 1 && acnx == 0 ) ||
	(acnx == 0 && acny == 0) 
      ) {
      if( seed_p == -1 || ( addBaseSeed_p && bseed_p == -1) ) {
	sseed_p = int( time(0) + acnx * 171 + acny*23 + 123);
      } else {
	sseed_p = seed_p + bseed_p;
      }
      gsl_rng_set( rng_p, sseed_p );
    };
    
    f_ch_vs = gsl_ran_flat( rng_p, f_ch_v0, f_ch_vm );
    f_ch_ve = gsl_ran_flat( rng_p, f_ch_v0, f_ch_vm );
    f_ch_ts = 0;
    f_ch_te = gsl_ran_flat( rng_p, f_ch_t0, f_ch_tm );
    f_ch_k = ( f_ch_ve - f_ch_vs ) / ( f_ch_te - f_ch_ts );
  };

  return 0;
}

int TSource::registered = reg();

int TSource::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tsource.cpp

