/***************************************************************************
  laboview.cpp  -  description
  begin                : Mon Jul 31 16:51:57 EEST 2000
  copyright            : (C) 2000-2015 by atu
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

#include <cmath>
// include files for Qt
#include <QtWidgets>


// application specific includes
#include "miscfun.h"
#include "laboview.h"
#include "labodoc.h"
#include "labowin.h"
#include "structview.h"
#include "outdataview.h"
#include "graphdataview.h"
#include "simulview.h"
#include "schemeview.h"
#include "statusmodel.h"
#include "doubletable.h"
#include "runview.h"
#include "mglview.h"
#include "simulview.h"
#include "datawidget.h"
#include "addelemdia.h"
#include "miscfun.h"

using namespace std;


LaboView::LaboView( LaboDoc* pDoc, QWidget *parent )
: CommonSubwin( parent, pDoc, QString() ),
  root( doc->getRoot() ),
  model( root->getElemT<TModel*>( "model" ) ),
  schems( model->getElemT<ContScheme*>( "schems" ) ),
  main_s( schems->getElemT<Scheme*>("main_s") ),
  outs( model->getElemT<ContOut*>( "outs" ) ),
  plots( model->getElemT<ContGraph*>( "plots" ) ),
  sims( model->getElemT<ContSimul*>( "sims" ) )
{
  main_win = true;
  title_prefix = QSL("model");

  em = LaboWin::Em();

  // auto vlay = new QVBoxLayout( this );
  // vlay->setContentsMargins( 2, 1, 2, 2 );

  auto main_part = new QWidget( this );
  setCentralWidget( main_part );

  setAttribute(Qt::WA_DeleteOnClose);


  auto grLay = new QGridLayout( main_part );
  grLay->setContentsMargins( 2, 2, 2, 2 );

  scrollArea = new QScrollArea( main_part );

  outs_view = new OutDataView( outs, this );
  outs_selmod = outs_view->selectionModel();

  plots_view = new GraphDataView( plots, this );
  plots_selmod = plots_view->selectionModel();

  sims_view = new SimulView( sims, this );

  schems_view = new SchemeView( schems, this );

  sview = new StructView( this, main_s, this, outs_view );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  stam = new StatusModel( this, this );

  grLay->addWidget( scrollArea, 0, 0, 2, 1 );
  grLay->addWidget( outs_view, 0, 1 );
  grLay->addWidget( plots_view, 0, 2 );
  grLay->addWidget( sims_view, 1, 1 );
  grLay->addWidget( schems_view, 1, 2 );

  main_part->setLayout( grLay );

  // vlay->addWidget( main_part );
  // vlay->addWidget( stam );
  // setLayout( vlay );
  setStatusBar( stam );

  // default: select object 0
  selectOut();
  selectGraph();
  selectSimul();
  // selectSheme();

  connect( this, &LaboView::viewChanged, this, &LaboView::updateViews ); // todo: from sview
  connect( sview, &StructView::viewChanged, this, &LaboView::updateViews ); // todo: from sview
  // connect( sview, &StructView::sig_changeSel,   this, &LaboView::changeSel );
  // connect( sview, &StructView::sig_changeLevel, this, &LaboView::changeLevel );

  connect( outs_selmod, &QItemSelectionModel::currentChanged, this, &LaboView::changeSelOut );
  connect( plots_selmod, &QItemSelectionModel::currentChanged, this, &LaboView::changeSelGraph );

  initEngine();

}

LaboView::~LaboView()
{
  delete doc; doc = nullptr; // BUG: is here?
}

bool LaboView::callSlot( const char *nm )
{
  return QMetaObject::invokeMethod( this, nm, Qt::AutoConnection );
}


bool LaboView::checkSlot( const char *nm )
{
  vector<QWidget*> svs { this, sview, outs_view, plots_view, sims_view, schems_view };
  for( auto w : svs ) {
    if( checkSlotSub( w, nm ) ) {
      return true;
    }
  }

  // tmp hack: move action to view
  QByteArray snm { nm };
  if( snm == "fileSave" || snm == "fileSaveAs" || snm == "fileClose" ) {
    return true;
  }
  return false;
}

const QString& LaboView::currentFile() const
{
  return doc->pathName(); // TODO: remove, use getFilePath
}

int LaboView::getSelX() const
{
  return sview->getSelX();
}

int LaboView::getSelY() const
{
  return sview->getSelY();
}

int LaboView::getSel() const
{
  return sview->getSel();
}

TMiso* LaboView::getSelObj() const
{
  return sview->getSelObj();
}

TMiso* LaboView::getMarkObj() const
{
  return sview->getMarkObj();
}

int LaboView::getLevel() const
{
  return sview->getLevel();
}

QString LaboView::getSelName( QAbstractItemView *view )
{
  if( !view ) {
    return QString();
  }

  QItemSelectionModel *selMod = view->selectionModel();
  if( !selMod ) {
    return QString();
  }

  QModelIndex cs = selMod->currentIndex();
  if( !cs.isValid() ) {
    return QString();
  }
  return cs.data( Qt::DisplayRole ).toString();
}

QSize LaboView::svSize() const
{
  return scrollArea->size();
}



void LaboView::update()
{
  updateViews();
  QWidget::update();
}

void LaboView::print()
{
  sview->printAll();
}

void LaboView::closeEvent( QCloseEvent *e )
{
  if( !doc ) { // close bad window
    e->accept();
    return;
  }
  if( !doc->canCloseFrame( this ) ) {
    e->ignore();
    return;
  }

  // close windows, related to current, if LaboView closed
  LaboWin *lw = LaboWin::win();
  if( lw ) {
    QString fp = getFilePath();
    lw->closeRelated( fp );
  }


  e->accept();
}

void LaboView::resizeEvent( QResizeEvent *e )
{
  updateViews();
  QWidget::resizeEvent( e );
}


void LaboView::updateViews()
{
  QPoint seco = sview->getSelCoords();
  scrollArea->ensureVisible( seco.x(), seco.y() );
  sview->update();
  // outs_view->reset();
  outs_view->update();
  plots_view->update();
  sims_view->update();
  stam->update();
}



void LaboView::changeSelOut( const QModelIndex & /*cur*/, const QModelIndex & )
{
  emit viewChanged();
}

