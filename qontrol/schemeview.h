/***************************************************************************
                          schemeview.h - view for schems list
                             -------------------
    begin                : 2015.10.04
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

#ifndef _SCHEMEVIEW_H
#define _SCHEMEVIEW_H

#include <QtWidgets>

#include "laboview.h"

class SchemeView : public QListView {
  Q_OBJECT
  public:
   SchemeView( HolderData *a_mod, LaboView *par );
  private:
   HolderData *mod;
   LaboView *laboview;
   QAction *act_new, *act_del, *act_edit, *act_rename,
           *act_clone;
  private:
   void init_actions();
};

#endif

