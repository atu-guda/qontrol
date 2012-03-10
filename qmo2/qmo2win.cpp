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
#include <qvbox.h>
#include <qsettings.h>
#include <qfontdialog.h>
#include <qapplication.h>

// application specific includes
#include "../config.h"
#include "qmo2view.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "mo2settdlg.h"
#include "resource.h"

#include "icons/filenew.xpm"
//#include "icons/fileopen.xpm"
//#include "icons/filesave.xpm"
#include "icons/test.xpm"
#include "icons/mod_open.xpm"
#include "icons/mod_save.xpm"

#include "icons/newelm.xpm"
#include "icons/delelm.xpm"
#include "icons/editelm.xpm"
#include "icons/linkelm.xpm"
#include "icons/markelm.xpm"
#include "icons/orderelm.xpm"
#include "icons/lockelm.xpm"
#include "icons/infoelm.xpm"

#include "icons/newout.xpm"
#include "icons/delout.xpm"
#include "icons/editout.xpm"
#include "icons/showoutdata.xpm"

#include "icons/newgraph.xpm"
#include "icons/delgraph.xpm"
#include "icons/editgraph.xpm"
#include "icons/showgraph.xpm"
#include "icons/showgraphdata.xpm"

#include "icons/editmodel.xpm"

#include "icons/run.xpm"
#include "icons/run_p1.xpm"
#include "icons/run_p2.xpm"

#include "icons/exit.xpm"

#include "icons/app.xpm"
#include "icons/model.xpm"

QMo2Win* QMo2Win::qmo2win = 0;

QMo2Win::QMo2Win(void)
{
  setCaption( PACKAGE " " VERSION );
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
  initView();
  setIcon( app_icon );
  if( sett.showmax )
    showMaximized();
  qmo2win = this;
}

QMo2Win::~QMo2Win()
{
  delete printer;
  printer = 0;
  qmo2win = 0;
}

