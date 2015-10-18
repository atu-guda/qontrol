/***************************************************************************
                          outdataview.cpp - view for outs
                             -------------------
    begin                : 2014.11.15
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
#include "outdataview.h"
#include "labowin.h"


OutDataView::OutDataView( HolderData *a_mod, LaboView *par )
  : QListView( par ), mod( a_mod ), laboview( par )
{

  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 120,220,252 ) );
  setPalette( s_pal );

  int em = LaboWin::Em();
  setFixedWidth( 12*em );

  setContextMenuPolicy( Qt::ActionsContextMenu );

  setModel( mod );
}

void OutDataView::init_actions()
{
  act_new = new QAction( QIcon::fromTheme("list-add"), "&New", this );
  addAction( act_new );
  connect( act_new, SIGNAL(triggered()), laboview, SLOT(addOut()) );

  act_del = new QAction( QIcon::fromTheme("list-remove"), "&Delete", this );
  addAction( act_del );
  connect( act_del, SIGNAL(triggered()), laboview, SLOT(delOut()) );

  act_edit = new QAction( QIcon::fromTheme("document-properties"), "&Edit", this );
  addAction( act_edit );
  connect( act_edit, SIGNAL(triggered()), laboview, SLOT(editOut()) );

  act_rename = new QAction( "Rename", this );
  addAction( act_rename );
  connect( act_rename, SIGNAL(triggered()), laboview, SLOT(renameOut()) );

  act_graphaddout = new QAction( "&Add to plot", this );
  addAction( act_graphaddout  );
  connect( act_graphaddout, SIGNAL(triggered()), laboview, SLOT(graphAddOut()) );

  act_dump = new QAction( "D&ump", this );
  addAction( act_dump );
  connect( act_dump, SIGNAL(triggered()), laboview, SLOT(exportOut()) );

  act_showdata = new QAction( "&Show data", this );
  addAction( act_showdata );
  connect( act_showdata, SIGNAL(triggered()), laboview, SLOT(showOutData()) );

  connect( this, SIGNAL(doubleClicked(QModelIndex)), laboview, SLOT(editOut()) );

}
