/***************************************************************************
                          labowin.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2016 by atu
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

#include <cstdlib>
#include <QtWidgets>
#include <QPrinter>
#include <QTextEdit>

#include "miscfun.h"
#include "laboview.h"
#include "structview.h"
#include "labodoc.h"
#include "labowin.h"

using namespace std;

LaboWin* LaboWin::labowin = nullptr;

LaboWin::LaboWin()
  : split( new QSplitter( Qt::Vertical, this ) ),
    mdiArea( new QMdiArea ),
    logViewer( new QTextEdit(this) ),
    printer( new QPrinter ),
    log_timer( new QTimer( this ) ),
    setti( new SettingsData( "settings", nullptr, 0, "settings", "Data settings" ) )
{
  setti->load();
  setWindowTitle( PACKAGE " " VERSION );

  split->addWidget( mdiArea );
  split->addWidget( logViewer );

  mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

  setCentralWidget( split );

  connect( mdiArea, &QMdiArea::subWindowActivated, this, &LaboWin::slotHandleSubWindowActivated );
  windowMapper = new QSignalMapper(this);
  connect( windowMapper,
      static_cast<void (QSignalMapper::*)(QWidget*)>(&QSignalMapper::mapped),
      this, &LaboWin::setActiveSubWindow );


  untitledCount = 0;

  // call inits to invoke all other construction parts
  setFonts();
  initIface();
  initStatusBar();

  setWindowIcon( QIcon( QSL(":icons/app.png") ) );
  if( setti->getDataD( QSL("showMax"),0 ) ) {
    showMaximized();
  }

  logViewer->setReadOnly( true );
  logViewer->setMinimumSize( 50*em, 6*em ); // TODO: fix!
  logViewer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  QList<int> szs { 640, 80 };
  split->setSizes( szs );


  connect( log_timer, &QTimer::timeout, this, &LaboWin::slotUpdateLog );
  log_timer->setInterval( 500 ); // 0.5 sec
  log_timer->start();

  labowin = this;
  updateActions();
}

LaboWin::~LaboWin()
{
  delete printer;
  printer = nullptr;
  delete setti;
  setti = nullptr;
  labowin = nullptr;
}


void LaboWin::setFonts()
{
  QVariant v;
  const QFont &mainFont = setti->getAsFont( QSL("mainFont") );
  QFontMetrics fm( mainFont );
  em = fm.width( 'W' );
  QApplication::setFont( mainFont );
}

void LaboWin::registerAction( QAction *act, const char *nm )
{
  if( !nm || !act ) {
    return;
  }
  act->setObjectName( nm );
  acts[ nm ] = act;
}

QAction* LaboWin::makeAction( const QString &text, const char *actName, SlotVV targetSlot,
        const QString &icon_str, QMenu *tmenu, unsigned aflags )
{
  auto a = new QAction( text, this );
  // a->setShortcuts( shortcuts );
  registerAction( a, actName );
  connect( a, &QAction::triggered, this, targetSlot );
  if( tmenu ) {
    tmenu->addAction( a );
    if( aflags & ActionMenuSep ) {
      tmenu->addSeparator();
    }
  }
  if( ! icon_str.isEmpty() ) {
    if( icon_str[0] == ':' ) { // from resource
      a->setIcon( QIcon( icon_str ) );
    } else {
      a->setIcon( QIcon::fromTheme( icon_str ) );
    }
  }
  if( aflags & ActionAlwaysVis ) {
    a->setProperty( "alwaysVisible", true );
  }
  if( aflags & ActionToolBar ) {
    mainToolBar->addAction( a );
  }
  if( aflags & ActionToolSep ) {
    mainToolBar->addSeparator();
  }
  return a;
}

void LaboWin::initIface()
{
  // make manu beforehand
  pFileMenu = menuBar()->addMenu( tr("&File") );
  pEditMenu = menuBar()->addMenu( tr("&Edit") );
  pElmMenu = menuBar()->addMenu( tr("E&lement") );
  pOutMenu = menuBar()->addMenu( tr("&Out") );
  pGraphMenu = menuBar()->addMenu( tr("&Plot") );
  pSimulMenu = menuBar()->addMenu( tr("&Simulation") );
  pModelMenu = menuBar()->addMenu( tr("&Model") );
  pRunMenu = menuBar()->addMenu( tr("&Run") );
  pViewMenu = menuBar()->addMenu( tr("&View") );
  pWindowMenu = menuBar()->addMenu( tr("&Window") );
  pHelpMenu = menuBar()->addMenu( tr("&Help") );

  // and toolbars make too
  mainToolBar = addToolBar( tr("Main") );

  // Actions
  // File group

  {
    auto a = makeAction( QSL("&New model"), "fileNew", &LaboWin::slotFileNew,
        QSL(":icons/filenew.png"), pFileMenu, ActionAlwaysVis | ActionToolBar | ActionMenuSep );
    a->setShortcuts( QKeySequence::New );
  }

  {
    auto a = makeAction( QSL("&Open model"), "fileOpen", &LaboWin::slotFileOpen,
        QSL("document-open"), pFileMenu, ActionAlwaysVis | ActionToolBar );
    a->setShortcuts( QKeySequence::Open );
  }

  {
    auto a = makeAction( QSL("&Save model"), "fileSave", &LaboWin::slotFileSave,
        QSL("document-save"), pFileMenu, ActionToolBar );
    a->setShortcuts( QKeySequence::Save );
  }

  {
    /* auto a = */ makeAction( QSL("Save model &As"), "fileSaveAs", &LaboWin::slotFileSaveAs,
        QSL("document-save-as"), pFileMenu, ActionMenuSep );
  }

  {
    auto a = makeAction( QSL("&Print"), "print", &LaboWin::slotFilePrint,
        QSL("document-print"), pFileMenu, ActionMenuSep );
    a->setShortcuts( QKeySequence::Print );
  }

  {
    auto a = makeAction( QSL("&Close"), "fileClose", &LaboWin::slotFileClose,
        QSL("document-close"), pFileMenu, ActionMenuSep );
    a->setShortcuts( QKeySequence::Close );
  }

  {
    /* auto a = */ makeAction( QSL("Sett&ings"), "fileSettings", &LaboWin::slotFileSettings,
        QSL("preferences-system"), pFileMenu, ActionAlwaysVis );
  }

  {
    /* auto a = */ makeAction( QSL("Save s&ettings"), "fileSaveSett", &LaboWin::slotFileSaveSett,
        QSL(""), pFileMenu, ActionAlwaysVis  | ActionMenuSep );
  }


  {
    auto a = makeAction( QSL("&Quit"), "fileQuit", &LaboWin::slotFileQuit,
        QSL("application-exit"), pFileMenu, ActionAlwaysVis );
    a->setShortcuts( QKeySequence::Quit );
  }

  // ==== Edit + Object group
  {
    auto a = makeAction( QSL("&Undo"), "editUndo", &LaboWin::slotEditUndo,
        QSL("edit-undo"), pEditMenu, ActionMenuSep );
    a->setShortcuts( QKeySequence::Undo );
  }

  // -- object part
  {
    auto a = makeAction( QSL("&Add object"), "addObj", &LaboWin::slotAddObj,
        QSL("list-add"), pEditMenu, 0 );
    a->setShortcut( Qt::Key_Insert );
  }
  {
    auto a = makeAction( QSL("&Delete object"), "delObj", &LaboWin::slotDelObj,
        QSL("edit-delete"), pEditMenu, 0 );
    a->setShortcut( Qt::Key_Delete );
  }
  {
    auto a = makeAction( QSL("&Edit object"), "editObj", &LaboWin::slotEditObj,
        QSL("document-properties"), pEditMenu, 0 );
    a->setShortcut( Qt::Key_Enter );
  }
  {
    auto a = makeAction( QSL("&Cut object"), "cutObj", &LaboWin::slotCutObj,
        QSL("edit-cut"), pEditMenu, 0 );
    a->setShortcuts( QKeySequence::Cut );
  }
  {
    auto a = makeAction( QSL("c&Opy object"), "copyObj", &LaboWin::slotCopyObj,
        QSL("edit-copy"), pEditMenu, 0 );
    a->setShortcuts( QKeySequence::Copy );
  }
  {
    auto a = makeAction( QSL("&Paste object"), "pasteObj", &LaboWin::slotPasteObj,
        QSL("edit-paste"), pEditMenu, ActionMenuSep );
    a->setShortcuts( QKeySequence::Paste );
  }
  {
    /* auto a = */ makeAction( QSL("&Rename object"), "renameObj", &LaboWin::slotRenameObj,
        QSL("edit-rename"), pEditMenu, 0 );
  }
  {
    /* auto a = */ makeAction( QSL("c&Lone object"), "cloneObj", &LaboWin::slotCloneObj,
        QSL(":icons/edit-clone.png"), pEditMenu, ActionMenuSep );
  }
  {
    auto a = makeAction( QSL("show &Info"), "infoObj", &LaboWin::slotInfoObj,
        QSL(":icons/info-obj.png"), pEditMenu, ActionToolBar | ActionMenuSep | ActionToolSep );
    a->setShortcut( Qt::Key_I );
  }
  {
    /* auto a = */ makeAction( QSL("show object &Tree"), "showTreeObj", &LaboWin::slotShowTreeObj,
        QSL("view-list-tree"), pEditMenu, ActionMenuSep );
  }
  {
    /* auto a = */ makeAction( QSL("Test object"), "testObj", &LaboWin::slotTestObj,
        QSL("tools-check-spelling"), pEditMenu, ActionMenuSep );
  }
  {
    /* auto a = */ makeAction( QSL("Test"), "test", &LaboWin::slotTest,
        QSL(":icons/test.png"), pEditMenu, ActionAlwaysVis | ActionToolBar | ActionToolSep );
  }



  // ==== Element group
  {
    auto a = makeAction( QSL("&Quick link element"), "qlinkElm", &LaboWin::slotqLinkElm,
        QSL("insert-link"), pElmMenu, ActionToolBar );
    a->setShortcut( Qt::CTRL+Qt::Key_L );
  }
  {
    auto a = makeAction( QSL("&Parametr link element"), "qplinkElm", &LaboWin::slotqpLinkElm,
        QSL(""), pElmMenu, 0 );
    a->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_L );
  }
  {
    /* auto a = */ makeAction( QSL("&Unlink element"), "unlinkElm", &LaboWin::slotUnlinkElm,
        QSL("network-offline"), pElmMenu, ActionMenuSep );
  }
  {
    auto a = makeAction( QSL("Loc&k element"), "lockElm", &LaboWin::slotLockElm,
        QSL(":icons/lockelm.png"), pElmMenu, 0 );
    a->setShortcut( Qt::CTRL+Qt::Key_K );
  }
  {
    auto a = makeAction( QSL("&Mark element"), "markElm", &LaboWin::slotMarkElm,
        QSL(":icons/markelm.png"), pElmMenu, ActionToolBar );
    a->setShortcut( Qt::Key_M );
  }
  {
    auto a = makeAction( QSL("Move element"), "moveElm", &LaboWin::slotMoveElm,
        QSL("go-down"), pElmMenu, 0 );
    a->setShortcut( Qt::SHIFT+Qt::Key_M );
  }
  // ==== out group
  {
    auto a = makeAction( QSL("&New Out"), "addOut", &LaboWin::slotNewOut,
        QSL(":icons/newout.png"), pOutMenu, ActionToolBar );
    a->setShortcut( Qt::Key_U );
  }
  {
    auto a = makeAction( QSL("Add out to plo&t"), "graphAddOut", &LaboWin::slotGraphAddOut,
        QSL("arrow-right"), pOutMenu, ActionMenuSep );
    a->setShortcut( Qt::CTRL | Qt::Key_U );
  }
  {
    /* auto a = */ makeAction( QSL("&Show out data"), "showOutData", &LaboWin::slotShowOutData,
        QSL(":icons/showoutdata.png"), pOutMenu, ActionToolBar | ActionToolSep );
  }
  {
    auto a = makeAction( QSL("E&xport out data"), "exportOut", &LaboWin::slotExportOut,
        QSL("document-export"), pOutMenu, 0 );
    a->setShortcut( Qt::CTRL+Qt::Key_E );
  }

  // ==== graph=plot group
  {
    auto a = makeAction( QSL("&New Plot"), "addGraph", &LaboWin::slotNewGraph,
        QSL(":icons/newgraph.png"), pGraphMenu, ActionToolBar );
    a->setShortcut( Qt::Key_P );
  }
  {
    auto a = makeAction( QSL("&Show plot"), "showGraph", &LaboWin::slotShowGraph,
        QSL(":icons/showgraph.png"), pGraphMenu, ActionMenuSep | ActionToolBar );
    a->setShortcut( Qt::Key_S );
  }

  {
    auto a = makeAction( QSL("show plot &Data"), "showGraphData", &LaboWin::slotShowGraphData,
        QSL(":icons/showgraphdata.png"), pGraphMenu, ActionToolBar );
    a->setShortcut(  Qt::SHIFT+Qt::Key_D );
  }
  {
    auto a = makeAction( QSL("E&xport plot data"), "exportGraphData", &LaboWin::slotExportGraphData,
        QSL("document-export"), pGraphMenu, 0 );
    a->setShortcut( Qt::SHIFT+Qt::Key_E );
  }
  // ==== simulation group
  {
    /* auto a = */ makeAction( QSL("&New Simulation"), "addSimul", &LaboWin::slotNewSimul,
        QSL("list-add"), pSimulMenu, 0 );
  }
  {
    auto a = makeAction( QSL("Set &Active Simulation"), "setActiveSimul", &LaboWin::slotSetActiveSimul,
        QSL("checkmark"), pSimulMenu, 0 );
    a->setShortcut( Qt::ALT+Qt::CTRL+Qt::Key_Y );
  }

  // ==== model/Scheme group
  {
    auto a = makeAction( QSL("&Edit Model"), "editModel", &LaboWin::slotEditModel,
        QSL(":icons/editmodel.png"), pModelMenu, ActionToolBar | ActionToolSep | ActionMenuSep );
    a->setShortcut(  Qt::CTRL+Qt::Key_R );
  }
  {
    auto a = makeAction( QSL("show model &Tree"), "showTreeModel", &LaboWin::slotShowTreeModel,
        QSL("view-list-tree"), pModelMenu, ActionMenuSep );
    a->setShortcut( Qt::CTRL+Qt::Key_T );
  }
  //
  {
    /* auto a = */ makeAction( QSL("&New Scheme"), "addScheme", &LaboWin::slotNewScheme,
        QSL("list-add"), pModelMenu, 0 );
  }
  {
    /* auto a = */ makeAction( QSL("&Import schemes"), "importAllSchemes", &LaboWin::importAllSchemes,
        QSL("document-import"), pModelMenu, 0 );
  }

  // ====  run group
  {
    auto a = makeAction( QSL("&Run"), "runRun", &LaboWin::slotRunRun,
        QSL(":icons/run.png"), pRunMenu, ActionToolBar | ActionMenuSep );
    a->setShortcut( Qt::Key_F9 );
  }
  {
    /* auto a = */ makeAction( QSL("run preRun script"), "runPreScript", &LaboWin::slotRunPreScript,
        QSL(""), pRunMenu, ActionToolBar  );
  }
  {
    /* auto a = */ makeAction( QSL("Rese&t"), "resetModel", &LaboWin::slotReset,
        QSL(""), pRunMenu, 0 );
  }
  {
    /* auto a = */ makeAction( QSL("Init &engine"), "initEngine", &LaboWin::slotInitEngine,
        QSL(""), pRunMenu, 0 );
  }
  {
    auto a = makeAction( QSL("run &Script"), "runScript", &LaboWin::slotRunScript,
        QSL("application-x-javascript"), pRunMenu, ActionToolBar );
    a->setShortcut( Qt::SHIFT+Qt::Key_F9 );
  }
  {
    /* auto a = */ makeAction( QSL("run &Model script"), "runModelScript", &LaboWin::slotRunModelScript,
        QSL(""), pRunMenu, 0 );
  }

  // ==== iface group
  {
    auto a = makeAction( QSL("show &Toolbar"), "viewToolBar", &LaboWin::slotViewToolBar,
        QSL("configure-toolbars"), pViewMenu, ActionAlwaysVis );
    a->setCheckable( true );
    a->setChecked( true );
    pViewMenu->addAction( a );
    act_tbar = a; // to on/off
  }
  {
    auto a = makeAction( QSL("Show &Grid"), "showGrid", &LaboWin::slotShowGrid,
        QSL("view-grid"), pViewMenu, ActionAlwaysVis );
    a->setCheckable( true );
    a->setChecked( setti->getDataD( QSL("showGrid"), 1 ) );
  }
  {
    auto a = makeAction( QSL("Show &Names"), "showNames", &LaboWin::slotShowNames,
        QSL(""), pViewMenu, ActionAlwaysVis );
    a->setCheckable( true );
    a->setChecked( setti->getDataD( QSL("showNames"), 0 ) );
  }
  {
    auto a = makeAction( QSL("Show &Icons"), "showIcons", &LaboWin::slotShowIcons,
        QSL(""), pViewMenu, ActionAlwaysVis );
    a->setCheckable( true );
    a->setChecked( setti->getDataD( QSL("showIcons"), 0 ) );
  }
  {
    auto a = makeAction( QSL("Show &Links"), "showLinks", &LaboWin::slotShowLinks,
        QSL(""), pViewMenu, ActionAlwaysVis | ActionMenuSep );
    a->setCheckable( true );
    a->setChecked( setti->getDataD( QSL("showLinks"), 0 ) );
  }
  {
    auto a = makeAction( QSL("Clear log"), "logClear", &LaboWin::slotLogClear,
        QSL("edit-clear"), pViewMenu, ActionAlwaysVis );
    a->setShortcut(  Qt::CTRL | Qt::SHIFT | Qt::Key_C );
  }
  {
    /* auto a = */ makeAction( QSL("Save log"), "logSave", &LaboWin::slotLogSave,
        QSL("document-save"), pViewMenu, ActionAlwaysVis );
  }

  // ==== window group // most autorecreated by windowMenuAboutToShow()
  {
    auto a = makeAction( QSL("Cl&ose Window"), "windowClose", &LaboWin::slotWindowClose,
        QSL("window-close"), pWindowMenu, ActionAlwaysVis );
    act_winClose = a; // for autorecreation
  }
  {
    auto a = makeAction( QSL("Close &All Windows"), "windowCloseAll", &LaboWin::slotWindowCloseAll,
        QSL(""), pWindowMenu, ActionAlwaysVis );
    act_winCloseAll = a;
  }
  {
    auto a = makeAction( QSL("&Tile Windows"), "windowTile", &LaboWin::slotWindowTile,
        QSL(""), pWindowMenu, ActionAlwaysVis );
    act_winTile = a;
  }
  {
    auto a = makeAction( QSL("&Cascade Windows"), "windowCascase", &LaboWin::slotWindowCascade,
        QSL(""), pWindowMenu, ActionAlwaysVis );
    act_winCascade = a;
  }
  {
    auto a = new QAction( QSL("Next window"),  this ); // old approach: not my member
    a->setShortcut( Qt::Key_F6 );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "nextSubWindow" );
    connect( a, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow );
    act_winNext = a;
  }
  {
    auto a = new QAction( QSL("Previous window"),  this );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "prevSubWindow" );
    connect( a, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow );
    act_winPrev = a;
  }


  // ==== help group
  {
    /* auto a = */ makeAction( QSL("&About"), "helpAbout", &LaboWin::slotHelpAbout,
        QSL("help-about"), pHelpMenu, ActionAlwaysVis );
  }
  {
    /* auto a = */ makeAction( QSL("About &Qt"), "helpAboutQt", &LaboWin::slotHelpAboutQt,
        QSL(""), pHelpMenu, ActionAlwaysVis );
  }


  // Connect the submenu slots with signals

  connect( pWindowMenu, &QMenu::aboutToShow, this, &LaboWin::windowMenuAboutToShow );


}

