/***************************************************************************
                          holdermodel.cpp  -  HolderModel implementation
                             -------------------
    begin                : 20 Jul 2012
    copyright            : GPL (C) 2000-2012 by atu
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

  if( index.column() == 0 ) {
    return ho->targetName() + " " +  ho->getType();
  }
  if( ho->isObject() ) {
    return "*";
  }
  return ho->get();
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

  TDataSet *parentItem;
  HolderData *parentHolder;
  HolderObj  *parentHolderObj;

  if( !parent.isValid() ) {
    parentItem = ds;
  } else {
    parentHolder = static_cast<HolderData*>( parent.internalPointer() );
    if( ! parentHolder->isObject() )
      return QModelIndex();
    parentHolderObj = qobject_cast<HolderObj*>(parentHolder);
    parentItem = parentHolderObj->getObj();
  }

  HolderData *ho = parentItem->getHolder( row );
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
  TDataSet *cds = ho->getParent(); // not 0
  if( cds == ds )
    return QModelIndex();
  
  HolderData *pho = cds->myHolder();
  if( ! pho )
    return QModelIndex();
  
  int ho_idx = cds->indexOfHolder( ho );

  return createIndex( ho_idx, 0, pho );
}

int HolderModel::rowCount( const QModelIndex &parent ) const
{
  if( ! parent.isValid() ) 
    return ds->getNumObj();

  HolderData *ho = static_cast<HolderData*>( parent.internalPointer() );
  HolderObj *hob = qobject_cast<HolderObj*>(ho);
  if( ! hob )
    return 0;

  TDataSet *cds = hob->getObj();
  return cds->getNumObj();
}


// end of holdermodel.cpp

