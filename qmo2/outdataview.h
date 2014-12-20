/***************************************************************************
                          outdataview.h - view for outs
                             -------------------
    begin                : 2014.11.15
    copyright            : (C) 2014-2014 by atu
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

#include "laboview.h"

class OutDataView : public QListView {
  Q_OBJECT
  public:
   OutDataView( HolderData *a_mod, LaboView *par );
  private:
   HolderData *mod;
   LaboView *laboview;
   QAction *act_new, *act_del, *act_edit, *act_rename,
           *act_dump, *act_showdata, *act_graphaddout;
  private:
   void init_actions();
};

#endif

