/***************************************************************************
                          trand.cpp  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2012 by atu
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
#include <miscfun.h>
#include "tmodel.h"
#include "trand.h"

const char* TRand::helpstr = "<H1>TRand</H1>\n"
 "Random values signal (noise) generator: <br>\n"
 "Integer parameter <b>type</b> selects type of distribution (see GSL docs).<br>\n"
 "Argument <b>tau</b> defined recalculation time of generator<br>.\n"
 "Argument <b>ampl</b> - scale of output.<br>\n"
 "Argument <b>zval</b> - zero level of output.<br>\n"
 "Arguments <b>sigma, a, b, c</b> -- may me parameters of distribution.<br>\n"
 "Integer parameter <b>seed</b> -- seed of generator, if -1 -- new seed every seeding.<br>\n"
 "Integer <b>seedType</b> -- type of seedeng\n"
 "Flag <b>addBaseSeed</b> -- add value of base seed to current\n"
 "Double parameters <b>ampl, zval, sigma, a, b, c</b> can be changed at any time.<br>\n"
 "About distributions and parameters see GSL documentation.<br>\n"
 "Input is u[0], <b>not time</b>. If need time, link first input to 't'.<br>\n"
;

static const char* const trand_list_type = 
     "flat(-sigma,sigma)\n"       // 0
     "gaussian(sigma)\n"          // 1 
     "gaussian_tail(a,sigma)\n"   // 2
     "exponential(sigma)\n"       // 3
     "laplace(a)\n"               // 4
     "exppow(a,b)"                // 5
;

static const char* const trand_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

TClassInfo TRand::class_info = {
  CLASS_ID_TRand, "TRand", TRand::create,
  &TMiso::class_info, helpstr, clpElem };

TDataInfo TRand::trand_d_i[31] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 420, 450, 0,  0.0, 0.0, 0, 0, "rand_dial", "", "Dialog for TRand"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type", "", "Type"},
 { dtpInt, dtpsList,   6,   20,  70, 220,  20, 2,  0.0, 0.0, 0, 0, "type", "dist type", trand_list_type },
 { dtpLabel,      0,   0,   30, 130, 100,  20, 0,  0.0, 0.0, 0, 0, "l_tau",   "", "tau"}, //5
 { dtpDou,        0,   0,   20, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "tau", "tau", ""},
 { dtpLabel,      0,   0,   30, 180, 100,  20, 0,  0.0, 0.0, 0, 0, "l_sigma",   "", "sigma"},
 { dtpDou,        0,   0,   20, 200, 120,  20, 0,  -1e300, 1e300, 0, 0, "sigma", "sigma", ""},
 { dtpLabel,      0,   0,   30, 230, 100,  20, 0,  0.0, 0.0, 0, 0, "l_ampl",   "", "ampl"},
 { dtpDou,        0,   0,   20, 250, 120,  20, 0,  -1e300, 1e300, 0, 0, "ampl", "ampl", ""}, //10
 { dtpLabel,      0,   0,   30, 280, 100,  20, 0,  0.0, 0.0, 0, 0, "l_zval",   "", "zval"},
 { dtpDou,        0,   0,   20, 300, 120,  20, 0,  -1e300, 1e300, 0, 0, "zval", "zval", ""},
 { dtpLabel,      0,   0,  270, 130,  50,  20, 0,  0.0, 0.0, 0, 0, "l_a",   "", "a"},
 { dtpDou,        0,   0,  260, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "a", "a", ""},
 { dtpLabel,      0,   0,  270, 180,  50,  20, 0,  0.0, 0.0, 0, 0, "l_b",   "", "b"},
 { dtpDou,        0,   0,  260, 200, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpLabel,      0,   0,  270, 230,  50,  20, 0,  0.0, 0.0, 0, 0, "l_c",   "", "c"},
 { dtpDou,        0,   0,  260, 250, 120,  20, 0,  -1e300, 1e300, 0, 0, "c", "c", ""},
 { dtpLabel,      0,   0,  270, 280,  50,  20, 0,  0.0, 0.0, 0, 0, "l_seed",   "", "seed"},
 { dtpInt,        0,   0,  260, 300, 120,  20, 0,  1.0, 0.0, 0, 0, "seed", "seed", ""}, // 20
 { dtpInt,dtpsSwitch,  0,   20, 330, 150,  20, efNoDial | efRO,  0.0, 0.0, 0, 0, "useSameSeed",   "", "useSameSeed"},
 { dtpInt, dtpsList,   4,   20, 330, 150,  20, efNoRunChange,  0.0, 0.0, 0, 0, "seedType", "Seed at", trand_list_seedType },
 { dtpInt,dtpsSwitch,  0,  260, 330, 150,  20, efNoRunChange,  0.0, 0.0, 0, 0, "addBaseSeed",   "", "addBaseSeed"},
 { dtpButton,     0,   0,   20, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"}, // 22
 { dtpButton,     1,   0,  140, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 400,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};

TRand::TRand( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( type, "Type" ),
	 PRM_INIT( tau, "tau" ),
	 PRM_INIT( ampl, "Amplitude" ),
	 PRM_INIT( zval, "Base value" ),
	 PRM_INIT( sigma, "\\sigma" ),
	 PRM_INIT( a, "a" ),
	 PRM_INIT( b, "b" ),
	 PRM_INIT( c, "c" ),
	 PRM_INIT( seed, "Seed" ),
	 PRM_INIT( useSameSeed, "Same seed" ),
	 PRM_INIT( seedType, "Seed type" ),
	 PRM_INIT( addBaseSeed, "Add base" )
{
  int i;
  type = 0;
  tau = 0.05; ampl = 1; zval = 0;
  sigma = a = b = c = 1;
  useSameSeed = 0; 
  seed = 2942 + ( time(0) & 0x07FF );
  seedType = 3; addBaseSeed = 1;
  old_val = 0; sp_time = 1e300; old_in = 0; sseed = 0; bseed = 0;
  rng = 0;
  d_i = trand_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = &descr;  // from TMiso
  ptrs[4] = &type; 
  ptrs[6] = &tau; 
  ptrs[8] = &sigma; 
  ptrs[10] = &ampl; 
  ptrs[12] = &zval; 
  ptrs[14] = &a;  ptrs[16] = &b; ptrs[18] = &c; 
  ptrs[20] = &seed; 
  ptrs[21] = &useSameSeed;
  ptrs[22] = &seedType;
  ptrs[23] = &addBaseSeed;
  // from TMiso 
  ptrs[27] = links;
  ptrs[28] = &vis_x; ptrs[29] = &vis_y;

  PRMI(type).setDescr( "Type of distribution" );
  PRMI(type).setElems( trand_list_type );
  PRMI(tau).setDescr( "time of const output value, if <=0 -- change every tick " );
  PRMI(ampl).setDescr( "Amplitude of output" );
  PRMI(zval).setDescr( "Zero value of output " );
  PRMI(sigma).setDescr( "\\sigma" );
  PRMI(a).setDescr( "a" );
  PRMI(b).setDescr( "b" );
  PRMI(c).setDescr( "c" );
  PRMI(seed).setDescr( "Seed value for generator" );
  PRMI(useSameSeed).setDescr( "Obsoleted" );
  PRMI(seedType).setDescr( "0 - every run 1- 1d loop .. 3-by model " );
  PRMI(seedType).setElems( trand_list_seedType );
  PRMI(addBaseSeed).setDescr( "Add base seed to element seed " );
}

TRand::~TRand()
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  };
}

TDataSet* TRand::create( TDataSet* apar )
{
  return new TRand( apar );
}

int TRand::getClassId(void) const 
{
  return CLASS_ID_TRand;
}

const TClassInfo* TRand::getClassInfo(void) const
{
  return &class_info;
}

const char *TRand::getHelp(void) const
{
  return helpstr;
}

const char** TRand::getIcon(void) const
{
  return icon;
}

// =========================== meat ================================

int TRand::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int rc = TMiso::preRun( run_tp, an, anx, any, adt );
  const gsl_rng_type *t = gsl_rng_default;
  rng = gsl_rng_alloc( t );
  eff_seedType = seedType;
  if( seedType == 3 ) { // as model 
    model->getDataSI( "seedType", &eff_seedType, 0 ); 
  };
  bseed = 0;
  if( addBaseSeed ) {
    model->getDataSI( "seed", &bseed, 0 );
  };
  return rc;
}

int TRand::postRun( int good )
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  };
  return TMiso::postRun( good );
}

int TRand::startLoop( int acnx, int acny )
{
  int k = TMiso::startLoop( acnx, acny );
  old_val = 0; sp_time = 1e300; old_in = 0;
  if( (eff_seedType == 0) ||                // need to seed now
      (eff_seedType == 1 && acnx == 0 ) ||
      (acnx == 0 && acny == 0) 
    ) {
    if( seed == -1 || ( addBaseSeed && bseed == -1) ) {
      sseed = int( time(0) + acnx * 1001 + acny );
    } else {
      sseed = seed + bseed;
    }
    gsl_rng_set( rng, sseed );
  };
  return k;
}

double TRand::f( const double*  u, double  /*t*/  )
{
  double v, du;
  if( sp_time >= tau || tau <=0  ) {
    sp_time -= tau;
    if( sp_time >= tau )
      sp_time = 0;
    switch( type ) {
      case 0:  v = gsl_ran_flat( rng, -sigma, sigma ); break;
      case 1:  v = gsl_ran_gaussian( rng, sigma ); break;
      case 2:  v = gsl_ran_gaussian_tail( rng, a, sigma ); break;
      case 3:  v = gsl_ran_exponential( rng, sigma ); break;
      case 4:  v = gsl_ran_laplace( rng, a ); break;
      case 5:  v = gsl_ran_exppow( rng, a, b ); break;
      default: v = 0;
    };
    old_val = zval + ampl * v;
  };
  du = u[0] - old_in; old_in = u[0];
  sp_time += du;
  return old_val; 
}

int TRand::registered = reg();

int TRand::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TRand::icon[] = {
"32 32 5 1",
"c c #808080",
"b c #000000",
"a c #0000ff",
". c #dcdcdc",
"# c #ff0000",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"....aaaaaaa.....................",
"....aaaaaaa.....................",
"....aa...aa.....................",
"....aa...aa...aaaaaaa...........",
"....aa...aa...aaaaaaa...........",
"....aa...aa...aa...aa...........",
"....aa...aa...aa...aa...........",
"....aa...aaaaaaa...aa...........",
"....aa...aaaaaaa...aa...........",
"....aa.............aa...........",
"....aa.............aa...........",
"....aa.............aa...........",
"....aa.............aa...........",
"....aa.............aa...........",
"....aa.............aa...........",
"....aa.............aaaaaaa......",
"....aa.............aaaaaaa......",
"....aa..................aa......",
"....aa..................aa......",
"....aa..................aa......",
"..aaaa..........................",
"..aaaa..........................",
"................................"
};
  
// end of trand.cpp


