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
 "Container of ouput arrays";

STD_CLASSINFO(ContOut,clpSpecial | clpContainer);

CTOR(ContOut,TDataContainer)
{
  allowed_types = "TOutArr";
}

ContOut::~ContOut()
{
}

DEFAULT_FUNCS_REG(ContOut)

void ContOut::takeAllVals( int level )
{
  for( auto o : children() ) {
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) { // unlikely
      continue;
    }
    arr->take_val( level );
  }
}

// end of contout.cpp