void LaboView::changeSelGraph( const QModelIndex &cur, const QModelIndex & )
{
  sel_graph = cur.row();
  emit viewChanged();
}

// ==== element related

void LaboView::newElm()
{
  return sview->newElm();
}

void LaboView::delElm()
{
  return sview->delElm();
}

void LaboView::editElm()
{
  return sview->editElm();
}

void LaboView::renameElm()
{
  return sview->renameElm();
}


void LaboView::qlinkElm()
{
  return sview->qlinkElm();
}


void LaboView::qplinkElm()
{
  return sview->qplinkElm();
}

void LaboView::unlinkElm()
{
  return sview->unlinkElm();
}

void LaboView::lockElm()
{
  return sview->lockElm();
}

void LaboView::ordElm()
{
  return sview->ordElm();
}

void LaboView::markElm()
{
  return sview->markElm();
}

void LaboView::moveElm()
{
  return sview->moveElm();
}

void LaboView::infoElm()
{
  return sview->infoElm();
}

void LaboView::showTreeElm()
{
  return sview->showTreeElm();
}


void LaboView::testElm1()
{
  return sview->testElm1();
}

void LaboView::testElm2()
{
  return sview->testElm2();
}

void LaboView::cutElm()
{
  copyElm();
  delElm();
}

void LaboView::copyElm()
{
  return sview->copyElm();
}

void LaboView::pasteElm()
{
  return sview->pasteElm();
}

// ==== outs related

void LaboView::newOut()
{
  int rc;
  QString onameq, enameq;

  TMiso *selObj = sview->getSelObj();

  if( selObj ) {
    enameq = selObj->objectName();
    onameq = QString("out_") + QString(enameq);
  } else {
    enameq = "t";
    onameq = QString("out_t");
  };

  auto dia = new QDialog( this );
  dia->setWindowTitle( "Creating new output array" );

  auto lay = new QGridLayout( dia );

  auto lab1 = new QLabel( "Output array name", dia );
  lay->addWidget( lab1, 0, 0 );

  auto oname_ed = new QLineEdit( dia );
  oname_ed->setMaxLength( MAX_NAMELEN-1 );
  oname_ed->setText( onameq ); oname_ed->setFocus();
  lay->addWidget( oname_ed, 0, 1 );

  auto lab2 = new QLabel( "Element name",  dia );
  lay->addWidget( lab2, 1, 0 );

  auto ename_ed = new QLineEdit( dia );
  ename_ed->setMaxLength( MAX_NAMELEN-1 );
  ename_ed->setText( enameq );
  lay->addWidget( ename_ed, 1, 1 );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  rc = dia->exec();
  onameq = oname_ed->text(); enameq = ename_ed->text();
  delete dia;
  if( rc != QDialog::Accepted ) {
    return;
  }

  if( ! isGoodName( onameq ) ) {
    handleError( this, QString("Bad output name: \"%1\"").arg(onameq) );
    return;
  }

  int irc = model->insOut( onameq, enameq );
  if( !irc  ) {
    handleError( this, QString("Fail to add Output: \"%1\"").arg(onameq) );
  }
  emit viewChanged();
  return;
}

