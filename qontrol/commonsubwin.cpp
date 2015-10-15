/***************************************************************************
                          commonsubwin.h - common MDI subwindow inner widget
                             -------------------
    begin                : 2015.10.12
    copyright            : (C) 2015-2015 by atu
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

#include <QFileInfo>

#include "commonsubwin.h"


using namespace std;

CommonSubwin::CommonSubwin( QWidget *parent, const QString &o_name )
          : QWidget( parent ), objName( o_name ),
            sels( selNums, -1 )
{
}

CommonSubwin::~CommonSubwin()
{
}

void CommonSubwin::setFilePath( const QString &fp )
{
  filePath = QFileInfo( fp ).canonicalFilePath();
  setWindowTitle( title_prefix + QSL(": ") + objName + QSL(" ") + filePath );
}

int CommonSubwin::getSelNum( unsigned sn ) const
{
  if( sn >= selNums ) {
    return -1;
  }
  return sels[sn];
}

bool CommonSubwin::checkSlotSub( QWidget *w, const char *nm )
{
  if( !w ) {
    return false;
  }

  const QMetaObject *mo = w->metaObject();
  if( !mo ) {
    return false;
  }
  QByteArray  nnm ( nm );
  nnm.append( "()" );
  int idx = mo->indexOfMethod( nnm.data() );
  return ( idx >= 0);
}


// end of commonsubwin.cpp

