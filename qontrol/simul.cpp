/***************************************************************************
                          simul.cpp  -  description
                             -------------------
    begin                : 2013.10.05
    copyright            : (C) 2013-2015 by atu
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

CTOR(Simulation,LinkedObj)
{
  show_active = true;
  // allowed_types = "double"; // double + inputs? TODO: check alias
  allowed_types = "HolderValue"; // double + inputs? TODO: check alias
}

Simulation::~Simulation()
{
}

QIcon Simulation::getIcon() const
{
  int xtype = runType;
  if( xtype >= 3 ) { // bad type
    xtype = 3;
  }
  QString iconName = QString( ":icons/elm_simul_" ) + QSN(xtype) + ".png";
  QIcon el_ico( iconName );
  return el_ico;
}


void Simulation::do_post_set()
{
  LinkedObj::do_post_set();

  n1_eff = n2_eff = 1;
  if( runType > Simulation::runSingle ) {
    n1_eff = (int)N1;
  }
  if( runType > Simulation::runLoop ) {
    n2_eff = (int)N2;
  }

  n_tot = N * n1_eff * n2_eff;
}


DEFAULT_FUNCS_REG(Simulation)

// ------------------ container of Simulations --------------


const char* ContSimul::helpstr = "<H1>ContSimul</H1>\n"
 "Container of simulations";

STD_CLASSINFO(ContSimul,clpSpecial | clpContainer);

CTOR(ContSimul,TDataSet)
{
  allowed_types = "Simulation,+SPECIAL";
}

ContSimul::~ContSimul()
{
}

DEFAULT_FUNCS_REG(ContSimul)


// end of simul.cpp

