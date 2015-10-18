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
  model( root->getObjT<TModel*>( "model" ) ),
  schems( model->getObjT<ContScheme*>( "schems" ) ),
  main_s( schems->getObjT<Scheme*>("main_s") ),
  outs( model->getObjT<ContOut*>( "outs" ) ),
  plots( model->getObjT<ContGraph*>( "plots" ) ),
  sims( model->getObjT<ContSimul*>( "sims" ) )
{
  main_win = true;
  title_prefix = QSL("model");

  em = LaboWin::Em();

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

bool LaboView::prepareSomething( QAbstractItemView *view, QString &subname, QString &nm,
    HolderData **a_ob, HolderData **a_co, QString *a_newname )
{
  if( !view || !model ) {
    return false;
  }
  subname = view->model()->objectName();
  nm = getSelName( view );
  if( nm.isEmpty() ) {
    return false;
  }

  HolderData *co = model->getObj( subname );
  if( !co ) {
    return false;
  }
  if( a_co ) { *a_co = co; };

  HolderData *obj = co->getObj( nm );
  if( !obj ) {
    return false;
  }
  if( a_ob ) { *a_ob = obj; }

  if( a_newname ) {
    QString new_name = co->hintName( obj->getType(), nm );
    bool ok;
    new_name = QInputDialog::getText( this, "Object:" + obj->getFullName(),
        "Enter new name:", QLineEdit::Normal, new_name, &ok );

    if( !ok ) {
      return false;
    }
    *a_newname = new_name;
  }
  return true;
}


bool LaboView::editSomething( QAbstractItemView *view, bool noReset )
{
  QString nm, subname;
  HolderData *obj, *co;
  if( !prepareSomething( view, subname, nm, &obj, &co, nullptr ) ) {
    return false;
  }

  bool ok = editObj( this, obj );
  if( ok ) {
    if( !noReset ) {
      model->reset();
    }
    emit viewChanged();
    return true;
  }
  return false;
}

bool LaboView::delSomething( QAbstractItemView *view )
{
  QString nm, subname;
  HolderData *obj, *co;
  if( !prepareSomething( view, subname, nm, &obj, &co, nullptr ) ) {
    return false;
  }

  if( !confirmDelete( this, "object", nm ) ) {
    return false;
  }

  if( co->delObj( nm ) ) {
    model->reset();
    emit viewChanged();
    return true;
  }
  return false;
}


bool LaboView::renameSomething( QAbstractItemView *view )
{
  QString nm, subname, new_name;
  HolderData *obj, *co;
  if( !prepareSomething( view, subname, nm, &obj, &co, &new_name ) ) {
    return false;
  }

  if( co->renameObj( nm, new_name ) ) {
    model->reset();
    emit viewChanged();
    return true;
  }
  return false;
}

bool LaboView::cloneSomething( QAbstractItemView *view )
{
  QString nm, subname, new_name;
  HolderData *obj, *co;
  if( !prepareSomething( view, subname, nm, &obj, &co, &new_name ) ) {
    return false;
  }

  if( co->cloneObj( nm, new_name ) ) {
    model->reset();
    emit viewChanged();
    return true;
  }
  return false;
}



// ==== element related

void LaboView::addElm()
{
  return sview->addElm();
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

void LaboView::addOut() // TODO: common
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

  int irc = model->addOut( onameq, enameq );
  if( !irc  ) {
    handleError( this, QString("Fail to add Output: \"%1\"").arg(onameq) );
  }
  emit viewChanged();
  return;
}

void LaboView::delOut()
{
  delSomething( outs_view );
}


void LaboView::editOut()
{
  editSomething( outs_view );
}


void LaboView::renameOut()
{
  renameSomething( outs_view );
  // // TODO: change graph links names
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

void LaboView::addGraph()
{
  bool ok;

  QString grnameq = plots->hintName( "TGraph" );
  QString aname = QInputDialog::getText( this, "Creating new plot",
      "Enter name of new plot:", QLineEdit::Normal,
      grnameq, &ok );
  if( !ok ) {
    return;
  }
  if( ! isGoodName( aname ) ) {
    handleError( this, QString("Bad plot name: %1").arg(aname) );
    return;
  }
  if( ! model->addGraph( aname ) ) {
    handleError( this, QString("Fail to add plot: %1").arg(aname) );
    return;
  }
  emit viewChanged();
}


void LaboView::delGraph()
{
  delSomething( plots_view );
}


void LaboView::editGraph()
{
  editSomething( plots_view, true ); // no reset
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
  renameSomething( plots_view );
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
  LaboWin *mwin = LaboWin::win();
  if( !mwin ) { return;  }

  QString fileName = doc->pathName();
  // like CommonSubwin
  QString wtit = QSL( "plot: " ) % gra->objectName() % QSL(" - " ) % fileName;
  QMdiSubWindow *swin = mwin->findMdiByTitle( wtit, true ); // true = activate
  if( swin ) {
    return;
  }

  auto plotWnd = new MglSubwin( this, doc, gra );
  mwin->addChild( plotWnd );
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
  cloneSomething( plots_view );
}


// ==== simulation related

void LaboView::addSimul()
{
  bool ok;
  QString simName = sims->hintName( QSL("Simulation") );
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
  ok = model->addSimul( simName );
  if( !ok ) {
    handleError( this, QString("Fail to create simulation: \"%1\"").arg(simName) );
  }
  emit viewChanged();
}

void LaboView::delSimul()
{
  delSomething( sims_view );
}

void LaboView::editSimul()
{
  editSomething( sims_view );
}

void LaboView::renameSimul()
{
  renameSomething( sims_view );
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
  cloneSomething( sims_view );
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

void LaboView::addScheme()
{
  bool ok;
  QString schName = schems->hintName( "Scheme" );
  schName = QInputDialog::getText( this, "Creating new Scheme",
      "Enter name of new scheme:", QLineEdit::Normal,
      schName, &ok );
  if( !ok ) { return; }

  if( ! isGoodName( schName ) ) {
    handleError( this, QString("Bad scheme name: \"%1\"").arg(schName) );
  }
  ok = model->addScheme( schName );
  if( !ok ) {
    handleError( this, QString("Fail to create scheme: \"%1\"").arg(schName) );
  }
  emit viewChanged();
}

void LaboView::delScheme()
{
  delSomething( schems_view );
}

void LaboView::editScheme() // special: separate window
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
  renameSomething( schems_view );
}

void LaboView::cloneScheme()
{
  cloneSomething( schems_view );
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
  Simulation *sim = sims->getActiveObjT<Simulation*>();
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