void LaboView::delOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( this, "output array", nm ) ) {
    model->delOut( nm );
    emit viewChanged();
  }
}


void LaboView::editOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  TOutArr *arr = model->getOutArr( nm );
  bool ok = editObj( this, arr );
  if( ok ) {
    model->reset();
    emit viewChanged();
  }
}

void LaboView::renameOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  TOutArr *arr = model->getOutArr( nm );
  if( !arr ) {
    return;
  }

  QString old_name = arr->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + arr->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( ok ) {
    if( outs->rename_obj( old_name, new_name ) ) {
      // model->setModified();
      model->reset();
      emit viewChanged();
    }
  }

  // TODO: change graph links names
}


void LaboView::selectOut()
{
  QItemSelectionModel *selMod = outs_view->selectionModel();
  if( !selMod ) {
    return;
  }
  int level = sview->getLevel();

  QModelIndex s_idx = outs->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}


void LaboView::showOutData() // TODO: special dialog (+ for many rows)
{
  DatasInfo di;

  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    qWarning() << "output array not selected" << WHE;
    return;
  }
  auto arr = model->getOutArr( nm );
  if( !arr ) {
    qWarning() << "fail to find output array " <<  nm << WHE;
    return;
  }

  int k = arr->fillDatasInfo( &di );
  if( !k ) {
    qWarning() << "fail to fill info about output array " <<  arr->getFullName() << WHE;
    return;
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
       | Qt::TextSelectableByKeyboard);
  lay->addWidget( lab, 0, 1 );

  auto bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  lay->addWidget( bt_ok, 1, 0, 1, 2 );
  dia->setLayout( lay );

  dia->exec();
  delete dia;
}

void LaboView::exportOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  TOutArr *arr = model->getOutArr( nm );
  if( !arr ) {
    return;
  }

  QString fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() ) {
    return;
  }
  arr->dump( fnq, " " );
}


// ==== graph related

void LaboView::newGraph()
{
  QString grnameq, aname;
  bool ok;

  grnameq = QSL("plot_") + QSN( plots->countElemsOfType( "TGraph", QSL("plot_") ) );
  aname = QInputDialog::getText( this, "Creating new plot",
      "Enter name of new plot:", QLineEdit::Normal,
      grnameq, &ok );
  if( !ok ) {
    return;
  }
  if( ! isGoodName( aname ) ) {
    handleError( this, QString("Bad plot name: \"%1\"").arg(aname) );
    return;
  }
  if( ! model->insGraph( aname ) ) {
    handleError( this, QString("Fail to add plot: \"%1\"").arg(aname) );
    return;
  }
  emit viewChanged();
}


void LaboView::delGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( this, "plot", nm ) ) {
    model->delGraph( nm );
    emit viewChanged();
  }
}


void LaboView::editGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );

  bool ok = editObj( this, qobject_cast<HolderData*>(gra) );
  if( ok ) {
    // no reset if only view changed // model->reset();
    emit viewChanged();
  }
}

void LaboView::selectGraph()
{
  QItemSelectionModel *selMod = plots_view->selectionModel();
  if( !selMod ) {
    return;
  }

  int level = sview->getLevel();
  QModelIndex s_idx = plots->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}

void LaboView::renameGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );
  if( !gra ) {
    return;
  }

  QString old_name = gra->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + gra->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( ok ) {
    if( plots->rename_obj( old_name, new_name ) ) {
      model->reset();
      emit viewChanged();
    }
  }
}



void LaboView::showGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );
  if( !gra ) {
    return;
  }

  auto plotWnd = new QMainWindow( this ); // TODO: to MDI
  plotWnd->setWindowTitle( QString( "MGL: ") + gra->objectName() );
  plotWnd->setAttribute( Qt::WA_DeleteOnClose );
  auto pv = new MglView( gra, plotWnd );

  plotWnd->setCentralWidget( pv );
  pv->setFocus();
  plotWnd->show();
}

void LaboView::graphAddOut()
{
  QString nm_o = getSelName( outs_view );
  if( nm_o.isEmpty() ) {
    return;
  }

  QString nm_g = getSelName( plots_view );
  if( nm_g.isEmpty() ) {
    return;
  }

  model->addOutToGraph( nm_o, nm_g );
  emit viewChanged();
}

