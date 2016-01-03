/***************************************************************************
                      cmdlistview.h - common base for list-alike viewvs
                             -------------------
    begin                : 2015.10.31
    copyright            : (C) 2015-2016 by atu
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

#ifndef _CMDLISTVIEW_H
#define _CMDLISTVIEW_H

#include <QtWidgets>

#include "cmdview.h"
#include "commonsubwin.h"

class CmdListView : public CmdView {
  Q_OBJECT
  public:
   CmdListView( HolderData *a_mod, CommonSubwin *a_par );
   virtual HolderData* getSelObj() const override;
   virtual void handleSelChange() override;
   virtual QModelIndex currentIndex() const override;
  public slots:
    // virtual bool addObj() override;
    // virtual bool delObj() override;
    // virtual bool editObj() override;
    // virtual bool renameObj() override;
    // virtual bool cloneObj() override;
    // virtual bool cutObj() override;
    // virtual bool copyObj() override;
    // virtual bool pasteObj() override;
    // virtual bool infoObj() override;
    // virtual bool showTreeObj() override;
    // virtual bool testObj() override;
    // virtual bool showObj() override;
    // virtual bool showDataObj() override;
    // virtual bool exportObj() override;
  protected:
   QListView *lv = nullptr;
  protected:
   void init_base_actions();
};

#endif

