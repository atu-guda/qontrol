/***************************************************************************
  laboview.cpp  - combined model view
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
  vmap["outs_view"] = outs_view;

  plots_view = new GraphDataView( plots, this );
  plots_view->setObjectName( "plots_view" );
  vmap["plots_view"] = plots_view;

  sims_view = new SimulView( sims, this );
  sims_view->setObjectName( "sims_view" );
  vmap["sims_view"] = sims_view;

  schems_view = new SchemeView( schems, this );
  schems_view->setObjectName( "schems_view" );
  vmap["schems_view"] = schems_view;

  sview = new StructView( this, main_s );
  sview->setObjectName( "sview" );
  vmap["sview"] = sview;

  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  stam = new StatusModel( this );
  stam->setObjectName( "stam" );

  grLay->addWidget( scrollArea, 0, 0, 2, 1 );
  grLay->addWidget( outs_view, 0, 1 );
  grLay->addWidget( plots_view, 0, 2 );
  grLay->addWidget( sims_view, 1, 1 );
  grLay->addWidget( schems_view, 1, 2 );

  main_part->setLayout( grLay );

  setStatusBar( stam );

  connect( this, &LaboView::viewChanged, this, &LaboView::updateViews ); // todo: from sview ?
  connect( sview, &StructView::viewChanged, this, &LaboView::updateViews );
  connect( outs_view, &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( plots_view, &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( sims_view, &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( schems_view, &OutDataView::viewChanged, this, &LaboView::updateViews );

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



QSize LaboView::svSize() const
{
  return scrollArea->size();
}


void LaboView::update()
{
  // updateViews();
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
  // updateViews();
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
  sview->copyObj();
}


void LaboView::pasteElm()
{
  return sview->pasteElm();
}



// ==== outs related

void LaboView::addOut()
{
  outs_view->addObj();
}

void LaboView::delOut()
{
  outs_view->delObj();
}


void LaboView::editOut()
{
  outs_view->editObj();
}


void LaboView::renameOut()
{
  outs_view->renameObj();
}


void LaboView::showOutData()
{
  outs_view->showDataObj();
}

void LaboView::exportOut()
{
  outs_view->exportObj();
}


// ==== graph related

void LaboView::addGraph()
{
  plots_view->addObj();
}


void LaboView::delGraph()
{
  plots_view->delObj();
}


void LaboView::editGraph()
{
  plots_view->editObj(); // TODO: no reset
  // editSomething( plots_view, true );
}

void LaboView::renameGraph()
{
  plots_view->renameObj();
}



void LaboView::showGraph()
{
  plots_view->showObj();
}

void LaboView::graphAddOut()
{
  outs_view->addToPlot();
}

void LaboView::showGraphData()
{
  plots_view->showDataObj();
}

void LaboView::exportGraphData()
{
  plots_view->exportObj();
}


void LaboView::cloneGraph()
{
  plots_view->cloneObj();
}


// ==== simulation related

void LaboView::addSimul()
{
  sims_view->addObj();
}

void LaboView::delSimul()
{
  sims_view->delObj();
}

void LaboView::editSimul()
{
  sims_view->editObj();
}

void LaboView::renameSimul()
{
  sims_view->renameObj();
}


void LaboView::setActiveSimul()
{
  sims_view->setActive();
}

void LaboView::cloneSimul()
{
  sims_view->cloneObj();
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
  schems_view->addObj();
}

void LaboView::delScheme()
{
  schems_view->delObj();
}

void LaboView::editScheme() // special: separate window
{
  schems_view->editObj();
}

void LaboView::renameScheme()
{
  schems_view->renameObj();
}

void LaboView::cloneScheme()
{
  schems_view->cloneObj();
}


void LaboView::importAllSchemes()
{
  if( ! model ) {
    qCritical() << "no model" << WHE;
    return;
  }
  model->importAllSchemes();
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


