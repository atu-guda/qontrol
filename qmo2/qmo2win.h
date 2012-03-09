/***************************************************************************
                          qmo2win.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000 by atu
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

#ifndef QMO2WIN_H
#define QMO2WIN_H

// include files for QT
#include <qapp.h>
#include <qmainwindow.h>
#include <qworkspace.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>
#include <qaccel.h>
#include <qaction.h>
#include <qtoolbutton.h>
#include <qmsgbox.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qstring.h>
#include <qpixmap.h>

// forward declaration of the QMo2Win classes
class QMo2Doc;
class QMo2View;

/** auxilary class for storing QMo2 application settings */
class Mo2Settings {
  public:
   Mo2Settings();
   void load();
   void save() const;
  public: 
   /** flag: show element order: /view/showord */
   bool showord;
   /** flag: show grid in structure view: /view/showgrid */
   bool showgrid;
   /** flag: show element name: /view/shownames */
   bool shownames;
   /** flag: show element icon: /view/showicon */
   bool showicons;
   /** flag: maximize main window: /view/showmax */
   bool showmax;
   /** main font string: /fonts/main */
   QString mainFont;
   /** small font string: /fonts/small */
   QString smallFont;
   /** plot font string:  /fonts/plot */
   QString plotFont;
   /** structure view font string: /fonts/struct */
   QString structFont;
    
};

/**
  * QMo2Win is the main window class for application. It sets up the main
  * window and providing a menubar, toolbar
  * and statusbar. For the main view, an instance of class QMo2View is
  * created which creates your view.
  * 	
  * @author atu, based on kdevelop template
  */
class QMo2Win : public QMainWindow
{
  Q_OBJECT
  public:
    /** construtor */
    QMo2Win(void);
    /** destructor */
    ~QMo2Win();

    /** enables/disables menu entries/toolbar items */
    void enableActions( bool ena, int id_ );
    /** opens a file specified by commandline option */
    void openDocumentFile( const char *file = 0 );
    /** returns apps printer */
    QPrinter* getPrinter(void) { return printer; };
    /** finds resource file among resource dirs */
    QString findRes( const QString &res );
    /** returns pointer to settings object */
    Mo2Settings* getSettings() { return &sett; };

  protected:
    /** overloaded for Message box on last window exit */
    bool queryExit();
    /** event filter to catch close events for MDI child windows and is 
      * installed in createClient() on every child window.
      * Closing a window calls the eventFilter first which removes the 
      * view from the connected documents' view list. If the
      * last view is going to be closed, the eventFilter() tests 
      * if the document is modified; if yes, it asks the user to
      * save the document. If the document title contains "Untitled", 
      * slotFileSaveAs() gets called to get a save name and path.
    */
    virtual bool eventFilter( QObject* object, QEvent* event );
    /** creates a new child window. The document that will be connected to it
     * has to be created before and the instances filled, with e.g. openDocument().
     * Then call createClient() to get a new MDI child window.
     * @see QMo2Doc#addView
     * @see QMo2Doc#openDocument
     * @param doc pointer to the document instance that the view will
     * be connected to.
     */
     void createClient(QMo2Doc* doc);

  private slots:
  
    /** sets the main application window title each time 
      * the active MDI child window changes. */
    void setWndTitle( QWidget* );

    /** generate a new document in the actual view */
    void slotFileNew();
    /** open a document */
    void slotFileOpen();
    /** save a document */
    void slotFileSave();
    /** save a document under a different filename*/
    void slotFileSaveAs();
    /** close the actual file */
    void slotFileClose();
    /** print the actual file */
    void slotFilePrint();
    /** Edit application settings */
    void slotFileSettings();
    /** saves application settings */
    void slotFileSaveSett();
    /** exits the application */
    void slotFileQuit();

    /** reverts the last editing action */
    void slotEditUndo();
    /** put the marked text/object into the clipboard and remove
     * it from the document */
    void slotEditCut();
    /** put the marked text/object into the clipboard*/
    void slotEditCopy();
    /** paste the clipboard into the document*/
    void slotEditPaste();
    // atu start:
    
    // ==== element related
    void slotNewElm();
    void slotDelElm();
    void slotEditElm();
    void slotLinkElm();
    void slotqLinkElm();
    void slotqpLinkElm();
    void slotUnlinkElm();
    void slotLockElm();
    void slotOrdElm();
    void slotMarkElm();
    void slotMoveElm();
    void slotInfoElm();

