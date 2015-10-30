/***************************************************************************
                          outdataview.h - view for outs
                             -------------------
    begin                : 2014.11.15
    copyright            : (C) 2014-2015 by atu
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

#ifndef _OUTDATAVIEW_H
#define _OUTDATAVIEW_H

#include <QtWidgets>

#include "cmdview.h"
#include "commonsubwin.h"

class OutDataView : public CmdView {
  Q_OBJECT
  public:
   OutDataView( HolderData *a_mod, CommonSubwin *a_par );
   virtual HolderData* getSelObj() const override;
   virtual void handleSelChange() override;
   virtual QModelIndex currentIndex() const override;
  public slots:
    virtual bool addObj() override;
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
    virtual bool showDataObj() override;
    virtual bool exportObj() override;
    // specific functions
    bool addToPlot();
  protected:
   QListView *lv = nullptr;
  protected:
   void init_actions();
};

#endif

