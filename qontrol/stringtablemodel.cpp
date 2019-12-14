// =====================================================================================
//
//       Filename:  stringtablemodel.cpp
//
//    Description: StringTableModel implementation
//        Created:  2019-12-14 12:10:10
//
//         Author:  Anton Guda (atu), atu@nmetau.edu.ua
//      Copyright:  GPLv2+ 2019-2019 by atu
//   Organization:  NMetAU.ITS
//
// =====================================================================================

#include "defs.h"
#include "stringtablemodel.h"


StringTableModel::StringTableModel( QList<QStringList> *a_pd, QStringList *a_head, QObject *parent )
  : QAbstractTableModel( parent ), pd( a_pd ), head( a_head )
{
}

int StringTableModel::rowCount( const QModelIndex & /*par*/ ) const
{
  if( !pd ) {
    return 0;
  }
  return pd->size();
}

int StringTableModel::columnCount( const QModelIndex & /*par */ ) const
{
  if( !head ) {
    return 0;
  }
  return head->size();
}

QVariant StringTableModel::data( const QModelIndex &index, int role ) const
{
  if( !pd ) {
    return QVariant();
  }

  if( role != Qt::DisplayRole ) {
    return QVariant();
  }
  if( ! index.isValid() ) {
    return QVariant();
  }

  auto col = index.column(), row = index.row();
  if( row >= pd->size() ) {
    return QVariant();
  }
  if( col >= (*pd)[row].size() ) {
    return QVariant();
  }
  return (*pd)[row][col];
}

QVariant StringTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( !head || role != Qt::DisplayRole ) {
    return QVariant();
  }

  if( orientation == Qt::Vertical ) {
    return QVariant( QSN( section ) );
  }

  if( orientation == Qt::Horizontal ) {
    if( section >= head->size() ) {
      return QVariant();
    }
    return QVariant( (*head)[section] );
  }

  return QVariant();
}

