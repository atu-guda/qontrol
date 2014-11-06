#include <QAbstractListModel>

#include "simul.h"

class SimulModel : public QAbstractListModel {
  public:
   SimulModel( ContSimul *a_sims, QObject *par = 0 );
   virtual int rowCount( const QModelIndex &par = QModelIndex() ) const;
   virtual QVariant data( const QModelIndex &idx, int role = Qt::DisplayRole ) const;
   virtual Qt::ItemFlags flags( const QModelIndex &idx ) const;
   // virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)
   // virtual bool insertRows( int row, int count, const QModelIndex &par = QModelIndex() );
   // virtual bool removeRows( int row, int count, const QModelIndex &par = QModelIndex() );
  private:
   ContSimul *sims;
};

