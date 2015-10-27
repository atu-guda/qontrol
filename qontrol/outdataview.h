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
#include "laboview.h"

class OutDataView : public CmdView {
  Q_OBJECT
  public:
   OutDataView( HolderData *a_mod, LaboView *par );
   virtual HolderData* getSelObj() const override;
   virtual void handleSelChange() override;
   virtual QModelIndex currentIndex() const override;
  protected:
   QListView *lv = nullptr;
   LaboView *laboview;
  protected:
   void init_actions();
};

#endif

