#pragma once
/***************************************************************************
                          doubletable.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2019 by atu
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


#include <QAbstractTableModel>
#include <QWidget>
#include <QString>
#include <QStringList>
class QPainer; struct DatasInfo;


class DoubleTableModel : public QAbstractTableModel {
  public:
   DoubleTableModel( const DatasInfo &a_di, QObject *parent = 0 );
   virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
   virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   virtual QVariant headerData( int section, Qt::Orientation orientation,
            int role = Qt::DisplayRole ) const;
  protected:
   const DatasInfo &di;
};


