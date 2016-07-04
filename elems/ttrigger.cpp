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
 "<h2>Only RS and (X) works for now!!!! </h2>\n"
 "Different kinds of triggers: <br>\n"
 "Inputs: u0 - Reset, u1 - Set, u2 - special, u3 - ?enable.<br>\n"
 "Ordinary input assumed 1 if more then <b>level1</b><br>\n"
 "Shmitt trigger resetted by signal u2 < <b>level0</b>.<br>\n"
 "If enabled (<b>useT0</b>), trigger performs autoreset after <b>t0</b>.<br>\n"
 "Have switches: <b> useEnable, usePulse, useMinus, useT0</b>.";

STD_CLASSINFO(TTrigger,clpElem );

CTOR(TTrigger,TMiso)
{
}


int TTrigger::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  cst = (bool)(out0_init); et = 0;
  return 1;
}

double TTrigger::f() noexcept
{
  if( useEnable && !in_ena ) { // disabled state
    if( usePulse ) {
      return 0;
    }
    if( cst ) {
      return 1;
    }
    return useMinus ? -1 : 0;
  };

  int sw = 0;
  if( in_s ) { // SET/RESET inputs
    sw = 1;
  } else {
    if( in_r ) {
      sw = -1;
    };
  };

  if( sw == 0 ) { // no R ot S signal : main work
    switch( (int)type ) {
      case tr_rs:
        break;
      case tr_shmitt:
        sw = in_x.lval();
        break;
      case tr_crise:
        sw = in_x.lval() ? 2 : 0;
        break;
      case tr_cdown:
        sw = in_x.lval() ? 2 : 0;
        break;
      case tr_clev:
        sw = in_x.lval() ? 2 : 0; break;
      default: break;
    };
  };

  if( useT0 && sw == 0 && cst == 1 ) {
    if( et >= t0 ) {
      sw = -1; et = 0;
    } else {
      et += ctdt;
    };
  };
  int act = 0;
  switch( sw ) {
    case 0:  break;
    case 1:  if( cst == 0 ) { act = 1; }
             cst = 1; et = 0;
             break;
    case -1: if( cst == 1 ) { act = -1; }
             cst = 0;
             break;
    case 2:  cst = !cst;  act = cst ? 1 : -1;
             if( cst ) { et = 0; }
             break;
    default: break;
  };
  if( usePulse ) {
    return act;
  }
  if( cst ) {
    return 1;
  }
  return useMinus ? -1 : 0;
}


DEFAULT_FUNCS_REG(TTrigger)


// end of ttrigger.cpp

