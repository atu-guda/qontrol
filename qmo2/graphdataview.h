/***************************************************************************
                          graphdataview.h - view for plots
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

#ifndef _GRAPHDATAVIEW_H
#define _GRAPHDATAVIEW_H

#include <QtWidgets>

#include "laboview.h"

class GraphDataView : public QListView {
  Q_OBJECT
  public:
   GraphDataView( HolderData *a_mod, LaboView *par );
  private:
   HolderData *mod;
   LaboView *laboview;
   QAction *act_new, *act_del, *act_edit, *act_rename,
           *act_show, *act_dump, *act_showdata, *act_clone;
  private:
   void init_actions();
};

#endif

