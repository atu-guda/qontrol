// =====================================================================================
//
//       Filename:  stringtablemodel.h
//
//    Description: StringTableModel declaration
//        Created:  2019-12-14 12:10:10
//
//         Author:  Anton Guda (atu), atu@nmetau.edu.ua
//      Copyright:  GPLv2+ 2019-2019 by atu
//   Organization:  NMetAU.ITS
//
// =====================================================================================
#ifndef _STRINGTABLEMODEL_H
#define _STRINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class StringTableModel : public QAbstractTableModel {
  public:
    StringTableModel( QList<QStringList> *a_pd, QStringList *a_head, QObject *parent = nullptr );
  protected:
    int rowCount( const QModelIndex &par = QModelIndex() ) const override;
    int columnCount( const QModelIndex &par = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    QList<QStringList> *pd = nullptr;
    QStringList *head = nullptr;
};


#endif