void QMo2Win::initDirs()
{
  QString app = QString::fromLocal8Bit( "share/" PACKAGE );
  QString sep = QString::fromLocal8Bit( "/" );
  QString prefix = QString::fromLocal8Bit( "/usr" ); // TODO: from config
  
  xbuild_dir = QString::null;
  QDir d( qApp->applicationDirPath() );
  d.convertToAbs();
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
  d.setPath( QDir::homeDirPath() + sep + app );
  d.convertToAbs();
  if( d.isReadable() ) {
    local_dir = d.canonicalPath();
  };
  
  d.setPath( add_dir );
  d.convertToAbs();
  if( ! d.isReadable() ) {
    add_dir = QString::null;
  };

  env_dir = QString::null;
  const char *evar = getenv("QMO2DIR");
  if( evar ) {
    d.setPath( QString::fromLocal8Bit(evar) );
    if( d.isReadable() ) {
      d.convertToAbs();
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
    if( d.exists( res, false ) ) {
      return *all_dirs[i] + "/" + res;
    };
  };
  return QString::null;
}

void QMo2Win::initIface()
{
  // Actions
  // File group
 
  act_new = new QAction( QPixmap( filenew ), "&New model",
      CTRL+Key_N,  this, "new");
  act_new->setWhatsThis( tr("Click this button to create a new model file.") );
  connect( act_new, SIGNAL( activated() ), this, SLOT( slotFileNew() ) );

  act_open = new QAction( QPixmap( mod_open ), "&Open model",
      CTRL+Key_O, this, "open");
  act_open->setWhatsThis( tr("Click this button to open a model file." ) );
  connect( act_open, SIGNAL( activated() ), this, SLOT( slotFileOpen() ) );

  act_close = new QAction( "&Close", CTRL+Key_W, this, "close");
  act_close->setWhatsThis( tr("Close active window") );
  connect( act_close, SIGNAL( activated() ), this, SLOT( slotFileClose() ) );

  act_save = new QAction( QPixmap( mod_save ), "&Save model", 
      CTRL+Key_S, this, "save");
  act_save->setWhatsThis( tr("Click this button to save the model file you are "
                  "editing. You will be prompted for a file name." ) );
  connect( act_save, SIGNAL( activated() ), this, SLOT( slotFileSave() ) );

  act_saveas = new QAction( "Save &As", 0, this, "saveas");
  act_saveas->setWhatsThis( tr("Save current model with another filename") );
  connect( act_saveas, SIGNAL( activated() ), this, SLOT( slotFileSaveAs() ) );

  act_print = new QAction( "&Print", CTRL+Key_P, this, "print");
  act_print->setWhatsThis( tr("Print current model") );
  connect( act_print, SIGNAL( activated() ), this, SLOT( slotFilePrint() ) );

  act_settings = new QAction( "Sett&ings", 0, this, "settings");
  act_settings ->setWhatsThis( "Edit application settings" );
  connect( act_settings, SIGNAL( activated() ), this, SLOT( slotFileSettings() ) );
  
  act_savesett = new QAction( "Save s&ettings", 0, this, "savesett");
  act_savesett ->setWhatsThis( "Save application settings" );
  connect( act_savesett, SIGNAL( activated() ), this, SLOT( slotFileSaveSett() ) );


  act_quit = new QAction( QPixmap( exit_icon ), "&Quit", 
      CTRL+Key_Q, this, "quit");
  act_quit->setWhatsThis( tr("Click this button to quit application.") );
  connect( act_quit, SIGNAL( activated() ), this, SLOT( slotFileQuit() ) );

  // ==== Edit group
 
  act_undo = new QAction( "&Undo", CTRL+Key_Z, this, "undo");
  act_undo->setWhatsThis( tr("Undo last action") );
  connect( act_undo, SIGNAL( activated() ), this, SLOT( slotEditUndo() ) );
 
  act_cut = new QAction( "Cut", CTRL+Key_Delete, this, "cut");
  act_cut->setWhatsThis( tr("Cut selected") );
  connect( act_cut, SIGNAL( activated() ), this, SLOT( slotEditCut() ) );
 
  act_copy = new QAction( "Copy", CTRL+Key_Insert, this, "copy");
  act_copy->setWhatsThis( tr("Copy selected") );
  connect( act_copy, SIGNAL( activated() ), this, SLOT( slotEditCopy() ) );
 
  act_paste= new QAction( "Paste", SHIFT+Key_Insert, this, "paste");
  act_paste->setWhatsThis( tr("Paste selected") );
  connect( act_paste, SIGNAL( activated() ), this, SLOT( slotEditPaste() ) );

  // ==== Element group
 
  act_newelm= new QAction( QPixmap( newelm_icon ), "&New element", 
      Key_Insert, this, "newelm");
  act_newelm->setWhatsThis( tr("Create new element") );
  connect( act_newelm, SIGNAL( activated() ), this, SLOT( slotNewElm() ) );
 
  act_delelm= new QAction( QPixmap( delelm_icon ), "&Delete element",
      Key_Delete, this, "delelm");
  act_delelm->setWhatsThis( tr("Delete selected element") );
  connect( act_delelm, SIGNAL( activated() ), this, SLOT( slotDelElm() ) );
 
  act_editelm= new QAction( QPixmap( editelm_icon ), "&Edit element",
      Key_Enter, this, "editelm");
  act_editelm->setWhatsThis( tr("Edit selected element") );
  connect( act_editelm, SIGNAL( activated() ), this, SLOT( slotEditElm() ));
 
  act_linkelm= new QAction( QPixmap( linkelm_icon ), "&Link element",
      Key_L, this, "linkelm");
  act_linkelm->setWhatsThis( tr("Edit links of selected element") );
  connect( act_linkelm, SIGNAL( activated() ), this, SLOT( slotLinkElm() ));
 
  act_qlinkelm= new QAction( "&Quick link element", 
      CTRL+Key_L, this, "qlinkelm");
  act_qlinkelm->setWhatsThis( tr("Link marked to selected element") );
  connect( act_qlinkelm, SIGNAL( activated() ), this, SLOT( slotqLinkElm() ));
 
  act_qplinkelm= new QAction( "&Parametr link element", 
      SHIFT+CTRL+Key_L, this, "qplinkelm");
  act_qplinkelm->setWhatsThis( tr("Link marked to selected element "
	                          "(parameter input)") );
  connect( act_qplinkelm, SIGNAL( activated() ), this, SLOT( slotqpLinkElm()));
 
  act_unlinkelm= new QAction( "&Unlink element", 0, this, "unlinkelm");
  act_unlinkelm->setWhatsThis( tr("Remove links of selected element") );
  connect( act_unlinkelm, SIGNAL( activated() ), this, SLOT( slotUnlinkElm() ));
 
  act_lockelm= new QAction( QPixmap( lockelm_icon ), "Loc&k element", 
      CTRL+Key_K, this, "lockelm");
  act_lockelm->setWhatsThis( tr("Lock current element") );
  connect( act_lockelm, SIGNAL( activated() ), this, SLOT( slotLockElm() ));
 
  act_ordelm= new QAction( QPixmap( orderelm_icon ), "Change &Orger", 
      Key_O, this, "ordelm");
  act_ordelm->setWhatsThis( tr("Change order numper of selected element") );
  connect( act_ordelm, SIGNAL( activated() ), this, SLOT( slotOrdElm() ));
 
  act_markelm= new QAction( QPixmap( markelm_icon ), "&Mark element", 
      Key_M, this, "markelm");
  act_ordelm->setWhatsThis( tr("Mark selected element") );
  connect( act_markelm, SIGNAL( activated() ), this, SLOT( slotMarkElm() ));
 
  act_moveelm= new QAction( "Move element", SHIFT+Key_M, this, "moveelm");
  act_moveelm->setWhatsThis( tr("Move marked element to selected cell") );
  connect( act_moveelm, SIGNAL( activated() ), this, SLOT( slotMoveElm() ));
 
  act_infoelm= new QAction( QPixmap( infoelm_icon ), "show &Info",
      Key_I, this, "infoelm");
  act_infoelm->setWhatsThis( tr("Show information about element structure") );
  connect( act_infoelm, SIGNAL( activated() ), this, SLOT( slotInfoElm() ));
 
  // ==== out group 
 
  act_newout = new QAction( QPixmap( newout_icon ), "&New Out",
      Key_U, this, "newout");
  act_newout->setWhatsThis( tr("Create output collector") );
  connect( act_newout, SIGNAL( activated() ), this, SLOT( slotNewOut() ));

  act_delout = new QAction( QPixmap( delout_icon ), "&Delete out", 
      Key_X, this, "delout");
  act_delout->setWhatsThis( tr("Delete output collector with current level") );
  connect( act_delout, SIGNAL( activated() ), this, SLOT( slotDelOut() ));

  act_editout = new QAction( QPixmap( editout_icon ), "&Edit out",
      SHIFT+Key_U, this, "editout");
  act_editout->setWhatsThis( tr("Edit outsput collector withcurrent level") );
  connect( act_editout, SIGNAL( activated() ), this, SLOT( slotEditOut() ));

  act_showoutdata = new QAction( QPixmap( showoutdata_icon ), "&Show out data",
      Key_D, this, "showoutdata");
  act_showoutdata->setWhatsThis( tr("Show data collected by output.") );
  connect( act_showoutdata, SIGNAL( activated() ), this, SLOT( slotShowOutData() ));

  act_exportout = new QAction( "E&xport out data", Key_E, this, "exportout");
  act_exportout->setWhatsThis( tr("Export data collected by output to text file.") );
  connect( act_exportout, SIGNAL( activated() ), this, SLOT( slotExportOut() ));

  // ==== graph group

  act_newgraph = new QAction( QPixmap( newgraph_icon ), "&New Graph",
      Key_G, this, "newgraph");
  act_newgraph->setWhatsThis( tr("Create new graph") );
  connect( act_newgraph, SIGNAL( activated() ), this, SLOT( slotNewGraph()));

  act_delgraph = new QAction( QPixmap( delgraph_icon ), "&Delete graph",
      SHIFT+Key_X, this, "delgraph");
  act_delgraph->setWhatsThis( tr("Delete graph with selected level") );
  connect( act_delgraph, SIGNAL( activated() ), this, SLOT( slotDelGraph()));

  act_editgraph = new QAction( QPixmap( editgraph_icon ), "&Edit graph",
      SHIFT+Key_G, this, "editgraph");
  act_editgraph->setWhatsThis( tr("Edit graph with current level") );
  connect( act_editgraph, SIGNAL( activated() ), this, SLOT( slotEditGraph() ));

  act_showgraph = new QAction( QPixmap( showgraph_icon ), "&Show graph",
      Key_S, this, "showgraph");
  act_showgraph->setWhatsThis( tr("Show graph plot") );
  connect( act_showgraph, SIGNAL( activated() ), this, SLOT( slotShowGraph() ));

  act_showgraphdata = new QAction( QPixmap( showgraphdata_icon ),
      "show graph Data", SHIFT+Key_D, this, "showgraphdata");
  act_showgraphdata->setWhatsThis( tr("Show graph data") );
  connect( act_showgraphdata, SIGNAL( activated() ), this, SLOT( slotShowGraphData() ));

  act_exportgraphdata = new QAction( "E&xport graph data",
      SHIFT+Key_E, this, "exportgraphdata");
  act_exportgraphdata->setWhatsThis( tr("Export graph data to text file") );
  connect( act_exportgraphdata, SIGNAL( activated() ), this, SLOT( slotExportGraphData() ));

  act_gnuplotgraph = new QAction( "&Gnuplot graph", 0, this, "gnulpotgraph");
  act_gnuplotgraph->setWhatsThis( tr("Export graph data to gnuplot-compatiomle file") );
  connect( act_gnuplotgraph, SIGNAL( activated() ), this, SLOT( slotGnuplotGraph() ));

  // ==== model group

  act_editmodel = new QAction( QPixmap( editmodel_icon ), "&Edit Model",
      CTRL+Key_Enter, this, "editmodel");
  act_editmodel->setWhatsThis( tr("Edit model parameters.") );
  connect( act_editmodel, SIGNAL( activated() ), this, SLOT( slotEditModel()));

  act_showvars = new QAction( "&Show model vars", 
      SHIFT+CTRL+Key_D, this, "showvars");
  act_showvars->setWhatsThis( tr("Show model's vars field") );
  connect( act_showvars, SIGNAL( activated() ), this, SLOT( slotShowVars()));

  // ====  run group

  act_runrun = new QAction( QPixmap( run_icon ), "&Run", 
      Key_F9, this, "runrun");
  act_runrun->setWhatsThis( tr("Click this button start simple run.") );
  connect( act_runrun, SIGNAL( activated() ), this, SLOT( slotRunRun()) );
  
  act_runprm= new QAction( QPixmap( run_p1 ), "Run &1D Parm loop",
      CTRL+Key_F9, this, "runrpm");
  act_runprm->setWhatsThis( tr("Click this button start 1D parametric run.") );
  connect( act_runprm, SIGNAL( activated() ), this, SLOT( slotRunPrm()) );

  act_runprm2= new QAction( QPixmap( run_p2 ), "Run &2D Parm loop", 
      SHIFT+CTRL+Key_F9, this, "runrpm2");
  act_runprm2->setWhatsThis( tr("Click this button start 2D parametric run.") );
  connect( act_runprm2, SIGNAL( activated() ), this, SLOT( slotRunPrm2()) );

  act_reset= new QAction( "R&eset", Key_R, this, "reset");
  act_reset->setWhatsThis( tr("Reset model state.") );
  connect( act_reset, SIGNAL( activated() ), this, SLOT( slotReset()) );

  // ==== iface group

  act_tbar = new QAction( "View &Toolbar", 0, this, "tbar");
  act_tbar->setToggleAction( true );
  act_tbar->setOn( true );
  // act_tbar->setWhatsThis( tr("") );
  connect( act_tbar, SIGNAL( activated() ), this, SLOT( slotViewToolBar()) );

  act_sbar = new QAction( "View &Statusbar", 0, this, "sbar" );
  act_sbar->setToggleAction( true );
  act_sbar->setOn( true );
  // act_sbar->setWhatsThis( tr("") );
  connect( act_sbar, SIGNAL( activated() ), this, SLOT( slotViewStatusBar()) );

  act_showord = new QAction( "Show &Orders", 0, this, "showord" );
  act_showord->setToggleAction( true );
  act_showord->setOn( sett.showord );
  connect( act_showord, SIGNAL( activated() ), this, SLOT( slotShowOrd()) );

  act_showgrid = new QAction( "Show &Grid", 0, this, "showgrid" );
  act_showgrid->setToggleAction( true );
  act_showgrid->setOn( sett.showgrid );
  connect( act_showgrid, SIGNAL( activated() ), this, SLOT( slotShowGrid()) );

  act_shownames = new QAction( "Show &Names", 0, this, "shownames" );
  act_shownames->setToggleAction( true );
  act_shownames->setOn( sett.shownames );
  connect( act_shownames, SIGNAL( activated() ), this, SLOT( slotShowNames()) );

  act_showicons = new QAction( "Show &Icons", 0, this, "showicons" );
  act_showicons->setToggleAction( true );
  act_showicons->setOn( sett.showicons );
  connect( act_showicons, SIGNAL( activated() ), this, SLOT( slotShowIcons()) );

  // ==== window group

  act_winnew = new QAction( "&New Window", 0, this, "winnew");
  act_winnew->setWhatsThis( tr("Crerate new window for same model") );
  connect( act_winnew, SIGNAL( activated() ), this, SLOT( slotWindowNewWindow()) );

  // ==== help group

  act_helpabout = new QAction( "&About", 0, this, "helpabout");
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpabout, SIGNAL( activated() ), this, SLOT( slotHelpAbout()) );

  act_helpaboutqt = new QAction( "About &Qt", 0, this, "helpaboutqt");
  // act_helpabout->setWhatsThis( tr("") );
  connect( act_helpaboutqt, SIGNAL( activated() ), this, SLOT( slotHelpAboutQt()) );
  
  act_whatsthis = new QAction( "&What's This", 0, this, "whatsthis");
  // act_whatsthis->setWhatsThis( tr("") );
  // TODO:
  //connect( act_whatsthis, SIGNAL( activated() ), this, SLOT( slotWhatsThis()) );

  act_test = new QAction( QPixmap( test_icon ), "&Test", 0, this, "test");
  act_test->setWhatsThis( tr("Click this button to test something.") );
  connect( act_test, SIGNAL( activated() ), this, SLOT( slotTest()) );

  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pFileMenu
		
  pFileMenu = new QPopupMenu();
  act_new->addTo( pFileMenu );
  act_open->addTo( pFileMenu );
  act_close->addTo( pFileMenu );
    pFileMenu->insertSeparator();
  act_save->addTo( pFileMenu );
  act_saveas->addTo( pFileMenu );
    pFileMenu->insertSeparator();
  act_print->addTo( pFileMenu );
    pFileMenu->insertSeparator();
  act_settings->addTo( pFileMenu );
  act_savesett->addTo( pFileMenu );
    pFileMenu->insertSeparator();
  act_quit->addTo( pFileMenu );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pEditMenu
  pEditMenu = new QPopupMenu();
  act_undo->addTo( pEditMenu );
    pEditMenu->insertSeparator();
  act_cut->addTo( pEditMenu );
  act_copy->addTo( pEditMenu );
  act_paste->addTo( pEditMenu );
    pEditMenu->insertSeparator();
  act_test->addTo( pEditMenu );
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pElmMenu
  pElmMenu = new QPopupMenu();
  act_newelm->addTo( pElmMenu );
  act_delelm->addTo( pElmMenu );
  act_editelm->addTo( pElmMenu );
    pElmMenu->insertSeparator();
  act_linkelm->addTo( pElmMenu );
  act_qlinkelm->addTo( pElmMenu );
  act_qplinkelm->addTo( pElmMenu );
  act_unlinkelm->addTo( pElmMenu );
    pElmMenu->insertSeparator();
  act_lockelm->addTo( pElmMenu );
  act_ordelm->addTo( pElmMenu );
  act_markelm->addTo( pElmMenu );
  act_moveelm->addTo( pElmMenu );
    pElmMenu->insertSeparator();
  act_infoelm->addTo( pElmMenu );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pOutMenu
  pOutMenu = new QPopupMenu();
  act_newout->addTo( pOutMenu );
  act_delout->addTo( pOutMenu );
  act_editout->addTo( pOutMenu );
    pOutMenu->insertSeparator();
  act_showoutdata->addTo( pOutMenu );
  act_exportout->addTo( pOutMenu );

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pGraphMenu
  pGraphMenu = new QPopupMenu();
  act_newgraph->addTo( pGraphMenu );
  act_delgraph->addTo( pGraphMenu );
  act_editgraph->addTo( pGraphMenu );
    pGraphMenu->insertSeparator();
  act_showgraph->addTo( pGraphMenu );
    pGraphMenu->insertSeparator();
  act_showgraphdata->addTo( pGraphMenu );
  act_exportgraphdata->addTo( pGraphMenu );
  act_gnuplotgraph->addTo( pGraphMenu );
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pModelMenu
  pModelMenu = new QPopupMenu();
  act_editmodel->addTo( pModelMenu );
    pModelMenu->insertSeparator();
  act_showvars->addTo( pModelMenu );
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pRunMenu
  pRunMenu = new QPopupMenu();
  act_runrun->addTo( pRunMenu );
  act_runprm->addTo( pRunMenu );
  act_runprm2->addTo( pRunMenu );
    pRunMenu->insertSeparator();
  act_reset->addTo( pRunMenu );
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry pViewMenu
  pViewMenu = new QPopupMenu();
  // pViewMenu->setCheckable(true);
  act_tbar->addTo( pViewMenu );
  act_sbar->addTo( pViewMenu );
    pViewMenu->insertSeparator();
  act_showord->addTo( pViewMenu );
  act_showgrid->addTo( pViewMenu );
  act_shownames->addTo( pViewMenu );
  act_showicons->addTo( pViewMenu );


  ///////////////////////////////////////////////////////////////////
  // menuBar entry window-Menu
  pWindowMenu = new QPopupMenu(this);
  pWindowMenu->setCheckable(true);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry pHelpMenu
  pHelpMenu = new QPopupMenu();
  act_helpabout->addTo( pHelpMenu );
  act_helpaboutqt->addTo( pHelpMenu );
  pHelpMenu->insertSeparator();
  act_whatsthis->addTo( pHelpMenu );

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  // set menuBar() the current menuBar 

  menuBar()->insertItem( tr( "&File"), pFileMenu);
  menuBar()->insertItem( tr( "&Edit"), pEditMenu);
  menuBar()->insertItem( tr( "E&lement"), pElmMenu);
  menuBar()->insertItem( tr( "&Out"), pOutMenu);
  menuBar()->insertItem( tr( "&Graph"), pGraphMenu);
  menuBar()->insertItem( tr( "&Model"), pModelMenu);
  menuBar()->insertItem( tr( "&Run"), pRunMenu);
  menuBar()->insertItem( tr( "&View"), pViewMenu);
  menuBar()->insertItem( tr( "&Window"), pWindowMenu);
  menuBar()->insertItem( tr( "&Help"), pHelpMenu);
  
  ///////////////////////////////////////////////////////////////////
  // CONNECT THE SUBMENU SLOTS WITH SIGNALS

  connect( pWindowMenu, SIGNAL(aboutToShow() ), SLOT( windowMenuAboutToShow() ) );

  ///////////////////////////////////////////////////////////////////
  // TOOLBARS

  fileToolbar = new QToolBar( this, "file operations toolbar" );
  act_new->addTo( fileToolbar );
  act_open->addTo( fileToolbar );
  act_save->addTo( fileToolbar );
    fileToolbar->addSeparator();
  act_test->addTo( fileToolbar );
  
  elmToolbar = new QToolBar( this, "model operations toolbar" );
  act_newelm->addTo( elmToolbar );
  act_delelm->addTo( elmToolbar );
  act_editelm->addTo( elmToolbar );
  act_linkelm->addTo( elmToolbar );
  // act_lockelm->addTo( elmToolbar );
  act_ordelm->addTo( elmToolbar );
  act_markelm->addTo( elmToolbar );
  act_infoelm->addTo( elmToolbar );
    elmToolbar->addSeparator();
  act_newout->addTo( elmToolbar );
  act_delout->addTo( elmToolbar );
  act_editout->addTo( elmToolbar );
  act_showoutdata->addTo( elmToolbar );
    elmToolbar->addSeparator();
  act_newgraph->addTo( elmToolbar );
  act_delgraph->addTo( elmToolbar );
  act_editgraph->addTo( elmToolbar );
  act_showgraph->addTo( elmToolbar );
  act_showgraphdata->addTo( elmToolbar );
    elmToolbar->addSeparator();
  act_editmodel->addTo( elmToolbar );
    elmToolbar->addSeparator();
  act_runrun->addTo( elmToolbar );
  act_runprm->addTo( elmToolbar );
  act_runprm2->addTo( elmToolbar );

}

