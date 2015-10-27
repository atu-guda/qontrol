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

class HolderData;

class CmdView : public QWidget {
  Q_OBJECT
  public:
    CmdView( QWidget *a_par, HolderData *a_storage );
    virtual HolderData* getSelObj() const = 0;
    QString getSelName() const;
    virtual void handleSelChange() = 0;
    HolderData* getStorage() { return storage; }
    virtual QModelIndex currentIndex() const;
  public slots:
    virtual bool addObj();
    virtual bool delObj();
    virtual bool editObj();
    virtual bool renameObj();
    virtual bool cloneObj();
    virtual bool cutObj();
    virtual bool copyObj();
    virtual bool pasteObj();
    virtual bool infoObj();
    virtual bool showTreeObj();
    virtual bool testObj();
  signals:
    void viewChanged();
  protected:
    HolderData* storage;
};

#endif // CMDVIEW_H

