/***************************************************************************
                          simulview.cpp - view for simulations
                             -------------------
    begin                : 2014.10.14
    copyright            : (C) 2014-2015 by atu
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

#include "dataset.h"
#include "simulview.h"
#include "labowin.h"


SimulView::SimulView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdListView( a_mod, a_par )
{
  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 255, 200, 200 ) );
  setPalette( s_pal );
}

void SimulView::init_actions()
{
  auto a = new QAction( QIcon::fromTheme("checkmark"), "set Active", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(setActive()) );
}

bool SimulView::setActive()
{
  Simulation *sim = qobject_cast<Simulation*>( getSelObj() );
  if( !sim ) {
    return false;
  }
  storage->setActiveObj( sim->objectName() );
  emit viewChanged();
  return true;
}

