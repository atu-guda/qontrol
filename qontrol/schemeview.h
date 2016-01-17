/***************************************************************************
                          schemeview.h - view for schems list
                             -------------------
    begin                : 2015.10.04
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

#ifndef _SCHEMEVIEW_H
#define _SCHEMEVIEW_H

#include <QtWidgets>

#include "cmdlistview.h"

class SchemeView : public CmdListView {
  Q_OBJECT
  public:
   SchemeView( HolderData *a_mod, CommonSubwin *a_par );
  public Q_SLOTS:
    // virtual bool addObj() override;
    // virtual bool delObj() override;
    virtual bool editObj() override;
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
   void init_actions();
};

#endif

