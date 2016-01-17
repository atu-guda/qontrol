/***************************************************************************
                          ttrigger.cpp  -  description
                             -------------------
    begin                : Sun Sep 3 2000
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


int TTrigger::do_startLoop( int acnx, int acny )
{
  TMiso::do_startLoop( acnx, acny );
  cst = (int)useInit1; u2_old = et = 0;
  return 1;
}

double TTrigger::f() noexcept
{
  int sw, act;
  int ui_r = ( in_r > level1 );
  int ui_s = ( in_s > level1 );
  int ui_x = ( in_x > level1 );
  int ui_ena = ( in_ena > level1 );
  double du;

  if( t > tdt/2 ) {
    du = in_s - u2_old;
  } else {
    du = 0;
  };
  u2_old = in_s;

  if( useEnable && ! ui_ena ) { // disabled state
    if( usePulse ) {
      return 0;
    }
    if( cst ) {
      return 1;
    }
    return useMinus ? -1 : 0;
  };

  sw = 0;
  if( ui_s ) { // SET/RESET inputs
    sw = 1;
  } else {
    if( ui_r ) {
      sw = -1;
    };
  };

  if( sw == 0 ) { // no R ot S signal : main work
    switch( (int)type ) {
      case tr_rs:
        break;
      case tr_shmitt:
        if( in_x > level1 ) {
          sw =  1; break;
        };
        if( in_x < level0 ) {
          sw = -1; break;
        };
        break;
      case tr_crise:
        if( du >  level1 )
          sw = 2;
        break;
      case tr_cdown:
        if( du < -level1 )
          sw = 2;
        break;
      case tr_clev:
        sw = ui_x ? 2 : 0; break;
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
  if( cst )
    return 1;
  return useMinus ? -1 : 0;
}


DEFAULT_FUNCS_REG(TTrigger)


// end of ttrigger.cpp