void QMo2Win::initStatusBar()
{
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // set the main widget here
  QVBox* view_back = new QVBox( this );
  view_back->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  pWorkspace = new QWorkspace( view_back );
  connect( pWorkspace, SIGNAL(windowActivated(QWidget*)), 
           this, SLOT(setWndTitle(QWidget*)) );
  setCentralWidget( view_back );
  enableActions( false, 0 );
}

void QMo2Win::enableActions( bool ena, int id_ )
{
  switch( id_ ) {
    case 0: // model-specific actions
       act_close->setEnabled( ena );
       act_save->setEnabled( ena );
       act_saveas->setEnabled( ena );
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
       // iface
       act_winnew->setEnabled( ena );
       break;
    default: break;
  };
}

void QMo2Win::setWndTitle( QWidget* )
{
  if( pWorkspace->activeWindow() != 0 ) {
    setCaption( pWorkspace->activeWindow()->caption() + " " PACKAGE );
    enableActions( true, 0 );
  } else {
    setCaption( PACKAGE " " VERSION ); 
    enableActions( false, 0 );
  };
}

void QMo2Win::closeEvent ( QCloseEvent *e )
{
  QWidget *cw;  
  QWidgetList wl = pWorkspace->windowList();
  for( cw=wl.first(); cw != 0; cw=wl.next() ) {
    if( (cw->close( true )) == false ) {
      e->ignore();
      return;
    };
  };
  e->accept();
}

