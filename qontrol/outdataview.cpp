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
  : CmdView( par, a_mod ), laboview( par )
{
  auto lay = new QVBoxLayout( this );
  lv = new QListView( this );
  lay->addWidget( lv );

  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 120,220,252 ) );
  setPalette( s_pal );

  int em = LaboWin::Em();
  setFixedWidth( 12*em );

  lv->setContextMenuPolicy( Qt::ActionsContextMenu );

  lv->setModel( storage );
}

HolderData* OutDataView::getSelObj() const
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
  // qWarning() << "nm= " << nm << " xnm = " << oxx->getFullName() << WHE;
  return ob;
}

void OutDataView::handleSelChange()
{

}

QModelIndex OutDataView::currentIndex() const
{
  if( !lv ) {
    return QModelIndex();
  }
  return lv->currentIndex();
}


void OutDataView::init_actions()
{
  auto a = new QAction( QIcon::fromTheme("list-add"), "&New", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), laboview, SLOT(addOut()) );

  a = new QAction( QIcon::fromTheme("list-remove"), "&Delete", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(delObj()) );

  a = new QAction( QIcon::fromTheme("document-properties"), "&Edit", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(editObj()) );

  a = new QAction( "Rename", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(renameObj()) );

  a = new QAction( "&Add to plot", this );
  lv->addAction( a  );
  connect( a, SIGNAL(triggered()), laboview, SLOT(graphAddOut()) );

  a = new QAction( "D&ump", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), laboview, SLOT(exportOut()) );

  a = new QAction( "&Show data", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), laboview, SLOT(showOutData()) );

  connect( lv, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editObj()) );

}
