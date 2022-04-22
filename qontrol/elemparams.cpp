/***************************************************************************
  elemparams.h - element parameters
                             -------------------
    begin                : 2017.09.03 (copy from tmiso.cpp)
    copyright            : (C) 2017-2022 by atu
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

// int ElemParams::do_postFileRead()
// {
//   if( !par || cvtCount > 0 ) { // only on conversion
//     return 0;
//   }
//
//   // conversion may be here
//
//   return 1;
// }

