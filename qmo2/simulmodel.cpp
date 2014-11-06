#include "simulmodel.h"

SimulModel::SimulModel( ContSimul *a_sims, QObject *par )
  : QAbstractListModel( par ), sims( a_sims )
{
}

int SimulModel::rowCount( const QModelIndex & /*par*/ ) const
{
  if( ! sims ) {
    return 0;
  }
  return sims->size();
}

QVariant SimulModel::data( const QModelIndex &idx, int role ) const
{
  if( ! sims ) {
    return 0;
  }
  int r = idx.row();
  if( r > sims->size() || r < 0 ) {
    return QVariant();
  }

  const Simulation *sim = sims->getElemT<const Simulation*>( r );
  if( !sim ) {
    return QVariant();
  }

  switch( role ) {
    case Qt::DisplayRole : return QVariant( sim->objectName() );
    default: return QVariant();
  }

  return QVariant();
}

Qt::ItemFlags SimulModel::flags( const QModelIndex &idx ) const
{
  if( ! sims ) {
    return 0;
  }
  int r = idx.row();
  if( r > sims->size() || r < 0 ) {
    return Qt::NoItemFlags;
  }
  return Qt::ItemIsEnabled  | Qt::ItemIsSelectable;

  // Qt::ItemIsSelectable  1  It can be selected.
  // Qt::ItemIsEditable  2  It can be edited.
  // Qt::ItemIsDragEnabled  4  It can be dragged.
  // Qt::ItemIsDropEnabled  8  It can be used as a drop target.
  // Qt::ItemIsUserCheckable  16  It can be checked or unchecked by the user.
  // Qt::ItemIsEnabled  32  The user can interact with the item.
  // Qt::ItemIsTristate  64  The item is checkable with three separate states.
  // Qt::ItemNeverHasChildren
}

