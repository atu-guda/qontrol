/***************************************************************************
                          labowin.cpp  -  description
                             -------------------
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

#include <cstdlib>
#include <QtWidgets>
#include <QPrinter>
#include <QTextEdit>

#include "miscfun.h"
#include "laboview.h"
#include "structview.h"
#include "labodoc.h"
#include "labowin.h"
#include "mo2settdlg.h"

using namespace std;

LaboWin* LaboWin::labowin = nullptr;

LaboWin::LaboWin()
  : split( new QSplitter( Qt::Vertical, this ) ),
    mdiArea( new QMdiArea ),
    logViewer( new QTextEdit(this) ),
    printer( new QPrinter ),
    log_timer( new QTimer( this ) )
{
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

  setWindowIcon( QIcon( ":icons/app.png" ) );
  if( sett.showmax ) {
    showMaximized();
  }

  logViewer->setReadOnly( true );
  logViewer->setMinimumSize( 50*em, 8*em ); // TODO: fix!
  logViewer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  connect( log_timer, &QTimer::timeout, this, &LaboWin::slotUpdateLog );
  log_timer->setInterval( 1000 ); // 1 sec
  log_timer->start();

  labowin = this;
  updateActions();
}

LaboWin::~LaboWin()
{
  delete printer;
  printer = nullptr;
  labowin = nullptr;
}


void LaboWin::setFonts()
{
  mainFont.fromString( sett.mainFont );
  smallFont.fromString( sett.smallFont );
  plotFont.fromString( sett.plotFont );
  structFont.fromString( sett.structFont );
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
  fileToolbar = addToolBar( tr("File") );
  elmToolbar = addToolBar( tr("Element") );

  // Actions
  // File group

  {
    auto a = new QAction( QIcon( ":icons/filenew.png" ), "&New model", this );
    a->setShortcuts( QKeySequence::New );
    a->setWhatsThis( tr("Click this button to create a new model file.") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "fileNew" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileNew );
    pFileMenu->addAction( a );
    pFileMenu->addSeparator();
    fileToolbar->addAction( a );
  }

  {
    auto a = new QAction( QIcon::fromTheme("document-open"), "Open model", this );
    a->setShortcuts( QKeySequence::Open );
    a->setWhatsThis( tr("Click this button to open a model file." ) );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "fileOpen" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileOpen );
    pFileMenu->addAction( a );
    fileToolbar->addAction( a );
  }

  {
    auto a = new QAction( QIcon::fromTheme("document-save"), "Save model",  this );
    a->setShortcuts( QKeySequence::Save );
    a->setWhatsThis( tr("Click this button to save the model file you are "
          "editing. You will be prompted for a file name." ) );
    registerAction( a, "fileSave" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileSave );
    pFileMenu->addAction( a );
    fileToolbar->addAction( a );
    fileToolbar->addSeparator();
  }

  {
    auto a = new QAction(QIcon::fromTheme("document-save-as"), "Save model As", this );
    a->setWhatsThis( tr("Save current model with another filename") );
    registerAction( a, "fileSaveAs" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileSaveAs );
    pFileMenu->addAction( a );
    pFileMenu->addSeparator();
  }

  {
    auto a = new QAction(  QIcon::fromTheme("document-print"), "&Print", this );
    a->setShortcuts( QKeySequence::Print );
    a->setWhatsThis( tr("Print current model") );
    registerAction( a, "print" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFilePrint );
    pFileMenu->addAction( a );
    pFileMenu->addSeparator();
  }

  {
    auto a = new QAction( "&Close", this );
    a->setWhatsThis( tr("Close active window") );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileClose );
    registerAction( a, "fileClose" );
    pFileMenu->addSeparator();
    pFileMenu->addAction( a );
  }

  {
    auto a = new QAction( "Sett&ings", this );
    a ->setWhatsThis( "Edit application settings" );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "fileSettings" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileSettings );
    pFileMenu->addAction( a );
  }

  {
    auto a = new QAction( "Save s&ettings", this );
    a ->setWhatsThis( "Save application settings" );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "fileSaveSett" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileSaveSett );
    pFileMenu->addAction( a );
    pFileMenu->addSeparator();
  }


  {
    auto a = new QAction(  QIcon::fromTheme("application-exit"), "&Quit", this );
    a->setShortcuts( QKeySequence::Quit );
    a->setWhatsThis( tr("Click this button to quit application.") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "fileQuit" );
    connect( a, &QAction::triggered, this, &LaboWin::slotFileQuit );
    pFileMenu->addAction( a );
  }

  // ==== Edit + Object group
  {
    auto a = new QAction( QIcon::fromTheme("edit-undo"), "&Undo", this );
    a->setShortcuts( QKeySequence::Undo );
    a->setWhatsThis( tr("Undo last action") + " EMPTY" );
    registerAction( a, "editUndo" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditUndo );
    pEditMenu->addAction( a );
  }

  // -- object part
  {
    auto a = new QAction(  QIcon::fromTheme("document-new"),"New object", this ); // icon?
    a->setShortcut( Qt::Key_Insert );
    a->setWhatsThis( tr("Create new object") );
    registerAction( a, "newObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewObj );
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction(  QIcon::fromTheme("edit-delete"),"Delete object", this );
    a->setShortcut( Qt::Key_Delete );
    a->setWhatsThis( tr("Delete object") );
    registerAction( a, "delObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelObj );
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction(  QIcon::fromTheme("edit-node"),"Edit object", this );
    a->setShortcut( Qt::Key_Enter );
    a->setWhatsThis( tr("Edit object") );
    registerAction( a, "editObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditObj );
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction(  QIcon::fromTheme("edit-cut"),"Cut Object", this );
    a->setShortcuts( QKeySequence::Cut );
    a->setWhatsThis( tr("Cut selected object") );
    registerAction( a, "cutObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCutObj );
    pEditMenu->addSeparator();
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction( QIcon::fromTheme("edit-copy"), "Copy Object", this );
    a->setShortcuts( QKeySequence::Copy );
    a->setWhatsThis( tr("Copy selected object") );
    registerAction( a, "copyObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCopyObj );
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction( QIcon::fromTheme("edit-paste"), "Paste Object", this );
    a->setShortcuts( QKeySequence::Paste );
    a->setWhatsThis( tr("Paste selected object") );
    registerAction( a, "pasteObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotPasteObj );
    pEditMenu->addAction( a );
    pEditMenu->addSeparator();
  }
  {
    auto a = new QAction( "Rename object", this );
    a->setWhatsThis( tr("Rename selected object") );
    registerAction( a, "renameObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameObj );
    pEditMenu->addAction( a );
  }
  {
    auto a = new QAction( "Clone object", this );
    a->setWhatsThis( tr("Clone selected object") );
    registerAction( a, "cloneObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCloneObj );
    pEditMenu->addAction( a );
    pEditMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon( ":icons/infoelm.png" ), "show &Info", this );
    a->setShortcut( Qt::Key_I );
    a->setWhatsThis( tr("Show information about object structure") );
    registerAction( a, "infoObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotInfoObj );
    pEditMenu->addAction( a );
    elmToolbar->addAction( a );
    elmToolbar->addSeparator();
  }
  {
    auto a = new QAction( "show object tree", this );
    a->setWhatsThis( tr("Show tree-like object structure") );
    registerAction( a, "showTreeObj" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowTreeObj );
    pEditMenu->addAction( a );
    pEditMenu->addSeparator();
  }


  // ==== Element group
  {
    auto a = new QAction( QIcon( ":icons/newelm.png" ), "&New element", this );
    // a->setShortcut( Qt::Key_Insert );
    a->setWhatsThis( tr("Create new element") );
    registerAction( a, "addElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewElm );
    pElmMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/delelm.png" ), "&Delete element", this );
    // a->setShortcut( Qt::Key_Delete );
    a->setWhatsThis( tr("Delete selected element") );
    registerAction( a, "delElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelElm );
    pElmMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/editelm.png" ), "&Edit element", this );
    // a->setShortcut( Qt::Key_Enter );
    a->setWhatsThis( tr("Edit selected element") );
    registerAction( a, "editElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditElm );
    pElmMenu->addAction( a );
    pElmMenu->addSeparator();
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Rename element", this );
    a->setWhatsThis( tr("Rename selected element") );
    registerAction( a, "renameElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameElm );
    pElmMenu->addAction( a );
    pElmMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon::fromTheme("insert-link"), "&Quick link element", this );
    a->setShortcut( Qt::CTRL+Qt::Key_L );
    a->setWhatsThis( tr("Link marked to selected element") );
    registerAction( a, "qlinkElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotqLinkElm );
    pElmMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Parametr link element", this );
    a->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_L );
    a->setWhatsThis( tr("Link marked to selected element "
          "(parameter input)") );
    registerAction( a, "qplinkElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotqpLinkElm );
    pElmMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Unlink element", this );
    a->setWhatsThis( tr("Remove links of selected element") );
    registerAction( a, "unlinkElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotUnlinkElm );
    pElmMenu->addAction( a );
    pElmMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon( ":icons/lockelm.png" ), "Loc&k element",  this );
    a->setShortcut( Qt::CTRL+Qt::Key_K );
    a->setWhatsThis( tr("Lock current element") );
    registerAction( a, "lockElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotLockElm );
    pElmMenu->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/orderelm.png" ), "Change &Orger", this );
    a->setShortcut( Qt::Key_O );
    a->setWhatsThis( tr("Change order numper of selected element") );
    registerAction( a, "ordElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotOrdElm );
    pElmMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a= new QAction( QIcon( ":icons/markelm.png" ), "&Mark element", this );
    a->setShortcut( Qt::Key_M );
    a->setWhatsThis( tr("Mark selected element") );
    registerAction( a, "markElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotMarkElm );
    pElmMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Move element", this );
    a->setShortcut( Qt::SHIFT+Qt::Key_M );
    a->setWhatsThis( tr("Move marked element to selected cell") );
    registerAction( a, "moveElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotMoveElm );
    pElmMenu->addAction( a );
    pElmMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon( ":icons/infoelm.png" ), "show &Info", this );
    a->setShortcut( Qt::Key_I );
    a->setWhatsThis( tr("Show information about element structure") );
    registerAction( a, "infoElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotInfoElm );
    pElmMenu->addAction( a );
    elmToolbar->addAction( a );
    elmToolbar->addSeparator();
  }
  {
    auto a = new QAction( "show element tree", this );
    a->setWhatsThis( tr("Show tree-like element structure") );
    registerAction( a, "showTreeElm" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowTreeElm );
    pElmMenu->addAction( a );
    pElmMenu->addSeparator();
  }
  {
    auto a = new QAction( "test element 1", this );
    a->setWhatsThis( tr("Test something in element") );
    registerAction( a, "testElm1" );
    connect( a, &QAction::triggered, this, &LaboWin::slotTestElm1 );
    pElmMenu->addAction( a );
  }
  {
    auto a = new QAction( "test element 2", this );
    a->setWhatsThis( tr("Test something in element") );
    registerAction( a, "testElm2" );
    connect( a, &QAction::triggered, this, &LaboWin::slotTestElm2 );
    pElmMenu->addAction( a );
  }

  // ==== out group
  {
    auto a = new QAction( QIcon( ":icons/newout.png" ), "&New Out", this );
    a->setShortcut( Qt::Key_U );
    a->setWhatsThis( tr("Create output collector") );
    registerAction( a, "addOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewOut );
    pOutMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/delout.png" ), "&Delete out", this );
    a->setWhatsThis( tr("Delete output collector with current level") );
    registerAction( a, "delOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelOut );
    pOutMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/editout.png" ), "&Edit out", this );
    a->setShortcut( Qt::SHIFT+Qt::Key_U );
    a->setWhatsThis( tr("Edit outsput collector with current level") );
    registerAction( a, "editOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditOut );
    pOutMenu->addAction( a );
    pOutMenu->addSeparator();
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Rename output collector", this );
    a->setWhatsThis( tr("Rename output collector") );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameOut );
    registerAction( a, "renameOut" );
    pOutMenu->addAction( a );
    pOutMenu->addSeparator();
  }
  {
    auto a = new QAction( "Select out", this );
    a->setShortcut( Qt::ALT+Qt::Key_U );
    a->setWhatsThis( tr("Select output collector by current level") );
    registerAction( a, "selectOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotSelectOut );
    pOutMenu->addAction( a );
    pOutMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon( ":icons/showoutdata.png" ), "&Show out data", this );
    a->setWhatsThis( tr("Show data collected by output.") );
    registerAction( a, "showOutData" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowOutData );
    pOutMenu->addAction( a );
    elmToolbar->addAction( a );
    elmToolbar->addSeparator();
  }
  {
    auto a = new QAction( "E&xport out data", this );
    a->setShortcut( Qt::CTRL+Qt::Key_E );
    a->setWhatsThis( tr("Export data collected by output to text file.") );
    registerAction( a, "exportOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotExportOut );
    pOutMenu->addAction( a );
  }

  // ==== graph=plot group
  {
    auto a = new QAction( QIcon( ":icons/newgraph.png" ), "&New Plot", this );
    a->setShortcut( Qt::Key_P );
    a->setWhatsThis( tr("Create new plot") );
    registerAction( a, "addGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewGraph );
    pGraphMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/delgraph.png" ), "&Delete plot", this );
    a->setWhatsThis( tr("Delete plot with selected level") );
    registerAction( a, "delGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelGraph );
    pGraphMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( QIcon( ":icons/editgraph.png" ), "&Edit plot", this );
    a->setShortcut(  Qt::SHIFT+Qt::Key_P );
    a->setWhatsThis( tr("Edit plot") );
    registerAction( a, "editGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditGraph );
    pGraphMenu->addAction( a );
    pGraphMenu->addSeparator();
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Rename plot", this );
    a->setWhatsThis( tr("Rename plot") );
    registerAction( a, "renameGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameGraph );
    pGraphMenu->addAction( a );
    pGraphMenu->addSeparator();
  }
  {
    auto a = new QAction( "select plot", this );
    a->setShortcut(  Qt::ALT+Qt::Key_G ); // Alt-P is busy by menu
    a->setWhatsThis( tr("Select plot by current level") );
    registerAction( a, "selectGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotSelectGraph );
    pGraphMenu->addAction( a );
    pGraphMenu->addSeparator();
  }
  {
    auto a = new QAction( QIcon( ":icons/showgraph.png" ), "&Show plot", this );
    a->setShortcut( Qt::Key_S );
    a->setWhatsThis( tr("Show plot") );
    registerAction( a, "showGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowGraph );
    pGraphMenu->addAction( a );
    pGraphMenu->addSeparator();
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Add out to plot", this );
    a->setShortcut( Qt::CTRL | Qt::Key_U );
    a->setWhatsThis( tr("Add current output array to plot") );
    registerAction( a, "graphAddOut" );
    connect( a, &QAction::triggered, this, &LaboWin::slotGraphAddOut );
    pGraphMenu->addAction( a );
    pGraphMenu->addSeparator();
  }

  {
    auto a = new QAction( QIcon( ":icons/showgraphdata.png" ), "show plot Data", this );
    a->setShortcut(  Qt::SHIFT+Qt::Key_D );
    a->setWhatsThis( tr("Show plot data") );
    registerAction( a, "showGraphData" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowGraphData );
    pGraphMenu->addAction( a );
    elmToolbar->addAction( a );
    elmToolbar->addSeparator();
  }
  {
    auto a = new QAction( "E&xport plot data", this );
    a->setShortcut( Qt::SHIFT+Qt::Key_E );
    a->setWhatsThis( tr("Export plot data to text file") );
    registerAction( a, "exportGraphData" );
    connect( a, &QAction::triggered, this, &LaboWin::slotExportGraphData);
    pGraphMenu->addAction( a );
  }
  {
    auto a = new QAction( "Clone plot", this );
    a->setWhatsThis( tr("Clone current plot") );
    registerAction( a, "cloneGraph" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCloneGraph );
    pGraphMenu->addAction( a );
  }

  // ==== simulation group
  {
    auto a = new QAction( "&New Simulation", this );
    a->setWhatsThis( tr("Create new simulation") );
    registerAction( a, "addSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewSimul );
    pSimulMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Delete Simulation", this );
    a->setWhatsThis( tr("Delete simulation") );
    registerAction( a, "delSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelSimul );
    pSimulMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Edit Simulation", this );
    a->setShortcut(  Qt::SHIFT+Qt::Key_Y );
    a->setWhatsThis( tr("Edit simulation") );
    registerAction( a, "editSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditSimul );
    pSimulMenu->addAction( a );
    pSimulMenu->addSeparator();
  }
  {
    auto a = new QAction( "Rename simulation", this );
    a->setWhatsThis( tr("Rename simulation") );
    registerAction( a, "renameSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameSimul );
    pSimulMenu->addAction( a );
    pSimulMenu->addSeparator();
  }
  {
    auto a = new QAction( "Select Simulation", this );
    a->setShortcut(  Qt::ALT+Qt::Key_Y );
    a->setWhatsThis( tr("Setect simulation") );
    registerAction( a, "selectSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotSelectSimul );
    pSimulMenu->addAction( a );
    pSimulMenu->addSeparator();
  }
  {
    auto a = new QAction( "Set Active Simulation", this );
    a->setShortcut( Qt::ALT+Qt::CTRL+Qt::Key_Y );
    a->setWhatsThis( tr("Set current simulation to be active") );
    registerAction( a, "setActiveSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotSetActiveSimul );
    pSimulMenu->addAction( a );
  }

  {
    auto a = new QAction( "Clone Simulation", this );
    a->setWhatsThis( tr("Clone current simulation") );
    registerAction( a, "cloneSimul" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCloneSimul );
    pSimulMenu->addAction( a );
  }

  // ==== model/Scheme group
  {
    auto a = new QAction( QIcon( ":icons/editmodel.png" ), "&Edit Model", this );
    a->setShortcut(  Qt::CTRL+Qt::Key_R );
    a->setWhatsThis( tr("Edit model parameters.") );
    registerAction( a, "editModel" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditModel );
    pModelMenu->addAction( a );
    elmToolbar->addAction( a );
    elmToolbar->addSeparator();
  }
  {
    auto a = new QAction( "show model tree", this );
    a->setShortcut( Qt::CTRL+Qt::Key_T );
    a->setWhatsThis( tr("Show tree-like model structure") );
    registerAction( a, "showTreeModel" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowTreeModel );
    pModelMenu->addAction( a );
    pModelMenu->addSeparator();
  }
  //
  {
    auto a = new QAction( "&New Scheme", this );
    a->setWhatsThis( tr("Create new scheme") );
    registerAction( a, "addScheme" );
    connect( a, &QAction::triggered, this, &LaboWin::slotNewScheme );
    pModelMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Delete Scheme", this );
    a->setWhatsThis( tr("Delete scheme") );
    registerAction( a, "delScheme" );
    connect( a, &QAction::triggered, this, &LaboWin::slotDelScheme );
    pModelMenu->addAction( a );
  }
  {
    auto a = new QAction( "&Edit Scheme", this );
    a->setWhatsThis( tr("Edit scheme") );
    registerAction( a, "editScheme" );
    connect( a, &QAction::triggered, this, &LaboWin::slotEditScheme );
    pModelMenu->addAction( a );
    pModelMenu->addSeparator();
  }
  {
    auto a = new QAction( "Rename scheme", this );
    a->setWhatsThis( tr("Rename scheme") );
    registerAction( a, "renameScheme" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRenameScheme );
    pModelMenu->addAction( a );
  }
  {
    auto a = new QAction( "Clone Scheme", this );
    a->setWhatsThis( tr("Cone current scheme") );
    registerAction( a, "cloneScheme" );
    connect( a, &QAction::triggered, this, &LaboWin::slotCloneScheme );
    pModelMenu->addAction( a );
  }

  // ====  run group
  {
    auto a = new QAction( QIcon( ":icons/run.png" ), "&Run", this );
    a->setShortcut( Qt::Key_F9 );
    a->setWhatsThis( tr("Click this button to start simulation.") );
    registerAction( a, "runRun" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRunRun );
    pRunMenu->addAction( a );
    pRunMenu->addSeparator();
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "Rese&t", this );
    a->setWhatsThis( tr("Reset model state.") );
    registerAction( a, "reset" );
    connect( a, &QAction::triggered, this, &LaboWin::slotReset );
    pRunMenu->addSeparator();
    pRunMenu->addAction( a );
  }
  {
    auto a = new QAction( "Init engine", this );
    a->setWhatsThis( tr("Initialize model JS engine.") );
    registerAction( a, "initEngine" );
    connect( a, &QAction::triggered, this, &LaboWin::slotInitEngine );
    pRunMenu->addAction( a );
  }
  {
    auto a = new QAction( QIcon::fromTheme("application-x-javascript"), "run &Script", this );
    a->setShortcut( Qt::SHIFT+Qt::Key_F9 );
    a->setWhatsThis( tr("Run script on model") );
    connect( a, &QAction::triggered, this, &LaboWin::slotRunScript );
    pRunMenu->addAction( a );
    elmToolbar->addAction( a );
  }
  {
    auto a = new QAction( "run &Model script", this );
    a->setWhatsThis( tr("Run inner model script") );
    registerAction( a, "runModelScript" );
    connect( a, &QAction::triggered, this, &LaboWin::slotRunModelScript );
    pRunMenu->addAction( a );
  }

  // ==== iface group
  {
    auto a = new QAction( "View &Toolbar", this );
    a->setCheckable( true );
    a->setChecked( true );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "viewToolBar" );
    connect( a, &QAction::triggered, this, &LaboWin::slotViewToolBar );
    act_tbar = a; // to on/off
  }
  {
    auto a = new QAction( "View &Statusbar", this );
    a->setCheckable( true );
    a->setChecked( true );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "viewStatusBar" );
    connect( a, &QAction::triggered, this, &LaboWin::slotViewStatusBar );
    pViewMenu->addAction( a );
    pViewMenu->addSeparator();
    act_sbar = a;
  }
  {
    auto a = new QAction( "Show &Orders", this );
    a->setCheckable( true );
    a->setChecked( sett.showord );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "showOrd" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowOrd );
    pViewMenu->addAction( a );
  }
  {
    auto a = new QAction( "Show &Grid", this );
    a->setCheckable( true );
    a->setChecked( sett.showgrid );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "showGrid" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowGrid );
    pViewMenu->addAction( a );
  }
  {
    auto a = new QAction( "Show &Names", this );
    a->setCheckable( true );
    a->setChecked( sett.shownames );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "showNames" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowNames );
    pViewMenu->addAction( a );
  }
  {
    auto a = new QAction( "Show &Icons", this );
    a->setCheckable( true );
    a->setChecked( sett.showicons );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "showIcons" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowIcons );
    pViewMenu->addAction( a );
  }
  {
    auto a = new QAction( "Show &Links", this );
    a->setCheckable( true );
    a->setChecked( sett.showLinks );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "showLinks" );
    connect( a, &QAction::triggered, this, &LaboWin::slotShowLinks );
    pViewMenu->addAction( a );
    pViewMenu->addSeparator();
  }
  {
    auto a = new QAction( "Clear log", this );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "logClear" );
    connect( a, &QAction::triggered, this, &LaboWin::slotLogClear );
    pViewMenu->addAction( a );
  }
  {
    auto a = new QAction( "Save log", this );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "logSave" );
    connect( a, &QAction::triggered, this, &LaboWin::slotLogSave );
    pViewMenu->addAction( a );
  }

  // ==== window group
  {
    auto a = new QAction( QIcon::fromTheme("window-close"), "Cl&ose Window", this);
    a->setWhatsThis( tr("Close this window") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "windowClose" );
    connect( a, &QAction::triggered, this, &LaboWin::slotWindowClose );
    act_winClose = a;
  }
  {
    auto a = new QAction( "Close &All Windows",  this );
    a->setWhatsThis( tr("Close all windoww") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "windowCloseAll" );
    connect( a, &QAction::triggered, this, &LaboWin::slotWindowCloseAll );
    act_winCloseAll = a;
  }
  {
    auto a = new QAction( "&Tile Windows",  this );
    a->setWhatsThis( tr("Tile the windows") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "windowTile" );
    connect( a, &QAction::triggered, this, &LaboWin::slotWindowTile );
    act_winTile = a;
  }
  {
    auto a = new QAction( "&Cascade Windows",  this );
    a->setWhatsThis( tr("Cascade the windows") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "windowCascase" );
    connect( a, &QAction::triggered, this, &LaboWin::slotWindowCascade );
    act_winCascade = a;
  }
  {
    auto a = new QAction( "Next window",  this );
    a->setShortcut( Qt::Key_F6 );
    a->setWhatsThis( tr("Activate next window") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "nextSubWindow" );
    connect( a, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow );
    act_winNext = a;
  }
  {
    auto a = new QAction( "Previous window",  this );
    a->setWhatsThis( tr("Activate previous window") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "prevSubWindow" );
    connect( a, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow );
    act_winPrev = a;
  }


  // ==== help group
  {
    auto a = new QAction( QIcon::fromTheme("help-about"), "&About", this );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "helpAbout" );
    connect( a, &QAction::triggered, this, &LaboWin::slotHelpAbout );
    pHelpMenu->addAction( a );
  }
  {
    auto a = new QAction( "About &Qt", this );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "helpAboutQt" );
    connect( a, &QAction::triggered, this, &LaboWin::slotHelpAboutQt );
    pHelpMenu->addAction( a );
  }

  {
    auto a = new QAction( QIcon( ":icons/test.png" ), "&Test", this );
    a->setWhatsThis( tr("Click this button to test something.") );
    a->setProperty( "alwaysVisible", true );
    registerAction( a, "test" );
    connect( a, &QAction::triggered, this, &LaboWin::slotTest );
    pEditMenu->addAction( a );
    fileToolbar->addAction( a );
    fileToolbar->addSeparator();
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
    qWarning() << "w is nullptr in " << WHE;
    return nullptr;
  }
  w->setAttribute( Qt::WA_DeleteOnClose );

  QMdiSubWindow *subw = mdiArea->addSubWindow( w );
  if( !subw ) {
    qWarning() << "subw is nullptr in " << WHE;
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

  auto dia = new Mo2SettDlg( sett, this );
  if ( dia->exec() == QDialog::Accepted ) {
    setFonts();
  };

  statusBar()->showMessage( tr ( "Ready." ) );
}


void LaboWin::slotFileSaveSett()
{
  statusBar()->showMessage( tr ( "Saving settings..." ) );
  sett.save();
  statusBar()->showMessage( tr ( "Ready." ) );
}

void LaboWin::slotFileQuit()
{
  qApp->closeAllWindows();
}

void LaboWin::slotEditUndo()
{
  // callLaboViewSlot( "undo", tr ( "Reverting last action..." ) );
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


void LaboWin::slotNewObj()
{
  callLaboViewSlot( "newObj", tr( "Creating object..." ) );
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


void LaboWin::slotViewToolBar()
{
  statusBar()->showMessage( tr( "Toggle toolbar..." ) );
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if( fileToolbar->isVisible() ) {
    fileToolbar->hide();
    elmToolbar->hide();
    act_tbar->setChecked( false );
  } else {
    fileToolbar->show();
    elmToolbar->show();
    act_tbar->setChecked( true );
  };
  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::slotViewStatusBar()
{
  statusBar()->showMessage( tr( "Toggle statusbar..." ) );
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if( statusBar()->isVisible() ) {
    statusBar()->hide();
    act_sbar->setChecked( false );
  } else {
    statusBar()->show();
    act_sbar->setChecked( true );
  };

  statusBar()->showMessage( tr( "Ready." ) );
}

void LaboWin::slotShowOrd()
{
  sett.showord = ! sett.showord;
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotShowGrid()
{
  sett.showgrid = ! sett.showgrid;
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotShowNames()
{
  sett.shownames = ! sett.shownames;
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotShowIcons()
{
  sett.showicons = ! sett.showicons;
  callLaboViewSlot( "update", "Updating View" );
}

void LaboWin::slotShowLinks()
{
  sett.showLinks = ! sett.showLinks;
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


void LaboWin::slotStatusHelpMsg(const QString &text)
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
               "\\sum_{i=0}^{N-1}{(a+b+\\epsilon)} {\\Psi}^\\gamma";

  QString ls = tex2label( ts );
  //qDebug() <<  " ts=" << ts << WHE;
  //qDebug() <<  " ls=" << ls << WHE;

  ostr += ts % "<br/>\n" % ls %"<br/>\n";

  // // output some codes
  // for( unsigned ccode=0x03A0; ccode < 0x0400; ++ccode )  {
  //   ostr += QChar( ccode );
  //   if( (ccode & 0x1F) == 0x1F )
  //     ostr += " <:> " + QSN(ccode,16) + "\n";
  // }
  // ostr += "\n";

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
      qWarning() << "Null child i=" << QSN(i) << WHE;
      continue;
    }

    QString text;
    if( i < 9 ) {
      text = QSL( "&" );
    }
    text += QSN( i+1 ) + QSL( " " ) + child->windowTitle();

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



void LaboWin::slotNewElm()
{
  callLaboViewSlot( "addElm", tr( "Inserting new element..." ) );
}

void LaboWin::slotDelElm()
{
  callLaboViewSlot( "delElm", tr( "Deleting selected element..." ) );
}

void LaboWin::slotEditElm()
{
  callLaboViewSlot( "editElm", tr( "Editing element properties..." ) );
}

void LaboWin::slotRenameElm()
{
  callLaboViewSlot( "renameElm", tr( "Renaming element ..." ) );
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


void LaboWin::slotOrdElm()
{
  callLaboViewSlot( "ordElm", tr( "Setting new order of element..." ) );
}

void LaboWin::slotMarkElm()
{
  callLaboViewSlot( "markElm", tr( "Marking element..." ) );
}

void LaboWin::slotMoveElm()
{
  callLaboViewSlot( "moveElm", tr( "Moving element..." ) );
}

void LaboWin::slotInfoElm()
{
  callLaboViewSlot( "infoElm", tr( "Information about element..." ) );
}

void LaboWin::slotShowTreeElm()
{
  callLaboViewSlot( "showTreeElm", tr( "Show element tree..." ) );
}

void LaboWin::slotTestElm1()
{
  callLaboViewSlot( "testElm1", tr( "Test element 1..." ) );
}

void LaboWin::slotTestElm2()
{
  callLaboViewSlot( "testElm2", tr( "Test element 2..." ) );
}


void LaboWin::slotNewOut()
{
  callLaboViewSlot( "addOut", tr( "Inserting output array..." ) );
}

void LaboWin::slotDelOut()
{
  callLaboViewSlot( "delOut", tr( "Deleting output array..." ) );
}

void LaboWin::slotEditOut()
{
  callLaboViewSlot( "editOut", tr( "Editing output array..." ) );
}

void LaboWin::slotRenameOut()
{
  callLaboViewSlot( "renameOut", tr( "Renaming output collector ..." ) );
}

void LaboWin::slotSelectOut()
{
  callLaboViewSlot( "selectOut", tr( "Selectting output array..." ) );
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

void LaboWin::slotDelGraph()
{
  callLaboViewSlot( "delGraph", tr( "Deleting plot..." ) );
}

void LaboWin::slotEditGraph()
{
  callLaboViewSlot( "editGraph", tr( "Editing plot..." ) );
}

void LaboWin::slotRenameGraph()
{
  callLaboViewSlot( "renameGraph", tr( "Renaming plot ..." ) );
}

void LaboWin::slotSelectGraph()
{
  callLaboViewSlot( "selectGraph", tr( "Selecting plot..." ) );
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

void LaboWin::slotCloneGraph()
{
  callLaboViewSlot( "cloneGraph", tr( "Cloning plot ..." ) );
}



void LaboWin::slotNewSimul()
{
  callLaboViewSlot( "addSimul", tr( "Creating new simulation..." ) );
}

void LaboWin::slotDelSimul()
{
  callLaboViewSlot( "delSimul", tr( "Removing simulation" ) );
}

void LaboWin::slotEditSimul()
{
  callLaboViewSlot( "editSimul", tr( "Editing simulation data..." ) );
}

void LaboWin::slotRenameSimul()
{
  callLaboViewSlot( "renameSimul", tr( "Renaming simulation ..." ) );
}

void LaboWin::slotSelectSimul()
{
  callLaboViewSlot( "selectSimul", tr( "Selectiong simulation ..." ) );
}


void LaboWin::slotSetActiveSimul()
{
  callLaboViewSlot( "setActiveSimul", tr( "Setting active simulation ..." ) );
}


void LaboWin::slotCloneSimul()
{
  callLaboViewSlot( "cloneSimul", tr( "Cloning simulation ..." ) );
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

void LaboWin::slotDelScheme()
{
  callLaboViewSlot( "delScheme", tr( "Removing Scheme" ) );
}

void LaboWin::slotEditScheme()
{
  callLaboViewSlot( "editScheme", tr( "Editing Scheme data..." ) );
}

void LaboWin::slotRenameScheme()
{
  callLaboViewSlot( "renameScheme", tr( "Renaming Scheme ..." ) );
}


void LaboWin::slotCloneScheme()
{
  callLaboViewSlot( "cloneScheme", tr( "Cloning Scheme ..." ) );
}


// -------------------------- run ------------------------------

void LaboWin::slotRunRun()
{
  callLaboViewSlot( "runRun", tr( "Running simple loop..." ) );
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

// =============================== Mo2Settings =============================

Mo2Settings::Mo2Settings()
{
  load();
}

void Mo2Settings::load()
{
  QSettings sets( ORG, PACKAGE );
  sets.beginGroup("iface");
  /* -------- flags ------------ */
  showord   = sets.value("view/showord", false ).toBool();
  showgrid  = sets.value("view/showgrid", true ).toBool();
  shownames = sets.value("view/shownames", true ).toBool();
  showicons = sets.value("view/showicons", true ).toBool();
  showLinks = sets.value("view/showLinks", true ).toBool();
  showmax   = sets.value("view/showmax", true ).toBool();
  /* -------- fonts ----------- */
  //  "Arial,10,-1,5,50,0,0,0,0,0"
  QFont df;
  QString dfs = df.toString();
  mainFont    = sets.value("fonts/main", dfs ).toString();
  smallFont   = sets.value("fonts/small", "Sans,7" ).toString();
  plotFont    = sets.value("fonts/plot", "Sans,8" ).toString();
  structFont  = sets.value("fonts/struct", "Sans,8" ).toString();
  // misc
  editCmd  = sets.value("editCmd", "gvim -f " ).toString();
  sets.endGroup();
}

void Mo2Settings::save() const
{
  QSettings sets( ORG, PACKAGE );
  sets.beginGroup("iface");
  /* ---------- flags ------------- */
  sets.setValue( "view/showord", showord );
  sets.setValue( "view/showgrid", showgrid );
  sets.setValue( "view/shownames", shownames );
  sets.setValue( "view/showicons", showicons );
  sets.setValue( "view/showLinks", showLinks );
  sets.setValue( "view/showmax", showmax );
  /* ---------- fonts ------------- */
  sets.setValue( "fonts/main", mainFont );
  sets.setValue( "fonts/small", smallFont );
  sets.setValue( "fonts/plot", plotFont );
  sets.setValue( "fonts/struct", structFont );
  // ------------- more -------------
  sets.setValue( "editCmd", editCmd );

  sets.endGroup();
  sets.sync();

}


// end of labowin.cpp

