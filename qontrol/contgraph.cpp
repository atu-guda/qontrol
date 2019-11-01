/***************************************************************************
                          contgraph.cpp  - implementation of ContGraph,
                          containter for TGraph
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2019 by atu
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

#include "tgraph.h"
#include "contgraph.h"

using namespace std;


const char* ContGraph::helpstr = "<H1>ContGraph</H1>\n"
 "Container of ouput arrays";

STD_CLASSINFO(ContGraph,clpSpecial);

CTOR(ContGraph,LinkedObj)
{
  allowed_types = QSL("TGraph,+SPECIAL");
  needReadInputsRecurse = true;
}


int ContGraph::do_postRun( int /*good*/ )
{
  // readAllInputs(); // for labels...-> moved to prepare
  return 1;
}

DEFAULT_FUNCS_REG(ContGraph)


// end of contgraph.cpp

