/***************************************************************************
                      cmdview.cpp  -  common base for views vith std actions
                             -------------------
    begin                : 2015.10.27
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

#include <QDebug>

#include "dataset.h"
#include "cmdview.h"
#include "miscfun.h"

CmdView::CmdView( QWidget *a_par, TDataSet *a_storage )
  : QWidget( a_par ), storage( a_storage )
{
}


bool CmdView::addObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::delObj()
{
  TDataSet *ob = getSelObj();
  if( ! ob ) {
    return false;
  }

  QString oname = ob->objectName();

  if( confirmDelete( this, ob->getType(), oname ) ) {
    storage->delObj( oname );
    handleSelChange();
    emit viewChanged();
    return true;
  };
  return false;
}


bool CmdView::editObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::renameObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::cloneObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::cutObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::copyObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::pasteObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::infoObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::showTreeObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::testObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


