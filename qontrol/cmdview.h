#pragma once
/***************************************************************************
                     cmdview.h  -  common base for views vith std actions
                             -------------------
    begin                : 2015.10.27
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


#include <QWidget>

class HolderData;
class CommonSubwin;

class CmdView : public QWidget {
  Q_OBJECT
  public:
    CmdView( CommonSubwin *a_par, HolderData *a_storage );
    virtual HolderData* getSelObj() const = 0;
    QString getSelName() const;
    virtual void handleSelChange() = 0;
    HolderData* getStorage() { return storage; }
    virtual QModelIndex currentIndex() const;
    const QString& getLastObjName() const { return lastObjName; }
  public Q_SLOTS:
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
    virtual bool showObj();
    virtual bool showDataObj();
    virtual bool exportObj();
  Q_SIGNALS:
    void viewChanged();
    void focusChanged();
  protected:
    HolderData* storage;
    CommonSubwin *par;
    QString lastObjName;
    QString sugg_name;
  protected:
    virtual void focusInEvent( QFocusEvent *ev ) override;
};


