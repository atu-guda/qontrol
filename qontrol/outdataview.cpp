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
#include "labowin.h"
#include "addelemdia.h"
#include "doubletable.h"
#include "miscfun.h"
#include "outdataview.h"
#include "commonsubwin.h"
#include "tgraph.h"


OutDataView::OutDataView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdView( a_par, a_mod )
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

  auto selmod = lv->selectionModel();
  connect( selmod, &QItemSelectionModel::currentChanged, this, &OutDataView::viewChanged );
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
  connect( a, SIGNAL(triggered()), this, SLOT(addObj()) );

  a = new QAction( QIcon::fromTheme("list-remove"), "&Delete", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(delObj()) );

  a = new QAction( QIcon::fromTheme("document-properties"), "&Edit", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(editObj()) );

  a = new QAction( QIcon::fromTheme("edit-cut"), "Cu&t", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(cutObj()) );

  a = new QAction( QIcon::fromTheme("edit-copy"), "&Copy", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(copyObj()) );

  a = new QAction( QIcon::fromTheme("edit-paste"), "&Paste", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(pasteObj()) );

  a = new QAction( "Rename", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(renameObj()) );

  a = new QAction( "Clone", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(cloneObj()) );

  a = new QAction( "Info", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(infoObj()) );

  a = new QAction( "Show Tree", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showTreeObj()) );

  a = new QAction( "", this ); a->setSeparator( true );
  lv->addAction( a );

  a = new QAction( "&Add to plot", this );
  lv->addAction( a  );
  connect( a, SIGNAL(triggered()), this, SLOT(addToPlot()) );

  a = new QAction( "D&ump", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(exportObj()) );

  a = new QAction( "&Show data", this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showDataObj()) );

  connect( lv, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editObj()) );
}

bool OutDataView::addObj()
{
  QString tgt_nm = par->getSelNameInView( "sview" );
  QString out_nm;

  if( !tgt_nm.isEmpty() ) {
    out_nm = QSL("out_") % tgt_nm;
  } else {
    tgt_nm = QSL("t");
    out_nm = QSL("out_t");
  };
  sugg_name = out_nm;

  if( ! CmdView::addObj() ) {
    return false;
  }

  QString nm = getLastObjName();
  HolderData *ob = storage->getObj( nm );
  if( !ob ) {
    return false;
  }

  ob->setData( QSL("name"), tgt_nm ); // really source
  ::editObj( this, ob );
  ob->reset();
  emit viewChanged();
  return true;
}

bool OutDataView::showDataObj()
{
  DatasInfo di;

  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  int k = arr->fillDatasInfo( &di );
  if( !k ) {
    qWarning() << "fail to fill info about output array " <<  arr->getFullName() << WHE;
    return false;
  }

  // calculate statistical data TODO: separate struct and func (or/and in TOutArr)
  arr->calc_stat( true, true ); // ensure all and fresh data
  QString sinf = arr->getAllStats( ";\n" );


  auto dia = new QDialog( this );
  dia->setWindowTitle( QString("Data array: ") + di.title );
  auto lay = new QGridLayout( dia );

  auto dmod = new DoubleTableModel( di, dia );
  auto dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lay->addWidget( dtv, 0, 0 );

  auto lab = new QLabel( sinf, dia );
  lab->setTextInteractionFlags( Qt::TextSelectableByMouse
       | Qt::TextSelectableByKeyboard );
  lay->addWidget( lab, 0, 1 );

  auto bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  lay->addWidget( bt_ok, 1, 0, 1, 2 );
  dia->setLayout( lay );

  dia->exec();
  delete dia;
  return true;
}

bool OutDataView::exportObj()
{
  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  QString fnq = QFileDialog::getSaveFileName( this, tr("Export data"),
       arr->objectName() % QSL(".dat"),
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() ) {
    return false;
  }
  arr->dump( fnq, " " );
  return true;
}

bool OutDataView::addToPlot()
{
  QString nm_o = getSelName();
  if( nm_o.isEmpty() ) {
    return false;
  }

  TGraph *gra = qobject_cast<TGraph*>( par->getSelectedInView( "plots_view" ) );
  if( !gra ) {
    qWarning() << "No selected plot found for " << nm_o << WHE;
    return false;
  }

  int rc = gra->addOutArr( nm_o ); // complex fun
  if( !rc ) {
    qWarning() << "Fail to add output array " << nm_o << " to plot " << gra->objectName() << WHE;
    return false;
  }
  // TODO: model reset
  emit viewChanged();
  return true;
}

