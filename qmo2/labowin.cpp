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

#include "miscfun.h"
#include "laboview.h"
#include "labodoc.h"
#include "labowin.h"
#include "mo2settdlg.h"

using namespace std;

LaboWin* LaboWin::labowin = nullptr;

LaboWin::LaboWin()
{
  setWindowTitle( PACKAGE " " VERSION );

  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  setCentralWidget( mdiArea );

  connect( mdiArea, &QMdiArea::subWindowActivated, this, &LaboWin::updateActions );
  windowMapper = new QSignalMapper(this);
  connect( windowMapper,
      static_cast<void (QSignalMapper::*)(QWidget*)>(&QSignalMapper::mapped),
      this, &LaboWin::setActiveSubWindow );

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
  labowin = this;
  updateActions();
}

LaboWin::~LaboWin()
{
  delete printer;
  printer = nullptr;
  labowin = nullptr;
}

void LaboWin::initDirs() // TODO: remove or rewrite
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


void LaboWin::initIface()
{
  // Actions
  // File group

  act_new = new QAction( QIcon( ":icons/filenew.png" ), "&New model", this );
  act_new->setShortcuts( QKeySequence::New );
  act_new->setWhatsThis( tr("Click this button to create a new model file.") );
  connect( act_new, &QAction::triggered, this, &LaboWin::slotFileNew );

  act_openxml = new QAction( QIcon::fromTheme("document-open"), "Open model", this );
  act_openxml->setShortcuts( QKeySequence::Open );
  act_openxml->setWhatsThis( tr("Click this button to open a model file." ) );
  connect( act_openxml, &QAction::triggered, this, &LaboWin::slotFileOpenXML );

  act_close = new QAction( "&Close", this );
  act_close->setWhatsThis( tr("Close active window") );
  connect( act_close, &QAction::triggered, this, &LaboWin::slotFileClose );
  model_acts << act_close;

  act_savexml = new QAction( QIcon::fromTheme("document-save"), "Save model",  this );
  act_savexml->setShortcuts( QKeySequence::Save );
  act_savexml->setWhatsThis( tr("Click this button to save the model file you are "
                  "editing. You will be prompted for a file name." ) );
  connect( act_savexml, &QAction::triggered, this, &LaboWin::slotFileSaveXML );
  model_acts << act_savexml;

  act_savexmlas = new QAction(QIcon::fromTheme("document-save-as"), "Save model As", this );
  act_savexmlas->setWhatsThis( tr("Save current model with another filename") );
  connect( act_savexmlas, &QAction::triggered, this, &LaboWin::slotFileSaveXMLAs );
  model_acts << act_savexmlas;

  act_print = new QAction(  QIcon::fromTheme("document-print"), "&Print", this );
  act_print->setShortcuts( QKeySequence::Print );
  act_print->setWhatsThis( tr("Print current model") );
  connect( act_print, &QAction::triggered, this, &LaboWin::slotFilePrint );
  model_acts << act_print;

  act_settings = new QAction( "Sett&ings", this );
  act_settings ->setWhatsThis( "Edit application settings" );
  connect( act_settings, &QAction::triggered, this, &LaboWin::slotFileSettings );

  act_savesett = new QAction( "Save s&ettings", this );
  act_savesett ->setWhatsThis( "Save application settings" );
  connect( act_savesett, &QAction::triggered, this, &LaboWin::slotFileSaveSett );


  act_quit = new QAction(  QIcon::fromTheme("application-exit"), "&Quit", this );
  act_quit->setShortcuts( QKeySequence::Quit );
  act_quit->setWhatsThis( tr("Click this button to quit application.") );
  connect( act_quit, &QAction::triggered, this, &LaboWin::slotFileQuit );

  // ==== Edit group

  act_undo = new QAction( QIcon::fromTheme("edit-undo"), "&Undo", this );
  act_undo->setShortcuts( QKeySequence::Undo );
  act_undo->setWhatsThis( tr("Undo last action") + " EMPTY" );
  connect( act_undo, &QAction::triggered, this, &LaboWin::slotEditUndo );
  // model_acts << act_undo;

  act_cut = new QAction(  QIcon::fromTheme("edit-cut"),"Cut", this );
  act_cut->setShortcuts( QKeySequence::Cut );
  act_cut->setWhatsThis( tr("Cut selected")  + " EMPTY" );
  connect( act_cut, &QAction::triggered, this, &LaboWin::slotEditCut );
  model_acts << act_cut;

  act_copy = new QAction( QIcon::fromTheme("edit-copy"), "Copy", this );
  act_copy->setShortcuts( QKeySequence::Copy );
  act_copy->setWhatsThis( tr("Copy selected") + " EMPTY" );
  connect( act_copy, &QAction::triggered, this, &LaboWin::slotEditCopy );
  model_acts << act_copy;

  act_paste= new QAction( QIcon::fromTheme("edit-paste"), "Paste", this );
  act_paste->setShortcuts( QKeySequence::Paste );
  act_paste->setWhatsThis( tr("Paste selected") + " EMPTY" );
  connect( act_paste, &QAction::triggered, this, &LaboWin::slotEditPaste );
  model_acts << act_paste;

  // ==== Element group

  act_newelm= new QAction( QIcon( ":icons/newelm.png" ), "&New element", this );
  act_newelm->setShortcut( Qt::Key_Insert );
  act_newelm->setWhatsThis( tr("Create new element") );
  connect( act_newelm, &QAction::triggered, this, &LaboWin::slotNewElm );
  model_acts << act_newelm;

  act_delelm= new QAction( QIcon( ":icons/delelm.png" ), "&Delete element", this );
  act_delelm->setShortcut( Qt::Key_Delete );
  act_delelm->setWhatsThis( tr("Delete selected element") );
  connect( act_delelm, &QAction::triggered, this, &LaboWin::slotDelElm );
  model_acts << act_delelm;

  act_editelm= new QAction( QIcon( ":icons/editelm.png" ), "&Edit element", this );
  act_editelm->setShortcut( Qt::Key_Enter );
  act_editelm->setWhatsThis( tr("Edit selected element") );
  connect( act_editelm, &QAction::triggered, this, &LaboWin::slotEditElm );
  model_acts << act_editelm;

  act_renameelm= new QAction( "Rename element", this );
  act_renameelm->setWhatsThis( tr("Rename selected element") );
  connect( act_renameelm, &QAction::triggered, this, &LaboWin::slotRenameElm );
  model_acts << act_renameelm;

  act_qlinkelm= new QAction( QIcon::fromTheme("insert-link"), "&Quick link element", this );
  act_qlinkelm->setShortcut( Qt::CTRL+Qt::Key_L );
  act_qlinkelm->setWhatsThis( tr("Link marked to selected element") );
  connect( act_qlinkelm, &QAction::triggered, this, &LaboWin::slotqLinkElm);
  model_acts << act_qlinkelm;

  act_qplinkelm= new QAction( "&Parametr link element", this );
  act_qplinkelm->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_L );
  act_qplinkelm->setWhatsThis( tr("Link marked to selected element "
                                  "(parameter input)") );
  connect( act_qplinkelm, &QAction::triggered, this, &LaboWin::slotqpLinkElm);
  model_acts << act_qplinkelm;

  act_unlinkelm= new QAction( "&Unlink element", this );
  act_unlinkelm->setWhatsThis( tr("Remove links of selected element") );
  connect( act_unlinkelm, &QAction::triggered, this, &LaboWin::slotUnlinkElm);
  model_acts << act_unlinkelm;

  act_lockelm= new QAction( QIcon( ":icons/lockelm.png" ), "Loc&k element",  this );
  act_lockelm->setShortcut( Qt::CTRL+Qt::Key_K );
  act_lockelm->setWhatsThis( tr("Lock current element") );
  connect( act_lockelm, &QAction::triggered, this, &LaboWin::slotLockElm);
  model_acts << act_lockelm;

  act_ordelm= new QAction( QIcon( ":icons/orderelm.png" ), "Change &Orger", this );
  act_ordelm->setShortcut( Qt::Key_O );
  act_ordelm->setWhatsThis( tr("Change order numper of selected element") );
  connect( act_ordelm, &QAction::triggered, this, &LaboWin::slotOrdElm);
  model_acts << act_ordelm;

  act_markelm= new QAction( QIcon( ":icons/markelm.png" ), "&Mark element", this );
  act_markelm->setShortcut( Qt::Key_M );
  act_ordelm->setWhatsThis( tr("Mark selected element") );
  connect( act_markelm, &QAction::triggered, this, &LaboWin::slotMarkElm);
  model_acts << act_markelm;

  act_moveelm= new QAction( "Move element", this );
  act_moveelm->setShortcut( Qt::SHIFT+Qt::Key_M );
  act_moveelm->setWhatsThis( tr("Move marked element to selected cell") );
  connect( act_moveelm, &QAction::triggered, this, &LaboWin::slotMoveElm);
  model_acts << act_moveelm;

  act_infoelm= new QAction( QIcon( ":icons/infoelm.png" ), "show &Info", this );
  act_infoelm->setShortcut( Qt::Key_I );
  act_infoelm->setWhatsThis( tr("Show information about element structure") );
  connect( act_infoelm, &QAction::triggered, this, &LaboWin::slotInfoElm);
  model_acts << act_infoelm;

  act_showtreeelm = new QAction( "show element tree", this );
  act_showtreeelm->setWhatsThis( tr("Show tree-like element structure") );
  connect( act_showtreeelm, &QAction::triggered, this, &LaboWin::slotShowTreeElm);
  model_acts << act_showtreeelm;

  act_testelm1= new QAction( "test element 1", this );
  act_testelm1->setWhatsThis( tr("Test something in element") );
  connect( act_testelm1, &QAction::triggered, this, &LaboWin::slotTestElm1);
  model_acts << act_testelm1;

  act_testelm2= new QAction( "test element 2", this );
  act_testelm2->setWhatsThis( tr("Test something in element") );
  connect( act_testelm2, &QAction::triggered, this, &LaboWin::slotTestElm2);
  model_acts << act_testelm2;

  // ==== out group

  act_newout = new QAction( QIcon( ":icons/newout.png" ), "&New Out", this );
  act_newout->setShortcut( Qt::Key_U );
  act_newout->setWhatsThis( tr("Create output collector") );
  connect( act_newout, &QAction::triggered, this, &LaboWin::slotNewOut);
  model_acts << act_newout;

  act_delout = new QAction( QIcon( ":icons/delout.png" ), "&Delete out", this );
  // act_delout->setShortcut( Qt::Key_X );
  act_delout->setWhatsThis( tr("Delete output collector with current level") );
  connect( act_delout, &QAction::triggered, this, &LaboWin::slotDelOut);
  model_acts << act_delout;

  act_editout = new QAction( QIcon( ":icons/editout.png" ), "&Edit out", this );
  act_editout->setShortcut( Qt::SHIFT+Qt::Key_U );
  act_editout->setWhatsThis( tr("Edit outsput collector with current level") );
  connect( act_editout, &QAction::triggered, this, &LaboWin::slotEditOut);
  model_acts << act_editout;

  act_renameout= new QAction( "Rename output collector", this );
  act_renameout->setWhatsThis( tr("Rename output collector") );
  connect( act_renameout, &QAction::triggered, this, &LaboWin::slotRenameOut );
  model_acts << act_renameout;

  act_selectout = new QAction( "Select out", this );
  act_selectout->setShortcut( Qt::ALT+Qt::Key_U );
  act_selectout->setWhatsThis( tr("Select outsput collector by current level") );
  connect( act_selectout, &QAction::triggered, this, &LaboWin::slotSelectOut );
  model_acts << act_selectout;

  act_showoutdata = new QAction( QIcon( ":icons/showoutdata.png" ), "&Show out data", this );
  // act_showoutdata->setShortcut( Qt::Key_D );
  act_showoutdata->setWhatsThis( tr("Show data collected by output.") );
  connect( act_showoutdata, &QAction::triggered, this, &LaboWin::slotShowOutData);
  model_acts << act_showoutdata;

  act_exportout = new QAction( "E&xport out data", this );
  // act_exportout->setShortcut( Qt::Key_E );
  act_exportout->setWhatsThis( tr("Export data collected by output to text file.") );
  connect( act_exportout, &QAction::triggered, this, &LaboWin::slotExportOut);
  model_acts << act_exportout;

  // ==== graph=plot group

  act_newgraph = new QAction( QIcon( ":icons/newgraph.png" ), "&New Plot", this );
  act_newgraph->setShortcut( Qt::Key_P );
  act_newgraph->setWhatsThis( tr("Create new plot") );
  connect( act_newgraph, &QAction::triggered, this, &LaboWin::slotNewGraph);
  model_acts << act_newgraph;

  act_delgraph = new QAction( QIcon( ":icons/delgraph.png" ), "&Delete plot", this );
  // act_delgraph->setShortcut( Qt::SHIFT+Qt::Key_X );
  act_delgraph->setWhatsThis( tr("Delete plot with selected level") );
  connect( act_delgraph, &QAction::triggered, this, &LaboWin::slotDelGraph);
  model_acts << act_delgraph;

  act_editgraph = new QAction( QIcon( ":icons/editgraph.png" ), "&Edit plot", this );
  act_editgraph->setShortcut(  Qt::SHIFT+Qt::Key_P );
  act_editgraph->setWhatsThis( tr("Edit plot with current level") );
  connect( act_editgraph, &QAction::triggered, this, &LaboWin::slotEditGraph);
  model_acts << act_editgraph;

  act_renamegraph= new QAction( "Rename plot", this );
  act_renamegraph->setWhatsThis( tr("Rename plot") );
  connect( act_renamegraph, &QAction::triggered, this, &LaboWin::slotRenameGraph );
  model_acts << act_renamegraph;

  act_selectgraph = new QAction( "select plot", this );
  act_selectgraph->setShortcut(  Qt::ALT+Qt::Key_G ); // Alt-P is busy by menu
  act_selectgraph->setWhatsThis( tr("Select plot by current level") );
  connect( act_selectgraph, &QAction::triggered, this, &LaboWin::slotSelectGraph );
  model_acts << act_selectgraph;

  act_showgraph = new QAction( QIcon( ":icons/showgraph.png" ), "&Show plot", this );
  act_showgraph->setShortcut( Qt::Key_S );
  act_showgraph->setWhatsThis( tr("Show plot") );
  connect( act_showgraph, &QAction::triggered, this, &LaboWin::slotShowGraph);
  model_acts << act_showgraph;

  act_graphaddout = new QAction( "Add out to plot", this );
  act_graphaddout->setShortcut( Qt::CTRL | Qt::Key_U );
  act_graphaddout->setWhatsThis( tr("Add current output array to plot") );
  connect( act_graphaddout, &QAction::triggered, this, &LaboWin::slotGraphAddOut );
  model_acts << act_graphaddout;


  act_showgraphdata = new QAction( QIcon( ":icons/showgraphdata.png" ), "show plot Data", this );
  act_showgraphdata->setShortcut(  Qt::SHIFT+Qt::Key_D );
  act_showgraphdata->setWhatsThis( tr("Show plot data") );
  connect( act_showgraphdata, &QAction::triggered, this, &LaboWin::slotShowGraphData);
  model_acts << act_showgraphdata;

  act_exportgraphdata = new QAction( "E&xport plot data", this );
  act_exportgraphdata->setShortcut( Qt::SHIFT+Qt::Key_E );
  act_exportgraphdata->setWhatsThis( tr("Export plot data to text file") );
  connect( act_exportgraphdata, &QAction::triggered, this, &LaboWin::slotExportGraphData);
  model_acts << act_exportgraphdata;

  act_cloneGraph = new QAction( "Clone plot", this );
  act_cloneGraph->setWhatsThis( tr("Clone current plot") );
  connect( act_cloneGraph, &QAction::triggered, this, &LaboWin::slotCloneGraph );
  model_acts << act_cloneGraph;

  // ==== simulation group

  act_newSimul = new QAction( "&New Simulation", this );
  act_newSimul->setWhatsThis( tr("Create new simulation") );
  connect( act_newSimul, &QAction::triggered, this, &LaboWin::slotNewSimul );
  model_acts << act_newSimul;

  act_delSimul = new QAction( "&Delete Simulation", this );
  act_delSimul->setWhatsThis( tr("Delete simulation") );
  connect( act_delSimul, &QAction::triggered, this, &LaboWin::slotDelSimul );
  model_acts << act_delSimul;

  act_editSimul = new QAction( "&Edit Simulation", this );
  act_editSimul->setShortcut(  Qt::SHIFT+Qt::Key_Y );
  act_editSimul->setWhatsThis( tr("Edit simulation") );
  connect( act_editSimul, &QAction::triggered, this, &LaboWin::slotEditSimul );
  model_acts << act_editSimul;

  act_renameSimul= new QAction( "Rename simulation", this );
  act_renameSimul->setWhatsThis( tr("Rename simulation") );
  connect( act_renameSimul, &QAction::triggered, this, &LaboWin::slotRenameSimul );
  model_acts << act_renameSimul;

  act_selectSimul = new QAction( "Select Simulation", this );
  act_selectSimul->setShortcut(  Qt::ALT+Qt::Key_Y );
  act_selectSimul->setWhatsThis( tr("Edit simulation") );
  connect( act_selectSimul, &QAction::triggered, this, &LaboWin::slotSelectSimul );
  model_acts << act_selectSimul;

  act_setActiveSimul = new QAction( "Set Active Simulation", this );
  act_setActiveSimul->setShortcut( Qt::ALT+Qt::CTRL+Qt::Key_Y );
  act_setActiveSimul->setWhatsThis( tr("Set current simulation to be active") );
  connect( act_setActiveSimul, &QAction::triggered, this, &LaboWin::slotSetActiveSimul );
  model_acts << act_setActiveSimul;


  act_cloneSimul = new QAction( "Clone Simulation", this );
  act_cloneSimul->setWhatsThis( tr("Cone current cimulation") );
  connect( act_cloneSimul, &QAction::triggered, this, &LaboWin::slotCloneSimul );
  model_acts << act_cloneSimul;

  // ==== model group

  act_editmodel = new QAction( QIcon( ":icons/editmodel.png" ), "&Edit Model", this );
  // act_editmodel->setShortcut(  Qt::CTRL+Qt::Key_Enter );
  act_editmodel->setShortcut(  Qt::CTRL+Qt::Key_R );
  act_editmodel->setWhatsThis( tr("Edit model parameters.") );
  connect( act_editmodel, &QAction::triggered, this, &LaboWin::slotEditModel);
  model_acts << act_editmodel;

  act_showtreemodel = new QAction( "show model tree", this );
  act_showtreemodel->setShortcut( Qt::CTRL+Qt::Key_T );
  act_showtreemodel->setWhatsThis( tr("Show tree-like model structure") );
  connect( act_showtreemodel, &QAction::triggered, this, &LaboWin::slotShowTreeModel);
  model_acts << act_showtreemodel;

  // ====  run group

  act_runrun = new QAction( QIcon( ":icons/run.png" ), "&Run", this );
  act_runrun->setShortcut( Qt::Key_F9 );
  act_runrun->setWhatsThis( tr("Click this button to start simulation.") );
  connect( act_runrun, &QAction::triggered, this, &LaboWin::slotRunRun );
  model_acts << act_runrun;

  act_reset = new QAction( "Rese&t", this );
  act_reset->setWhatsThis( tr("Reset model state.") );
  connect( act_reset, &QAction::triggered, this, &LaboWin::slotReset );
  model_acts << act_reset;

  act_initengine = new QAction( "Init engine", this );
  act_initengine->setWhatsThis( tr("Initialize model JS engine.") );
  connect( act_initengine, &QAction::triggered, this, &LaboWin::slotInitEngine );
  model_acts << act_initengine;

  act_runscript = new QAction( QIcon::fromTheme("application-x-javascript"), "run &Script", this );
  act_runscript->setShortcut( Qt::SHIFT+Qt::Key_F9 );
  act_runscript->setWhatsThis( tr("Run script on model") );
  connect( act_runscript, &QAction::triggered, this, &LaboWin::slotRunScript );
  model_acts << act_runscript;

  act_runmodelscript = new QAction( "run &Model script", this );
  act_runmodelscript->setWhatsThis( tr("Run inner model script") );
  connect( act_runmodelscript, &QAction::triggered, this, &LaboWin::slotRunModelScript );
  model_acts << act_runmodelscript;

  // ==== iface group

  act_tbar = new QAction( "View &Toolbar", this );
  act_tbar->setCheckable( true );
  act_tbar->setChecked( true );
  // act_tbar->setWhatsThis( tr("") );
  connect( act_tbar, &QAction::triggered, this, &LaboWin::slotViewToolBar );

  act_sbar = new QAction( "View &Statusbar", this );
  act_sbar->setCheckable( true );
  act_sbar->setChecked( true );
  // act_sbar->setWhatsThis( tr("") );
  connect( act_sbar, &QAction::triggered, this, &LaboWin::slotViewStatusBar );

  act_showord = new QAction( "Show &Orders", this );
  act_showord->setCheckable( true );
  act_showord->setChecked( sett.showord );
  connect( act_showord, &QAction::triggered, this, &LaboWin::slotShowOrd );

  act_showgrid = new QAction( "Show &Grid", this );
  act_showgrid->setCheckable( true );
  act_showgrid->setChecked( sett.showgrid );
  connect( act_showgrid, &QAction::triggered, this, &LaboWin::slotShowGrid );

  act_shownames = new QAction( "Show &Names", this );
  act_shownames->setCheckable( true );
  act_shownames->setChecked( sett.shownames );
  connect( act_shownames, &QAction::triggered, this, &LaboWin::slotShowNames );

  act_showicons = new QAction( "Show &Icons", this );
  act_showicons->setCheckable( true );
  act_showicons->setChecked( sett.showicons );
  connect( act_showicons, &QAction::triggered, this, &LaboWin::slotShowIcons );

  act_showlinks = new QAction( "Show &Links", this );
  act_showlinks->setCheckable( true );
  act_showlinks->setChecked( sett.showLinks );
  connect( act_showlinks, &QAction::triggered, this, &LaboWin::slotShowLinks );

  // ==== window group

  act_winClose = new QAction( QIcon::fromTheme("window-close"), "Cl&ose Window", this);
  act_winClose->setWhatsThis( tr("Close this window") );
  connect( act_winClose, &QAction::triggered, this, &LaboWin::slotWindowClose );

  act_winCloseAll = new QAction( "Close &All Windows",  this );
  act_winCloseAll->setWhatsThis( tr("Close all windoww") );
  connect( act_winCloseAll, &QAction::triggered, this, &LaboWin::slotWindowCloseAll );

  act_winTile = new QAction( "&Tile Windows",  this );
  act_winTile->setWhatsThis( tr("Tile the windows") );
  connect( act_winTile, &QAction::triggered, this, &LaboWin::slotWindowTile );

  act_winCascade = new QAction( "&Cascade Windows",  this );
  act_winCascade->setWhatsThis( tr("Cascade the windows") );
  connect( act_winCascade, &QAction::triggered, this, &LaboWin::slotWindowCascade );


  // ==== help group

  act_helpabout = new QAction( QIcon::fromTheme("help-about"), "&About", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpabout, &QAction::triggered, this, &LaboWin::slotHelpAbout );

  act_helpaboutqt = new QAction( "About &Qt", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpaboutqt, &QAction::triggered, this, &LaboWin::slotHelpAboutQt );

  act_whatsthis = new QAction( "&What's This", this );
  // act_whatsthis->setWhatsThis( tr("") );
  // TODO:
  //connect( act_whatsthis, &QAction::triggered, this, &LaboWin::slotWhatsThis );

  act_test = new QAction( QIcon( ":icons/test.png" ), "&Test", this );
  act_test->setWhatsThis( tr("Click this button to test something.") );
  connect( act_test, &QAction::triggered, this, &LaboWin::slotTest );


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
  pElmMenu->addAction( act_renameelm );
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
  pOutMenu->addAction( act_renameout );
  pOutMenu->addSeparator();
  pOutMenu->addAction( act_selectout );
  pOutMenu->addSeparator();
  pOutMenu->addAction( act_showoutdata );
  pOutMenu->addAction( act_exportout );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pGraphMenu
  pGraphMenu = menuBar()->addMenu( tr("&Plot") );
  pGraphMenu->addAction( act_newgraph );
  pGraphMenu->addAction( act_delgraph );
  pGraphMenu->addAction( act_editgraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_renamegraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_selectgraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_showgraph );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_graphaddout );
  pGraphMenu->addSeparator();
  pGraphMenu->addAction( act_showgraphdata );
  pGraphMenu->addAction( act_exportgraphdata );
  pGraphMenu->addAction( act_cloneGraph );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pSimulMenu
  pSimulMenu = menuBar()->addMenu( tr("&Simulation") );
  pSimulMenu->addAction( act_newSimul );
  pSimulMenu->addAction( act_delSimul );
  pSimulMenu->addAction( act_editSimul );
  pSimulMenu->addSeparator();
  pSimulMenu->addAction( act_renameSimul );
  pSimulMenu->addSeparator();
  pSimulMenu->addAction( act_selectSimul );
  pSimulMenu->addSeparator();
  pSimulMenu->addAction( act_setActiveSimul );
  pSimulMenu->addAction( act_cloneSimul );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pModelMenu
  pModelMenu = menuBar()->addMenu( tr("&Model") );
  pModelMenu->addAction( act_editmodel );
  pModelMenu->addAction( act_showtreemodel );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pRunMenu
  pRunMenu = menuBar()->addMenu( tr("&Run") );
  pRunMenu->addAction( act_runrun );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_runscript );
  pRunMenu->addAction( act_runmodelscript );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_reset );
  pRunMenu->addAction( act_initengine );

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

  connect( pWindowMenu, &QMenu::aboutToShow, this, &LaboWin::windowMenuAboutToShow );

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
  elmToolbar->addAction( act_runscript );

}

