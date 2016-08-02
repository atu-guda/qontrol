/***************************************************************************
                          tdelay.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2016 by atu
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
#include "circbuf.h"

const char* TDelay::helpstr = "<H1>TDelay</H1>\n"
 "Delay signal for given time: <br>\n"
 "Have 2 parameters: <b>mdelay</b> - maximum delay, <br>\n"
 "<b>cdelay</b> - current delay, can changed at any time, but <= <b>mdelay</b>.";

STD_CLASSINFO(TDelay,clpElem );

CTOR(TDelay,TMiso)
{
}

double TDelay::f() noexcept
{
  double a1, a2;

  if( prm_mod ) {
    if( mdelay < cdelay ) {
      cdelay = (double)mdelay;
    }
    double v = cdelay / ctdt;
    icd = size_t( v );
    v2 = v - icd; v1 = 1.0 - v2;
    prm_mod = 0;
  }

  double cu = in_u;

  buf.push_back( cu );
  a1 = buf[icd]; a2 = buf[icd+1];
  aver = buf.aver();
  if( ct < cdelay ) {
    return out0_init;
  }
  return v1 * a1 + v2 * a2;
}

int TDelay::do_preRun()
{
  imd = size_t( ceil( mdelay / ctdt ) );
  buf.resize( imd, out0_init );
  return 1;
}

int TDelay::do_postRun( int /*good*/ )
{
  buf.clear();
  buf.shrink_to_fit();
  return 1;
}

int TDelay::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  if( cdelay > mdelay ) {
    cdelay = mdelay;
  }
  double v;
  buf.reset();
  v = cdelay / ctdt;
  icd = size_t( v );
  v2 = v - icd; v1 = 1.0 - v2;
  return 1;
}


DEFAULT_FUNCS_REG(TDelay)


// end of tdelay.cpp


