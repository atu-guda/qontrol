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

static const char* const tsource_list_type = 
             "U*sin(o*t)\n"           // 0
             "U*sign(sin(o*t))\n"     // 1
             "U*t/T*sin(o*t)\n"       // 2
             "U*t/T*sign(sin(o*t))\n" // 3
             "Dirac(t-tau)\n"         // 4
             "U*Thetta(t-tau)\n"      // 5
             "U*t/T\n"                // 6
             "saw(t,tau) /|/|/| \n"   // 7
             "saw2(t.tau) /\\/\\ \n"  // 8
             "Chaos Wave(Phi)\n"      // 9
             "U*triangle(o*t)\n"      // 10
             "Phase"                 // 11
;

static const char* const tsource_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

const char* TSource::helpstr = "<H1>TSource</H1>\n"
 "Source of different kind of signals: <br>\n"
 "Have N parameters: <b>type, U, omega, noise .. b</b>,"
 "each can be changed at any time ??.";

TClassInfo TSource::class_info = {
  CLASS_ID_TSource, "TSource", TSource::create,
  &TMiso::class_info, helpstr };

TDataInfo TSource::tsource_d_i[48] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial, 0, 0, 0, 0, 600, 400, 0, 0.0, 0.0, 0, 0, "dia", "", "" },
 { dtpInt, 0, 0, 20, 10, 60, 20, 8, 0.0, 0.0, 0, 0, "ord", "Order", "" }, 
 { dtpStr, 0, 60, 100, 10, 380, 20, 2, 0.0, 0.0, 0, 0, "descr", "Description", "" },
 { dtpLabel, 0, 0, 20, 40, 50, 20, 0, 0.0, 0.0, 0, 0, "l_type", "", "Type" }, 
 { dtpInt, dtpsList, 12, 20, 60, 210, 20, 2, 0.0, 0.0, 0, 0, "type", "Type", tsource_list_type },
 { dtpLabel, 0, 0, 250, 40, 90, 20, 0, 0.0, 0.0, 0, 0, "l_uu", "no", "U" }, 
 { dtpDbl, 0, 0, 240, 60, 100, 20, 0, 0.0, -1.0, 0, 0, "uu", "Amplitude", "" }, 
 { dtpLabel, 0, 0, 360, 40, 100, 20, 0, 0.0, 0.0, 0, 0, "l_omega", "", "omega (o)  | tau" },
 { dtpDbl, 0, 0, 360, 60, 100, 20, 0, 1e-50, 1e50, 0, 0, "omega", "Omega", "" },
 { dtpLabel, 0, 0, 490, 40, 60, 20, 0, 0.0, 0.0, 0, 0, "l_cc", "no", "+C" }, 
 { dtpDbl, 0, 0, 480, 60, 100, 20, 0, 0.0, -1.0, 0, 0, "cc", "Const", "" }, 
 { dtpInt, dtpsSw, 0, 240, 90, 100, 20, efNoDial | efNoSave, 0.0, 0.0, 0, 0, "use_noise", "", "Use Noise" },
 { dtpDbl, 0, 0, 360, 90, 100, 20, efNoDial | efNoSave, 0.0, -1.0, 0, 0, "noise", "Noise", "" }, 
 // U-chaos group
 { dtpGroup, 0, 0, 20, 120, 560, 105, 0, 0.0, 0.0, 0, 0, "grp_u_ch", "no", "U chaos" },
 { dtpInt, dtpsSw, 0, 30, 160, 50, 20, 2, 0.0, 0.0, 0, 0, "use_u_ch", "", "Use" },
 { dtpLabel, 0, 0, 100, 150, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chu_v", "", "Value" }, 
 { dtpLabel, 0, 0, 100, 180, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chu_t", "", "Time" }, 
 { dtpLabel, 0, 0, 160, 130, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chu_min", "", "Min" }, 
 { dtpLabel, 0, 0, 280, 130, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chu_max", "", "Max" }, 
 { dtpDbl, 0, 0, 150, 150, 100, 20, 2, 0.0, -1.0, 0, 0, "u_ch_v0", "U_ch_v0", "" },
 { dtpDbl, 0, 0, 270, 150, 100, 20, 2, 0.0, -1.0, 0, 0, "u_ch_vm", "U_ch_vm", "" },
 { dtpDbl, 0, 0, 150, 180, 100, 20, 2, 1e-20, 1e10, 0, 0, "u_ch_t0", "U_ch_t0", "" },
 { dtpDbl, 0, 0, 270, 180, 100, 20, 2, 1e-20, 1e10, 0, 0, "u_ch_tm", "", "" },
 // rnd_u
 { dtpLabel,      0,   0,  400, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "l_seed_u",   "", "seed"},
 { dtpInt,        0,   0,  390, 150,  80,  20, 0,  1.0, 0.0, 0, 0, "seed_u", "seed_u", ""},
 { dtpInt, dtpsList,   4,  390, 180, 100,  20, efNoRunChange,  0.0, 0.0, 0, 0, "seedType_u", "Seed at", tsource_list_seedType },
 { dtpInt,dtpsSwitch,  0,  390, 200, 100,  20, efNoRunChange,  0.0, 0.0, 0, 0, "addBaseSeed_u",   "", "addBaseSeed_u"},

 // Phi-chaos group
 { dtpGroup, 0, 0, 20, 230, 560, 105, 0, 0.0, 0.0, 0, 0, "grp_f_ch", "", "Phi chaos" },
 { dtpInt, dtpsSw, 0, 30, 270, 50, 20, 2, 0.0, 0.0, 0, 0, "use_f_ch", "", "Use" },
 { dtpLabel, 0, 0, 100, 260, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chf_v", "", "Value" }, 
 { dtpLabel, 0, 0, 100, 290, 50, 20, 0, 0.0, 0.0, 0, 0, "l_chf_t", "", "Time" }, 
 { dtpLabel, 0, 0, 160, 240, 80, 20, 0, 0.0, 0.0, 0, 0, "l_chf_min", "", "Min" }, 
 { dtpLabel, 0, 0, 280, 240, 70, 20, 0, 0.0, 0.0, 0, 0, "l_chf_max", "", "Max" }, 
 { dtpDbl, 0, 0, 150, 260, 100, 20, 2, 0.0, -1.0, 0, 0, "f_ch_v0", "F_Ch_v0", "" },
 { dtpDbl, 0, 0, 270, 260, 100, 20, 2, 0.0, -1.0, 0, 0, "f_ch_vm", "F_ch_vm", "" },
 { dtpDbl, 0, 0, 150, 290, 100, 20, 2, 1e-20,   1e10, 0, 0, "f_ch_t0", "F_ch_t0", "" },
 { dtpDbl, 0, 0, 270, 290, 100, 20, 2, 1e-20,   1e10, 0, 0, "f_ch_tm", "F_ch_tm", "" },
 // rnd_p
 { dtpLabel,      0,   0,  400, 240,  50,  20, 0,  0.0, 0.0, 0, 0, "l_seed_p",   "", "seed"},
 { dtpInt,        0,   0,  390, 260,  80,  20, 0,  1.0, 0.0, 0, 0, "seed_p", "seed_p", ""},
 { dtpInt, dtpsList,   4,  390, 290, 100,  20, efNoRunChange,  0.0, 0.0, 0, 0, "seedType_p", "Seed at", tsource_list_seedType },
 { dtpInt,dtpsSwitch,  0,  390, 310, 100,  20, efNoRunChange,  0.0, 0.0, 0, 0, "addBaseSeed_p",   "", "addBaseSeed_p"},

 { dtpButton,     0,   0,   20, 350,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  250, 350,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  490, 350,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};


TSource::TSource( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( type, "Type" ),
	PRM_INIT( uu, "Amplitude" ),
	PRM_INIT( omega, "Frequency" ),
	PRM_INIT( cc, "C" ),
	// --- U chaos
	PRM_INIT(   use_u_ch, "use U chaos" ),
	PRM_INIT(    u_ch_v0, "vU min" ),
	PRM_INIT(    u_ch_vm, "vU max" ),
	PRM_INIT(    u_ch_t0, "tU min" ),
	PRM_INIT(    u_ch_tm, "tU max" ),
	PRM_INIT(     seed_u, "U seed" ),
	PRM_INIT( seedType_u, "U seed type" ),
	PRM_INIT( addBaseSeed_u, "U add base seed" ),
	// --- Phi chaos
	PRM_INIT(   use_f_ch, "use Phi chaos" ),
	PRM_INIT(    f_ch_v0, "vF min" ),
	PRM_INIT(    f_ch_vm, "vF max" ),
	PRM_INIT(    f_ch_t0, "tF min" ),
	PRM_INIT(    f_ch_tm, "rF max" ),
	PRM_INIT(     seed_p, "F seed" ),
	PRM_INIT( seedType_p, "F seed type" ),
	PRM_INIT( addBaseSeed_p, "F add base to seed" ),
        // unused
	PRM_INIT( phi_shift, "unused" ),
	PRM_INIT( use_noise, "unused" ),
	PRM_INIT( noise, "unused" )
{
  int i;
  type = use_noise = use_u_ch = use_f_ch = 0;
  uu = 1; omega = 0.7; cc = phi_shift = 0; noise = 0.01;
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

  d_i = tsource_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr; 
  ptrs[4] = &type;  ptrs[6] = &uu; ptrs[8] = &omega;  ptrs[10] = &cc; 
  ptrs[11] = &use_noise; ptrs[12] = &noise; 
  // U chaos
  ptrs[14] = &use_u_ch;  ptrs[19] = &u_ch_v0; ptrs[20] = &u_ch_vm; 
  ptrs[21] = &u_ch_t0;  ptrs[22] = &u_ch_tm; 
   // U chaos rnd
  ptrs[24] = &seed_u; ptrs[25] = &seedType_u; ptrs[26] = &addBaseSeed_u;
  // Phi chaos
  ptrs[28] = &use_f_ch; ptrs[33] = &f_ch_v0; 
  ptrs[34] = &f_ch_vm; ptrs[35] = &f_ch_t0; ptrs[36] = &f_ch_tm;
   // Phi chaos rnd
  ptrs[38] = &seed_p; ptrs[39] = &seedType_p; ptrs[40] = &addBaseSeed_p;
  // from TMiso
  ptrs[44] = links;
  ptrs[45] = &vis_x; ptrs[46] = &vis_y;

  PRMI(type).setDescr( "Source type" );
  PRMI(type).setElems( tsource_list_type );
  PRMI(uu).setDescr( "Amplitude of signal (or pulse max)" );
  PRMI(omega).setDescr( "Frequency of source (or pulse width)" );
  PRMI(cc).setDescr( "Constant base" );
  // --- U chaos
  PRMI(use_u_ch).setDescr( "Use amplitude perturbations" );
  PRMI(u_ch_v0).setDescr( "Minimum value of amplitule factor" );
  PRMI(u_ch_vm).setDescr( "Maximum value of amplitule factor" );
  PRMI(u_ch_t0).setDescr( "Minimum time of change" );
  PRMI(u_ch_t0).setMinMax( 1e-20, 1e20 );
  PRMI(u_ch_tm).setDescr( "Maximum time of change" );
  PRMI(u_ch_tm).setMinMax( 1e-20, 1e20 );
  PRMI(seed_u).setDescr( "Random Generator seed" );
  PRMI(seedType_u).setDescr( "When to seed" );
  PRMI(seedType_u).setElems( tsource_list_seedType );
  PRMI(addBaseSeed_u).setDescr( "Add seed from base(model)" );
  // --- Phi chaos
  PRMI(use_f_ch).setDescr( "Use phase perturbations" );
  PRMI(f_ch_v0).setDescr( "Minimum value of phase factor" );
  PRMI(f_ch_vm).setDescr( "Maximum value of phase factor" );
  PRMI(f_ch_t0).setDescr( "Minimum time of change" );
  PRMI(f_ch_t0).setMinMax( 1e-20, 1e20 );
  PRMI(f_ch_tm).setDescr( "Maximum time of change" );
  PRMI(f_ch_tm).setMinMax( 1e-20, 1e20 );
  PRMI(seed_p).setDescr( "Random Generator seed" );
  PRMI(seedType_p).setDescr( "When to seed" );
  PRMI(seedType_p).setElems( tsource_list_seedType );
  PRMI(addBaseSeed_p).setDescr( "Add seed from base(model)" );
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

int TSource::getClassId(void) const
{
  return CLASS_ID_TSource;
}

const char* TSource::getClassName(void) const
{
  return "TSource";
}

const TClassInfo* TSource::getClassInfo(void) const
{
  return &class_info;
}

const char *TSource::getHelp(void) const
{
  return helpstr;
}

const char** TSource::getIcon(void) const
{
  return icon;
}

double TSource::f( const double* /* u */, double t )
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

int TSource::preRun( int run_tp, int an, int anx, int any, double atdt )
{
  int rc =  TMiso::preRun( run_tp, an, anx, any, atdt );
  const gsl_rng_type *t = gsl_rng_default;
  // U
  if( use_u_ch ) {
    rng_u = gsl_rng_alloc( t );
    eff_seedType_u = seedType_u;
    if( seedType_u == 3 ) { // as model 
      model->getDataSI( "seedType", &eff_seedType_u, 0 ); 
    };
    bseed_u = 0;
    if( addBaseSeed_u ) {
      model->getDataSI( "seed", &bseed_u, 0 );
    };
  };
  // Phi
  if( use_f_ch ) {
    rng_p = gsl_rng_alloc( t );
    eff_seedType_p = seedType_p;
    if( seedType_p == 3 ) { // as model 
      model->getDataSI( "seedType", &eff_seedType_p, 0 ); 
    };
    bseed_p = 0;
    if( addBaseSeed_p ) {
      model->getDataSI( "seed", &bseed_p, 0 );
    };
  };

  return rc;
}

int TSource::postRun( int good )
{
  if( rng_u ) {
    gsl_rng_free( rng_u );
    rng_u = 0;
  };
  if( rng_p ) {
    gsl_rng_free( rng_p );
    rng_p = 0;
  };
  return TMiso::postRun( good );
}


int TSource::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
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

  return rc;
}

/* XPM */
const char* TSource::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #408080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"lllllllllllllllgllllllllllllllll",
"llllllllllllllglglllllllllllllll",
"lllllllllllllglllgllllllllllllll",
"llllllllllllglllllglllllllllllll",
"llllgggggggglllllllgggggggglllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llgglllllllllgggglllllllllllllll",
"llggglllllllggggggglllllllllllll",
"llllgglllllgglllllgllllillllllll",
"lllllgglllggllllllllllliilllllll",
"llllllggggglllllllllllliiillllll",
"lllllllgglllllllllllllliiiilllll",
"llllllllllllllllllllllliiiiillll",
"liiiiiiiiiiiiiiiiiiiiiiiiiiiilll",
"liiiiiiiiiiiiiiiiiiiiiiiiiiiiill",
"liiiiiiiiiiiiiiiiiiiiiiiiiiiilll",
"llllllllllllllllllllllliiiiillll",
"llllllllllllllllllllllliiiilllll",
"llllllllllllllllllllllliiillllll",
"llllllllllllllllllllllliilllllll",
"lllllllllllllllllllllllillllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll"
};

// end of tsource.cpp

