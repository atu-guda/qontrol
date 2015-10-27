/***************************************************************************
                     cmdview.h  -  common base for views vith std actions
                             -------------------
    begin                : 2015.10.27
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

#ifndef CMDVIEW_H
#define CMDVIEW_H

#include <QWidget>

class CmdView : public QWidget {
  Q_OBJECT
  public:
    CmdView( QWidget *a_par );
  public slots:
    virtual void addObj();
    virtual void delObj();
    virtual void editObj();
    virtual void renameObj();
    virtual void cloneObj();
    virtual void cutObj();
    virtual void copyObj();
    virtual void pasteObj();
    virtual void infoObj();
    virtual void showTreeObj();
    virtual void testObj();
  signals:
    void viewChanged();
  protected:
};

#endif // CMDVIEW_H

