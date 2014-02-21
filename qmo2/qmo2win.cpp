/***************************************************************************
                          qmo2.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2013 by atu
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

#include "miscfun.h"
#include "qmo2view.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "mo2settdlg.h"

using namespace std;

QMo2Win* QMo2Win::qmo2win = 0;

QMo2Win::QMo2Win(void)
{
  setWindowTitle( PACKAGE " " VERSION );

  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  setCentralWidget( mdiArea );

  connect( mdiArea, &QMdiArea::subWindowActivated, this, &QMo2Win::updateActions );
  windowMapper = new QSignalMapper(this);
  connect( windowMapper,
      static_cast<void (QSignalMapper::*)(QWidget*)>(&QSignalMapper::mapped),
      this, &QMo2Win::setActiveSubWindow );

  printer = new QPrinter;
  untitledCount = 0;

  initDirs();
  // call inits to invoke all other construction parts
  setFonts();
  initIface();
  initStatusBar();

  setWindowIcon( QIcon( ":icons/app.png" ) );
  if( sett.showmax )
    showMaximized();
  qmo2win = this;
  updateActions();
}

QMo2Win::~QMo2Win()
{
  delete printer;
  printer = 0;
  qmo2win = 0;
}

void QMo2Win::initDirs() // TODO: remove or rewrite
{
  QStringList dl;
  QString app = L8B( "share/" PACKAGE );
  QString sep = L8B( "/" );
  QString prefix = L8B( "/usr" ); // TODO: from config

  QString xbuild_dir;
  QDir d( qApp->applicationDirPath() );
  d.makeAbsolute();
  xbuild_dir = d.canonicalPath() + sep + app;
  // DBGx( "dbg: test build dir: \"%s\"", qP(xbuild_dir) );
  if( d.exists( L8B( "main.cpp" ) )
      && d.exists( app  ) ) {
    dl << xbuild_dir;
    // DBGx( "dbg: add build dir: \"%s\"", qP(xbuild_dir) );
  };

  QString global_dir;
  d.setPath( prefix + sep + app );
  global_dir = d.canonicalPath();
  // DBGx( "dbg: test global dir: \"%s\"", qP(global_dir) );
  if( d.isReadable() ) {
    dl << global_dir;
    // DBGx( "dbg: add global dir: \"%s\"", qP(global_dir) );
  };

  QString local_dir;
  d.setPath( QDir::homePath() + sep + app );
  d.makeAbsolute();
  local_dir = d.canonicalPath();
  // DBGx( "dbg: test local dir: \"%s\"", qP(local_dir) );
  if( d.isReadable() ) {
    dl << local_dir;
    // DBGx( "dbg: add local dir: \"%s\"", qP(local_dir) );
  };


  QString env_dir;
  const char *evar = getenv("QMO2XDIR");
  if( evar ) {
    d.setPath( L8B(evar) );
    if( d.isReadable() ) {
      d.makeAbsolute();
      env_dir = d.canonicalPath();
      dl << env_dir;
      DBGx( "dbg: add env dir: \"%s\"", qP(env_dir) );
    };
  };

  QDir::setSearchPaths("app", dl );
  // DBGx( "dbg: app dirs: \"%s\"", qP( QDir::searchPaths("app").join(" ") ) );

}

void QMo2Win::setFonts()
{
  mainFont.fromString( sett.mainFont );
  smallFont.fromString( sett.smallFont );
  plotFont.fromString( sett.plotFont );
  structFont.fromString( sett.structFont );
  QApplication::setFont( mainFont );
}


void QMo2Win::initIface()
{
  // Actions
  // File group

  act_new = new QAction( QIcon( ":icons/filenew.png" ), "&New model", this );
  act_new->setShortcuts( QKeySequence::New );
  act_new->setWhatsThis( tr("Click this button to create a new model file.") );
  connect( act_new, &QAction::triggered, this, &QMo2Win::slotFileNew );

  act_openxml = new QAction( QIcon::fromTheme("document-open"), "Open model", this );
  act_openxml->setShortcuts( QKeySequence::Open );
  act_openxml->setWhatsThis( tr("Click this button to open a model file." ) );
  connect( act_openxml, &QAction::triggered, this, &QMo2Win::slotFileOpenXML );

  act_close = new QAction( "&Close", this );
  act_close->setWhatsThis( tr("Close active window") );
  connect( act_close, &QAction::triggered, this, &QMo2Win::slotFileClose );

  act_savexml = new QAction( QIcon::fromTheme("document-save"), "Save model",  this );
  act_savexml->setShortcuts( QKeySequence::Save );
  act_savexml->setWhatsThis( tr("Click this button to save the model file you are "
                  "editing. You will be prompted for a file name." ) );
  connect( act_savexml, &QAction::triggered, this, &QMo2Win::slotFileSaveXML );

  act_savexmlas = new QAction(QIcon::fromTheme("document-save-as"), "Save model As", this );
  act_savexmlas->setWhatsThis( tr("Save current model with another filename") );
  connect( act_savexmlas, &QAction::triggered, this, &QMo2Win::slotFileSaveXMLAs );

  act_print = new QAction(  QIcon::fromTheme("document-print"), "&Print", this );
  act_print->setShortcuts( QKeySequence::Print );
  act_print->setWhatsThis( tr("Print current model") );
  connect( act_print, &QAction::triggered, this, &QMo2Win::slotFilePrint );

  act_settings = new QAction( "Sett&ings", this );
  act_settings ->setWhatsThis( "Edit application settings" );
  connect( act_settings, &QAction::triggered, this, &QMo2Win::slotFileSettings );

  act_savesett = new QAction( "Save s&ettings", this );
  act_savesett ->setWhatsThis( "Save application settings" );
  connect( act_savesett, &QAction::triggered, this, &QMo2Win::slotFileSaveSett );


  act_quit = new QAction(  QIcon::fromTheme("application-exit"), "&Quit", this );
  act_quit->setShortcuts( QKeySequence::Quit );
  act_quit->setWhatsThis( tr("Click this button to quit application.") );
  connect( act_quit, &QAction::triggered, this, &QMo2Win::slotFileQuit );

  // ==== Edit group

  act_undo = new QAction( QIcon::fromTheme("edit-undo"), "&Undo", this );
  act_undo->setShortcuts( QKeySequence::Undo );
  act_undo->setWhatsThis( tr("Undo last action") + " EMPTY" );
  connect( act_undo, &QAction::triggered, this, &QMo2Win::slotEditUndo );

  act_cut = new QAction(  QIcon::fromTheme("edit-cut"),"Cut", this );
  act_cut->setShortcuts( QKeySequence::Cut );
  act_cut->setWhatsThis( tr("Cut selected")  + " EMPTY" );
  connect( act_cut, &QAction::triggered, this, &QMo2Win::slotEditCut );

  act_copy = new QAction( QIcon::fromTheme("edit-copy"), "Copy", this );
  act_copy->setShortcuts( QKeySequence::Copy );
  act_copy->setWhatsThis( tr("Copy selected") + " EMPTY" );
  connect( act_copy, &QAction::triggered, this, &QMo2Win::slotEditCopy );

  act_paste= new QAction( QIcon::fromTheme("edit-paste"), "Paste", this );
  act_paste->setShortcuts( QKeySequence::Paste );
  act_paste->setWhatsThis( tr("Paste selected") + " EMPTY" );
  connect( act_paste, &QAction::triggered, this, &QMo2Win::slotEditPaste );

  // ==== Element group

  act_newelm= new QAction( QIcon( ":icons/newelm.png" ), "&New element", this );
  act_newelm->setShortcut( Qt::Key_Insert );
  act_newelm->setWhatsThis( tr("Create new element") );
  connect( act_newelm, &QAction::triggered, this, &QMo2Win::slotNewElm );

  act_delelm= new QAction( QIcon( ":icons/delelm.png" ), "&Delete element", this );
  act_delelm->setShortcut( Qt::Key_Delete );
  act_delelm->setWhatsThis( tr("Delete selected element") );
  connect( act_delelm, &QAction::triggered, this, &QMo2Win::slotDelElm );

  act_editelm= new QAction( QIcon( ":icons/editelm.png" ), "&Edit element", this );
  act_editelm->setShortcut( Qt::Key_Enter );
  act_editelm->setWhatsThis( tr("Edit selected element") );
  connect( act_editelm, &QAction::triggered, this, &QMo2Win::slotEditElm);

  act_qlinkelm= new QAction( QIcon::fromTheme("insert-link"), "&Quick link element", this );
  act_qlinkelm->setShortcut( Qt::CTRL+Qt::Key_L );
  act_qlinkelm->setWhatsThis( tr("Link marked to selected element") );
  connect( act_qlinkelm, &QAction::triggered, this, &QMo2Win::slotqLinkElm);

  act_qplinkelm= new QAction( "&Parametr link element", this );
  act_qplinkelm->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_L );
  act_qplinkelm->setWhatsThis( tr("Link marked to selected element "
                                  "(parameter input)") );
  connect( act_qplinkelm, &QAction::triggered, this, &QMo2Win::slotqpLinkElm);

  act_unlinkelm= new QAction( "&Unlink element", this );
  act_unlinkelm->setWhatsThis( tr("Remove links of selected element") );
  connect( act_unlinkelm, &QAction::triggered, this, &QMo2Win::slotUnlinkElm);

  act_lockelm= new QAction( QIcon( ":icons/lockelm.png" ), "Loc&k element",  this );
  act_lockelm->setShortcut( Qt::CTRL+Qt::Key_K );
  act_lockelm->setWhatsThis( tr("Lock current element") );
  connect( act_lockelm, &QAction::triggered, this, &QMo2Win::slotLockElm);

  act_ordelm= new QAction( QIcon( ":icons/orderelm.png" ), "Change &Orger", this );
  act_ordelm->setShortcut( Qt::Key_O );
  act_ordelm->setWhatsThis( tr("Change order numper of selected element") );
  connect( act_ordelm, &QAction::triggered, this, &QMo2Win::slotOrdElm);

  act_markelm= new QAction( QIcon( ":icons/markelm.png" ), "&Mark element", this );
  act_markelm->setShortcut( Qt::Key_M );
  act_ordelm->setWhatsThis( tr("Mark selected element") );
  connect( act_markelm, &QAction::triggered, this, &QMo2Win::slotMarkElm);

  act_moveelm= new QAction( "Move element", this );
  act_moveelm->setShortcut( Qt::SHIFT+Qt::Key_M );
  act_moveelm->setWhatsThis( tr("Move marked element to selected cell") );
  connect( act_moveelm, &QAction::triggered, this, &QMo2Win::slotMoveElm);

  act_infoelm= new QAction( QIcon( ":icons/infoelm.png" ), "show &Info", this );
  act_infoelm->setShortcut( Qt::Key_I );
  act_infoelm->setWhatsThis( tr("Show information about element structure") );
  connect( act_infoelm, &QAction::triggered, this, &QMo2Win::slotInfoElm);

  act_showtreeelm = new QAction( "show element tree", this );
  act_showtreeelm->setWhatsThis( tr("Show tree-like element structure") );
  connect( act_showtreeelm, &QAction::triggered, this, &QMo2Win::slotShowTreeElm);

  act_testelm1= new QAction( "test element 1", this );
  act_testelm1->setWhatsThis( tr("Test something in element") );
  connect( act_testelm1, &QAction::triggered, this, &QMo2Win::slotTestElm1);

  act_testelm2= new QAction( "test element 2", this );
  act_testelm2->setWhatsThis( tr("Test something in element") );
  connect( act_testelm2, &QAction::triggered, this, &QMo2Win::slotTestElm2);
  // ==== out group

  act_newout = new QAction( QIcon( ":icons/newout.png" ), "&New Out", this );
  act_newout->setShortcut( Qt::Key_U );
  act_newout->setWhatsThis( tr("Create output collector") );
  connect( act_newout, &QAction::triggered, this, &QMo2Win::slotNewOut);

  act_delout = new QAction( QIcon( ":icons/delout.png" ), "&Delete out", this );
  act_delout->setShortcut( Qt::Key_X );
  act_delout->setWhatsThis( tr("Delete output collector with current level") );
  connect( act_delout, &QAction::triggered, this, &QMo2Win::slotDelOut);

  act_editout = new QAction( QIcon( ":icons/editout.png" ), "&Edit out", this );
  act_editout->setShortcut( Qt::SHIFT+Qt::Key_U );
  act_editout->setWhatsThis( tr("Edit outsput collector withcurrent level") );
  connect( act_editout, &QAction::triggered, this, &QMo2Win::slotEditOut);

  act_showoutdata = new QAction( QIcon( ":icons/showoutdata.png" ), "&Show out data", this );
  act_showoutdata->setShortcut( Qt::Key_D );
  act_showoutdata->setWhatsThis( tr("Show data collected by output.") );
  connect( act_showoutdata, &QAction::triggered, this, &QMo2Win::slotShowOutData);

  act_exportout = new QAction( "E&xport out data", this );
  act_exportout->setShortcut( Qt::Key_E );
  act_exportout->setWhatsThis( tr("Export data collected by output to text file.") );
  connect( act_exportout, &QAction::triggered, this, &QMo2Win::slotExportOut);

  // ==== graph group

  act_newgraph = new QAction( QIcon( ":icons/newgraph.png" ), "&New Graph", this );
  act_newgraph->setShortcut( Qt::Key_G );
  act_newgraph->setWhatsThis( tr("Create new graph") );
  connect( act_newgraph, &QAction::triggered, this, &QMo2Win::slotNewGraph);

  act_delgraph = new QAction( QIcon( ":icons/delgraph.png" ), "&Delete graph", this );
  act_delgraph->setShortcut( Qt::SHIFT+Qt::Key_X );
  act_delgraph->setWhatsThis( tr("Delete graph with selected level") );
  connect( act_delgraph, &QAction::triggered, this, &QMo2Win::slotDelGraph);

  act_editgraph = new QAction( QIcon( ":icons/editgraph.png" ), "&Edit graph", this );
  act_editgraph->setShortcut(  Qt::SHIFT+Qt::Key_G );
  act_editgraph->setWhatsThis( tr("Edit graph with current level") );
  connect( act_editgraph, &QAction::triggered, this, &QMo2Win::slotEditGraph);

  act_showgraph = new QAction( QIcon( ":icons/showgraph.png" ), "&Show graph", this );
  act_showgraph->setShortcut( Qt::Key_S );
  act_showgraph->setWhatsThis( tr("Show graph plot") );
  connect( act_showgraph, &QAction::triggered, this, &QMo2Win::slotShowGraph);

  act_showgraphdata = new QAction( QIcon( ":icons/showgraphdata.png" ), "show graph Data", this );
  act_showgraphdata->setShortcut(  Qt::SHIFT+Qt::Key_D );
  act_showgraphdata->setWhatsThis( tr("Show graph data") );
  connect( act_showgraphdata, &QAction::triggered, this, &QMo2Win::slotShowGraphData);

  act_exportgraphdata = new QAction( "E&xport graph data", this );
  act_exportgraphdata->setShortcut( Qt::SHIFT+Qt::Key_E );
  act_exportgraphdata->setWhatsThis( tr("Export graph data to text file") );
  connect( act_exportgraphdata, &QAction::triggered, this, &QMo2Win::slotExportGraphData);

  act_gnuplotgraph = new QAction( "&Gnuplot graph", this);
  act_gnuplotgraph->setWhatsThis( tr("Export graph data to gnuplot-compatiomle file") );
  connect( act_gnuplotgraph, &QAction::triggered, this, &QMo2Win::slotGnuplotGraph);

  // ==== model group

  act_editmodel = new QAction( QIcon( ":icons/editmodel.png" ), "&Edit Model", this );
  // act_editmodel->setShortcut(  Qt::CTRL+Qt::Key_Enter );
  act_editmodel->setShortcut(  Qt::CTRL+Qt::Key_R );
  act_editmodel->setWhatsThis( tr("Edit model parameters.") );
  connect( act_editmodel, &QAction::triggered, this, &QMo2Win::slotEditModel);

  act_showtreemodel = new QAction( "show model tree", this );
  act_showtreemodel->setShortcut( Qt::CTRL+Qt::Key_T );
  act_showtreemodel->setWhatsThis( tr("Show tree-like model structure") );
  connect( act_showtreemodel, &QAction::triggered, this, &QMo2Win::slotShowTreeModel);

  // ====  run group

  act_runrun = new QAction( QIcon( ":icons/run.png" ), "&Run", this );
  act_runrun->setShortcut( Qt::Key_F9 );
  act_runrun->setWhatsThis( tr("Click this button start simple run.") );
  connect( act_runrun, &QAction::triggered, this, &QMo2Win::slotRunRun );

  act_runprm= new QAction( QIcon( ":icons/run_p1.png" ), "Run &1D Parm loop", this );
  act_runprm->setShortcut( Qt::CTRL+Qt::Key_F9 );
  act_runprm->setWhatsThis( tr("Click this button start 1D parametric run.") );
  connect( act_runprm, &QAction::triggered, this, &QMo2Win::slotRunPrm );

  act_runprm2= new QAction( QIcon( ":icons/run_p2.png" ), "Run &2D Parm loop", this );
  act_runprm2->setShortcut( Qt::SHIFT+Qt::CTRL+Qt::Key_F9 );
  act_runprm2->setWhatsThis( tr("Click this button start 2D parametric run.") );
  connect( act_runprm2, &QAction::triggered, this, &QMo2Win::slotRunPrm2 );

  act_runscript = new QAction( QIcon::fromTheme("application-x-javascript"), "run &Script", this );
  act_runscript->setShortcut( Qt::SHIFT+Qt::Key_F9 );
  act_runscript->setWhatsThis( tr("Run script on model") );
  connect( act_runscript, &QAction::triggered, this, &QMo2Win::slotRunScript);

  act_reset= new QAction( "R&eset", this );
  act_reset->setWhatsThis( tr("Reset model state.") );
  connect( act_reset, &QAction::triggered, this, &QMo2Win::slotReset );

  act_newSimul = new QAction( "New Simulation", this );
  act_newSimul->setWhatsThis( tr("Create new simulation") );
  connect( act_newSimul, &QAction::triggered, this, &QMo2Win::slotNewSimul );

  act_delSimul = new QAction( "Delete Simulation", this );
  act_delSimul->setWhatsThis( tr("Delete simulation") );
  connect( act_delSimul, &QAction::triggered, this, &QMo2Win::slotDelSimul );

  act_editSimul = new QAction( "Edit Simulation", this );
  act_editSimul->setWhatsThis( tr("Edit simulation") );
  connect( act_editSimul, &QAction::triggered, this, &QMo2Win::slotEditSimul );

  act_setActiveSimul = new QAction( "Set Active Simulation", this );
  act_setActiveSimul->setWhatsThis( tr("Set current simulation to be active") );
  connect( act_setActiveSimul, &QAction::triggered, this, &QMo2Win::slotSetActiveSimul );

  // ==== iface group

  act_tbar = new QAction( "View &Toolbar", this );
  act_tbar->setCheckable( true );
  act_tbar->setChecked( true );
  // act_tbar->setWhatsThis( tr("") );
  connect( act_tbar, &QAction::triggered, this, &QMo2Win::slotViewToolBar );

  act_sbar = new QAction( "View &Statusbar", this );
  act_sbar->setCheckable( true );
  act_sbar->setChecked( true );
  // act_sbar->setWhatsThis( tr("") );
  connect( act_sbar, &QAction::triggered, this, &QMo2Win::slotViewStatusBar );

  act_showord = new QAction( "Show &Orders", this );
  act_showord->setCheckable( true );
  act_showord->setChecked( sett.showord );
  connect( act_showord, &QAction::triggered, this, &QMo2Win::slotShowOrd );

  act_showgrid = new QAction( "Show &Grid", this );
  act_showgrid->setCheckable( true );
  act_showgrid->setChecked( sett.showgrid );
  connect( act_showgrid, &QAction::triggered, this, &QMo2Win::slotShowGrid );

  act_shownames = new QAction( "Show &Names", this );
  act_shownames->setCheckable( true );
  act_shownames->setChecked( sett.shownames );
  connect( act_shownames, &QAction::triggered, this, &QMo2Win::slotShowNames );

  act_showicons = new QAction( "Show &Icons", this );
  act_showicons->setCheckable( true );
  act_showicons->setChecked( sett.showicons );
  connect( act_showicons, &QAction::triggered, this, &QMo2Win::slotShowIcons );

  act_showlinks = new QAction( "Show &Links", this );
  act_showlinks->setCheckable( true );
  act_showlinks->setChecked( sett.showLinks );
  connect( act_showlinks, &QAction::triggered, this, &QMo2Win::slotShowLinks );

  // ==== window group

  act_winClose = new QAction( QIcon::fromTheme("window-close"), "Cl&ose Window", this);
  act_winClose->setWhatsThis( tr("Close this window") );
  connect( act_winClose, &QAction::triggered, this, &QMo2Win::slotWindowClose );

  act_winCloseAll = new QAction( "Close &All Windows",  this );
  act_winCloseAll->setWhatsThis( tr("Close all windoww") );
  connect( act_winCloseAll, &QAction::triggered, this, &QMo2Win::slotWindowCloseAll );

  act_winTile = new QAction( "&Tile Windows",  this );
  act_winTile->setWhatsThis( tr("Tile the windows") );
  connect( act_winTile, &QAction::triggered, this, &QMo2Win::slotWindowTile );

  act_winCascade = new QAction( "&Cascade Windows",  this );
  act_winCascade->setWhatsThis( tr("Cascade the windows") );
  connect( act_winCascade, &QAction::triggered, this, &QMo2Win::slotWindowCascade );

  //act_winnew = new QAction( "&New Window", 0, this, "winnew");
  //act_winnew->setWhatsThis( tr("Crerate new window for same model") );
  //connect( act_winnew, &QAction::triggered, this, &QMo2Win::slotWindowNewWindow );

  // ==== help group

  act_helpabout = new QAction( QIcon::fromTheme("help-about"), "&About", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpabout, &QAction::triggered, this, &QMo2Win::slotHelpAbout );

  act_helpaboutqt = new QAction( "About &Qt", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpaboutqt, &QAction::triggered, this, &QMo2Win::slotHelpAboutQt );

  act_whatsthis = new QAction( "&What's This", this );
  // act_whatsthis->setWhatsThis( tr("") );
  // TODO:
  //connect( act_whatsthis, &QAction::triggered, this, &QMo2Win::slotWhatsThis );

  act_test = new QAction( QIcon( ":icons/test.png" ), "&Test", this );
  act_test->setWhatsThis( tr("Click this button to test something.") );
  connect( act_test, &QAction::triggered, this, &QMo2Win::slotTest );


  ///////////////////////////////////////////////////////////////////
  // menuBar entry pFileMenu

  pFileMenu = menuBar()->addMenu( tr("&File") );
  pFileMenu->addAction( act_new );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_openxml );
  pFileMenu->addAction( act_savexml );
  pFileMenu->addAction( act_savexmlas );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_print );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_close );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_settings );
  pFileMenu->addAction( act_savesett );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_quit );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pEditMenu
  pEditMenu = menuBar()->addMenu( tr("&Edit") );
  pEditMenu->addAction( act_undo );
  pEditMenu->addSeparator();
  pEditMenu->addAction( act_cut );
  pEditMenu->addAction( act_copy );
  pEditMenu->addAction( act_paste );
  pEditMenu->addSeparator();
  pEditMenu->addAction( act_test );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pElmMenu
  pElmMenu = menuBar()->addMenu( tr("E&lement") );
  pElmMenu->addAction( act_newelm );
  pElmMenu->addAction( act_delelm );
  pElmMenu->addAction( act_editelm );
  pElmMenu->addSeparator();
  pElmMenu->addAction( act_qlinkelm );
  pElmMenu->addAction( act_qplinkelm );
  pElmMenu->addAction( act_unlinkelm );
  pElmMenu->addSeparator();
  pElmMenu->addAction( act_lockelm );
  pElmMenu->addAction( act_ordelm );
  pElmMenu->addAction( act_markelm );
  pElmMenu->addAction( act_moveelm );
  pElmMenu->addSeparator();
  pElmMenu->addAction( act_infoelm );
  pElmMenu->addAction( act_showtreeelm );
  pElmMenu->addSeparator();
  pElmMenu->addAction( act_testelm1 );
  pElmMenu->addAction( act_testelm2 );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pOutMenu
  pOutMenu = menuBar()->addMenu( tr("&Out") );
  pOutMenu->addAction( act_newout );
  pOutMenu->addAction( act_delout );
  pOutMenu->addAction( act_editout );
  pOutMenu->addSeparator();
  pOutMenu->addAction( act_showoutdata );
  pOutMenu->addAction( act_exportout );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pGraphMenu
  pGraphMenu = menuBar()->addMenu( tr("&Graph") );
  pGraphMenu->addAction( act_newgraph );
  pGraphMenu->addAction( act_delgraph );
  pGraphMenu->addAction( act_editgraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_showgraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_showgraphdata );
  pGraphMenu->addAction( act_exportgraphdata );
  pGraphMenu->addAction( act_gnuplotgraph );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pModelMenu
  pModelMenu = menuBar()->addMenu( tr("&Model") );
  pModelMenu->addAction( act_editmodel );
  pModelMenu->addAction( act_showtreemodel );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pRunMenu
  pRunMenu = menuBar()->addMenu( tr("&Run") );
  pRunMenu->addAction( act_runrun );
  pRunMenu->addAction( act_runprm );
  pRunMenu->addAction( act_runprm2 );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_runscript );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_reset );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_newSimul );
  pRunMenu->addAction( act_delSimul );
  pRunMenu->addAction( act_editSimul );
  pRunMenu->addAction( act_setActiveSimul );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pViewMenu
  pViewMenu = menuBar()->addMenu( tr("&View") );
  // pViewMenu->setCheckable(true); TODO: what?
  pViewMenu->addAction( act_tbar );
  pViewMenu->addAction( act_sbar );
  pViewMenu->addSeparator();
  pViewMenu->addAction( act_showord );
  pViewMenu->addAction( act_showgrid );
  pViewMenu->addAction( act_shownames );
  pViewMenu->addAction( act_showicons );
  pViewMenu->addAction( act_showlinks );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry window-Menu
  pWindowMenu = menuBar()->addMenu( tr("&Window") );
  // all to separate function windowMenuAboutToShow

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pHelpMenu
  pHelpMenu = menuBar()->addMenu( tr("&Help") );
  pHelpMenu->addAction( act_helpabout );
  pHelpMenu->addAction( act_helpaboutqt );
  pHelpMenu->addSeparator();
  pHelpMenu->addAction( act_whatsthis );

  // Connect the submenu slots with signals

  connect( pWindowMenu, &QMenu::aboutToShow, this, &QMo2Win::windowMenuAboutToShow );

  ///////////////////////////////////////////////////////////////////
  // TOOLBARS

  fileToolbar = addToolBar( tr("File") );
  fileToolbar->addAction( act_new );
  fileToolbar->addAction( act_openxml );
  fileToolbar->addAction( act_savexml );
    fileToolbar->addSeparator();
  fileToolbar->addAction( act_test );
    fileToolbar->addSeparator();

  elmToolbar = addToolBar( tr("Element") );
  elmToolbar->addAction( act_newelm );
  elmToolbar->addAction( act_delelm );
  elmToolbar->addAction( act_editelm );
  // elmToolbar->addAction( act_lockelm );
  elmToolbar->addAction( act_ordelm );
  elmToolbar->addAction( act_markelm );
  elmToolbar->addAction( act_infoelm );
    elmToolbar->addSeparator();
  elmToolbar->addAction( act_newout );
  elmToolbar->addAction( act_delout );
  elmToolbar->addAction( act_editout );
  elmToolbar->addAction( act_showoutdata );
    elmToolbar->addSeparator();
  elmToolbar->addAction( act_newgraph );
  elmToolbar->addAction( act_delgraph );
  elmToolbar->addAction( act_editgraph );
  elmToolbar->addAction( act_showgraph );
  elmToolbar->addAction( act_showgraphdata );
    elmToolbar->addSeparator();
  elmToolbar->addAction( act_editmodel );
    elmToolbar->addSeparator();
  elmToolbar->addAction( act_runrun );
  elmToolbar->addAction( act_runprm );
  elmToolbar->addAction( act_runprm2 );
  elmToolbar->addAction( act_runscript );

}

void QMo2Win::initStatusBar()
{
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::enableActions( bool ena, int id_ )
{
  switch( id_ ) {
    case 0: // model-specific actions
       act_close->setEnabled( ena );
       act_savexml->setEnabled( ena );
       act_savexmlas->setEnabled( ena );
       act_print->setEnabled( ena );
       // TODO: implement this
       act_undo->setEnabled( false );
       act_cut->setEnabled( ena );
       act_copy->setEnabled( ena );
       act_paste->setEnabled( ena );
       // elm
       act_newelm->setEnabled( ena );
       act_delelm->setEnabled( ena );
       act_editelm->setEnabled( ena );
       act_qlinkelm->setEnabled( ena );
       act_qplinkelm->setEnabled( ena );
       act_unlinkelm->setEnabled( ena );
       act_lockelm->setEnabled( ena );
       act_ordelm->setEnabled( ena );
       act_markelm->setEnabled( ena );
       act_moveelm->setEnabled( ena );
       act_infoelm->setEnabled( ena );
       act_showtreeelm->setEnabled( ena );
       act_testelm1->setEnabled( ena );
       act_testelm2->setEnabled( ena );
       // out
       act_newout->setEnabled( ena );
       act_delout->setEnabled( ena );
       act_editout->setEnabled( ena );
       act_showoutdata->setEnabled( ena );
       act_exportout->setEnabled( ena );
       // graph
       act_newgraph->setEnabled( ena );
       act_delgraph->setEnabled( ena );
       act_editgraph->setEnabled( ena );
       act_showgraph->setEnabled( ena );
       act_showgraphdata->setEnabled( ena );
       act_exportgraphdata->setEnabled( ena );
       act_gnuplotgraph->setEnabled( ena );
       // model
       act_editmodel->setEnabled( ena );
       act_showtreemodel->setEnabled( ena );
       // run
       act_runrun->setEnabled( ena );
       act_runprm->setEnabled( ena );
       act_runprm2->setEnabled( ena );
       act_runscript->setEnabled( ena );
       act_reset->setEnabled( ena );
       act_newSimul->setEnabled( ena );
       act_delSimul->setEnabled( ena );
       act_editSimul->setEnabled( ena );
       act_setActiveSimul->setEnabled( ena );
       // win
       act_winClose->setEnabled( ena );
       act_winCloseAll->setEnabled( ena );
       act_winTile->setEnabled( ena );
       act_winCascade->setEnabled( ena );
       break;
    default: break;
  };
}

void QMo2Win::updateActions()
{
  if( mdiArea->currentSubWindow() != 0 ) { // TODO: different windows
    enableActions( true, 0 );
  } else {
    enableActions( false, 0 );
  };
  setWndTitle( this ); // "this" is fake

}

void QMo2Win::setWndTitle( QWidget* )
{
  if( mdiArea->currentSubWindow() != 0 ) {
    setWindowTitle( mdiArea->currentSubWindow()->windowTitle() + " " PACKAGE );
  } else {
    setWindowTitle( PACKAGE " " VERSION );
  };
}

void QMo2Win::closeEvent ( QCloseEvent *e )
{
  mdiArea->closeAllSubWindows();
  if (mdiArea->currentSubWindow()) {
    e->ignore();
  } else {
    // writeSettings();
    e->accept();
  }
}

QMo2View* QMo2Win::createChild( QMo2Doc* doc )
{
  QMo2View* w = new QMo2View( doc, this );
  mdiArea->addSubWindow( w );
  return w;
}


bool QMo2Win::queryExit()
{
  int exit = QMessageBox::information( this,
      tr("Quit..."), tr("Do your really want to quit?"),
      QMessageBox::Ok, QMessageBox::Cancel);

  return ( exit==1 );
}

QMdiSubWindow* QMo2Win::findMdiChild( const QString &fileName )
{
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  foreach( QMdiSubWindow *window, mdiArea->subWindowList() ) {
    QMo2View *mdiChild = qobject_cast<QMo2View *>(window->widget());
    if( mdiChild->currentFile() == canonicalFilePath ) {
      return window;
    }
  }
  return 0;
}

QMo2View* QMo2Win::activeMdiChild()
{
  if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    return qobject_cast<QMo2View *>( activeSubWindow->widget() );
  return 0;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void QMo2Win::slotFileNew()
{
  statusBar()->showMessage( tr( "Creating new model file..." ) );

  QMo2Doc* doc = new QMo2Doc();
  ++untitledCount;
  QString fileName = QString( "untitled_%1.mo2" ).arg(untitledCount);
  doc->newDocument();
  doc->setPathName(fileName);
  doc->setTitle(fileName);

  QMo2View *cw = createChild( doc );
  updateActions();
  cw->show();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotFileOpenXML()
{
  statusBar()->showMessage( tr( "Opening model file..." ) );
  QString fileName =
    QFileDialog::getOpenFileName( this, tr("Open model file"),
        "", "Model *.qol files (*.qol);;All files(*)" );
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

  bool rc = doFileOpenXML( fileName );

  statusBar()->showMessage( rc ? tr( "Ready." ) : tr( "Fail" ) );
}


bool QMo2Win::doFileOpenXML( const QString &fn )
{
  QMo2Doc* doc = new QMo2Doc();
  if( ! doc->openDocumentXML( fn ) ) {
    QMessageBox::critical( this, tr("Error !"),
                           tr("Could not read file !" ) + fn );
    delete doc;
    updateActions();
    statusBar()->showMessage( tr( "Open Failed." ) );
    return false;
  };

  QMo2View *cw = createChild( doc );
  cw->show();
  updateActions();
  return true;
}


void QMo2Win::slotFileSaveXML()
{
  statusBar()->showMessage( tr( "Saving model file...") );
  QMo2View* m =  activeMdiChild();
  if( m ) {
    QMo2Doc* doc = m->getDocument();
    if( doc->nonamed() ) {
      slotFileSaveXMLAs();
    } else {
      if( ! doc->saveDocumentXML( doc->pathName() ) )
         QMessageBox::critical( this,
           tr("I/O Error !"), tr("Could not save the current document !" ) );
    };
    m->update();
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotFileSaveXMLAs()
{
  statusBar()->showMessage( tr ( "Saving model file under new filename..." ) );
  QMo2View* m =  activeMdiChild();
  if( !m ) {
    QMessageBox::critical ( this,
      tr("Critical error!"), tr("Fail to find active window while saving file!") );
  }
  QString fn = QFileDialog::getSaveFileName( this, tr("Save model"),
      QString::null, "Model *.qol files (*.qol);;All files(*)" );

  if ( !fn.isEmpty() ) {
    QFileInfo fi( fn );
    QString pfn = fi.fileName();
    if( ! pfn.contains('.') )
      fn += ".qol";
    fi.setFile( fn );

    QMo2Doc* doc = m->getDocument();
    if( ! doc->saveDocumentXML( fn ) ) {
       QMessageBox::critical ( this,
           tr("I/O Error !"), tr("Could not save the current model file!") );
       return;
    };
    m->setWindowTitle( doc->title() );
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotFileClose()
{
  slotWindowClose(); // tmp for now: 1 for now
}

void QMo2Win::slotFilePrint()
{
  statusBar()->showMessage( tr ( "Printing..." ) );
  QMo2View* m =  activeMdiChild();
  if ( m != 0 )
    m->print();
  updateActions();
  statusBar()->showMessage( tr ( "Ready." ) );
}

void QMo2Win::slotFileSettings()
{
  statusBar()->showMessage( tr ( "Edit settings..." ) );

  Mo2SettDlg *dia = new Mo2SettDlg( sett, this );
  if ( dia->exec() == QDialog::Accepted ) {
    setFonts();
  };

  statusBar()->showMessage( tr ( "Ready." ) );
}


void QMo2Win::slotFileSaveSett()
{
  statusBar()->showMessage( tr ( "Saving settings..." ) );
  sett.save();
  statusBar()->showMessage( tr ( "Ready." ) );
}

void QMo2Win::slotFileQuit()
{
  qApp->closeAllWindows();
}

void QMo2Win::slotEditUndo()
{
  statusBar()->showMessage( tr( "Reverting last action..." ) );
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->undo();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditCut()
{
  statusBar()->showMessage( tr( "Cutting selection..." ) );
  QMo2View* m = activeMdiChild();
  if ( m )
    m->cutElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditCopy()
{
  statusBar()->showMessage( tr( "Copying selection to clipboard..." ) );
  QMo2View* m = activeMdiChild();
  if ( m )
    m->copyElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditPaste()
{
  statusBar()->showMessage( tr( "Inserting clipboard contents..." ) );
  QMo2View* m = activeMdiChild();
  if ( m )
    m->pasteElm();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotViewToolBar()
{
  statusBar()->showMessage( tr( "Toggle toolbar..." ) );
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if (fileToolbar->isVisible()) {
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

void QMo2Win::slotViewStatusBar()
{
  statusBar()->showMessage( tr( "Toggle statusbar..." ) );
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if (statusBar()->isVisible()) {
    statusBar()->hide();
    act_sbar->setChecked( false );
  } else {
    statusBar()->show();
    act_sbar->setChecked( true );
  };

  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotShowOrd()
{
  sett.showord = ! sett.showord;
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->update();
}

void QMo2Win::slotShowGrid()
{
  sett.showgrid = ! sett.showgrid;
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->update();
}

void QMo2Win::slotShowNames()
{
  sett.shownames = ! sett.shownames;
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->update();
}

void QMo2Win::slotShowIcons()
{
  sett.showicons = ! sett.showicons;
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->update();
}

void QMo2Win::slotShowLinks()
{
  sett.showLinks = ! sett.showLinks;
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->update();
}


void QMo2Win::slotWindowClose()
{
  mdiArea->closeActiveSubWindow();
  updateActions();
}


void QMo2Win::slotWindowCloseAll()
{
  mdiArea->closeAllSubWindows();
  updateActions();
}

void QMo2Win::slotWindowTile()
{
  mdiArea->tileSubWindows();
  updateActions();
}

void QMo2Win::slotWindowCascade()
{
  mdiArea->cascadeSubWindows();
  updateActions();
}


void QMo2Win::slotHelpAbout()
{
  QString ostr = QString( PACKAGE "\nVersion " VERSION " build: "
      __DATE__ " " __TIME__ "\n"  COPYRIGHT );
  QMessageBox::about( this, tr( "About..." ), ostr );
}

void QMo2Win::slotHelpAboutQt()
{
  QMessageBox::aboutQt( this );
}


void QMo2Win::slotStatusHelpMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->showMessage( text, 2000 );
}

void QMo2Win::slotTest(void)
{
  QString ostr( "Test called\n" );
  statusBar()->showMessage( tr( "Test something..." ) );

  for( unsigned ccode=0x03A0; ccode < 0x0400; ++ccode )  {
    ostr += QChar( ccode );
    if( (ccode & 0x1F) == 0x1F )
      ostr += " <:> " + QSN(ccode,16) + "\n";
  }
  ostr += "\n";

  //ostr += "\nDMIN: " + QSN( DMIN );
  //ostr += " DMAX: "  + QSN( DMAX );
  //ostr += "\niDMIN: " + QSN( (int)(DMIN) );
  //ostr += " iDMAX: "  + QSN( (int)(DMAX) );
  QString first, rest;
  int idx, nmt;
  const char* const tst_str[] =  {
    "[5]",
    "abcd",
    "abcd[5]",
    "abcd[5].xxx[6]",
    "abcd.xxx[6]",
    "abcd[5].xxx",
    "aaa6.bbb.ccc.ddd.eee",
    "aaa6.bbb.ccc.ddd.eee[5]"
  };
  for( auto s : tst_str ) {
    QString t = s;
    nmt = splitName( t, first, rest, idx );
    ostr += "t=\"" + t + "\" nmt=" + QSN(nmt) + " first=\"" + first
         + "\" rest=\"" + rest + "\" idx=" + QSN(idx) + "\n";
  }


  QMessageBox::information( this, tr( "Test" ), ostr, QMessageBox::Ok );
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::windowMenuAboutToShow()
{
  pWindowMenu->clear();
  pWindowMenu->addAction( act_winClose );
  pWindowMenu->addAction( act_winCloseAll );
  pWindowMenu->addSeparator();
  pWindowMenu->addAction( act_winTile );
  pWindowMenu->addAction( act_winCascade );
  pWindowMenu->addSeparator();

  // add windows to menu
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  for( int i = 0; i < windows.size(); ++i ) {
    QMo2View *child = qobject_cast<QMo2View *>(windows.at(i)->widget());

    QString text;
    if (i < 9) {
      text = tr("&%1 %2").arg(i + 1).arg(child->currentFile());
    } else {
      text = tr("%1 %2").arg(i + 1).arg(child->currentFile());
    }
    QAction *action  = pWindowMenu->addAction( text );
    action->setCheckable(true);
    action->setChecked( child == activeMdiChild() );
    // cast to fight unbiguity:
    connect( action, &QAction::triggered, windowMapper,
        static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map) );
    windowMapper->setMapping( action, windows.at(i) );
  }

  updateActions();

}



void QMo2Win::slotNewElm()
{
  statusBar()->showMessage( tr( "Inserting new element..." ) );
  QMo2View* m = activeMdiChild();
  if ( m )
    m->newElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotDelElm()
{
  statusBar()->showMessage( tr( "Deleting selected element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->delElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditElm()
{
  statusBar()->showMessage( tr( "Editing element properties..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->editElm();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotqLinkElm()
{
  statusBar()->showMessage( tr( "Quick linking element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->qlinkElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotqpLinkElm()
{
  statusBar()->showMessage( tr( "Quick parametr linking element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->qplinkElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotUnlinkElm()
{
  statusBar()->showMessage( tr( "Uninking element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->unlinkElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotLockElm()
{
  statusBar()->showMessage( tr( "Locking element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->lockElm();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotOrdElm()
{
  statusBar()->showMessage( tr( "Setting new order of element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->ordElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotMarkElm()
{
  statusBar()->showMessage( tr( "Marking element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->markElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotMoveElm()
{
  statusBar()->showMessage( tr( "Moving element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->moveElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotInfoElm()
{
  statusBar()->showMessage( tr( "Information about element..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->infoElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotShowTreeElm()
{
  statusBar()->showMessage( tr( "Show element tree..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showTreeElm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotTestElm1()
{
  statusBar()->showMessage( tr( "Test element 1..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->testElm1();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotTestElm2()
{
  statusBar()->showMessage( tr( "Test element 2..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->testElm2();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotNewOut()
{
  statusBar()->showMessage( tr( "Inserting output array..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->newOut();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotDelOut()
{
  statusBar()->showMessage( tr( "Deleting output array..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->delOut();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditOut()
{
  statusBar()->showMessage( tr( "Editing output array..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->editOut();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotShowOutData()
{
  statusBar()->showMessage( tr( "Data from output array..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showOutData();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotExportOut()
{
  statusBar()->showMessage( tr( "Exporting data from output array..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->exportOut();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotNewGraph()
{
  statusBar()->showMessage( tr( "Inserting new graph..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->newGraph();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotDelGraph()
{
  statusBar()->showMessage( tr( "Deleting graph..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->delGraph();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditGraph()
{
  statusBar()->showMessage( tr( "Editing graph..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->editGraph();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotShowGraph()
{
  statusBar()->showMessage( tr( "Showing graph plot..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showGraph();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotShowGraphData()
{
  statusBar()->showMessage( tr( "Showing graph data..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showGraphData();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotExportGraphData()
{
  statusBar()->showMessage( tr( "Exporting graph data..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->exportGraphData();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotGnuplotGraph()
{
  statusBar()->showMessage( tr( "Exporting graph to gnuplot..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->gnuplotGraph();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditModel()
{
  statusBar()->showMessage( tr( "Editing model parametrs..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->editModel();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotShowTreeModel()
{
  statusBar()->showMessage( tr( "Show model tree..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showTreeModel();
  statusBar()->showMessage( tr( "Ready." ) );
}


void QMo2Win::slotRunRun()
{
  statusBar()->showMessage( tr( "Running simple loop..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->runRun();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotRunPrm()
{
  statusBar()->showMessage( tr( "Running parametric loop..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->runPrm();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotRunPrm2()
{
  statusBar()->showMessage( tr( "Running 2D parametric loop..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->runPrm2();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotReset()
{
  statusBar()->showMessage( tr( "Reseting model..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->resetModel();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotNewSimul()
{
  statusBar()->showMessage( tr( "Adding new simulation ..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->newSimul();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotDelSimul()
{
  statusBar()->showMessage( tr( "Removing selected simulation ..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->delSimul();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditSimul()
{
  statusBar()->showMessage( tr( "Editing selected simulation ..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->editSimul();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotSetActiveSimul()
{
  statusBar()->showMessage( tr( "Setting active simulation ..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->setActiveSimul();
  statusBar()->showMessage( tr( "Ready." ) );
}



void QMo2Win::slotRunScript()
{
  statusBar()->showMessage( tr( "Running script..." ) );

  QMo2View* m =  activeMdiChild();
  if ( m )
    m->runScript();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::setActiveSubWindow( QWidget *win )
{
  if( !win )
    return;
  mdiArea->setActiveSubWindow( qobject_cast<QMdiSubWindow *>(win) );
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
  smallFont   = sets.value("fonts/small", "Arial,7" ).toString();
  plotFont    = sets.value("fonts/plot", "Arial,8" ).toString();
  structFont  = sets.value("fonts/struct", "Arial,8" ).toString();
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

  sets.endGroup();
  sets.sync();

}


// end of qmo2win.cpp

