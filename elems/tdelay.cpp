/***************************************************************************
                          tdelay.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2013 by atu
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

#include "tdelay.h"
#include "miscfun.h" 
#include "tdelay.h"

const char* TDelay::helpstr = "<H1>TDelay</H1>\n"
 "Delay signal for given time: <br>\n"
 "Have 2 parameters: <b>mdelay</b> - maximum delay, <br>\n"
 "<b>cdelay</b> - current delay, can changed at any time, but <= <b>mdelay</b>.";

STD_CLASSINFO(TDelay,clpElem );

CTOR(TDelay,TMiso)
{
  mdelay = 1.5; cdelay = 1; v1 = v2 = u00 = 0; buf = 0; 
}

TDelay::~TDelay()
{
  delete buf; buf = 0; // in case of do_postRun missing
}


double TDelay::f( double t )
{
  double a1, a2;
  if( buf == 0 ) return 0;
  
  if( prm_mod ) {
    if( mdelay < cdelay ) 
      cdelay = (double)mdelay;
    double v = cdelay / tdt;
    icd = int( v );
    v2 = v - icd; v1 = 1.0 - v2;
  }
  
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

int TDelay::do_postRun( int /*good*/ )
{
  delete buf; buf = 0;
  return 0;
}

int TDelay::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  double v;
  buf->reset(); u00 = 0; 
  v = cdelay / tdt;
  icd = int( v );
  v2 = v - icd; v1 = 1.0 - v2;
  return 0;
}


DEFAULT_FUNCS_REG(TDelay)


// end of tdelay.cpp

 
