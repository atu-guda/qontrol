/***************************************************************************
                          contout.cpp  - inplementation of ContOut,
                          containter for TOutArr
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2014 by atu
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

#include "toutarr.h"
#include "contout.h"

using namespace std;


const char* ContOut::helpstr = "<H1>ContOut</H1>\n"
 "Container of output arrays";

STD_CLASSINFO(ContOut,clpSpecial | clpContainer);

CTOR(ContOut,TDataContainer)
{
  allowed_types = "TOutArr,+SPECIAL";
  vo.reserve( 64 );
}

ContOut::~ContOut()
{
}

int ContOut::preRun( int run_tp, int an, int anx, int any, double adt )
{
  vo.clear();

  // fill array for pure TOutArr entries: for fast access later
  for( auto o : children() ) {
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) { // unlikely
      continue;
    }
    vo.push_back( arr );
    if( ! arr->preRun( run_tp, an, anx, any, adt ) ) {
      vo.clear();
      return 0;
    }
  }
  return 1;
}

int ContOut::postRun( int good )
{
  for( auto arr : vo ) {
    arr->postRun( good );
  }
  vo.clear();
  return 1;
}

int ContOut::startLoop( int acnx, int acny )
{
  for( auto arr : vo ) {
    if( ! arr->startLoop( acnx, acny ) ) {
      return 0;
    }
  }

  return 1;
}

int ContOut::endLoop( int acnx, int acny )
{
  for( auto arr : vo ) {
    arr->endLoop( acnx, acny );
  }

  return 1;
}


void ContOut::takeAllVals()
{
  for( auto arr : vo ) {
    arr->take_val();
  }
}


DEFAULT_FUNCS_REG(ContOut)

// end of contout.cpp

