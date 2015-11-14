/***************************************************************************
                      cmdlistview.h - common base for list-alike viewvs
                             -------------------
    begin                : 2015.10.31
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
#include "labowin.h"
#include "addelemdia.h"
#include "miscfun.h"
#include "cmdlistview.h"
#include "commonsubwin.h"


CmdListView::CmdListView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdView( a_par, a_mod )
{
  auto lay = new QVBoxLayout( this );
  lv = new QListView( this );
  lay->addWidget( lv );

  init_base_actions();

  int em = LaboWin::Em();
  setFixedWidth( 12*em );

  lv->setContextMenuPolicy( Qt::ActionsContextMenu );

  lv->setModel( storage );

  auto selMod = lv->selectionModel();
  QModelIndex s_idx = storage->index( 0, 0, QModelIndex() );
  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  connect( selMod, &QItemSelectionModel::currentChanged, this, &CmdListView::viewChanged );
}

HolderData* CmdListView::getSelObj() const
{
  if( !lv ) {
    return nullptr;
  }

  QItemSelectionModel *selMod = lv->selectionModel();
  if( !selMod ) {
    return nullptr;
  }

  QModelIndex cs = selMod->currentIndex();
  if( !cs.isValid() ) {
    return nullptr;
  }
  // HolderData *oxx = static_cast<HolderData*>( cs.internalPointer() );
  QString nm =  cs.data( Qt::DisplayRole ).toString(); // TODO: make better
  HolderData *ob = storage->getObjT<HolderData*>( nm );
  return ob;
}

void CmdListView::handleSelChange()
{

}

QModelIndex CmdListView::currentIndex() const
{
  if( !lv ) {
    return QModelIndex();
  }
  return lv->currentIndex();
}


void CmdListView::init_base_actions()
{
  auto a = new QAction( QIcon::fromTheme("document-properties"), "&Edit", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(editObj()) );

  a = new QAction( QIcon::fromTheme("list-add"), "&New", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(addObj()) );

  a = new QAction( QIcon::fromTheme("list-remove"), "&Delete", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(delObj()) );

  a = new QAction( QIcon::fromTheme("edit-cut"), "Cu&t", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(cutObj()) );

  a = new QAction( QIcon::fromTheme("edit-copy"), "&Copy", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(copyObj()) );

  a = new QAction( QIcon::fromTheme("edit-paste"), "&Paste", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(pasteObj()) );

  a = new QAction( QIcon::fromTheme("edit-rename"), "Rename", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(renameObj()) );

  a = new QAction( QIcon( ":icons/edit-clone.png" ), "Clone", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(cloneObj()) );

  a = new QAction( QIcon( ":icons/info-obj.png" ), "Info", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(infoObj()) );

  a = new QAction( QIcon::fromTheme("view-list-tree"), "Show Tree", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showTreeObj()) );

  a = new QAction( "", this ); a->setSeparator( true );
  lv->addAction( a );

  connect( lv, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editObj()) );
}

