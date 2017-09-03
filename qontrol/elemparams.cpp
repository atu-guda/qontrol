/***************************************************************************
  elemparams.h - element parameters
                             -------------------
    begin                : 2017.09.03 (copy from tmiso.cpp)
    copyright            : (C) 2017-2017 by atu
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

#include "elemparams.h"

using namespace  std;

// ------------------------ ElemParams --------------------------

const char* ElemParams::helpstr = "<h1>ElemParams</h1>\n"
 "<p>Structure to hold misc auxilary element parameters.</p>\n";

STD_CLASSINFO(ElemParams,clpSpecial);

CTOR(ElemParams,TDataSet)
{
}

DEFAULT_FUNCS_REG(ElemParams);

int ElemParams::do_postFileRead()
{
  if( !par || cvtCount > 0 ) { // only on conversion
    return 0;
  }

  // convert from old
  vis_x       = par->getDataD( QSL("vis_x"), 1 );
  vis_y       = par->getDataD( QSL("vis_y"), 1 );
  noCalcStart = par->getDataD( QSL("noCalcStart"), 0 );
  locked      = par->getDataD( QSL("locked"), 0 );
  ignored     = par->getDataD( QSL("ignored"), 0 );
  onlyFirst   = par->getDataD( QSL("onlyFirst"), 0 );
  onlyLast    = par->getDataD( QSL("onlyLast"), 0 );
  flip        = par->getDataD( QSL("flip"), 0 );
  noIcon      = par->getDataD( QSL("noIcon"), 0 );
  showBaloon  = par->getDataD( QSL("showBaloon"), 0 );
  return 1;
}