    // ==== outs related
    void slotNewOut();
    void slotDelOut();
    void slotEditOut();
    void slotShowOutData();
    void slotExportOut();

    // ==== graphs related
    void slotNewGraph();
    void slotDelGraph();
    void slotEditGraph();
    void slotShowGraph();
    void slotShowGraphData();
    void slotExportGraphData();
    void slotGnuplotGraph();

    // ==== model related
    void slotEditModel();
    void slotShowVars();
    
    // ==== run related
    void slotRunRun();
    void slotRunPrm();
    void slotRunPrm2();
    void slotReset();

    // ==== iface related
    /** toggle the toolbar*/
    void slotViewToolBar();
    /** toggle the statusbar*/
    void slotViewStatusBar();
    /** toggle the showorder flag */
    void slotShowOrd();
    /** toggle the showgrid flag */
    void slotShowGrid();
    /** toggle the shownames flag */
    void slotShowNames();
    /** toggle the showicons flag */
    void slotShowIcons();
    /** creates a new view for the current document */
    void slotWindowNewWindow();
    /** shows an about dlg*/
    void slotHelpAbout();
    /** shows an aboutQt dlg*/
    void slotHelpAboutQt();
    /** test something */
    void slotTest(void);
    /** change the status message of the whole statusbar temporary */
    void slotStatusHelpMsg(const QString &text);
    /** gets called when the window menu is activated; recreates 
        the window menu with all opened window titles. */
    void windowMenuAboutToShow();
    /** activates the MDI child widget when it gets selected from 
        the window menu. */
    void windowMenuActivated( int id );

  private:
    /** handle close command */
    virtual void closeEvent ( QCloseEvent *e );
    /** initIface creates the menu_bar, tool_bar and inserts the items */
    void initIface();
    /** setup the statusbar */
    void initStatusBar();
    /** setup the mainview*/
    void initView();
    /** fill directories where resources reside */
    void initDirs();
		
    /** pWorkspace is the MDI frame widget that handles MDI child widgets. 
      * Inititalized in initView()
      */
    QWorkspace *pWorkspace;
    /** the printer instance */
    QPrinter *printer;
    /** a counter that gets increased each time the user creates 
          a new document with "File"->"New" */
    int untitledCount;
    /**  settings  */
    Mo2Settings sett;
    
    /* ------------------------ dirs ---------------------------- */
    /** directories to search resources */
    QString global_dir, local_dir, add_dir, xbuild_dir, env_dir;
    /** array of pointers to search dir strings */
    QString* all_dirs[6];

    QPopupMenu *pFileMenu;
    QPopupMenu *pEditMenu;
    QPopupMenu *pElmMenu;
    QPopupMenu *pOutMenu;
    QPopupMenu *pGraphMenu;
    QPopupMenu *pModelMenu;
    QPopupMenu *pRunMenu;
    QPopupMenu *pViewMenu;
    QPopupMenu *pHelpMenu;
    QPopupMenu *pWindowMenu;

    QToolBar *fileToolbar;
    QToolBar *elmToolbar;

    /** actions */
    QAction *act_new, *act_open, *act_close, *act_save, *act_saveas, 
            *act_print, *act_settings, *act_savesett, *act_quit,
	    *act_undo, *act_cut, *act_copy, *act_paste,
	    // elm
	    *act_newelm, *act_delelm, *act_editelm, 
	    *act_linkelm, *act_qlinkelm, *act_qplinkelm, *act_unlinkelm,
	    *act_lockelm, *act_ordelm, *act_markelm, *act_moveelm,
	    *act_infoelm,
	    // out
	    *act_newout, *act_delout, *act_editout,
	    *act_showoutdata, *act_exportout,
	    // graph
	    *act_newgraph, *act_delgraph, *act_editgraph,
	    *act_showgraph, *act_showgraphdata, 
	    *act_exportgraphdata, *act_gnuplotgraph,
	    // model
	    *act_editmodel, *act_showvars,
	    // run
	    *act_runrun, *act_runprm, *act_runprm2, *act_reset,
	    // iface
	    *act_tbar, *act_sbar,
	    *act_showord, *act_showgrid, *act_shownames, *act_showicons,
	    *act_winnew, *act_helpabout, *act_helpaboutqt, *act_whatsthis,
	    *act_test;
  public:
    /** static pointer to main window -- the only allowed */
    static QMo2Win *qmo2win;

};


#endif 

// end of qmo2win.h

