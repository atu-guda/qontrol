/***************************************************************************
                          labowin.h  -  description
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

#ifndef LABOWIN_H
#define LABOWIN_H

// include files for QT
#include <QMainWindow>
#include <QString>

#include "defs.h"

class QToolBar;
class QPrinter;
class QCloseEvent;

class QMdiArea;
class QSignalMapper;
class QMenu;
class QAction;
class QMdiSubWindow;
class QStatusBar;
class QSplitter;
class QTextEdit;
class QTimer;

// forward declaration of the LaboWin classes
class LaboDoc;
class LaboView;

/** auxilary class for storing Labo application settings */
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
   /** flag: show links (new) in structure /view/showlinks */
   bool showLinks;
   /** main font string: /fonts/main */
   QString mainFont;
   /** small font string: /fonts/small */
   QString smallFont;
   /** plot font string:  /fonts/plot */
   QString plotFont;
   /** structure view font string: /fonts/struct */
   QString structFont;
   //* cmd to exec external editor
   QString editCmd;

};

/**
  * LaboWin is the main window class for application. It sets up the main
  * window and providing a menubar, toolbar
  * and statusbar. For the main view, an instance of class LaboView is
  * created which creates your view.
  *
  * @author atu
  */
class LaboWin : public QMainWindow
{
  Q_OBJECT
  public:
    LaboWin();
    ~LaboWin();

    /** enables/disables menu entries/toolbar items */
    void enableActions( bool ena, int id_ );
    /** returns apps printer */
    QPrinter* getPrinter() { return printer; }
    /** returns pointer to settings object */
    Mo2Settings* getSettings() { return &sett; }
    /** reads new XML file: to use from slot and commanline */
    bool doFileOpenXML( const QString &fn );
    const QFont& getMainFont()   const  { return mainFont; }
    const QFont& getSmallFont()  const  { return smallFont; }
    const QFont& getPlotFont()   const  { return plotFont; }
    const QFont& getStructFont() const  { return structFont; }
    int getEm() const { return em; }
    static int Em() { return ( labowin ? labowin->getEm() : 10 ); }
    static LaboWin* win() { return labowin; }

  protected:
    /** overloaded for Message box on last window exit */
    bool queryExit();
    LaboView* createChild( LaboDoc* doc );
    QMdiSubWindow* findMdiChild( const QString &fileName );
    LaboView* activeLaboView();
    //* call gives simple slot from active LaboView
    void callLaboViewSlot( const char *slot, const QString &mess );

  private slots:

    /** sets the main application window title each time
      * the active MDI child window changes. */
    void setWndTitle( QWidget* );
    /** automaticaly enables/disable actions */
    void updateActions();

    /** generate a new document in the actual view */
    void slotFileNew();
    /** open a XML document */
    void slotFileOpenXML();
    /** save a document as XML */
    void slotFileSaveXML();
    /** save a document as XML under a different filename*/
    void slotFileSaveXMLAs();
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
    void slotRenameElm();
    void slotqLinkElm();
    void slotqpLinkElm();
    void slotUnlinkElm();
    void slotLockElm();
    void slotOrdElm();
    void slotMarkElm();
    void slotMoveElm();
    void slotInfoElm();
    void slotShowTreeElm();
    void slotTestElm1();
    void slotTestElm2();

    // ==== outs related
    void slotNewOut();
    void slotDelOut();
    void slotEditOut();
    void slotRenameOut();
    void slotSelectOut();
    void slotShowOutData();
    void slotExportOut();

    // ==== graphs related
    void slotNewGraph();
    void slotDelGraph();
    void slotEditGraph();
    void slotRenameGraph();
    void slotSelectGraph();
    void slotShowGraph();
    void slotGraphAddOut();
    void slotShowGraphData();
    void slotExportGraphData();
    void slotCloneGraph();

    // ==== simulation related
    void slotNewSimul();
    void slotDelSimul();
    void slotEditSimul();
    void slotRenameSimul();
    void slotSelectSimul();
    void slotSetActiveSimul();
    void slotCloneSimul();

    // ==== model/scheme related
    void slotEditModel();
    void slotShowTreeModel();
    void slotNewScheme();
    void slotDelScheme();
    void slotEditScheme();
    void slotRenameScheme();
    void slotCloneScheme();

