/***************************************************************************
                          schemeview.cpp - view for Schems list
                             -------------------
    begin                : 2015.10.04
    copyright            : (C) 2015-2015 by atu
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
#include "schemeview.h"
#include "labowin.h"


SchemeView::SchemeView( HolderData *a_mod, LaboView *par )
  : QListView( par ), mod( a_mod ), laboview( par )
{

  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 190, 200, 200 ) );
  setPalette( s_pal );

  int em = LaboWin::Em();
  setFixedWidth( 12*em );
  // setPreferredWidth( 12*em );

  setContextMenuPolicy( Qt::ActionsContextMenu );

  setModel( mod );
}

void SchemeView::init_actions()
{
  act_new = new QAction( QIcon::fromTheme("list-add"), "New", this );
  addAction( act_new );
  connect( act_new, SIGNAL(triggered()), laboview, SLOT(newScheme()) );

  act_del = new QAction( QIcon::fromTheme("list-remove"), "Delete", this );
  addAction( act_del );
  connect( act_del, SIGNAL(triggered()), laboview, SLOT(delScheme()) );

  act_edit = new QAction( QIcon::fromTheme("document-properties"), "Edit", this );
  addAction( act_edit );
  connect( act_edit, SIGNAL(triggered()), laboview, SLOT(editScheme()) );

  act_rename = new QAction( "Rename", this );
  addAction( act_rename );
  connect( act_rename, SIGNAL(triggered()), laboview, SLOT(renameScheme()) );

  act_clone = new QAction( "Clone", this );
  addAction( act_clone );
  connect( act_clone, SIGNAL(triggered()), laboview, SLOT(cloneScheme()) );

  connect( this, SIGNAL(doubleClicked(QModelIndex)), laboview, SLOT(editScheme()) );

}

