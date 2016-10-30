/***************************************************************************
    datapool.cpp - container for misc data
                             -------------------
    begin                : 2016.10.30
    copyright            : (C) 2016-2016 by atu
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

#include "datapool.h"

using namespace std;

const char* DataPool::helpstr = "<H1>DataPool</H1>\n"
 "<p>DataPool: misc data to add to model, simulation....</p>";

STD_CLASSINFO(DataPool,0);

CTOR(DataPool,LinkedObj)
{
  allowed_types = "HolderValue,DataPool";
}

// no real actions: only data storage

DEFAULT_FUNCS_REG(DataPool)



// end of datapool.cpp

