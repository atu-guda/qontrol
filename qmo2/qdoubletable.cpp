/***************************************************************************
                          qdoubletable.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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
#include <QtGui>
#include "miscfun.h"
#include "qdoubletable.h"

DoubleTableModel::DoubleTableModel( const GraphInfo *gi, QObject *parent )
  : QAbstractTableModel( parent ), 
    row( gi->row ), col( gi->col ), ny ( gi->ny ),
    title( gi->title ),
    dat( gi->row )
{
  for( int i=0; i<max_graphs; ++i ) { 
    labels << gi->label[i];
    dat[i] = gi->dat[i];
  }
}

int DoubleTableModel::columnCount( const QModelIndex & /*parent*/ ) const
{
  return col;
}

int DoubleTableModel::rowCount( const QModelIndex & /*parent*/ ) const
{
  return row; 
}

QVariant DoubleTableModel::data( const QModelIndex & index, int role ) const
{
  if( role != Qt::DisplayRole )
    return QVariant();
  if( ! index.isValid() )
    return QVariant();
  return QVariant( (*dat[index.column()])[index.row()] );
}


QVariant DoubleTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole )
    return QVariant();
  if( orientation == Qt::Horizontal ) {
    if( section >= col )
      return QVariant();
    return QVariant( labels[section] );
  }
  if( section >= row )
    return QVariant();
  return QVariant( QString::number(section) );
}

