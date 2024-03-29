/***************************************************************************
                          trandtrigg.cpp  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2022 by atu
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

#include "miscfun.h"
#include "trandtrigg.h"


const char* TRandTrigg::helpstr = "<H1>TRandTrigg</H1>\n"
 "Random - deterministic trigger: <br>\n"
 "it sense change or level of input signal u[0],<br>\n"
 "than if forced state or good probability, change output signal.<br>\n"
 "Parametr <b>prob</b> defines probability of change.<br>\n"
 "Parametr <b>nforce</b> defines how often will be forced click. .<br>\n"
 "Parametrs <b>seed, seedType, addBaseSeed </b> controls random ganerator<br>\n"
 "Parameters: <b>useLevel, useForce, useLock, useZero</b>, - misc flags.";

STD_CLASSINFO(TRandTrigg,clpElem);

CTOR(TRandTrigg,TMiso)
{
}



int TRandTrigg::do_preRun()
{
  eff_seedType = seedType;
  if( seedType == asModel ) {
    getUpData( "seedType", &eff_seedType );
  };
  bseed = 0;
  if( addBaseSeed ) {
    getUpData( "seed", &bseed );
  };
  return 1;
}


int TRandTrigg::miso_startLoop( long acnx, long acny )
{
  ns = -1; u_old = 0; currOut = 0;
  if( ( eff_seedType == everyRun ) ||                // need to seed now
      ( eff_seedType == startLoop && acnx == 0 ) ||
      ( acnx == 0 && acny == 0)
    ) {
    if( seed == -1 || ( addBaseSeed && bseed == -1) ) {
      sseed = int( time(0) + acnx * 1001 + acny );
    } else {
      sseed = seed + bseed;
    }
    rng.set( sseed );
  };
  return 1;
}


double TRandTrigg::f() noexcept
{
  double du, uc = in_u;
  du = uc - u_old; u_old = uc;
  if( ns == -1 ) { du = 0; ns = 0; };
  if( in_force ) {
    ns = 0; currOut = !currOut;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( in_lock ) {
    // ns++;
    return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
  };
  if( ( useLevel && uc > 0.1 ) || ( !useLevel && fabs(du) > 0.5 ) ) {
    double rv = rng.flat( 0, 1 );
    if( rv <= prob || nforce <= 1 || ns >= nforce-1 ) {
      ns = 0; currOut = !currOut;
    } else {
      ns++;
    };
  };
  return ( currOut ) ? 1 : ( useZero ? 0 : -1 );
}

DEFAULT_FUNCS_REG(TRandTrigg)


// end of trandtrigg.cpp


