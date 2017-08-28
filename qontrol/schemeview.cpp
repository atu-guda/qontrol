/***************************************************************************
                          schemeview.cpp - view for Schems list
                             -------------------
    begin                : 2015.10.04
    copyright            : (C) 2015-2017 by atu
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

#include "dataset.h"
#include "schemeview.h"
#include "labowin.h"
#include "structview.h"


SchemeView::SchemeView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdListView( a_mod, a_par )
{
  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 190, 200, 200 ) );
  setPalette( s_pal );
}

void SchemeView::init_actions()
{
  // none?
}

bool SchemeView::editObj()
{
  Scheme *sch = qobject_cast<Scheme*>( getSelObj() );
  if( !sch || sch->objectName() == QSL("main_s") ) {
    return false;
  }

  LaboWin *mwin = MAINWIN;
  if( !mwin ) {
    return false;
  }

  LaboDoc *doc = par->getDocument();
  if( !doc ) { return false;  }


  QString fileName = doc->pathName();
  // like CommonSubwin
  QString wtit = QSL( "scheme: " ) % sch->objectName() % QSL(" - " ) % fileName;
  QMdiSubWindow *swin = mwin->findMdiByTitle( wtit, true ); // true = activate
  if( swin ) {
    return false;
  }

  auto sw = new StructSubwin( mwin, doc, sch );
  mwin->addChild( sw );
  return true;
}

