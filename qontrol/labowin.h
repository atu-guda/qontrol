#pragma once
/***************************************************************************
                          labowin.h  -  description
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


// include files for QT
#include <QMainWindow>
#include <QString>
#include <QMap>

#include "commonsubwin.h"
#include "settingsdata.h"

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

enum ActionFlags {
  ActionToolBar   = 1,
  ActionToolSep   = 2,
  ActionMenuSep   = 4,
  ActionAlwaysVis = 8
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
    using SlotVV = void (LaboWin::*)();

    /** returns apps printer */
    QPrinter* getPrinter() { return printer; }
    /** returns pointer to settings object */
    SettingsData* getSettings() { return setti; }
    bool doFileOpen( const QString &fn );
    int getEm() const { return em; }
    QMdiSubWindow* addChild( CommonSubwin* w );
    QMdiSubWindow* findMdiByTitle( const QString &tit, bool activate = false );
    QMdiSubWindow* findMdiChild( const QString &fileName );
    int closeRelated( const QString &fp );
    static int Em() { return ( labowin ? labowin->getEm() : 10 ); }
    static LaboWin* win() { return labowin; }

  protected:
    CommonSubwin* activeView();
    //* call gives simple slot from active LaboView
    void callLaboViewSlot( const char *slot, const QString &mess );
    //* make action and link to slot, set property
    QAction* makeAction( const QString &text, const char *actName, SlotVV targetSlot,
        const QString &icon_str = QString(), QMenu *tmenu = nullptr, unsigned aflags = 0 );

  private Q_SLOTS:

    /** sets the main application window title each time
      * the active MDI child window changes. */
    void setWndTitle();
    /** automaticaly enables/disable actions */
    void updateActions();
    /** gets called when the window menu is activated; recreates
        the window menu with all opened window titles. */
    void windowMenuAboutToShow();

    //* called by timer to update log view
    void slotUpdateLog();

  public Q_SLOTS:
    /** generate a new document in the actual view */
    void slotFileNew();
    /** open a document */
    void slotFileOpen();
    /** save a document as */
    void slotFileSave();
    /** save a document as under a different filename*/
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

    // ==== genetic object related
    void slotAddObj();
    void slotDelObj();
    void slotCutObj();
    void slotEditObj();
    void slotCopyObj();
    void slotPasteObj();
    void slotRenameObj();
    void slotCloneObj();
    void slotInfoObj();
    void slotShowTreeObj();
    void slotTestObj();

    // ==== element related
    void slotqLinkElm();
    void slotqpLinkElm();
    void slotUnlinkElm();
    void slotLockElm();
    void slotMarkElm();
    void slotMoveElm();

    // ==== outs related
    void slotNewOut();
    void slotShowOutData();
    void slotExportOut();

    // ==== graphs related
    void slotNewGraph();
    void slotShowGraph();
    void slotGraphAddOut();
    void slotShowGraphData();
    void slotExportGraphData();

    // ==== simulation related
    void slotNewSimul();
    void slotSetActiveSimul();

    // ==== model/scheme related
    void slotEditModel();
    void slotShowTreeModel();
    void slotNewScheme();
    void importAllSchemes();

    // ==== run related
    void slotRunRun();
    void slotReset();
    void slotInitEngine();
    void slotRunScript();
    void slotRunModelScript();

    // ==== view related
    /** toggle the toolbar*/
    void slotViewToolBar();
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
    void slotHandleSubWindowActivated( QMdiSubWindow *swin );
    // ==== help related
    /** shows an about dlg*/
    void slotHelpAbout();
    /** shows an aboutQt dlg*/
    void slotHelpAboutQt();
    /** test something */
    void slotTest();
    /** change the status message of the whole statusbar temporary */
    void slotStatusHelpMsg(const QString &text);

  private:
    /** handle close command */
    virtual void closeEvent ( QCloseEvent *e );
    /** initIface creates the menu_bar, tool_bar and inserts the items */
    void initIface();
    /** setup the statusbar */
    void initStatusBar();
    /** recreate fonts from config */
    void setFonts();
    /** register action in 'acts' and set name */
    void registerAction( QAction *act, const char *nm );

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
    SettingsData *setti;


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

    QToolBar *mainToolBar;

    /** actions: only special */
    QAction *act_tbar,
            // win
            *act_winClose, *act_winCloseAll, *act_winTile, *act_winCascade,
            *act_winNext, *act_winPrev;
    QMap<QByteArray,QAction*> acts;

    /** static pointer to main window -- the only allowed */
    static LaboWin *labowin;
};

#define MAINWIN LaboWin::win()
#define SETTINGS (LaboWin::win()->getSettings())



// end of labowin.h

