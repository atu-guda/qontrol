/***************************************************************************
                          qmo2.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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
#include <QtGui>

// application specific includes
#include "../config.h"
#include "qmo2view.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "mo2settdlg.h"


QMo2Win* QMo2Win::qmo2win = 0;

QMo2Win::QMo2Win(void)
{
  setWindowTitle( PACKAGE " " VERSION );
  
  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  setCentralWidget( mdiArea );

  connect( mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
          this, SLOT(updateActions()) );
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)),
          this, SLOT(setActiveSubWindow(QWidget*)));

  printer = new QPrinter;
  printer->setPageSize( QPrinter::A4 );
  printer->setColorMode( QPrinter::GrayScale ); 
  untitledCount = 0;
  /* -------- other ----------- */
  add_dir = "" ;

  initDirs();
  // call inits to invoke all other construction parts
  QFont mf;
  mf.fromString( sett.mainFont );
  QApplication::setFont( mf );
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
  QString app = QString::fromLocal8Bit( "share/" PACKAGE );
  QString sep = QString::fromLocal8Bit( "/" );
  QString prefix = QString::fromLocal8Bit( "/usr" ); // TODO: from config
  
  xbuild_dir = QString::null;
  QDir d( qApp->applicationDirPath() );
  d.makeAbsolute();
  if( d.exists( QString::fromLocal8Bit( "main.cpp" ) )  
      && d.exists( app  ) ) {
    xbuild_dir = d.canonicalPath() + sep + app;
  };

  global_dir = QString::null;
  d.setPath( prefix + sep + app );
  if( d.isReadable() ) {
    global_dir = d.canonicalPath();
  };

  local_dir = QString::null;
  d.setPath( QDir::homePath() + sep + app );
  d.makeAbsolute();
  if( d.isReadable() ) {
    local_dir = d.canonicalPath();
  };
  
  d.setPath( add_dir );
  d.makeAbsolute();
  if( ! d.isReadable() ) {
    add_dir = QString::null;
  };

  env_dir = QString::null;
  const char *evar = getenv("QMO2DIR");
  if( evar ) {
    d.setPath( QString::fromLocal8Bit(evar) );
    if( d.isReadable() ) {
      d.makeAbsolute();
      env_dir = d.canonicalPath();
    };
  };

  all_dirs[4] = &global_dir; all_dirs[3] = &local_dir;
  all_dirs[2] = &add_dir;    all_dirs[1] = &xbuild_dir;
  all_dirs[0] = &env_dir;    all_dirs[5] = 0;

}

QString QMo2Win::findRes( const QString &res )
{
  for( int i=0; i<5; i++ ) {
    if( all_dirs[i]->isEmpty() )
      continue;
    QDir d( *all_dirs[i] );
    if( d.exists( ) ) { // TODO: it was 2 args?
      return *all_dirs[i] + "/" + res;
    };
  };
  return QString::null;
}

