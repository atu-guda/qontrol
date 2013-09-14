/***************************************************************************
                          qdoubletable.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef QDOUBLETABLE_H
#define QDOUBLETABLE_H

#include <QAbstractTableModel>
#include <QWidget>
#include <QString>
#include <QStringList>
class QPainer; struct GraphInfo;


/** new model for model/view Qt4 approach */
class DoubleTableModel : public QAbstractTableModel {
  public:
   DoubleTableModel( const GraphInfo *gi, QObject *parent = 0 );
   virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
   virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
   virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
   virtual QVariant headerData( int section, Qt::Orientation orientation,
            int role = Qt::DisplayRole ) const;
  protected:
   int row, col, ny;
   QString title;
   QStringList labels;
   std::vector<const dvector *> dat;
};

#endif

