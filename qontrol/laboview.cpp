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
  outs_view->setObjectName( "outs_view" );

  plots_view = new GraphDataView( plots, this );
  plots_view->setObjectName( "plots_view" );
  plots_selmod = plots_view->selectionModel();

  sims_view = new SimulView( sims, this );
  sims_view->setObjectName( "sims_view" );

  schems_view = new SchemeView( schems, this );
  schems_view->setObjectName( "schems_view" );

  sview = new StructView( this, main_s, this, outs_view );
  sview->setObjectName( "sview" );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  stam = new StatusModel( this, this );
  stam->setObjectName( "stam" );

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

  connect( this, &LaboView::viewChanged, this, &LaboView::updateViews ); // todo: from sview ?
  connect( sview, &StructView::viewChanged, this, &LaboView::updateViews );
  connect( outs_view, &OutDataView::viewChanged, this, &LaboView::updateViews );

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

TMiso* LaboView::getSelElm() const
{
  return sview->getSelElm();
}

TMiso* LaboView::getMarkElm() const
{
  return sview->getMarkElm();
}

int LaboView::getLevel() const
{
  return sview->getLevel();
}

QString LaboView::getViewSelName( QAbstractItemView *view ) const // TODO: remove
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

HolderData* LaboView::getViewSelObj( QAbstractItemView *view ) const // TODO: remove
{
  QString nm = getViewSelName( view );
  if( nm.isEmpty() ) {
    return nullptr;
  }
  HolderData *po = qobject_cast<HolderData*>( view->model() );
  if( !po ) {
    return nullptr;
  }
  HolderData *obj =  po->getObj( nm );
  if( obj ) {
    return obj;
  }
  qWarning() << "not found object " << nm << " in " << po->getFullName() << WHE;
  return nullptr;
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




bool LaboView::prepareSomething( QAbstractItemView *view, QString &subname, QString &nm,
    HolderData **a_ob, HolderData **a_co, QString *a_newname )
{
  if( !view || !model ) {
    return false;
  }
  subname = view->model()->objectName();
  nm = getViewSelName( view );
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

  bool ok = ::editObj( this, obj );
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

bool LaboView::actionObj( const char *action )
{
  QWidget *fw = focusWidget();
  if( !fw ) {
    return false;
  }
  CmdView *cv = qobject_cast<CmdView*>( fw );
  if( !cv ) {
    cv = qobject_cast<CmdView*>( fw->parent() ); // may by view in parent
  }
  if( !cv ) {
    qWarning() << "Bad focused widget " << fw->objectName() << WHE;
    return false;
  }
  return QMetaObject::invokeMethod( cv, action, Qt::AutoConnection );
}


bool LaboView::addObj()
{
  return actionObj( "addObj" );
}

bool LaboView::delObj()
{
  return actionObj( "delObj" );
}

bool LaboView::editObj()
{
  return actionObj( "editObj" );
}

bool LaboView::cutObj()
{
  return actionObj( "cutObj" );
}

bool LaboView::copyObj()
{
  return actionObj( "copyObj" );
}

bool LaboView::pasteObj()
{
  return actionObj( "pasteObj" );
}

bool LaboView::renameObj()
{
  return actionObj( "renameObj" );
}

bool LaboView::cloneObj()
{
  return actionObj( "cloneObj" );
}

bool LaboView::infoObj()
{
  return actionObj( "infoObj" );
}

bool LaboView::showTreeObj()
{
  return actionObj( "showTreeObj" );
}

bool LaboView::testObj()
{
  return actionObj( "testObj" );
}

// ---------------- old elem --------------



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
  QString tgt_nm, out_nm;

  TMiso *selElm = sview->getSelElm();

  if( selElm ) {
    tgt_nm = selElm->objectName();
    out_nm = QSL("out_") % tgt_nm;
  } else {
    tgt_nm = QSL("t");
    out_nm = QSL("out_t");
  };

  bool ok;
  out_nm = QInputDialog::getText( this, "Creating new output array",
      "Enter optput array name:", QLineEdit::Normal,
       out_nm, &ok );
  if( !ok ) {
    return;
  }

  TOutArr *arr;
  if( (arr = model->addOut( out_nm, tgt_nm ) ) != nullptr ) {
    ::editObj( this, arr );
    emit viewChanged();
  }
  return;
}

void LaboView::delOut()
{
  // delSomething( outs_view );
}


void LaboView::editOut()
{
  // editSomething( outs_view );
}


void LaboView::renameOut()
{
  // renameSomething( outs_view );
  // // TODO: change graph links names
}


void LaboView::selectOut()
{
  // QItemSelectionModel *selMod = outs_view->selectionModel();
  // if( !selMod ) {
  //   return;
  // }
  // int level = sview->getLevel();
  //
  // QModelIndex s_idx = outs->index( level, 0, QModelIndex() );
  //
  // selMod->clear();
  // selMod->select( s_idx, QItemSelectionModel::Select );
  // selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}


// TODO: to OutDataView
void LaboView::showOutData() // TODO: special dialog (+ for many rows)
{
  DatasInfo di;

  // TOutArr *arr = qobject_cast<TOutArr*>( getViewSelObj( outs_view ) );
  TOutArr *arr = qobject_cast<TOutArr*>( outs_view->getSelObj() );
  if( !arr ) {
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
  // TOutArr *arr = qobject_cast<TOutArr*>( getViewSelObj( outs_view ) );
  TOutArr *arr = qobject_cast<TOutArr*>( outs_view->getSelObj() );
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

  QString grnameq = plots->hintName( "" );
  QString aname = QInputDialog::getText( this, "Creating new plot",
      "Enter name of new plot:", QLineEdit::Normal,
      grnameq, &ok );
  if( !ok ) {
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
  TGraph *gra = qobject_cast<TGraph*>( getViewSelObj( plots_view ) );
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
  QString nm_o = outs_view->getSelName();;
  if( nm_o.isEmpty() ) {
    return;
  }

  QString nm_g = getViewSelName( plots_view );
  if( nm_g.isEmpty() ) {
    return;
  }

  model->addOutToGraph( nm_o, nm_g );
  emit viewChanged();
}

void LaboView::showGraphData()
{
  TGraph *gra = qobject_cast<TGraph*>( getViewSelObj( plots_view ) );
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
  TGraph *gra = qobject_cast<TGraph*>( getViewSelObj( plots_view ) );
  if( !gra ) {
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
  QString simName = sims->hintName( QString() );
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
  QString nm = getViewSelName( sims_view );
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
  bool ok = ::editObj( this, model );
  if( ok ) {
    model->reset();
    emit viewChanged();
  }
}


void LaboView::showTreeModel()
{
  ::showTree( this, root );
  emit viewChanged();
  return;
}

void LaboView::addScheme()
{
  bool ok;
  QString schName = schems->hintName( QString() );
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
  Scheme *sch = qobject_cast<Scheme*>( getViewSelObj( schems_view ) );
  if( !sch || sch->objectName() == QSL("main_s") ) {
    return;
  }

  LaboWin *mwin = LaboWin::win();
  if( !mwin ) {
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


