/***************************************************************************
                          holdermodel.cpp  -  HolderModel implementation
                             -------------------
    begin                : 20 Jul 2012
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "dataset.h"
#include "holdermodel.h"

HolderModel::HolderModel( TDataSet *a_ds, QObject *a_parent )
            :QAbstractItemModel( a_parent ),
             ds(a_ds)
{
}

int HolderModel::columnCount( const QModelIndex & /*parent*/ ) const
{
  return 2;
  //if( parent.isValid() )
  //return 0;
}

QVariant HolderModel::data( const QModelIndex &index, int role ) const
{
  if( !index.isValid() )
    return QVariant();

  if( role != Qt::DisplayRole )
    return QVariant();

  HolderData *ho = static_cast<HolderData*>(index.internalPointer());
  if( !ho )
    return QVariant();

  int col = index.column();

  if( col == 0 ) { // label
    QString r = ho->objectName() + " " +  ho->getType();
    if( ho->isDyn() )
      r += '.';
    return r;
  }

  if( col != 1 ) { // no data - fake output
    return QString("col: ") + QSN(col) + QString(" row: ") + QSN(index.row());
  }

  if( ! ho->isObject() ) { // simple data
    return ho->get();
  }

  TDataSet *ob = qobject_cast<TDataSet*>(ho);
  if( !ob )
      return "?cast?";

  return QString("*_") + QSN( ob->size() );
}

//bool HolderModel::hasChildren( const QModelIndex &parent ) const
//{
//  return false;
//}

QModelIndex HolderModel::index( int row, int column,
                        const QModelIndex &parent ) const
{
  if( ! hasIndex(row, column, parent) )
    return QModelIndex();

  HolderData *hop;
  TDataSet *parentItem = ds;

  if( parent.isValid() ) {
    hop = static_cast<HolderData*>( parent.internalPointer() );
    if( !hop ) {
      DBG1( "ERR: parent.internalPointer is nullptr " );
      return QModelIndex();
    }
    parentItem = qobject_cast<TDataSet*>(hop);
    if( !parentItem  || ! parentItem->isObject()  )
      return QModelIndex();
  }

  HolderData *ho = parentItem->getElem( row );
  if( !ho )
    return QModelIndex();

  return createIndex( row, column, ho );
}

QModelIndex HolderModel::parent( const QModelIndex &index ) const
{
  if( ! index.isValid() )
    return QModelIndex();

  // FIXME: perverted (used grandparent and name)
  HolderData *ho =  static_cast<HolderData*>( index.internalPointer() );
  if( ! ho ) {
    DBG1( "ERR: index.internalPointer() is nullptr" );
    return QModelIndex();
  }
  TDataSet *cds = ho->getParent(); // not 0
  if( cds == ds )
    return QModelIndex();

  int ho_idx = cds->indexOfHolder( ho );

  return createIndex( ho_idx, 0, cds );
}

int HolderModel::rowCount( const QModelIndex &parent ) const
{
  if( ! parent.isValid() )
    return ds->size();

  HolderData *ho = static_cast<HolderData*>( parent.internalPointer() );
  if( ! ho ) {
    DBG1( "ERR: parent.internalPointer() is nullptr" );
    return 0;
  }
  return ho->size();
}


// end of holdermodel.cpp

