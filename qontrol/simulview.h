/***************************************************************************
                          simulview.h - view for simulations
                             -------------------
    begin                : 2014.10.15
    copyright            : (C) 2014-2016 by atu
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

#ifndef _SIMULVIEW_H
#define _SIMULVIEW_H

#include <QtWidgets>

#include "cmdlistview.h"

class SimulView : public CmdListView {
  Q_OBJECT
  public:
   SimulView( HolderData *a_mod, CommonSubwin *a_par );
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
    //  specific:
    bool setActive();
  protected:
    void init_actions();
};

#endif

