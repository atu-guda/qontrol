/***************************************************************************
                          ttrigger.cpp  -  description
                             -------------------
    begin                : Sun Sep 3 2000
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

#include "ttrigger.h"

const char* TTrigger::helpstr = "<H1>TTrigger</H1>\n"
 "Different kinds of triggers: <br>\n"
 "Inputs: u0 - Reset, u1 - Set, u2 - special, u3 - ?enable.<br>\n"
 "Ordinary input assumed 1 if more then <b>level1</b><br>\n"
 "Shmitt trigger resetted by signal u2 < <b>level0</b>.<br>\n"
 "If enabled (<b>useT0</b>), trigger performs autoreset after <b>t0</b>.<br>\n"
 "Have 5 switches: <b>useInit1, useEnable, usePulse, useMinus, useT0</b>.";

STD_CLASSINFO(TTrigger,clpElem );

CTOR(TTrigger,TMiso)
{
}


int TTrigger::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  cst = (int)useInit1; u2_old = et = 0;
  return 0;
}

double TTrigger::f( double t )
{
  int i, ui[4], sw, act;
  double du;
  if( t > tdt/2 ) {
    du = *in_so[2] - u2_old;
  } else {
    du = 0;
  };
  u2_old = *in_so[2];
  for( i=0; i<4; i++ )
    ui[i] = ( *in_so[i]>level1 );
  if( useEnable && ui[3] < 1 ) {
    if( usePulse ) return 0;
    if( cst ) return 1;
    return useMinus ? -1 : 0;
  };
  sw = 0;
  if( ui[1] ) { // SET/RESET inputs 
    sw = 1; 
  } else {
    if( ui[0] ) { 
      sw = -1; 
    };
  };
  if( sw == 0 ) {
    switch( (int)type ) {
      case 0: break;
      case 1: if( *in_so[2] > level1 ) { sw = 1; break; };
	      if( *in_so[2] < level0 ) { sw = -1; break; };
	      break;
      case 2: if( du > 0.5 ) sw = 2; 
	      break;  
      case 3: if( du < -0.5 ) sw = 2;
	      break;
      case 4: sw = ui[2] ? 2 : 0; break;
      default: break;
    };
  };
  if( useT0 && sw == 0 && cst == 1 ) {
    if( et >= t0 ) {
      sw = -1; et = 0;
    } else {
      et += tdt;
    };
  };
  act = 0;
  switch( sw ) {
    case 0:  break;
    case 1:  if( cst == 0 ) act = 1;
	     cst = 1; et = 0; break;
    case -1: if( cst == 1 ) act = -1;
	     cst = 0; break;
    case 2:  cst = !cst;  act = cst ? 1 : -1;
	     if( cst ) et = 0; break;
    default: break;
  };
  if( usePulse ) 
    return act;
  if( cst ) return 1;
  return useMinus ? -1 : 0;
}


DEFAULT_FUNCS_REG(TTrigger)


// end of ttrigger.cpp

