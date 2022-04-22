/***************************************************************************
                          simul.cpp  -  description
                             -------------------
    begin                : 2013.10.05
    copyright            : (C) 2013-2022 by atu
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

STD_CLASSINFO(Simulation,clpSpecial);

CTOR(Simulation,LinkedObj)
{
  show_active = true;
  // allowed_types = "double"; // double + inputs? TODO: check alias
  allowed_types = QSL("HolderValue,ParamDouble,DataPool"); // double + inputs? TODO: check alias
  // do_post_set(); // need recalc BUG: not here, need common
}


QIcon Simulation::getIcon() const
{
  int xtype = runType;
  if( xtype >= 3 ) { // bad type
    xtype = 3;
  }
  const QString iconName = QSL( ":icons/elm_simul_" ) + QSN(xtype) + QSL(".png");
  QIcon el_ico( iconName );
  return el_ico;
}


void Simulation::do_post_set()
{
  LinkedObj::do_post_set();
  tdt.set( T / N );
  // qWarning() << "T= " << T << " N= " << N << " tdt= " << tdt << NWHE;

  n1_eff = n2_eff = 1;
  if( runType > Simulation::runSingle ) {
    n1_eff = (long)N1;
  }
  if( runType > Simulation::runLoop ) {
    n2_eff = (long)N2;
  }

  n_tot = (long)N * n1_eff * n2_eff;
}


DEFAULT_FUNCS_REG(Simulation)

// ------------------ container of Simulations --------------


const char* ContSimul::helpstr = "<H1>ContSimul</H1>\n"
 "Container of simulations";

STD_CLASSINFO(ContSimul,clpSpecial);

CTOR(ContSimul,TDataSet)
{
  allowed_types = "Simulation,+SPECIAL";
}

ContSimul::~ContSimul()
{
}

DEFAULT_FUNCS_REG(ContSimul)


// end of simul.cpp