void LaboWin::initStatusBar()
{
  statusBar()->showMessage( tr( "Ready." ) );
}


void LaboWin::enableActions( bool ena, int id_ )
{
  switch( id_ ) {
    case 0: // model-specific actions
      for( auto a : model_acts ) {
        a->setEnabled( ena );
      };
      break;
    default:
      break;
  };
}

void LaboWin::updateActions()
{
  if( mdiArea->currentSubWindow() != 0 ) { // TODO: different windows
    enableActions( true, 0 );
  } else {
    enableActions( false, 0 );
  };
  setWndTitle( this ); // "this" is fake

}

void LaboWin::setWndTitle( QWidget* )
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
  if (mdiArea->currentSubWindow()) {
    e->ignore();
  } else {
    // writeSettings();
    e->accept();
  }
}

LaboView* LaboWin::createChild( LaboDoc* doc )
{
  LaboView* w = new LaboView( doc, this );
  mdiArea->addSubWindow( w );
  return w;
}


bool LaboWin::queryExit()
{
  int exit = QMessageBox::information( this,
      tr("Quit..."), tr("Do your really want to quit?"),
      QMessageBox::Ok, QMessageBox::Cancel);

  return ( exit==1 );
}

QMdiSubWindow* LaboWin::findMdiChild( const QString &fileName )
{
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  for( QMdiSubWindow *window : mdiArea->subWindowList() ) {
    LaboView *mdiChild = qobject_cast<LaboView *>(window->widget());
    if( !mdiChild ) {
      continue;
    }
    QString cp = QFileInfo( mdiChild->currentFile() ).canonicalFilePath();
    if( cp  == canonicalFilePath ) {
      return window;
    }
  }
  return 0;
}

