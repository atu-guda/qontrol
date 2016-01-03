/***************************************************************************
  linkcompleter.cpp - completer for links fields
  -------------------
begin                : 2015.01.20
copyright            : (C) 2015-2016 by atu
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

#include "linkcompleter.h"

LinkCompleter::LinkCompleter( QObject *parent )
   : QCompleter( parent )
{
}

LinkCompleter::LinkCompleter( QAbstractItemModel *mdl, QObject *parent )
   : QCompleter( mdl, parent )
{
}

QStringList LinkCompleter::splitPath( const QString &path ) const
{
  return path.split( "." );
}

QString LinkCompleter::pathFromIndex( const QModelIndex &idx ) const
{
  QStringList dataList;
  for( QModelIndex i=idx; i.isValid(); i=i.parent() ) {
    dataList.prepend( model()->data( i, completionRole()).toString() );
  }

  return dataList.join( "." );
}


