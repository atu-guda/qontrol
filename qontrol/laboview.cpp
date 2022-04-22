/***************************************************************************
  laboview.cpp  - combined model view
  begin                : Mon Jul 31 16:51:57 EEST 2000
  copyright            : (C) 2000-2022 by atu
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
#include "prog_opts.h"
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
#include "scriptdialog.h"
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
  model( root->getObjT<TModel*>( QSL("model") ) ),
  schems( model->getObjT<ContScheme*>( QSL("schems") ) ),
  main_s( schems->getObjT<Scheme*>( QSL("main_s") ) ),
  outs( model->getObjT<ContOut*>( QSL("outs") ) ),
  plots( model->getObjT<ContGraph*>( QSL("plots") ) ),
  sims( model->getObjT<ContSimul*>( QSL("sims") ) )
{
  main_win = true;
  title_prefix = QSL("model");

  em = QFontMetrics( font() ).horizontalAdvance( QSL("W") );

  auto split_h = new QSplitter( Qt::Horizontal, this );
  setCentralWidget( split_h );

  setAttribute(Qt::WA_DeleteOnClose);

  scrollArea = new QScrollArea( this );
  scrollArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  outs_view = new OutDataView( outs, this );
  outs_view->setObjectName( QSL("outs_view") );
  vmap[QSL("outs_view")] = outs_view;

  plots_view = new GraphDataView( plots, this );
  plots_view->setObjectName( QSL("plots_view") );
  vmap[QSL("plots_view")] = plots_view;

  sims_view = new SimulView( sims, this );
  sims_view->setObjectName( QSL("sims_view") );
  vmap[QSL("sims_view")] = sims_view;

  schems_view = new SchemeView( schems, this );
  schems_view->setObjectName( QSL("schems_view") );
  vmap[QSL("schems_view")] = schems_view;

  sview = new StructView( this, main_s );
  sview->setObjectName( QSL("sview") );
  vmap[QSL("sview")] = sview;

  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  stam = new StatusModel( this );
  stam->setObjectName( QSL("stam") );

  split_h->addWidget( scrollArea );
  split_h->addWidget( outs_view );
  auto split_v = new QSplitter( Qt::Vertical, this );
  split_h->addWidget( split_v );
  split_v->addWidget( plots_view );
  split_v->addWidget( sims_view );
  split_v->addWidget( schems_view );

  split_h->setStretchFactor( 0, 7 );
  split_h->setStretchFactor( 1, 1 );
  split_h->setStretchFactor( 2, 1 );

  setStatusBar( stam );

  connect( this,        &LaboView::viewChanged,    this, &LaboView::updateViews ); // todo: from sview ?
  connect( sview,       &StructView::viewChanged,  this, &LaboView::updateViews );
  connect( outs_view,   &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( plots_view,  &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( sims_view,   &OutDataView::viewChanged, this, &LaboView::updateViews );
  connect( schems_view, &OutDataView::viewChanged, this, &LaboView::updateViews );

  connect( outs_view,   &OutDataView::focusChanged,   stam, &StatusModel::update );
  connect( plots_view,  &GraphDataView::focusChanged, stam, &StatusModel::update );
  connect( sims_view,   &SimulView::focusChanged,     stam, &StatusModel::update );
  connect( schems_view, &SchemeView::focusChanged,    stam, &StatusModel::update );

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
  LaboWin *lw = MAINWIN;
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


void LaboView::markElm()
{
  return sview->markElm();
}

void LaboView::moveElm()
{
  return sview->moveElm();
}



void LaboView::testElm2()
{
  return sview->testElm2();
}


bool LaboView::actionObj( const char *action )
{
  CmdView *cv = getFocusedCmdView();
  if( !cv ) {
    return false;
  }
  return QMetaObject::invokeMethod( cv, action, Qt::AutoConnection );
}


bool LaboView::addObj()
{
  return actionObj( "addObj" ); // no QSL!
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





// ==== outs related

void LaboView::addOut()
{
  outs_view->addObj();
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


// ==== simulation related

void LaboView::addSimul()
{
  sims_view->addObj();
}


void LaboView::setActiveSimul()
{
  sims_view->setActive();
}


// ==== model related

void LaboView::editModel()
{
  bool ok = ::editObj( this, model );
  model->readAllInputs();
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


void LaboView::importAllSchemes()
{
  if( ! model ) {
    qCritical() << QSL("no model") << WHE;
    return;
  }
  model->importAllSchemes();
}

void LaboView::initEngine()
{
  if( ! model ) {
    qCritical() << QSL("no model") << WHE;
    return;
  }
  model->initEngine();
}


int LaboView::runModelScript()
{
  if( ! model ) {
    qCritical() << QSL("no model") << WHE;
    return 0;
  }
  ScriptResult sres;
  int rc = model->runModelScript( &sres );
  return rc;
}



void LaboView::runScript() // TODO: saved script or pool
{
  if( ! model ) {
    qCritical() << QSL("no model") << WHE;
    return;
  }
  QString scr = model->getDataD( QSL("scriptpad"), QSL("// comment") );

  auto dia0 = new ScriptDialog( scr, model, this );
  dia0->exec();
  delete dia0; // BUG here?
  model->setData( QSL("scriptpad"), scr );

  emit viewChanged();
}



// ==== run related

void LaboView::runRun()
{
  Simulation *sim = sims->getActiveObjT<Simulation*>();
  if( !sim ) {
    HANDLE_ERROR( QSL("No active simulations") );
    return;
  }

  RunView *rv = new RunView( model, this );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void LaboView::runPreScript()
{
  model->runPreScript();
  emit viewChanged();
  sview->setFocus();
}



void LaboView::resetModel()
{
  model->reset();
  emit viewChanged();
}



// end of laboview.cpp


