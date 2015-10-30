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
#include "cmdview.h"


using namespace std;

CommonSubwin::CommonSubwin( QWidget *parent, LaboDoc *a_doc, const QString &o_name )
          : QMainWindow( parent ), doc( a_doc ), objName( o_name )
{
}

CommonSubwin::~CommonSubwin()
{
}

QString CommonSubwin::getFilePath() const
{
  if( !doc ) { return QString(); }
  return QFileInfo( doc->pathName() ).canonicalFilePath();
}

void CommonSubwin::updateTitle()
{
  if( !doc ) {
    setWindowTitle( "NO_DOC" );
  }
  setWindowTitle( title_prefix % QSL(": ") % objName % QSL(" - ") % doc->pathName() );
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

CmdView* CommonSubwin::getView( const QString& view_name ) const
{
  auto it = vmap.find( view_name );
  if( it == vmap.end() ) {
    return nullptr;
  }
  return it.value();
}

HolderData* CommonSubwin::getSelectedInView( const QString& view_name ) const
{
  CmdView *vi = getView( view_name );
  if( !vi ) {
    return nullptr;
  }
  return vi->getSelObj();
}

QString CommonSubwin::getSelNameInView( const QString& view_name ) const
{
  CmdView *vi = getView( view_name );
  if( !vi ) {
    return nullptr;
  }
  return vi->getSelName();
}

HolderData* CommonSubwin::getSelectedInFocus() const
{
  QWidget *w = focusWidget();
  CmdView *vi = qobject_cast<CmdView*>( w );
  if( !vi ) {
    return nullptr;
  }
  return vi->getSelObj();
}

QString CommonSubwin::getSelNameInFocus() const
{
  QWidget *w = focusWidget();
  CmdView *vi = qobject_cast<CmdView*>( w );
  if( !vi ) {
    return nullptr;
  }
  return vi->getSelName();
}


 // end of commonsubwin.cpp