void QMo2Win::createClient( QMo2Doc* doc )
{
  QMo2View* w = new QMo2View( doc, pWorkspace, 0, WDestructiveClose );
  // w->installEventFilter( this );
  doc->addView( w );
  w->setIcon( model_icon );
  w->show();
}

void QMo2Win::openDocumentFile( const char* file )
{
  statusBar()->message( tr("Opening file...") );
  QWidget *cw;  
  QMo2Doc* doc;
  // check, if document already open. If yes, set the focus to the first view
  QWidgetList wl = pWorkspace->windowList();
  for( cw=wl.first(); cw != 0; cw=wl.next() ) {
    if( ! ( cw->isA( "QMo2View" ) ) ) 
      continue;
    doc = (static_cast<QMo2View*>(cw))->getDocument();
    if( doc->pathName() == file ) {
      QMo2View* view = doc->firstView();	
      view->setFocus();
      return;
    };
  };
  doc = new QMo2Doc();
  // Creates an untitled window if file is 0	
  if( !file ) {
    untitledCount += 1;
    QString fileName = QString( "Untitled%1" ).arg(untitledCount);
    doc->newDocument();
    doc->setPathName(fileName);
    doc->setTitle(fileName);
  } else { // Open the file
    if( ! doc->openDocument( file ) ) {
      QMessageBox::critical( this, tr("Error !"), 
                             tr("Could not open document !" ) );
      delete doc;
      return;
    };
  };
  // create the window
  createClient( doc );
  enableActions( true, 0 );

  statusBar()->message( tr( "Ready." ) );
}