void QMo2Win::initIface()
{
  // Actions
  // File group
 
  act_new = new QAction( QIcon( ":icons/filenew.png" ), "&New model", this );
  act_new->setShortcuts( QKeySequence::New );
  act_new->setWhatsThis( tr("Click this button to create a new model file.") );
  connect( act_new, SIGNAL( activated() ), this, SLOT( slotFileNew() ) );

  act_open = new QAction( QIcon( ":icons/mod_open.png" ), "&Open model", this );
  act_open->setShortcuts( QKeySequence::Open );
  act_open->setWhatsThis( tr("Click this button to open a model file." ) );
  connect( act_open, SIGNAL( activated() ), this, SLOT( slotFileOpen() ) );

  act_openxml = new QAction( QIcon::fromTheme("document-open"), "Open XML model", this );
  // act_openxml->setShortcuts( QKeySequence::Open );
  act_openxml->setWhatsThis( tr("Click this button to open a XML model file." ) );
  connect( act_openxml, SIGNAL( activated() ), this, SLOT( slotFileOpenXML() ) );

  act_close = new QAction( "&Close", this );
  act_close->setWhatsThis( tr("Close active window") );
  connect( act_close, SIGNAL( activated() ), this, SLOT( slotFileClose() ) );

  act_save = new QAction( QIcon( ":icons/mod_save.png" ), "&Save model",  this );
  act_save->setShortcuts( QKeySequence::Save );
  act_save->setWhatsThis( tr("Click this button to save the model file you are "
                  "editing. You will be prompted for a file name." ) );
  connect( act_save, SIGNAL( activated() ), this, SLOT( slotFileSave() ) );

  act_savexml = new QAction( QIcon::fromTheme("document-save"), "Save XML model",  this );
  // act_savexml->setShortcuts( QKeySequence::Save );
  act_savexml->setWhatsThis( tr("Click this button to save the XML model file you are "
                  "editing. You will be prompted for a file name." ) );
  connect( act_savexml, SIGNAL( activated() ), this, SLOT( slotFileSaveXML() ) );

  act_saveas = new QAction( "Save &As", this );
  act_saveas->setWhatsThis( tr("Save current model with another filename") );
  connect( act_saveas, SIGNAL( activated() ), this, SLOT( slotFileSaveAs() ) );

  act_savexmlas = new QAction(QIcon::fromTheme("document-save-as"), "Save XML As", this );
  act_savexmlas->setWhatsThis( tr("Save current XML model with another filename") );
  connect( act_savexmlas, SIGNAL( activated() ), this, SLOT( slotFileSaveXMLAs() ) );

  act_print = new QAction(  QIcon::fromTheme("document-print"), "&Print", this );
  act_print->setShortcuts( QKeySequence::Print );
  act_print->setWhatsThis( tr("Print current model") );
  connect( act_print, SIGNAL( activated() ), this, SLOT( slotFilePrint() ) );

  act_settings = new QAction( "Sett&ings", this );
  act_settings ->setWhatsThis( "Edit application settings" );
  connect( act_settings, SIGNAL( activated() ), this, SLOT( slotFileSettings() ) );
  
  act_savesett = new QAction( "Save s&ettings", this );
  act_savesett ->setWhatsThis( "Save application settings" );
  connect( act_savesett, SIGNAL( activated() ), this, SLOT( slotFileSaveSett() ) );


  act_quit = new QAction(  QIcon::fromTheme("application-exit"), "&Quit", this );
  act_quit->setShortcuts( QKeySequence::Quit );
  act_quit->setWhatsThis( tr("Click this button to quit application.") );
  connect( act_quit, SIGNAL( activated() ), this, SLOT( slotFileQuit() ) );

  // ==== Edit group
 
  act_undo = new QAction( QIcon::fromTheme("edit-undo"), "&Undo", this );
  act_undo->setShortcuts( QKeySequence::Undo );
  act_undo->setWhatsThis( tr("Undo last action") + " EMPTY" );
  connect( act_undo, SIGNAL( activated() ), this, SLOT( slotEditUndo() ) );
 
  act_cut = new QAction(  QIcon::fromTheme("edit-cut"),"Cut", this );
  act_cut->setShortcuts( QKeySequence::Cut );
  act_cut->setWhatsThis( tr("Cut selected")  + " EMPTY" );
  connect( act_cut, SIGNAL( activated() ), this, SLOT( slotEditCut() ) );
 
  act_copy = new QAction( QIcon::fromTheme("edit-copy"), "Copy", this );
  act_copy->setShortcuts( QKeySequence::Copy );
  act_copy->setWhatsThis( tr("Copy selected") + " EMPTY" );
  connect( act_copy, SIGNAL( activated() ), this, SLOT( slotEditCopy() ) );
 
  act_paste= new QAction( QIcon::fromTheme("edit-paste"), "Paste", this );
  act_paste->setShortcuts( QKeySequence::Paste );
  act_paste->setWhatsThis( tr("Paste selected") + " EMPTY" );
  connect( act_paste, SIGNAL( activated() ), this, SLOT( slotEditPaste() ) );

  // ==== Element group
 
  act_newelm= new QAction( QIcon( ":icons/newelm.png" ), "&New element", this ); 
  act_newelm->setShortcut( Qt::Key_Insert );
  act_newelm->setWhatsThis( tr("Create new element") );
  connect( act_newelm, SIGNAL( activated() ), this, SLOT( slotNewElm() ) );
 
  act_delelm= new QAction( QIcon( ":icons/delelm.png" ), "&Delete element", this );
  act_delelm->setShortcut( Qt::Key_Delete );
  act_delelm->setWhatsThis( tr("Delete selected element") );
  connect( act_delelm, SIGNAL( activated() ), this, SLOT( slotDelElm() ) );
 
  act_editelm= new QAction( QIcon( ":icons/editelm.png" ), "&Edit element", this );
  act_editelm->setShortcut( Qt::Key_Enter );
  act_editelm->setWhatsThis( tr("Edit selected element") );
  connect( act_editelm, SIGNAL( activated() ), this, SLOT( slotEditElm() ));
 
  act_linkelm= new QAction( QIcon( ":icons/linkelm.png" ), "&Link element", this );
  act_linkelm->setShortcut( Qt::Key_L );
  act_linkelm->setWhatsThis( tr("Edit links of selected element") );
  connect( act_linkelm, SIGNAL( activated() ), this, SLOT( slotLinkElm() ));
 
  act_qlinkelm= new QAction( "&Quick link element", this );
  act_qlinkelm->setShortcut( Qt::CTRL+Qt::Key_L );
  act_qlinkelm->setWhatsThis( tr("Link marked to selected element") );
  connect( act_qlinkelm, SIGNAL( activated() ), this, SLOT( slotqLinkElm() ));
 
  act_qplinkelm= new QAction( "&Parametr link element", this );
  act_qplinkelm->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_L );
  act_qplinkelm->setWhatsThis( tr("Link marked to selected element "
	                          "(parameter input)") );
  connect( act_qplinkelm, SIGNAL( activated() ), this, SLOT( slotqpLinkElm()));
 
  act_unlinkelm= new QAction( "&Unlink element", this );
  act_unlinkelm->setWhatsThis( tr("Remove links of selected element") );
  connect( act_unlinkelm, SIGNAL( activated() ), this, SLOT( slotUnlinkElm() ));
 
  act_lockelm= new QAction( QIcon( ":icons/lockelm.png" ), "Loc&k element",  this );
  act_lockelm->setShortcut( Qt::CTRL+Qt::Key_K );
  act_lockelm->setWhatsThis( tr("Lock current element") );
  connect( act_lockelm, SIGNAL( activated() ), this, SLOT( slotLockElm() ));
 
  act_ordelm= new QAction( QIcon( ":icons/orderelm.png" ), "Change &Orger", this );
  act_ordelm->setShortcut( Qt::Key_O );
  act_ordelm->setWhatsThis( tr("Change order numper of selected element") );
  connect( act_ordelm, SIGNAL( activated() ), this, SLOT( slotOrdElm() ));
 
  act_markelm= new QAction( QIcon( ":icons/markelm.png" ), "&Mark element", this );
  act_markelm->setShortcut( Qt::Key_M );
  act_ordelm->setWhatsThis( tr("Mark selected element") );
  connect( act_markelm, SIGNAL( activated() ), this, SLOT( slotMarkElm() ));
 
  act_moveelm= new QAction( "Move element", this );
  act_moveelm->setShortcut( Qt::SHIFT+Qt::Key_M );
  act_moveelm->setWhatsThis( tr("Move marked element to selected cell") );
  connect( act_moveelm, SIGNAL( activated() ), this, SLOT( slotMoveElm() ));
 
  act_infoelm= new QAction( QIcon( ":icons/infoelm.png" ), "show &Info", this );
  act_infoelm->setShortcut( Qt::Key_I );
  act_infoelm->setWhatsThis( tr("Show information about element structure") );
  connect( act_infoelm, SIGNAL( activated() ), this, SLOT( slotInfoElm() ));
 
  act_testelm1= new QAction( "test element 1", this );
  act_testelm1->setWhatsThis( tr("Test something in element") );
  connect( act_testelm1, SIGNAL( activated() ), this, SLOT( slotTestElm1() ));
  
  act_testelm2= new QAction( "test element 2", this );
  act_testelm2->setWhatsThis( tr("Test something in element") );
  connect( act_testelm2, SIGNAL( activated() ), this, SLOT( slotTestElm2() ));
  // ==== out group 
 
  act_newout = new QAction( QIcon( ":icons/newout.png" ), "&New Out", this );
  act_newout->setShortcut( Qt::Key_U );
  act_newout->setWhatsThis( tr("Create output collector") );
  connect( act_newout, SIGNAL( activated() ), this, SLOT( slotNewOut() ));

  act_delout = new QAction( QIcon( ":icons/delout.png" ), "&Delete out", this );
  act_delout->setShortcut( Qt::Key_X );
  act_delout->setWhatsThis( tr("Delete output collector with current level") );
  connect( act_delout, SIGNAL( activated() ), this, SLOT( slotDelOut() ));

  act_editout = new QAction( QIcon( ":icons/editout.png" ), "&Edit out", this );
  act_editout->setShortcut( Qt::SHIFT+Qt::Key_U );
  act_editout->setWhatsThis( tr("Edit outsput collector withcurrent level") );
  connect( act_editout, SIGNAL( activated() ), this, SLOT( slotEditOut() ));

  act_showoutdata = new QAction( QIcon( ":icons/showoutdata.png" ), "&Show out data", this );
  act_showoutdata->setShortcut( Qt::Key_D );
  act_showoutdata->setWhatsThis( tr("Show data collected by output.") );
  connect( act_showoutdata, SIGNAL( activated() ), this, SLOT( slotShowOutData() ));

  act_exportout = new QAction( "E&xport out data", this );
  act_exportout->setShortcut( Qt::Key_E );
  act_exportout->setWhatsThis( tr("Export data collected by output to text file.") );
  connect( act_exportout, SIGNAL( activated() ), this, SLOT( slotExportOut() ));

  // ==== graph group

  act_newgraph = new QAction( QIcon( ":icons/newgraph.png" ), "&New Graph", this );
  act_newgraph->setShortcut( Qt::Key_G );
  act_newgraph->setWhatsThis( tr("Create new graph") );
  connect( act_newgraph, SIGNAL( activated() ), this, SLOT( slotNewGraph()));

  act_delgraph = new QAction( QIcon( ":icons/delgraph.png" ), "&Delete graph", this );
  act_delgraph->setShortcut( Qt::SHIFT+Qt::Key_X );
  act_delgraph->setWhatsThis( tr("Delete graph with selected level") );
  connect( act_delgraph, SIGNAL( activated() ), this, SLOT( slotDelGraph()));

  act_editgraph = new QAction( QIcon( ":icons/editgraph.png" ), "&Edit graph", this );
  act_editgraph->setShortcut(  Qt::SHIFT+Qt::Key_G );
  act_editgraph->setWhatsThis( tr("Edit graph with current level") );
  connect( act_editgraph, SIGNAL( activated() ), this, SLOT( slotEditGraph() ));

  act_showgraph = new QAction( QIcon( ":icons/showgraph.png" ), "&Show graph", this );
  act_showgraph->setShortcut( Qt::Key_S );
  act_showgraph->setWhatsThis( tr("Show graph plot") );
  connect( act_showgraph, SIGNAL( activated() ), this, SLOT( slotShowGraph() ));

  act_showgraphdata = new QAction( QIcon( ":icons/showgraphdata.png" ), "show graph Data", this );
  act_showgraphdata->setShortcut(  Qt::SHIFT+Qt::Key_D );
  act_showgraphdata->setWhatsThis( tr("Show graph data") );
  connect( act_showgraphdata, SIGNAL( activated() ), this, SLOT( slotShowGraphData() ));

  act_exportgraphdata = new QAction( "E&xport graph data", this );
  act_exportgraphdata->setShortcut( Qt::SHIFT+Qt::Key_E );
  act_exportgraphdata->setWhatsThis( tr("Export graph data to text file") );
  connect( act_exportgraphdata, SIGNAL( activated() ), this, SLOT( slotExportGraphData() ));

  act_gnuplotgraph = new QAction( "&Gnuplot graph", this);
  act_gnuplotgraph->setWhatsThis( tr("Export graph data to gnuplot-compatiomle file") );
  connect( act_gnuplotgraph, SIGNAL( activated() ), this, SLOT( slotGnuplotGraph() ));

  // ==== model group

  act_editmodel = new QAction( QIcon( ":icons/editmodel.png" ), "&Edit Model", this );
  act_editmodel->setShortcut(  Qt::CTRL+Qt::Key_Enter );
  act_editmodel->setWhatsThis( tr("Edit model parameters.") );
  connect( act_editmodel, SIGNAL( activated() ), this, SLOT( slotEditModel()));

  act_showvars = new QAction( "&Show model vars", this ); 
  act_showvars->setShortcut(  Qt::SHIFT+Qt::CTRL+Qt::Key_D );
  act_showvars->setWhatsThis( tr("Show model vars field") );
  connect( act_showvars, SIGNAL( activated() ), this, SLOT( slotShowVars()));

  // ====  run group

  act_runrun = new QAction( QIcon( ":icons/run.png" ), "&Run", this );
  act_runrun->setShortcut( Qt::Key_F9 );
  act_runrun->setWhatsThis( tr("Click this button start simple run.") );
  connect( act_runrun, SIGNAL( activated() ), this, SLOT( slotRunRun()) );
  
  act_runprm= new QAction( QIcon( ":icons/run_p1.png" ), "Run &1D Parm loop", this );
  act_runprm->setShortcut( Qt::CTRL+Qt::Key_F9 );
  act_runprm->setWhatsThis( tr("Click this button start 1D parametric run.") );
  connect( act_runprm, SIGNAL( activated() ), this, SLOT( slotRunPrm()) );

  act_runprm2= new QAction( QIcon( ":icons/run_p2.png" ), "Run &2D Parm loop", this );
  act_runprm2->setShortcut( Qt::SHIFT+Qt::CTRL+Qt::Key_F9 );
  act_runprm2->setWhatsThis( tr("Click this button start 2D parametric run.") );
  connect( act_runprm2, SIGNAL( activated() ), this, SLOT( slotRunPrm2()) );

  act_reset= new QAction( "R&eset", this );
  act_reset->setWhatsThis( tr("Reset model state.") );
  connect( act_reset, SIGNAL( activated() ), this, SLOT( slotReset()) );

  // ==== iface group

  act_tbar = new QAction( "View &Toolbar", this );
  act_tbar->setCheckable( true );
  act_tbar->setChecked( true );
  // act_tbar->setWhatsThis( tr("") );
  connect( act_tbar, SIGNAL( activated() ), this, SLOT( slotViewToolBar()) );

  act_sbar = new QAction( "View &Statusbar", this );
  act_sbar->setCheckable( true );
  act_sbar->setChecked( true );
  // act_sbar->setWhatsThis( tr("") );
  connect( act_sbar, SIGNAL( activated() ), this, SLOT( slotViewStatusBar()) );

  act_showord = new QAction( "Show &Orders", this );
  act_showord->setCheckable( true );
  act_showord->setChecked( sett.showord );
  connect( act_showord, SIGNAL( activated() ), this, SLOT( slotShowOrd()) );

  act_showgrid = new QAction( "Show &Grid", this );
  act_showgrid->setCheckable( true );
  act_showgrid->setChecked( sett.showgrid );
  connect( act_showgrid, SIGNAL( activated() ), this, SLOT( slotShowGrid()) );

  act_shownames = new QAction( "Show &Names", this );
  act_shownames->setCheckable( true );
  act_shownames->setChecked( sett.shownames );
  connect( act_shownames, SIGNAL( activated() ), this, SLOT( slotShowNames()) );

  act_showicons = new QAction( "Show &Icons", this );
  act_showicons->setCheckable( true );
  act_showicons->setChecked( sett.showicons );
  connect( act_showicons, SIGNAL( activated() ), this, SLOT( slotShowIcons()) );

  // ==== window group

  act_winClose = new QAction( QIcon::fromTheme("window-close"), "Cl&ose Window", this);
  act_winClose->setWhatsThis( tr("Close this window") );
  connect( act_winClose, SIGNAL( activated() ), this, SLOT( slotWindowClose()) );

  act_winCloseAll = new QAction( "Close &All Windows",  this );
  act_winCloseAll->setWhatsThis( tr("Close all windoww") );
  connect( act_winCloseAll, SIGNAL( activated() ), this, SLOT( slotWindowCloseAll()) );

  act_winTile = new QAction( "&Tile Windows",  this );
  act_winTile->setWhatsThis( tr("Tile the windows") );
  connect( act_winTile, SIGNAL( activated() ), this, SLOT( slotWindowTile()) );

  act_winCascade = new QAction( "&Cascade Windows",  this );
  act_winCascade->setWhatsThis( tr("Cascade the windows") );
  connect( act_winCascade, SIGNAL( activated() ), this, SLOT( slotWindowCascade()) );
  
  //act_winnew = new QAction( "&New Window", 0, this, "winnew");
  //act_winnew->setWhatsThis( tr("Crerate new window for same model") );
  //connect( act_winnew, SIGNAL( activated() ), this, SLOT( slotWindowNewWindow()) );

  // ==== help group

  act_helpabout = new QAction( QIcon::fromTheme("help-about"), "&About", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpabout, SIGNAL( activated() ), this, SLOT( slotHelpAbout()) );

  act_helpaboutqt = new QAction( "About &Qt", this );
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpaboutqt, SIGNAL( activated() ), this, SLOT( slotHelpAboutQt()) );
  
  act_whatsthis = new QAction( "&What's This", this );
  // act_whatsthis->setWhatsThis( tr("") );
  // TODO:
  //connect( act_whatsthis, SIGNAL( activated() ), this, SLOT( slotWhatsThis()) );

  act_test = new QAction( QIcon( ":icons/test.png" ), "&Test", this );
  act_test->setWhatsThis( tr("Click this button to test something.") );
  connect( act_test, SIGNAL( activated() ), this, SLOT( slotTest()) );

  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pFileMenu
		
  pFileMenu = menuBar()->addMenu( tr("&File") );
  pFileMenu->addAction( act_new );
  pFileMenu->addAction( act_open );
  pFileMenu->addAction( act_close );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_save );
  pFileMenu->addAction( act_saveas );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_openxml );
  pFileMenu->addAction( act_savexml );
  pFileMenu->addAction( act_savexmlas );
  pFileMenu->addSeparator();
  pFileMenu->addAction( act_print );
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
  pElmMenu->addAction( act_linkelm );
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
  pModelMenu->addSeparator();
  pModelMenu->addAction( act_showvars );
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pRunMenu
  pRunMenu = menuBar()->addMenu( tr("&Run") );
  pRunMenu->addAction( act_runrun );
  pRunMenu->addAction( act_runprm );
  pRunMenu->addAction( act_runprm2 );
  pRunMenu->addSeparator();
  pRunMenu->addAction( act_reset );
  
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
  
  ///////////////////////////////////////////////////////////////////
  // CONNECT THE SUBMENU SLOTS WITH SIGNALS

  connect( pWindowMenu, SIGNAL(aboutToShow() ), SLOT( windowMenuAboutToShow() ) );

  ///////////////////////////////////////////////////////////////////
  // TOOLBARS

  fileToolbar = addToolBar( tr("File") );
  fileToolbar->addAction( act_new );
  fileToolbar->addAction( act_open );
  fileToolbar->addAction( act_save );
    fileToolbar->addSeparator();
  fileToolbar->addAction( act_test );
    fileToolbar->addSeparator();
  fileToolbar->addAction( act_openxml );
  fileToolbar->addAction( act_savexml );
  
  elmToolbar = addToolBar( tr("Element") );
  elmToolbar->addAction( act_newelm );
  elmToolbar->addAction( act_delelm );
  elmToolbar->addAction( act_editelm );
  elmToolbar->addAction( act_linkelm );
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
       act_save->setEnabled( ena );
       act_saveas->setEnabled( ena );
       act_savexml->setEnabled( ena );
       act_savexmlas->setEnabled( ena );
       act_print->setEnabled( ena );
       // TODO: implement this
       act_undo->setEnabled( false );
       act_cut->setEnabled( false );
       act_copy->setEnabled( false );
       act_paste->setEnabled( false );
       // elm
       act_newelm->setEnabled( ena );
       act_delelm->setEnabled( ena );
       act_editelm->setEnabled( ena );
       act_linkelm->setEnabled( ena );
       act_qlinkelm->setEnabled( ena );
       act_qplinkelm->setEnabled( ena );
       act_unlinkelm->setEnabled( ena );
       act_lockelm->setEnabled( ena );
       act_ordelm->setEnabled( ena );
       act_markelm->setEnabled( ena );
       act_moveelm->setEnabled( ena );
       act_infoelm->setEnabled( ena );
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
       act_showvars->setEnabled( ena );
       // run
       act_runrun->setEnabled( ena );
       act_runprm->setEnabled( ena );
       act_runprm2->setEnabled( ena );
       act_reset->setEnabled( ena );
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

