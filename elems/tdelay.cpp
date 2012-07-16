/***************************************************************************
                          tdelay.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#include "tdelay.h"
#include "miscfun.h" 
#include "tdelay.h"

const char* TDelay::helpstr = "<H1>TDelay</H1>\n"
 "Delay signal for given time: <br>\n"
 "Have 2 parameters: <b>mdelay</b> - maximum delay, <br>\n"
 "<b>cdelay</b> - current delay, can changed at any time, but <= <b>mdelay</b>.";

TClassInfo TDelay::class_info = {
  CLASS_ID_TDelay, "TDelay", TDelay::create,
  &TMiso::class_info, helpstr, clpElem };


TDelay::TDelay( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( mdelay, "Max. delay" ),
	PRM_INIT( cdelay, "Current delay" )
{
  mdelay = 1.5; cdelay = 1; v1 = v2 = u00 = 0; buf = 0; 
  
  PRMI(mdelay).setDescr( "Maximum delay" );
  PRMI(mdelay).setMinMax( 0, 1e10 );
  PRMI(cdelay).setDescr( "Current delay" );
  PRMI(cdelay).setMinMax( 0, 1e10 );
}

TDelay::~TDelay()
{
  delete buf; buf = 0; // in case of postRun missing
}

TDataSet* TDelay::create( TDataSet* apar )
{
  return new TDelay( apar );
}

int TDelay::getClassId(void) const
{
  return CLASS_ID_TDelay;
}


const TClassInfo* TDelay::getClassInfo(void) const
{
  return &class_info;
}

const char *TDelay::getHelp(void) const
{
  return helpstr;
}

const char** TDelay::getIcon(void) const
{
  return icon;
}

double TDelay::f( double t )
{
  double a1, a2;
  if( buf == 0 ) return 0;
  
  // TODO: add check for param change
  if( mdelay < cdelay ) 
    cdelay = mdelay;
  double v = cdelay / tdt;
  icd = int( v );
  v2 = v - icd; v1 = 1.0 - v2;
  
  //
  if( t < 1.3 * tdt )
    u00 = *in_so[0];
  buf->add( *in_so[0] );
  a1 = (*buf)[icd]; a2 = (*buf)[icd+1];
  if( t < cdelay )
    return u00;
  return v1 * a1 + v2 * a2;
}

int TDelay::do_preRun( int /*run_tp*/, int /*an*/,
                       int /*anx*/, int /*any*/, double /*atdt*/ )
{
  imd = int( mdelay / tdt );
  buf = new TCircBuf( imd );
  return 0;
}

int TDelay::postRun( int good )
{
  delete buf; buf = 0;
  TMiso::postRun( good );
  return 0;
}

int TDelay::startLoop( int acnx, int acny )
{
  double v;
  int rc = TMiso::startLoop( acnx, acny );
  buf->reset(); u00 = 0; 
  v = cdelay / tdt;
  icd = int( v );
  v2 = v - icd; v1 = 1.0 - v2;
  return rc;
}


int TDelay::registered = reg();

int TDelay::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TDelay::icon[] = {
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
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjiiijjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjjijjjjjjjjjjjjijjjjjjjjjj",
"jjjjjjjiiijjjjjjjjjjjijijjjjjjjj",
"jjjjjjjiiijjjjjjjjjjjjijjjjjjjjj",
"jjjjjjjijiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiiiiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiijiijjjjjjjjjjjjjjjjjjjjj",
"jjjjjjiijiijjjjjjjjjjjjjjjjjjjjj",
"jjiiiiiijiiiiiiiiiiiiiiiiiiiijjj",
"jjiiiiijjjiiiiiiiiiiiiiiiiiiijjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjjgjjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjgggjjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjjgggjjjjjjjjjjjjjj",
"jjjjj.j.j.jjjjggjggjjjjjjjjjjjjj",
"jjjjjj...jjjjjggjggjjjjjjjjjjjjj",
"jjjjjjj.jjjjjjggjggjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjggjggjjjjjjjjjjjjj",
"jjgggggggggggggjjjgggggggggggjjj",
"jjgggggggggggggjjjgggggggggggjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"
};

// end of tdelay.cpp

 