bool QMo2Win::queryExit()
{
  int exit = QMessageBox::information( this, 
      tr("Quit..."), tr("Do your really want to quit?"),
      QMessageBox::Ok, QMessageBox::Cancel);

  //  if(exit==1) {
  //
  //  } else {
  //
  //  };

  return ( exit==1 );
}

bool QMo2Win::eventFilter( QObject* object, QEvent* event )
{
  return QMainWindow::eventFilter( object, event ); // standard event processing
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void QMo2Win::slotFileNew()
{
  statusBar()->message( tr( "Creating new model file..." ) );
  openDocumentFile();		
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotFileOpen()
{
  statusBar()->message( tr( "Opening model file..." ) );
  QString fileName = 
    QFileDialog::getOpenFileName( 0, "Model files (*.mo2);;All files(*)", this );
  if ( ! fileName.isEmpty() ) {
     openDocumentFile( fileName );
  };
  statusBar()->message( tr( "Ready." ) );
}


void QMo2Win::slotFileSave()
{
  statusBar()->message( tr( "Saving model file...") );	
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
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
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotFileSaveAs()
{
  statusBar()->message( tr ( "Saving model file under new filename..." ) );
  QString fn = QFileDialog::getSaveFileName( QString::null,
               "Model files (*.mo2);;All files(*)", this );
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
       statusBar()->message( tr( "Ready." ) );
       return;
    }
    QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
    if( m ) {
      QMo2Doc* doc = m->getDocument();
      if( ! doc->saveDocument( fn ) ) {
         QMessageBox::critical ( this, 
	     tr("I/O Error !"), tr("Could not save the current model file!") );
         return;
      };
      doc->changedViewList();
      setWndTitle( m );
    };
  };
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotFileClose()
{
  statusBar()->message( tr ( "Closing model file..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if( m ) {
    m->close();
  };
  if ( pWorkspace->windowList().isEmpty() ) {
    enableActions( false, 0 );
  };
  statusBar()->message( tr ( "Ready." ) );
}

void QMo2Win::slotFilePrint()
{
  statusBar()->message( tr ( "Printing..." ) );	
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m != 0 )
    m->print();
  statusBar()->message( tr ( "Ready." ) );
}

void QMo2Win::slotFileSettings()
{
  statusBar()->message( tr ( "Edit settings..." ) );	
  
  Mo2SettDlg *dia = new Mo2SettDlg( sett, this );
  if ( dia->exec() == QDialog::Accepted ) {
    QFont mf;
    mf.fromString( sett.mainFont );
    QApplication::setFont( mf );
  };
  
  statusBar()->message( tr ( "Ready." ) );
}


void QMo2Win::slotFileSaveSett()
{
  statusBar()->message( tr ( "Saving settings..." ) );	
  sett.save();
  statusBar()->message( tr ( "Ready." ) );
}

void QMo2Win::slotFileQuit()
{ 
  QWidget *cw;  
  statusBar()->message( tr( "Exiting application..." ) );
  QWidgetList wl = pWorkspace->windowList();
  for( cw=wl.first(); cw != 0; cw=wl.next() ) {
    if( (cw->close( true )) == false ) {
      statusBar()->message( tr ( "Ready." ) );
      return;
    };
  };
  qApp->quit();
  statusBar()->message( tr ( "Ready." ) );
}

void QMo2Win::slotEditUndo()
{
  statusBar()->message( tr( "Reverting last action..." ) );	
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->undo();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditCut()
{
  statusBar()->message( tr( "Cutting selection..." ) );	
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->cut();	
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditCopy()
{
  statusBar()->message( tr( "Copying selection to clipboard..." ) );

//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->copy();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditPaste()
{
  statusBar()->message( tr( "Inserting clipboard contents..." ) );
	
//  QMo2View* m = (QMo2View*) pWorkspace->activeWindow();
//  if ( m )
//    m->paste();

  statusBar()->message( tr( "Ready." ) );
}


void QMo2Win::slotViewToolBar()
{
  statusBar()->message( tr( "Toggle toolbar..." ) );
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  
  if (fileToolbar->isVisible()) {
    fileToolbar->hide();
    elmToolbar->hide();
    act_tbar->setOn( false );
  } else {
    fileToolbar->show();
    elmToolbar->show();
    act_tbar->setOn( true );
  };
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotViewStatusBar()
{
  statusBar()->message( tr( "Toggle statusbar..." ) );
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  
  if (statusBar()->isVisible()) {
    statusBar()->hide();
    act_sbar->setOn( false );
  } else {
    statusBar()->show();
    act_sbar->setOn( true );
  };
  
  statusBar()->message( tr( "Ready." ) );
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

void QMo2Win::slotWindowNewWindow()
{
  statusBar()->message( tr( "Opening new document view..." ) );
	
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m ) {
    QMo2Doc* doc = m->getDocument();
    createClient( doc );
  };

  statusBar()->message( tr( "Ready." ) );
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
  statusBar()->message( text, 2000 );
}

void QMo2Win::slotTest(void)
{
  static const char *loc_test = 
    "áâ÷çäåöúéëìíîïðòóôõæèãþûýùÿøüàñ:"
    "ÁÂ×ÇÄÅÖÚÉËÌÍÎÏÐÒÓÔÕÆÈÃÞÛÝÙßØÜÀÑ:¦¶§·¤´£³ ";
  QString ostr( "Test called\n" );
  statusBar()->message( tr( "Test something..." ) );
  ostr += QString::fromLocal8Bit( loc_test ) + "(Local8bit)\n" ;
  ostr += QString::fromAscii( loc_test ) + "(Ascii)\n";
  ostr += loc_test;
  ostr += "(simple)\n";
  ostr += QChar(0x03C0);
  ostr += QChar(0x03C9);
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
  QMessageBox::information( this, tr( "Test" ), ostr, QMessageBox::Ok );
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::windowMenuAboutToShow()
{
  pWindowMenu->clear();	
  act_winnew->addTo( pWindowMenu );
  pWindowMenu->insertItem( tr( "&Cascade"), pWorkspace, SLOT(cascade() ),0 , ID_WINDOW_CASCADE );
  pWindowMenu->insertItem( tr( "&Tile"), pWorkspace, SLOT(tile() ),0 , ID_WINDOW_TILE );
	
  if ( pWorkspace->windowList().isEmpty() ) {
    enableActions( false, 0 );
  };

  pWindowMenu->insertSeparator();

  QWidgetList windows = pWorkspace->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
    int id = pWindowMenu->insertItem(QString("&%1 ").arg(i+1) + 
         windows.at(i)->caption(), this, SLOT( windowMenuActivated( int ) ) );
    pWindowMenu->setItemParameter( id, i );
    pWindowMenu->setItemChecked( id, 
              pWorkspace->activeWindow() == windows.at(i) );
  };
}

void QMo2Win::windowMenuActivated( int id )
{
  QWidget* w = pWorkspace->windowList().at( id );
  if ( w )
    w->setFocus();
}



void QMo2Win::slotNewElm()
{
  statusBar()->message( tr( "Inserting new element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->newElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotDelElm()
{
  statusBar()->message( tr( "Deleting selected element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->delElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditElm()
{
  statusBar()->message( tr( "Editing element properties..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->editElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotLinkElm()
{
  statusBar()->message( tr( "Linking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->linkElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotqLinkElm()
{
  statusBar()->message( tr( "Quick linking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->qlinkElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotqpLinkElm()
{
  statusBar()->message( tr( "Quick parametr linking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->qplinkElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotUnlinkElm()
{
  statusBar()->message( tr( "Uninking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->unlinkElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotLockElm()
{
  statusBar()->message( tr( "Locking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->lockElm();
  statusBar()->message( tr( "Ready." ) );
}


void QMo2Win::slotOrdElm()
{
  statusBar()->message( tr( "Setting new order of element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->ordElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotMarkElm()
{
  statusBar()->message( tr( "Marking element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->markElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotMoveElm()
{
  statusBar()->message( tr( "Moving element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->moveElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotInfoElm()
{
  statusBar()->message( tr( "Information about element..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->infoElm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotNewOut()
{
  statusBar()->message( tr( "Inserting output array..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->newOut();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotDelOut()
{
  statusBar()->message( tr( "Deleting output array..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->delOut();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditOut()
{
  statusBar()->message( tr( "Editing output array..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->editOut();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotShowOutData()
{
  statusBar()->message( tr( "Data from output array..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->showOutData();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotExportOut()
{
  statusBar()->message( tr( "Exporting data from output array..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->exportOut();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotNewGraph()
{
  statusBar()->message( tr( "Inserting new graph..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->newGraph();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotDelGraph()
{
  statusBar()->message( tr( "Deleting graph..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->delGraph();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditGraph()
{
  statusBar()->message( tr( "Editing graph..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->editGraph();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotShowGraph()
{
  statusBar()->message( tr( "Showing graph plot..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->showGraph();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotShowGraphData()
{
  statusBar()->message( tr( "Showing graph data..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->showGraphData();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotExportGraphData()
{
  statusBar()->message( tr( "Exporting graph data..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->exportGraphData();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotGnuplotGraph()
{
  statusBar()->message( tr( "Exporting graph to gnuplot..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->gnuplotGraph();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotEditModel()
{
  statusBar()->message( tr( "Editing model parametrs..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->editModel();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotShowVars()
{
  statusBar()->message( tr( "Showing model vars..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->showVars();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotRunRun()
{
  statusBar()->message( tr( "Running simple loop..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->runRun();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotRunPrm()
{
  statusBar()->message( tr( "Running parametric loop..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->runPrm();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotRunPrm2()
{
  statusBar()->message( tr( "Running 2D parametric loop..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->runPrm2();
  statusBar()->message( tr( "Ready." ) );
}

void QMo2Win::slotReset()
{
  statusBar()->message( tr( "Reseting model..." ) );
  QMo2View* m = static_cast<QMo2View*>( pWorkspace->activeWindow() );
  if ( m )
    m->resetModel();
  statusBar()->message( tr( "Ready." ) );
}

// =============================== Mo2Settings =============================

Mo2Settings::Mo2Settings()
{
  load();
}

void Mo2Settings::load()
{
  QSettings sets;
  sets.setPath("atu.localnet","qmo2");
  sets.beginGroup("/qmo2");
  /* -------- flags ------------ */
  showord = sets.readBoolEntry("/view/showord", false );
  showgrid = sets.readBoolEntry("/view/showgrid", true );
  shownames = sets.readBoolEntry("/view/shownames", false );
  showicons = sets.readBoolEntry("/view/showicons", true );
  showmax = sets.readBoolEntry("/view/showmax", true );
  /* -------- fonts ----------- */
  //  "Arial,10,-1,5,50,0,0,0,0,0" 
  QFont df;
  QString dfs = df.toString();
  mainFont = sets.readEntry("/fonts/main", dfs );
  smallFont = sets.readEntry("/fonts/small", "Arial,7" );
  plotFont = sets.readEntry("/fonts/plot", "Arial,8" );
  structFont = sets.readEntry("/fonts/struct", "Arial,8" );
}

void Mo2Settings::save() const
{
  QSettings sets;
  sets.setPath( "atu.localnet", "qmo2" );
  sets.beginGroup("/qmo2");
  /* ---------- flags ------------- */
  sets.writeEntry( "/view/showord", showord );
  sets.writeEntry( "/view/showgrid", showgrid );
  sets.writeEntry( "/view/shownames", shownames );
  sets.writeEntry( "/view/showicons", showicons );
  sets.writeEntry( "/view/showmax", showmax );
  /* ---------- fonts ------------- */
  sets.writeEntry( "/fonts/main", mainFont );
  sets.writeEntry( "/fonts/small", smallFont );
  sets.writeEntry( "/fonts/plot", plotFont );
  sets.writeEntry( "/fonts/struct", structFont );

  sets.endGroup();

}

// end of qmo2win.cpp