void QMo2Win::slotFileOpen()
{
  statusBar()->showMessage( tr( "Opening model file..." ) );
  QString fileName = 
    QFileDialog::getOpenFileName( this, tr("Open model file"),
	"", "Model files (*.mo2);;All files(*)" );
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

void QMo2Win::slotFileOpenXML()
{
  statusBar()->showMessage( tr( "Opening XML model file..." ) );
  QString fileName = 
    QFileDialog::getOpenFileName( this, tr("Open model file"),
	"", "Model XML files (*.qol);;All files(*)" );
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


bool QMo2Win::doFileOpen( const QString &fn )
{
  QMo2Doc* doc = new QMo2Doc();
  if( ! doc->openDocument( fn ) ) {
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

void QMo2Win::slotFileSave()
{
  statusBar()->showMessage( tr( "Saving model file...") );	
  QMo2View* m =  activeMdiChild();
  if( m ) {
    QMo2Doc* doc = m->getDocument();
    if( doc->nonamed() ) {
      slotFileSaveAs();
    } else {
      if( ! doc->saveDocument( doc->pathName() ) )
         QMessageBox::critical( this, 
	   tr("I/O Error !"), tr("Could not save the current document !" ) );
    };	 
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotFileSaveXML()
{
  statusBar()->showMessage( tr( "Saving XML model file...") );	
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
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotFileSaveAs()
{
  statusBar()->showMessage( tr ( "Saving model file under new filename..." ) );
  QMo2View* m =  activeMdiChild();
  if( !m ) {
    QMessageBox::critical ( this, 
      tr("Critical error!"), tr("Fail to find active window while saving file!") );
  }
  QString fn = QFileDialog::getSaveFileName( this, tr("Save File"),
      QString::null, "Model files (*.mo2);;All files(*)" );

  if ( !fn.isEmpty() ) {
    QFileInfo fi( fn );
    QString pfn = fi.fileName();
    if( ! pfn.contains('.') )
      fn += ".mo2";
    fi.setFile( fn );
    if( fi.exists () && 
	QMessageBox::Yes != 
	QMessageBox::question( this, "Confirm overwrite", 
	  QString("File %1 already exists.\nOverwrite?").arg(fn),
	  QMessageBox::Yes, QMessageBox::No ) ) 
    {
       statusBar()->showMessage( tr( "Ready." ) );
       return;
    }
    QMo2Doc* doc = m->getDocument();
    if( ! doc->saveDocument( fn ) ) {
       QMessageBox::critical ( this, 
	   tr("I/O Error !"), tr("Could not save the current model file!") );
       return;
    };
    doc->changedViewList();
    m->setWindowTitle( doc->title() );
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotFileSaveXMLAs()
{
  statusBar()->showMessage( tr ( "Saving XML model file under new filename..." ) );
  QMo2View* m =  activeMdiChild();
  if( !m ) {
    QMessageBox::critical ( this, 
      tr("Critical error!"), tr("Fail to find active window while saving file!") );
  }
  QString fn = QFileDialog::getSaveFileName( this, tr("Save XML model"),
      QString::null, "Model XML files (*.qol);;All files(*)" );

  if ( !fn.isEmpty() ) {
    QFileInfo fi( fn );
    QString pfn = fi.fileName();
    if( ! pfn.contains('.') )
      fn += ".qol";
    fi.setFile( fn );
    if( fi.exists () && 
	QMessageBox::Yes != 
	QMessageBox::question( this, "Confirm overwrite", 
	  QString("File %1 already exists.\nOverwrite?").arg(fn),
	  QMessageBox::Yes, QMessageBox::No ) ) 
    {
       statusBar()->showMessage( tr( "Ready." ) );
       return;
    }
    QMo2Doc* doc = m->getDocument();
    if( ! doc->saveDocumentXML( fn ) ) {
       QMessageBox::critical ( this, 
	   tr("I/O Error !"), tr("Could not save the current model file!") );
       return;
    };
    doc->changedViewList();
    m->setWindowTitle( doc->title() );
  };
  updateActions();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotFileClose()
{
  statusBar()->showMessage( tr ( "Closing model file..." ) );
  QMo2View* m =  activeMdiChild();
  if( m ) {
    m->close();
  };
  updateActions();
  statusBar()->showMessage( tr ( "Ready." ) );
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
    QFont mf;
    mf.fromString( sett.mainFont );
    QApplication::setFont( mf );
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
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->cut();	
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditCopy()
{
  statusBar()->showMessage( tr( "Copying selection to clipboard..." ) );

//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->copy();
  statusBar()->showMessage( tr( "Ready." ) );
}

void QMo2Win::slotEditPaste()
{
  statusBar()->showMessage( tr( "Inserting clipboard contents..." ) );
	
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->paste();

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
}

void QMo2Win::slotShowGrid()
{
  sett.showgrid = ! sett.showgrid;
}

void QMo2Win::slotShowNames()
{
  sett.shownames = ! sett.shownames;
}

void QMo2Win::slotShowIcons()
{
  sett.showicons = ! sett.showicons;
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
  static const char *loc_test = 
    "áâ÷çäåöúéëìíîïðòóôõæèãþûýùÿøüàñ:"
    "ÁÂ×ÇÄÅÖÚÉËÌÍÎÏÐÒÓÔÕÆÈÃÞÛÝÙßØÜÀÑ:¦¶§·¤´£³ ";
  QString ostr( "Test called\n" );
  statusBar()->showMessage( tr( "Test something..." ) );
  ostr += QString::fromLocal8Bit( loc_test ) + "(Local8bit)\n" ;
  ostr += QString::fromAscii( loc_test ) + "(Ascii)\n";
  ostr += loc_test;
  ostr += "(simple)\n";
  for( unsigned ccode=0x0380; ccode < 0x0400; ++ccode )  {
    ostr += QChar( ccode );
    if( (ccode & 0x1F) == 0x1F ) 
      ostr += QChar('\n');
  }
  ostr += "\n";

  for(int i=0; i<5; i++) {
    ostr += QString::number(i) + ": ";
    ostr += *all_dirs[i] + "\n";
  };
  ostr += "local: "; 
  ostr += findRes("local.dat");
  ostr += "\nglobal: "; 
  ostr += findRes("global.dat");
  ostr += "\nbuild: "; 
  ostr += findRes("build.dat");
  ostr += "\nenv: "; 
  ostr += findRes("env.dat");
  ostr += "\nDMIN: " + QString::number( DMIN );
  ostr += " DMAX: "  + QString::number( DMAX );
  ostr += "\niDMIN: " + QString::number( (int)(DMIN) );
  ostr += " iDMAX: "  + QString::number( (int)(DMAX) );
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
    connect( action, SIGNAL(triggered()), windowMapper, SLOT(map()) );
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

void QMo2Win::slotLinkElm()
{
  statusBar()->showMessage( tr( "Linking element..." ) );
  
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->linkElm();
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

void QMo2Win::slotShowVars()
{
  statusBar()->showMessage( tr( "Showing model vars..." ) );
  
  QMo2View* m =  activeMdiChild();
  if ( m )
    m->showVars();
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

