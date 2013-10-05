/***************************************************************************
                          simul.cpp  -  description
                             -------------------
    begin                : 2013.10.05
    copyright            : (C) 2013-2013 by atu
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

#include "simul.h"

using namespace std;

const char* Simulation::helpstr = "<H1>Simulation</H1>\n"
 "Simulation data and description";

STD_CLASSINFO(Simulation,clpSpecial | clpContainer);

CTOR(Simulation,TDataSet)
{
  allow_add = allowParam;
  // allowed_types = ""; // double + inputs?
}

Simulation::~Simulation()
{
}



DEFAULT_FUNCS_REG(Simulation)


// end of simul.cpp