void LaboView::showGraphData()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }
  TGraph *gra = model->getGraph( nm );
  if( !gra ) {
    return;
  }

  DatasInfo di;
  int k = gra->fillDatasInfo( &di );
  if( !k ) {
    return;
  }

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString("Plot data: ") + di.title );
  auto lv = new QVBoxLayout( dia );

  auto dmod = new DoubleTableModel( di, dia );
  auto dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lv->addWidget( dtv );

  auto bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  lv->addWidget( bt_ok );

  int w0 = di.size() * 12 * em;
  dia->resize( w0, em*40 );

  dia->exec();
  delete dia;

}

void LaboView::exportGraphData()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );
  if( !gra  ) {
    return;
  }
  QString fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() ) {
    return;
  }

  gra->dump( fnq, " " );
}


void LaboView::cloneGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  QString nn = nm + "_1";
  bool ok;
  QString new_name = QInputDialog::getText(
      this, tr( "New plot name" ), tr( "Plot name:" ), QLineEdit::Normal,
      nn, &ok );

  if( ok ) {
    model->cloneGraph( nm, new_name );
    model->handleStructChanged();
    emit viewChanged();
  }
}


// ==== simulation related

void LaboView::newSimul()
{
  bool ok;
  QString simName = QSL("sim") + QSN( sims->countElemsOfType( QSL("Simulation"), QSL("sim") ) );
  simName = QInputDialog::getText( this, "Creating new Simulation",
      "Enter name of new simulation:", QLineEdit::Normal,
      simName, &ok );
  if( !ok ) {
    return;
  }
  if( ! isGoodName( simName ) ) {
    handleError( this, QString("Bad simulation name: \"%1\"").arg(simName) );
    return;
  }
  ok = model->newSimul( simName );
  if( !ok ) {
    handleError( this, QString("Fail to create simulation: \"%1\"").arg(simName) );
  }
  emit viewChanged();
}

void LaboView::delSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( this, "simulation", nm ) ) {
    model->delSimul( nm );
    emit viewChanged();
  }
}

void LaboView::editSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }
  Simulation *sim = model->getSimul( nm );

  bool ok = editObj( this, sim );
  if( ok ) {
    model->reset();
    emit viewChanged();
  }
}

void LaboView::renameSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  Simulation *sim = model->getSimul( nm );
  if( !sim ) {
    return;
  }

  QString old_name = sim->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + sim->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( ok ) {
    if( sims->rename_obj( old_name, new_name ) ) {
      model->reset();
      emit viewChanged();
    }
  }

}


void LaboView::selectSimul() // TODO: propagate to all
{
  QItemSelectionModel *selMod = sims_view->selectionModel();
  if( !selMod ) {
    return;
  }

  int level = sview->getLevel();
  QModelIndex s_idx = sims->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}

void LaboView::setActiveSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  model->setActiveSimul( nm );
  emit viewChanged();
}

void LaboView::cloneSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  QString nn = nm + "_1";
  bool ok;
  QString new_name = QInputDialog::getText(
      this, tr( "New simulation name" ), tr( "Simulation name:" ), QLineEdit::Normal,
      nn, &ok );

  if( ok ) {
    model->cloneSimul( nm, new_name );
    model->handleStructChanged();
    emit viewChanged();
  }
}


// ==== model related

void LaboView::editModel()
{
  bool ok = editObj( this, model );
  if( ok ) {
    model->reset();
    emit viewChanged();
  }
}


void LaboView::showTreeModel()
{
  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Model ") );

  auto lay = new QVBoxLayout();

  auto treeView = new QTreeView( dia );
  treeView->setModel( root );

  lay->addWidget( treeView );

  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 90*em, 50*em ); // TODO: unmagic
  treeView->setColumnWidth( 0, 35*em );
  treeView->setColumnWidth( 1, 10*em );
  treeView->setColumnWidth( 2, 35*em );
  treeView->setColumnWidth( 3,  8*em );
  treeView->expandAll();
  dia->exec();
  delete dia;
  emit viewChanged();
  return;
}

void LaboView::newScheme()
{
  bool ok;
  QString schName = QSL("sch_") + QSN( schems->countElemsOfType( "Scheme", QSL("sch_") ) ); // TODO: suggestName
  schName = QInputDialog::getText( this, "Creating new Scheme",
      "Enter name of new scheme:", QLineEdit::Normal,
      schName, &ok );
  if( !ok ) { return; }

  if( ! isGoodName( schName ) ) {
    handleError( this, QString("Bad scheme name: \"%1\"").arg(schName) );
  }
  ok = model->newScheme( schName );
  if( !ok ) {
    handleError( this, QString("Fail to create scheme: \"%1\"").arg(schName) );
  }
  emit viewChanged();
}

