/***************************************************************************
                          doubletable.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2015 by atu
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

#include <QString>
#include <QtWidgets>
#include "miscfun.h"
#include "doubletable.h"

DoubleTableModel::DoubleTableModel( const DatasInfo &a_di, QObject *parent )
  : QAbstractTableModel( parent ),
    di( a_di )
{
}

int DoubleTableModel::columnCount( const QModelIndex & /*parent*/ ) const
{
  return di.size();
}

int DoubleTableModel::rowCount( const QModelIndex & /*parent*/ ) const
{
  return di.nn;
}

QVariant DoubleTableModel::data( const QModelIndex & index, int role ) const
{
  if( role != Qt::DisplayRole ) {
    return QVariant();
  }
  if( ! index.isValid() ) {
    return QVariant();
  }

  int col = index.column(), row = index.row();
  if( col >= di.size() || row >= di.nn ) {
    return QVariant();
  }
  auto ve = di.ves[col];

  return QVariant( (*ve)[row] );
}


QVariant DoubleTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole ) {
    return QVariant();
  }

  if( orientation == Qt::Horizontal ) {
    if( section >= di.size() ) {
      return QVariant();
    }
    return QVariant( di.labels[section] );
  }

  if( section >= di.nn ) {
    return QVariant();
  }
  return QVariant( QSN(section) );
}

