/***************************************************************************
                          commonsubwin.h - common MDI subwindow inner widget
                             -------------------
    begin                : 2015.10.12
    copyright            : (C) 2015-2015 by atu
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

#ifndef COMMONSUBWIN_H
#define COMMONSUBWIN_H


#include "defs.h"
#include <QMainWindow>
#include <QMap>
#include "labodoc.h"

class CmdView;


class CommonSubwin : public QMainWindow  {
 Q_OBJECT
 public:
   CommonSubwin( QWidget *parent, LaboDoc *a_doc, const QString &o_name );
   ~CommonSubwin();
   QString getFilePath() const;
   QString getFileBase() const;
   void updateTitle();
   bool isMainWin() const { return main_win; }
   LaboDoc *getDocument() const { return  doc; };
   virtual bool callSlot( const char *nm ) = 0;
   virtual bool checkSlot( const char *nm ) = 0;
   using ViewNameMap = QMap<QString,CmdView*>;
   CmdView* getView( const QString& view_name ) const;
   HolderData* getSelectedInView( const QString& view_name ) const;
   QString getSelNameInView( const QString& view_name ) const;
   HolderData* getSelectedInFocus() const;
 public slots:
   QString getSelNameInFocus() const;
   virtual int getLevel() const = 0;
 signals:
   void closeMe(); // ask main window to close me

 protected:
   bool checkSlotSub( QWidget *w, const char *nm );

   LaboDoc *doc;
   QString filePath, objName;
   QString title_prefix = QSL( "Unk" );
   bool main_win = false; //* flag of close this close all non-main windows with same filePath
   ViewNameMap vmap;
};

#endif

// end of commonsubwin.h