void LaboView::delScheme()
{
  QString nm = getSelName( schems_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( this, "scheme", nm ) ) {
    model->delScheme( nm );
    emit viewChanged();
  }
}

void LaboView::editScheme()
{
  QString nm = getSelName( schems_view );
  if( nm.isEmpty()  || nm == QSL( "main_s" ) ) {
    return;
  }
  LaboWin *mwin = LaboWin::win();
  if( !mwin ) {
    return;
  }

  Scheme *sch = model->getScheme( nm );
  if( !sch ) {
    return;
  }

  QString fileName = doc->pathName();
  // like CommonSubwin
  QString wtit = QSL( "scheme: " ) % sch->objectName() % QSL(" - " ) % fileName;
  QMdiSubWindow *swin = mwin->findMdiByTitle( wtit, true ); // true = activate
  if( swin ) {
    return;
  }

  auto sw = new StructSubwin( mwin, doc, sch, this, nullptr );
  mwin->addChild( sw );

    // emit viewChanged();
}

void LaboView::renameScheme()
{
  QString nm = getSelName( schems_view );
  if( nm.isEmpty() ) {
    return;
  }

  Scheme *sch = model->getScheme( nm );
  if( !sch ) {
    return;
  }

  QString old_name = sch->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + sch->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( !ok ) { return; }

  if( schems->rename_obj( old_name, new_name ) ) {
    model->reset();
    emit viewChanged();
  }

}

void LaboView::cloneScheme()
{
  QString nm = getSelName( schems_view );
  if( nm.isEmpty() ) {
    return;
  }
  QString nn = nm + QSN( schems->countElemsOfType( "Scheme", nm ) ); // TODO: suggestName

  bool ok;
  QString new_name = QInputDialog::getText(
      this, tr( "New scheme name" ), tr( "Scheme name:" ), QLineEdit::Normal,
      nn, &ok );

  if( ok ) {
    model->cloneScheme( nm, new_name );
    model->handleStructChanged();
    emit viewChanged();
  }
}



void LaboView::initEngine()
{
  if( ! model ) {
    qCritical() << "no model" << WHE;
    return;
  }
  model->initEngine();
}

QString LaboView::runScript( const QString& script )
{
  if( ! model ) {
    qCritical() << "no model" << WHE;
    return QString::null;
  }
  QString r = model->runScript( script );
  return r;
}

QString LaboView::runModelScript()
{
  if( ! model ) {
    qCritical() << "no model" << WHE;
    return QString::null;
  }
  QString r = model->runModelScript();
  return r;
}



// TODO: 2-pane dialog + script pool
void LaboView::runScript()
{
  if( ! model ) {
    qCritical() << "no model" << WHE;
    return;
  }
  // TODO: special class to edit js and view results

  auto dia = new QDialog( this );
  dia->setWindowTitle( "Edit script" );
  auto lv = new QVBoxLayout( dia );

  auto ted = new QTextEdit( dia );
  ted->setText( scr );
  lv->addWidget( ted );

  auto bt_ok = new QPushButton( "&Ok", dia );
  lv->addWidget( bt_ok );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 80*em, 60*em );

  int rc = dia->exec();
  QString res;
  if( rc != QDialog::Accepted ) {
    delete dia; dia = nullptr;
    return;
  }
  scr = ted->toPlainText();
  delete dia; dia = nullptr;

  res = runScript( scr );

  auto dia1 = new QDialog( this );
  dia1->setWindowTitle( "Script result" );
  auto lv1 = new QVBoxLayout( dia1 );

  auto ted1 = new QTextEdit( dia1 );
  ted1->setText( res );
  ted1->setReadOnly( true );
  lv1->addWidget( ted1 );

  auto bt_ok1 = new QPushButton( "&Ok", dia1 );
  lv1->addWidget( bt_ok1 );

  connect( bt_ok1, &QPushButton::clicked, dia1, &QDialog::accept );

  dia1->resize( 80*em, 60*em );
  dia1->exec();
  delete dia1; dia1 = nullptr;
  emit viewChanged();

}



// ==== run related

void LaboView::runRun()
{
  Simulation *sim = sims->getActiveElemT<Simulation*>();
  if( !sim ) {
    handleError( this, "No active simulations" );
    return;
  }

  RunView *rv = new RunView( model, this );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}


void LaboView::resetModel()
{
  model->reset();
  emit viewChanged();
}



// end of laboview.cpp


