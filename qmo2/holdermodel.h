/***************************************************************************
                          holdermodel.h  -  description
                             -------------------
    begin                : 20 Jul 2012
    copyright            : GPL (C) 2012-2012 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _HOLDERMODEL_H
#define _HOLDERMODEL_H

#include <QAbstractItemModel>

class TDataSet;
class HolderData;

class HolderModel : public QAbstractItemModel {
  Q_OBJECT
 public:
   HolderModel( TDataSet *a_ds, QObject *a_parent ); 
   virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
   virtual QVariant data( const QModelIndex &index, 
                      int role = Qt::DisplayRole ) const;
   // virtual bool hasChildren( const QModelIndex &parent = QModelIndex() ) const;
   virtual QModelIndex index( int row, int column, 
                        const QModelIndex &parent = QModelIndex() ) const;
   virtual QModelIndex parent( const QModelIndex &index ) const;
   virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
 protected:
   TDataSet *ds;
};

#endif  // _HOLDERMODEL_H
