#pragma once
/***************************************************************************
  linkcompleter.h - completer for links fields description
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


#include <QCompleter>
#include <QStringList>

class LinkCompleter : public QCompleter
{
  Q_OBJECT
  public:
    explicit LinkCompleter( QObject *parent = nullptr );
    explicit LinkCompleter( QAbstractItemModel *mdl, QObject *parent = nullptr );

  protected:
    QStringList splitPath( const QString &path ) const override;
    QString pathFromIndex( const QModelIndex &idx ) const override;
};


