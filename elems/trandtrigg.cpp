/***************************************************************************
                          trandtrigg.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

#include <math.h>
#include "miscfun.h"
#include "tmodel.h"
#include "trandtrigg.h"

static const char* const trandtrigg_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

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
  &TMiso::class_info, helpstr, clpElem };


TRandTrigg::TRandTrigg( TDataSet* aparent )
        :TMiso( aparent ),
	 PRM_INIT( prob, "Probability" ), 
	 PRM_INIT( nforce, "Forced every N" ),
	 PRM_INIT( useLevel, "Use Level to trigg" ),
	 PRM_INIT( useForce, "Use u[1] to force trigg"),
	 PRM_INIT( useLock, "Use u[2] to lock"),
	 PRM_INIT( useZero, "Use 0 as negative output"),
	 PRM_INIT( seed, "Seed"),
	 PRM_INIT( seedType, "Seed at"),
	 PRM_INIT( addBaseSeed, "add Base to Seed")
{
  prob = 0.5; u_old = 0; nforce = 2;
  useLevel = useForce = useLock = useZero = 0; ns = -1;
  seed = 9561 + (time(0) & 0x1AFFF );
  seedType = 3; addBaseSeed = 1;
  rng = 0;

  PRMI(prob).setDescr( "Probability of spontatious flip " ); 
  PRMI(nforce).setDescr( "Forced every N changes" );
  PRMI(useLevel).setDescr( "Use Level to trigger, not +- pulse" );
  PRMI(useForce).setDescr( "Use u[1] to force flip");
  PRMI(useLock).setDescr( "Use u[2] to lock");
  PRMI(useZero).setDescr( "Use 0 as negative output, not -1");
  PRMI(seed).setDescr( "Random generator seed");
  PRMI(seedType).setDescr( "Specifies, when to seed");
  PRMI(seedType).setElems( trandtrigg_list_seedType );
  PRMI(addBaseSeed).setDescr( "add Base (model) value to Seed");
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

int TRandTrigg::do_preRun( int /*run_tp*/, int /*an*/, 
                           int /*anx*/, int /*any*/, double /*adt*/ )
{
  const gsl_rng_type *t = gsl_rng_default;
  rng = gsl_rng_alloc( t );
  eff_seedType = seedType;
  if( seedType == 3 ) { // as model 
    model->getData( "seedType", &eff_seedType ); 
  };
  bseed = 0;
  if( addBaseSeed ) {
    model->getData( "seed", &bseed );
  };
  return 0;
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


double TRandTrigg::f( double /* t */ )
{
  double du, rv;
  du = *in_so[0] - u_old; u_old = *in_so[0];
  if( ns == -1 ) { du = 0; ns = 0; };
  if( useForce && *in_so[1] > 0.1 ) {
    ns = 0; currOut = !currOut;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( useLock && *in_so[2] > 0.1 ) {
    // ns++;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( ( useLevel && *in_so[0] > 0.1 ) || ( !useLevel && fabs(du) > 0.5 ) ) {
    rv = gsl_ran_flat( rng, 0, 1 ); 
    if( rv <= prob || nforce <= 1 || ns >= nforce-1 ) {
      ns = 0; currOut = !currOut;
    } else {
      ns++;
    };
  };
  return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
}

int TRandTrigg::registered = reg();

int TRandTrigg::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
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