LaboView* LaboWin::activeLaboView()
{
  if( QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow() )
    return qobject_cast<LaboView *>( activeSubWindow->widget() );
  return nullptr;
}

void LaboWin::callLaboViewSlot( const char *slot, const QString &mess )
{
  statusBar()->showMessage( mess );

  LaboView* m =  activeLaboView();
  if ( m ) {
    QMetaObject::invokeMethod( m, slot, Qt::DirectConnection );
    m->update();
  }
  statusBar()->showMessage( tr( "Ready." ) );
}


/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void LaboWin::slotFileNew()
{
  statusBar()->showMessage( tr( "Creating new model file..." ) );

  LaboDoc* doc = new LaboDoc();
  ++untitledCount;
  QString fileName = QString( "untitled_%1.mo2" ).arg(untitledCount);
  doc->newDocument();
  doc->setPathName(fileName);
  doc->setTitle(fileName);

  LaboView *cw = createChild( doc );
  updateActions();
  cw->show();
  statusBar()->showMessage( tr( "Ready." ) );
}


void LaboWin::slotFileOpenXML()
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


bool LaboWin::doFileOpenXML( const QString &fn )
{
  LaboDoc* doc = new LaboDoc();
  if( ! doc->openDocumentXML( fn ) ) {
    QMessageBox::critical( this, tr("Error !"),
                           tr("Could not read file !" ) + fn );
    delete doc;
    updateActions();
    statusBar()->showMessage( tr( "Open Failed." ) );
    return false;
  };

  LaboView *cw = createChild( doc );
  cw->show();
  updateActions();
  return true;
}