    // ==== run related
    void slotRunRun();
    void slotReset();
    void slotInitEngine();
    void slotRunScript();
    void slotRunModelScript();

    // ==== view related
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
    /** toggle the showLinks flag */
    void slotShowLinks();
    void slotLogClear();
    void slotLogSave();
    // ==== window related
    void slotWindowClose();
    void slotWindowCloseAll();
    void slotWindowTile();
    void slotWindowCascade();
    void setActiveSubWindow( QWidget *win );
    // ==== help related
    /** shows an about dlg*/
    void slotHelpAbout();
    /** shows an aboutQt dlg*/
    void slotHelpAboutQt();
    /** test something */
    void slotTest();
    /** change the status message of the whole statusbar temporary */
    void slotStatusHelpMsg(const QString &text);
    /** gets called when the window menu is activated; recreates
        the window menu with all opened window titles. */
    void windowMenuAboutToShow();

    //* called by timer to update log view
    void slotUpdateLog();

  private:
    /** handle close command */
    virtual void closeEvent ( QCloseEvent *e );
    /** initIface creates the menu_bar, tool_bar and inserts the items */
    void initIface();
    /** setup the statusbar */
    void initStatusBar();
    /** recreate fonts from config */
    void setFonts();

    QSplitter *split;
    QMdiArea *mdiArea;
    QTextEdit *logViewer;
    QSignalMapper *windowMapper;
    /** the printer instance */
    QPrinter *printer;
    //* timer to update log
    QTimer *log_timer;
    /** a counter that gets increased each time the user creates
          a new document with "File"->"New" */
    int untitledCount;
    //* default width unit (and safe default value)
    int em = 10;
    //* last log size - for update
    int lastLogSize = 0;
    /**  settings  */
    Mo2Settings sett;
    /** common fonts */
    QFont mainFont, smallFont, plotFont, structFont;


    QMenu *pFileMenu;
    QMenu *pEditMenu;
    QMenu *pElmMenu;
    QMenu *pOutMenu;
    QMenu *pGraphMenu;
    QMenu *pSimulMenu;
    QMenu *pModelMenu;
    QMenu *pRunMenu;
    QMenu *pViewMenu;
    QMenu *pHelpMenu;
    QMenu *pWindowMenu;

    QToolBar *fileToolbar;
    QToolBar *elmToolbar;

    /** actions */
    QAction *act_new, *act_openxml, *act_close,
            *act_savexml, *act_savexmlas,
            *act_print, *act_settings, *act_savesett, *act_quit,
            *act_undo, *act_cut, *act_copy, *act_paste,
            // elm
            *act_newelm, *act_delelm, *act_editelm, *act_renameelm,
            *act_qlinkelm, *act_qplinkelm, *act_unlinkelm,
            *act_lockelm, *act_ordelm, *act_markelm, *act_moveelm,
            *act_infoelm, *act_showtreeelm,
            *act_testelm1, *act_testelm2,
            // out
            *act_newout, *act_delout, *act_editout, *act_selectout, *act_renameout,
            *act_showoutdata, *act_exportout,
            // graph
            *act_newgraph, *act_delgraph, *act_editgraph, *act_selectgraph,
            *act_renamegraph,
            *act_showgraph, *act_graphaddout, *act_showgraphdata,
            *act_exportgraphdata, *act_cloneGraph,
            // simulation
            *act_newSimul, *act_delSimul, *act_editSimul, *act_selectSimul,
            *act_renameSimul,
            *act_setActiveSimul, *act_cloneSimul,
            // model/scheme
            *act_editmodel, *act_showtreemodel,
            *act_newScheme, *act_delScheme, *act_editScheme,
            *act_renameScheme, *act_cloneScheme,
            // run
            *act_runrun, *act_reset,
             *act_initengine, *act_runscript, *act_runmodelscript,
            // iface
            *act_tbar, *act_sbar,
            *act_showord, *act_showgrid, *act_shownames, *act_showicons, *act_showlinks,
            *act_logclear, *act_logsave,
            // win
            *act_winClose, *act_winCloseAll, *act_winTile, *act_winCascade,
            // help
            *act_helpabout, *act_helpaboutqt, *act_whatsthis,
            *act_test;
    QList<QAction*> model_acts; //* list of actions, depending on model

    /** static pointer to main window -- the only allowed */
    static LaboWin *labowin;
};


#endif

// end of labowin.h