void LaboWin::initStatusBar()
{
  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::slotHandleSubWindowActivated( QMdiSubWindow *swin )
{
  updateActions();
  if( !swin ) {
    setWindowTitle( PACKAGE " " VERSION );
    // qWarning() << "swin == nullptr " << WHE;
    return;
  }
  QString tit = swin->windowTitle() + QSL( " " PACKAGE );
  setWindowTitle( tit );
  // qWarning() << "swin: " << tit << WHE;

}


void LaboWin::updateActions()
{
  auto cv = activeView();

  for( auto a = acts.begin(); a != acts.end(); ++a ) {
    if( a.value()->property("alwaysVisible").toBool() ) {
      a.value()->setEnabled( true );
      continue;
    }
    if( !cv ) {
      a.value()->setEnabled( false );
      continue;
    }
    a.value()->setEnabled( cv->checkSlot( a.key() ) );
  };

  setWndTitle();
}

void LaboWin::setWndTitle()
{
  if( mdiArea->currentSubWindow() != 0 ) {
    setWindowTitle( mdiArea->currentSubWindow()->windowTitle() + " " PACKAGE );
  } else {
    setWindowTitle( PACKAGE " " VERSION );
  };
}

void LaboWin::closeEvent ( QCloseEvent *e )
{
  mdiArea->closeAllSubWindows();
  if( mdiArea->currentSubWindow() ) {
    e->ignore();
  } else {
    // writeSettings();
    e->accept();
  }
}

QMdiSubWindow* LaboWin::addChild( CommonSubwin *w )
{
  if( !w ) {
    qWarning() << QSL("w is nullptr in ") << WHE;
    return nullptr;
  }
  w->setAttribute( Qt::WA_DeleteOnClose );

  QMdiSubWindow *subw = mdiArea->addSubWindow( w );
  if( !subw ) {
    qWarning() << QSL("subw is nullptr in ") << WHE;
    return nullptr;
  }
  w->updateTitle();
  connect( w, &CommonSubwin::closeMe, this, &LaboWin::slotWindowClose );
  subw->show();
  // subw->setFocus(); // BEWARE: may be bug here!
  // updateActions();
  return subw;
}



QMdiSubWindow* LaboWin::findMdiChild( const QString &fileName )
{
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  for( QMdiSubWindow *window : mdiArea->subWindowList() ) {
    CommonSubwin *chi = qobject_cast<CommonSubwin *>(window->widget());
    if( !chi ) {
      continue;
    }
    QString cp = chi->getFilePath();
    if( cp  == canonicalFilePath ) {
      return window;
    }
  }
  return 0;
}

QMdiSubWindow* LaboWin::findMdiByTitle( const QString &tit, bool activate )
{
  for( QMdiSubWindow *subw : mdiArea->subWindowList() ) {
    QWidget *mdiChild = subw->widget();
    if( !mdiChild ) {
      continue;
    }
    QString wtit = mdiChild->windowTitle();
    if( wtit  == tit ) {
      if( activate ) {
        mdiArea->setActiveSubWindow( subw );
      }
      return subw;
    }
  }
  return 0;
}

int LaboWin::closeRelated( const QString &fp )
{
  int n_closed = 0;
  for( QMdiSubWindow *subw : mdiArea->subWindowList() ) {
    CommonSubwin *chi = qobject_cast<CommonSubwin*>( subw->widget() );
    if( !chi ) {
      continue;
    }
    if( chi->isMainWin() ) {
      continue;
    }
    QString wfp = chi->getFilePath();
    if( wfp  == fp ) {
     subw->close();
    }
  }
  return n_closed;
}

CommonSubwin* LaboWin::activeView()
{
  if( QMdiSubWindow *subw = mdiArea->activeSubWindow() ) {
    return qobject_cast<CommonSubwin*>( subw->widget() );
  }
  return nullptr;
}

void LaboWin::callLaboViewSlot( const char *slot, const QString &mess )
{
  CommonSubwin *sv = activeView();
  if( !sv ) { return; };

  // TODO: check type

  statusBar()->showMessage( mess );

  sv->callSlot( slot );
  sv->update();
  statusBar()->showMessage( tr( "Ready." ) );
}


/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void LaboWin::slotFileNew()
{
  statusBar()->showMessage( tr( "Creating new model file..." ) );

  auto  doc = new LaboDoc();
  ++untitledCount;
  QString fileName = QString( "untitled_" ) % QSN( untitledCount ) % model_file_suff;
  doc->newDocument();
  doc->setPathName( fileName );

  auto  cw = new LaboView( doc, this );
  addChild( cw );
  statusBar()->showMessage( tr( "Ready." ) );
}


void LaboWin::slotFileOpen()
{
  statusBar()->showMessage( tr( "Opening model file..." ) );
  QString fileName =
    QFileDialog::getOpenFileName( this, tr("Open model file"),
        ".", model_files_sel );

  if ( fileName.isEmpty() ) {
     statusBar()->showMessage( tr( "Open canceled." ) );
     updateActions();
     return;
  };

  // find existing
  QMdiSubWindow *existing = findMdiChild(fileName);
  if( existing ) {
    mdiArea->setActiveSubWindow(existing);
    statusBar()->showMessage( tr( "Already opened." ) );
    updateActions();
    return;
  }

  bool rc = doFileOpen( fileName );

  statusBar()->showMessage( rc ? tr( "Ready." ) : tr( "Fail" ) );
}


bool LaboWin::doFileOpen( const QString &fn )
{
  auto  doc = new LaboDoc();
  if( ! doc->openDocument( fn ) ) { // message inside
    delete doc; doc = nullptr;
    updateActions();
    statusBar()->showMessage( tr( "Open Failed." ) );
    return false;
  };

  auto  cw = new LaboView( doc, this );
  addChild( cw );
  return true;
}


void LaboWin::slotFileSave()
{
  statusBar()->showMessage( tr( "Saving model file...") );
  LaboView* m =  qobject_cast<LaboView*>( activeView() );
  if( m ) {
    LaboDoc* doc = m->getDocument();
    if( !doc ) {
      return;
    }
    doc->saveDocument( false );
    m->updateTitle();
    m->update();
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}


void LaboWin::slotFileSaveAs()
{
  statusBar()->showMessage( tr ( "Saving model file under new filename..." ) );
  LaboView* m =  qobject_cast<LaboView*>( activeView() );
  if( !m ) {
    handleError( this, tr("Fail to find active window while saving file!") );
    return;
  }

  LaboDoc* doc = m->getDocument();
  if( !doc ) {
    handleError( this, tr("Fail to get document!") );
    return;
  }

  doc->saveDocument( true );

  m->updateTitle();
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::slotFileClose()
{
  slotWindowClose(); // tmp for now: 1 for now
}

void LaboWin::slotFilePrint()
{
  callLaboViewSlot( "print", tr ( "Printing..." ) );
}

void LaboWin::slotFileSettings()
{
  statusBar()->showMessage( tr ( "Edit settings..." ) );

  ::editObj( this, setti );

  statusBar()->showMessage( tr ( "Ready." ) );
}


void LaboWin::slotFileSaveSett()
{
  statusBar()->showMessage( tr ( "Saving settings..." ) );
  setti->save();
  statusBar()->showMessage( tr ( "Ready." ) );
}

void LaboWin::slotFileQuit()
{
  qApp->closeAllWindows();
}

void LaboWin::slotEditUndo()
{
  // callLaboViewSlot( QSL("undo"), tr ( "Reverting last action..." ) );
}

void LaboWin::slotEditCut()
{
  callLaboViewSlot( "cutElm", tr( "Cutting selection..." ) );
}

void LaboWin::slotEditCopy()
{
  callLaboViewSlot( "copyElm", tr( "Copying selection to clipboard..." ) );
}

void LaboWin::slotEditPaste()
{
  callLaboViewSlot( "pasteElm", tr( "Inserting clipboard contents..." ) );
}


void LaboWin::slotAddObj()
{
  callLaboViewSlot( "addObj", tr( "Creating object..." ) );
}

void LaboWin::slotDelObj()
{
  callLaboViewSlot( "delObj", tr( "Deleting object..." ) );
}

void LaboWin::slotEditObj()
{
  callLaboViewSlot( "editObj", tr( "Editing object..." ) );
}

void LaboWin::slotCutObj()
{
  callLaboViewSlot( "cutObj", tr( "Cutting object..." ) );
}

void LaboWin::slotCopyObj()
{
  callLaboViewSlot( "copyObj", tr( "Copying selected object to clipboard..." ) );
}

void LaboWin::slotPasteObj()
{
  callLaboViewSlot( "pasteObj", tr( "Inserting clipboard contents to object" ) );
}

void LaboWin::slotRenameObj()
{
  callLaboViewSlot( "renameObj", tr( "Renaming object..." ) );
}

void LaboWin::slotCloneObj()
{
  callLaboViewSlot( "cloneObj", tr( "Cloning object..." ) );
}

void LaboWin::slotInfoObj()
{
  callLaboViewSlot( "infoObj", tr( "Information about object..." ) );
}

void LaboWin::slotShowTreeObj()
{
  callLaboViewSlot( "showTreeObj", tr( "Object tree..." ) );
}

void LaboWin::slotTestObj()
{
  callLaboViewSlot( "testObj", tr( "Test object..." ) );
}



void LaboWin::slotViewToolBar()
{
  statusBar()->showMessage( tr( "Toggle toolbar..." ) );
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if( mainToolBar->isVisible() ) {
    mainToolBar->hide();
    act_tbar->setChecked( false );
  } else {
    mainToolBar->show();
    act_tbar->setChecked( true );
  };
  statusBar()->showMessage( tr( "Ready." ) );
}



void LaboWin::slotShowGrid()
{
  int v = ! setti->getDataD( QSL("showGrid"), 0 );
  setti->setData( QSL("showGrid"), v );
  callLaboViewSlot( "update", QSL("Updating View") );
}

void LaboWin::slotShowNames()
{
  int v = ! setti->getDataD( QSL("showNames"), 0 );
  setti->setData( QSL("showNames"), v );
  callLaboViewSlot( "update", QSL("Updating View") );
}

void LaboWin::slotShowIcons()
{
  int v = ! setti->getDataD( QSL("showIcons"), 0 );
  setti->setData( QSL("showIcons"), v );
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotShowLinks()
{
  int v = ! setti->getDataD( QSL("showLinks"), 0 );
  setti->setData( QSL("showLinks"), v );
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotLogClear()
{
  log_app.clear();
  update();
}

void LaboWin::slotLogSave()
{
  statusBar()->showMessage( tr ( "Saving log to file..." ) );

  QString fn = QFileDialog::getSaveFileName( this, tr("Save log"),
      PACKAGE ".log", "Log files (*.log);;All files(*)" );

  if ( !fn.isEmpty() ) {
    QFile of( fn );
    if( ! of.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
      handleError( this, tr("Fail to open file \"") % fn % QSL("\"") );
      return;
    }
    QTextStream os( &of );
    os << log_app << endl;

  };

  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::slotWindowClose()
{
  mdiArea->closeActiveSubWindow();
  updateActions();
}


void LaboWin::slotWindowCloseAll()
{
  mdiArea->closeAllSubWindows();
  updateActions();
}

void LaboWin::slotWindowTile()
{
  mdiArea->tileSubWindows();
  updateActions();
}

void LaboWin::slotWindowCascade()
{
  mdiArea->cascadeSubWindows();
  updateActions();
}


void LaboWin::slotHelpAbout()
{
  QString ostr = QString( PACKAGE "\nVersion " VERSION " build: "
      __DATE__ " " __TIME__ "\n"  COPYRIGHT );
  QMessageBox::about( this, tr( "About..." ), ostr );
}

void LaboWin::slotHelpAboutQt()
{
  QMessageBox::aboutQt( this );
}


void LaboWin::slotStatusHelpMsg( const QString &text )
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->showMessage( text, 2000 );
}


void LaboWin::slotTest()
{
  QString ostr( "Test called" );
  statusBar()->showMessage( tr( "Test something..." ) );
  qDebug() << "Test0 in " << objectName() << WHE;

  ostr += QString(" <b>em</b>=" ) + QSN(em) + "<br/>\n";

  QString ts = "a_0^1{b}c\\approx \\alpha\\cdot\\phi^2 \\omega{}\\n"
               "\\sum_{i=0}^{N-1}{(a+b+\\epsilon)} {\\Psi}^\\gamma \\Bad" % QChar(0x2222);

  QString ls = tex2label( ts );
  //qDebug() <<  " ts=" << ts << WHE;
  //qDebug() <<  " ls=" << ls << WHE;

  ostr += ts % "<br/>\n" % ls %"<br/>\n";

  //ostr += "\nDMIN: " + QSN( DMIN );
  //ostr += " DMAX: "  + QSN( DMAX );
  //ostr += "\niDMIN: " + QSN( (int)(DMIN) );
  //ostr += " iDMAX: "  + QSN( (int)(DMAX) );

  // // test splitName
  // QString first, rest;
  // int idx, nmt;
  // const char* const tst_str[] =  {
  //   "[5]",
  //   "abcd",
  //   "abcd[5]",
  //   "abcd[5].xxx[6]",
  //   "abcd.xxx[6]",
  //   "abcd[5].xxx",
  //   "aaa6.bbb.ccc.ddd.eee",
  //   "aaa6.bbb.ccc.ddd.eee[5]"
  // };
  // for( auto s : tst_str ) {
  //   QString t = s;
  //   nmt = splitName( t, first, rest, idx );
  //   ostr += "t=\"" + t + "\" nmt=" + QSN(nmt) + " first=\"" + first
  //        + "\" rest=\"" + rest + "\" idx=" + QSN(idx) + "\n";
  // }

  ostr += QSL("<hr/>\n");
  auto scriptsDirs = QDir::searchPaths( SCRIPT_DIR );
  ostr += "<b>Scripts dirs</b>: <br/>\n";
  for( auto s: scriptsDirs ) {
    ostr += QSL(" \"") % s % QSL("\"<br>\n");
  }
  auto libDirs = QDir::searchPaths( LIB_DIR );
  ostr += "<b>Lib dirs</b>: <br/>\n";
  for( auto s: libDirs ) {
    ostr += QSL(" \"") % s % QSL("\"<br>\n");
  }
  // ostr += QSL("<br>\n");

  QMessageBox::information( this, tr( "Test" ), ostr, QMessageBox::Ok );
  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::windowMenuAboutToShow()
{
  pWindowMenu->clear();
  pWindowMenu->addAction( act_winClose );
  pWindowMenu->addAction( act_winCloseAll );
  pWindowMenu->addSeparator();
  pWindowMenu->addAction( act_winTile );
  pWindowMenu->addAction( act_winCascade );
  pWindowMenu->addSeparator();
  pWindowMenu->addAction( act_winNext );
  pWindowMenu->addAction( act_winPrev );
  pWindowMenu->addSeparator();

  // add windows to menu
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  QMdiSubWindow *aswin = mdiArea->activeSubWindow();

  for( int i = 0; i < windows.size(); ++i ) {
    QMdiSubWindow *swin = windows.at(i);
    QWidget *child = swin->widget();
    if( !child ) {
      qWarning() << QSL("Null child i=") << QSN(i) << WHE;
      continue;
    }

    QString text;
    if( i < 9 ) {
      text = QSL("&");
    }
    text += QSN( i+1 ) + QSL(" ") + child->windowTitle();

    QAction *action  = pWindowMenu->addAction( text );
    action->setCheckable( true );
    action->setChecked( swin == aswin );
    // cast to fight unbiguity:
    connect( action, &QAction::triggered, windowMapper,
        static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map) );
    windowMapper->setMapping( action, swin );
  }

  updateActions();

}

void LaboWin::slotUpdateLog()
{
  if( lastLogSize != log_app.size() ) {
    logViewer->setPlainText( log_app );
    logViewer->moveCursor( QTextCursor::End );
    logViewer->ensureCursorVisible();

    lastLogSize = log_app.size();
  }
}



void LaboWin::slotqLinkElm()
{
  callLaboViewSlot( "qlinkElm", tr( "Quick linking element..." ) );
}

void LaboWin::slotqpLinkElm()
{
  callLaboViewSlot( "qplinkElm", tr( "Quick parametr linking element..." ) );
}

void LaboWin::slotUnlinkElm()
{
  callLaboViewSlot( "unlinkElm", tr( "Uninking element..." ) );
}

void LaboWin::slotLockElm()
{
  callLaboViewSlot( "lockElm", tr( "Locking element..." ) );
}


void LaboWin::slotMarkElm()
{
  callLaboViewSlot( "markElm", tr( "Marking element..." ) );
}

void LaboWin::slotMoveElm()
{
  callLaboViewSlot( "moveElm", tr( "Moving element..." ) );
}


void LaboWin::slotNewOut()
{
  callLaboViewSlot( "addOut", tr( "Inserting output array..." ) );
}

void LaboWin::slotShowOutData()
{
  callLaboViewSlot( "showOutData", tr( "Data from output array..." ) );
}

void LaboWin::slotExportOut()
{
  callLaboViewSlot( "exportOut", tr( "Exporting data from output array..." ) );
}

void LaboWin::slotNewGraph()
{
  callLaboViewSlot( "addGraph", tr( "Inserting new plot..." ) );
}

void LaboWin::slotShowGraph()
{
  callLaboViewSlot( "showGraph", tr( "Showing plot..." ) );
}

void LaboWin::slotGraphAddOut()
{
  callLaboViewSlot( "graphAddOut", tr( "Adding current output to plot..." ) );
}

void LaboWin::slotShowGraphData()
{
  callLaboViewSlot( "showGraphData", tr( "Showing plot data..." ) );
}

void LaboWin::slotExportGraphData()
{
  callLaboViewSlot( "exportGraphData", tr( "Exporting plot data..." ) );
}



void LaboWin::slotNewSimul()
{
  callLaboViewSlot( "addSimul", tr( "Creating new simulation..." ) );
}

void LaboWin::slotSetActiveSimul()
{
  callLaboViewSlot( "setActiveSimul", tr( "Setting active simulation ..." ) );
}



// ------- model / scheme -----------

void LaboWin::slotEditModel()
{
  callLaboViewSlot( "editModel", tr( "Editing model parametrs..." ) );
}


void LaboWin::slotShowTreeModel()
{
  callLaboViewSlot( "showTreeModel", tr( "Show model tree..." ) );
}

void LaboWin::slotNewScheme()
{
  callLaboViewSlot( "addScheme", tr( "Creating new Scheme..." ) );
}


void LaboWin::importAllSchemes()
{
  callLaboViewSlot( "importAllSchemes", tr( "Importing required schemes..." ) );
}

// -------------------------- run ------------------------------

void LaboWin::slotRunRun()
{
  callLaboViewSlot( "runRun", tr( "Running simulation..." ) );
}

void LaboWin::slotRunPreScript()
{
  callLaboViewSlot( "runPreScript", tr( "Running preRun script..." ) );
}


void LaboWin::slotReset()
{
  callLaboViewSlot( "resetModel", tr( "Reseting model..." ) );
}


void LaboWin::slotInitEngine()
{
  callLaboViewSlot( "initEngine", tr( "Initializing engine..." ) );
}


void LaboWin::slotRunScript()
{
  callLaboViewSlot( "runScript", tr( "Running script..." ) );
}

void LaboWin::slotRunModelScript()
{
  callLaboViewSlot( "runModelScript", tr( "Running model script..." ) );
}

void LaboWin::setActiveSubWindow( QWidget *win )
{
  if( !win ) {
    return;
  }
  QMdiSubWindow *swin =  qobject_cast<QMdiSubWindow *>(win);
  if( !swin ) {
    return;
  }
  mdiArea->setActiveSubWindow( swin );
  updateActions();
}


// end of labowin.cpp

