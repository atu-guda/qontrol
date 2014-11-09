/***************************************************************************
                          inputparams.cpp
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2000-2014 by atu
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

#include "inputparams.h"

using namespace std;

const char* InputParams::helpstr = "<H1>InputParams</H1>\n"
 "Hold parametric inputs of element";

STD_CLASSINFO(InputParams,clpSpecial | clpContainer);

CTOR(InputParams,TDataContainer)
{
  allowed_types = "InputParam,+SPECIAL";
}

InputParams::~InputParams()
{
}



void InputParams::prepare()
{
  srcs.clear(); targets.clear();
  srcs_p.clear(); targets_p.clear();
  int flg;
  for( auto c: children() ) {
    InputParam* inp = qobject_cast<InputParam*>(c);
    if( ! inp )
      continue;
    flg = inp->getOnlyFirst();
    // force only-first of noRunChange target
    if( inp->getTargetFlag() & efNoRunChange )
      flg = 1;

    if( flg ) {
      srcs_p.push_back( inp->caddr() );
      targets_p.push_back( inp->targ_addr() );
    } else {
      srcs.push_back( inp->caddr() );
      targets.push_back( inp->targ_addr() );
    }
  }
}

int InputParams::apply()
{
  int n = targets.size();
  int nmod = 0;
  for( int i=0; i<n; ++i ) {
    *targets[i] = *srcs[i];
    ++nmod;
  }
  return nmod;
}

int InputParams::apply_pre()
{
  int n = targets_p.size();
  int nmod = 0;
  for( int i=0; i<n; ++i ) {
    *targets_p[i] = *srcs_p[i];
    ++nmod;
  }
  return nmod;
}


DEFAULT_FUNCS_REG(InputParams)


// end of inputparams.cpp

