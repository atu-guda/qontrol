/***************************************************************************
                          graphdataview.cpp - view for plots
                             -------------------
    begin                : 2014.11.15
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

#include "dataset.h"
#include "graphdataview.h"
#include "labowin.h"


GraphDataView::GraphDataView( HolderData *a_mod, LaboView *par )
  : QListView( par ), mod( a_mod ), laboview( par )
{

  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 96,252,192 ) );
  setPalette( s_pal );

  int em = LaboWin::labowin->getEm();
  setFixedWidth( 12*em );

  setContextMenuPolicy( Qt::ActionsContextMenu );

  setModel( mod );
}

void GraphDataView::init_actions()
{
  act_new = new QAction( QIcon::fromTheme("list-add"), "&New", this );
  addAction( act_new );
  connect( act_new, SIGNAL(triggered()), laboview, SLOT(newGraph()) );

  act_del = new QAction( QIcon::fromTheme("list-remove"), "&Delete", this );
  addAction( act_del );
  connect( act_del, SIGNAL(triggered()), laboview, SLOT(delGraph()) );

  act_edit = new QAction( QIcon::fromTheme("document-properties"), "&Edit", this );
  addAction( act_edit );
  connect( act_edit, SIGNAL(triggered()), laboview, SLOT(editGraph()) );

  act_rename = new QAction( "Rename", this );
  addAction( act_rename );
  connect( act_rename, SIGNAL(triggered()), laboview, SLOT(renameGraph()) );

  act_setActive = new QAction( QIcon::fromTheme("checkmark"), "set &Active", this );
  addAction( act_setActive );
  //connect( act_setActive, SIGNAL(triggered()), laboview, SLOT(setActiveGraph()) );

  act_show = new QAction( "&Show", this );
  addAction( act_show );
  connect( act_show, SIGNAL(triggered()), laboview, SLOT(showGraph()) );

  act_dump = new QAction( "D&ump", this );
  addAction( act_dump );
  connect( act_dump, SIGNAL(triggered()), laboview, SLOT(exportGraphData()) );

  act_showdata = new QAction( "&Show data", this );
  addAction( act_showdata );
  connect( act_showdata, SIGNAL(triggered()), laboview, SLOT(showGraphData()) );

  act_clone = new QAction( "Clone", this );
  addAction( act_clone );
  connect( act_clone, SIGNAL(triggered()), laboview, SLOT(cloneGraph()) );

  connect( this, SIGNAL(doubleClicked(QModelIndex)), laboview, SLOT(editGraph()) );

}
