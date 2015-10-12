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
#include <QWidget>

enum ActionGroupFlag {
  agfNone = 0,
  agfFile = 1,
  agfElem = 2,
  agfOut  = 4,
  agfPlot = 8,
  agfPltWin = 0x10
};


class CommonSubwin : public QWidget  {
 Q_OBJECT
 public:
   CommonSubwin( QWidget *parent, const QString &o_name );
   ~CommonSubwin();
   void setFilePath( const QString &fp );
   QString getFilePath() const { return filePath; };
   bool isMainWin() const { return main_win; }
   unsigned getActGroups() const { return act_group_flags; }
 public slots:

  protected:
   QString filePath, objName;
   QString title_prefix = QSL( "Unk" );
   bool main_win = false; //* flag of close this close all non-main windows with same filePath
   unsigned act_group_flags = 0;
};

#endif

// end of commonsubwin.h

