/***************************************************************************
                          trandtrigg.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000 by atu
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

#include <math.h>
#include "miscfun.h"
#include "tmodel.h"
#include "trandtrigg.h"

const char* TRandTrigg::helpstr = "<H1>TRandTrigg</H1>\n"
 "Random - deterministic trigger: <br>\n"
 "it sense change or level of input signal u[0],<br>\n"
 "than if forced state or good probability, change output signal.<br>\n"
 "Parametr <b>prob</b> defines probability of change.<br>\n"
 "Parametr <b>nforce</b> defines how often will be forced click. .<br>\n"
 "Parametrs <b>seed, seedType, addBaseSeed </b> controls random ganerator<br>\n"
 "Parameters: <b>useLevel, useForce, useLock, useZero</b>, - misc flags.";

TClassInfo TRandTrigg::class_info = {
  CLASS_ID_TRandTrigg, "TRandTrigg", TRandTrigg::create,
  &TMiso::class_info, helpstr };

TDataInfo TRandTrigg::trandtrigg_d_i[22] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 450, 300, 0,  0.0, 0.0, 0, 0, "rt_dial", "", "Dialog for TRandTrigg"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   30,  50, 100,  20, 0,  0.0, 0.0, 0, 0, "l_prob", "", "Probability"},
 { dtpDou,        0,   0,   20,  70, 120,  20, 0,  0.0, 2.0, 0, 0, "prob", "Probability", ""},
 { dtpLabel,      0,   0,   30, 100, 100,  20, 0,  0.0, 0.0, 0, 0, "l_nforce", "", "Forced every N"},
 { dtpInt,        0,   0,   20, 120, 120,  20, 2,  0.0, 0.0, 0, 0, "nforce", "Forced tigg N", ""},
 { dtpInt,   dtpsSw,   0,  180,  50, 170,  20, 2,  0.0, 0.0, 0, 0, "useLevel", "", "Use Level to trigg"},
 { dtpInt,   dtpsSw,   0,  180,  80, 170,  20, 2,  0.0, 0.0, 0, 0, "useForce", "", "Use u[1] to force trigg"},
 { dtpInt,   dtpsSw,   0,  180, 110, 170,  20, 2,  0.0, 0.0, 0, 0, "useLock", "", "Use u[2] to lock"},
 { dtpInt,   dtpsSw,   0,  180, 140, 170,  20, 2,  0.0, 0.0, 0, 0, "useZero", "", "Use 0 as negative output"},

 { dtpLabel,      0,   0,   30, 180,  50,  20, 0,  0.0, 0.0, 0, 0, "l_seed",   "", "seed"},
 { dtpInt,        0,   0,   20, 200, 120,  20, 0,  1.0, 0.0, 0, 0, "seed", "seed", ""}, // 20
 { dtpInt, dtpsList,   4,  160, 200, 150,  20, efNoRunChange,  0.0, 0.0, 0, 0, "seedType", "Seed at",
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
 },
 { dtpInt,dtpsSwitch,  0,  330, 200, 150,  20, efNoRunChange,  0.0, 0.0, 0, 0, "addBaseSeed",   "", "addBaseSeed"},
 
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  170, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  320, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



TRandTrigg::TRandTrigg( TDataSet* aparent )
        :TMiso( aparent )
{
  int i;
  prob = 0.5; u_old = 0; nforce = 2;
  useLevel = useForce = useLock = useZero = 0; ns = -1;
  seed = 9561 + (time(0) & 0x1AFFF );
  seedType = 3; addBaseSeed = 1;
  rng = 0;
  d_i = trandtrigg_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &prob; ptrs[6] = &nforce; ptrs[7] = &useLevel;
  ptrs[8] = &useForce; ptrs[9] = &useLock; ptrs[10] = &useZero;
  ptrs[12] = &seed; ptrs[13] = &seedType; ptrs[14] = &addBaseSeed;
  // from TMiso 
  ptrs[18] = links;
  ptrs[19] = &vis_x; ptrs[20] = &vis_y;
}

TRandTrigg::~TRandTrigg()
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  }
}

TDataSet* TRandTrigg::create( TDataSet* apar )
{
  return new TRandTrigg( apar );
}

int TRandTrigg::getClassId(void) const 
{
  return CLASS_ID_TRandTrigg;
}

const char* TRandTrigg::getClassName(void) const 
{
  return "TRandTrigg";
}

const TClassInfo* TRandTrigg::getClassInfo(void) const
{
  return &class_info;
}

const char *TRandTrigg::getHelp(void) const
{
  return helpstr;
}

const char** TRandTrigg::getIcon(void) const
{
  return icon;
}

int TRandTrigg::preRun( int run_tp, int an, int anx, int any, double adt )
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

int TRandTrigg::postRun( int good )
{
  if( rng ) {
    gsl_rng_free( rng );
    rng = 0;
  };
  return TMiso::postRun( good );
}

int TRandTrigg::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  ns = -1; u_old = 0; currOut = 0;
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
  return rc;
}


double TRandTrigg::f( const double* u, double /* t */ )
{
  double du, rv;
  du = u[0] - u_old; u_old = u[0];
  if( ns == -1 ) { du = 0; ns = 0; };
  if( useForce && u[1] > 0.1 ) {
    ns = 0; currOut = !currOut;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( useLock && u[2] > 0.1 ) {
    // ns++;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( ( useLevel && u[0] > 0.1 ) || ( !useLevel && fabs(du) > 0.5 ) ) {
    rv = gsl_ran_flat( rng, 0, 1 ); 
    if( rv <= prob || nforce <= 1 || ns >= nforce-1 ) {
      ns = 0; currOut = !currOut;
    } else {
      ns++;
    };
  };
  return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
}

/* XPM */
const char* TRandTrigg::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjggggggggggggggggj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjg````````mmmmmmgj",
"jjjjjjjjjjjjjjjggggggggggggggggj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jiiiiigggggiiiggijjjjiiiiiiijjjj",
"jiiiiigggggiiiggijjjjiiiiiiijjjj",
"jjjjjiijjggiiiggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jffffiiffggifiggiiiffiifffiifffj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiijjggijjggijjjjiijjjiijjjj",
"jjjjjiiiiggiggggiiiiiiijjjiiiijj",
"jjjjjiiiigggggggiiiiiiijjjiiiijj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};


// end of trandtrigg.cpp