void LaboWin::slotFileSaveXML()
{
  statusBar()->showMessage( tr( "Saving model file...") );
  LaboView* m =  activeLaboView();
  if( m ) {
    LaboDoc* doc = m->getDocument();
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


void LaboWin::slotFileSaveXMLAs()
{
  statusBar()->showMessage( tr ( "Saving model file under new filename..." ) );
  LaboView* m =  activeLaboView();
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

    LaboDoc* doc = m->getDocument();
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

  Mo2SettDlg *dia = new Mo2SettDlg( sett, this );
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


void LaboWin::slotViewToolBar()
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

void LaboWin::slotViewStatusBar()
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

  ostr += QString(" em=" ) + QSN(em) + "\n";

  QString ts = "ab\\approx \\alpha\\cdot\\phi \\omega{}\\n\\sum_{i=0}^{N-1}{(a+b+\\epsilon)} ";

  QString ls = tex2label( ts );
  DBGx( "dbg: ts=\"%s\"", qP(ts) );
  DBGx( "dbg: ls=\"%s\"", qP(ls) );

  ostr += ts % "\n" % ls;

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

  // add windows to menu
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  for( int i = 0; i < windows.size(); ++i ) {
    LaboView *child = qobject_cast<LaboView *>(windows.at(i)->widget());

    QString text;
    if (i < 9) {
      text = tr("&%1 %2").arg(i + 1).arg(child->currentFile());
    } else {
      text = tr("%1 %2").arg(i + 1).arg(child->currentFile());
    }
    QAction *action  = pWindowMenu->addAction( text );
    action->setCheckable(true);
    action->setChecked( child == activeLaboView() );
    // cast to fight unbiguity:
    connect( action, &QAction::triggered, windowMapper,
        static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map) );
    windowMapper->setMapping( action, windows.at(i) );
  }

  updateActions();

}



void LaboWin::slotNewElm()
{
  callLaboViewSlot( "newElm", tr( "Inserting new element..." ) );
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
  callLaboViewSlot( "newOut", tr( "Inserting output array..." ) );
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
  callLaboViewSlot( "newGraph", tr( "Inserting new plot..." ) );
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
  callLaboViewSlot( "newSimul", tr( "Creating new simulation..." ) );
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


// ------- model -----------

void LaboWin::slotEditModel()
{
  callLaboViewSlot( "editModel", tr( "Editing model parametrs..." ) );
}


void LaboWin::slotShowTreeModel()
{
  callLaboViewSlot( "showTreeModel", tr( "Show model tree..." ) );
}


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

