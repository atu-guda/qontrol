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

#include <QtWidgets>
#include <QDebug>

#include "dataset.h"
#include "cmdview.h"
#include "miscfun.h"

CmdView::CmdView( QWidget *a_par, HolderData *a_storage )
  : QWidget( a_par ), storage( a_storage )
{
}

QString CmdView::getSelName() const
{
  HolderData *ob = getSelObj();
  if( !ob ) {
    return QString();
  }
  return ob->objectName();
}

QModelIndex CmdView::currentIndex() const
{
  return QModelIndex();
}

bool CmdView::addObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::delObj()
{
  HolderData *ob = getSelObj();
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
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }
  bool ok = ::editObj( this, selObj );
  if( ok ) {
    emit viewChanged();
    return true;
  }
  return false;
}


bool CmdView::renameObj()
{
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }

  QString old_name = selObj->objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + selObj->getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );

  if( ok ) {
    if( storage->renameObj( old_name, new_name ) ) {
      emit viewChanged();
      return true;
    }
  }
  return false;
}


bool CmdView::cloneObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::cutObj()
{
  if( copyObj() ) {
    return delObj();
  }
  return false;
}


bool CmdView::copyObj()
{
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }

  QClipboard *clp = QApplication::clipboard();
  if( !selObj || !clp ) {
    return false;
  }
  QString s = selObj->toString();
  clp->setText( s );
  return true;
}


bool CmdView::pasteObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


bool CmdView::infoObj()
{
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Structure of ") + selObj->getFullName() );

  QFontMetrics fm( dia->font() );
  int em = fm.width( 'W' );

  auto lay = new QVBoxLayout();

  auto tv = new QTableWidget( 100, 6, dia );
  QStringList hlabels;
  hlabels << "Name" << "Type" << "Value" << "Descr" << "Target"<< "Flags";
  tv->setHorizontalHeaderLabels( hlabels );

  QObjectList childs = selObj->children();

  int i = 0;
  for( auto o :  childs ) {
    QObject *ob = o;
    tv->setItem( i, 0, new  QTableWidgetItem( ob->objectName() ) );
    if( ob->inherits("TDataSet" ) ) {
      TDataSet *ds = qobject_cast<TDataSet*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ds->getType()) );
      tv->setItem( i, 2, new QTableWidgetItem( ds->toString() ) );
    } else if( ob->inherits("HolderData" ) ) {
      HolderData *ho = qobject_cast<HolderData*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ho->getType() ) );
      tv->setItem( i, 2, new QTableWidgetItem(ho->toString() ) );
      tv->setItem( i, 3, new QTableWidgetItem(ho->getParm("vis_name") + " \""
                    + ho->getParm("descr" ) + "\"" ) );
      tv->setItem( i, 4, new QTableWidgetItem( ho->objectName() ) );
      tv->setItem( i, 5, new QTableWidgetItem( flags2str(ho->getFlags()) ) );

    } else { // unknown
      tv->setItem( i, 1, new QTableWidgetItem("???unknown???" ) );
      tv->setItem( i, 2, new QTableWidgetItem(ob->metaObject()->className() ) );
    }
    ++i;
  }

  lay->addWidget( tv );

  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 72*em, 50*em ); // TODO: adjust to inner table width
  dia->exec();
  delete dia;
  emit viewChanged();
  return true;

}


bool CmdView::showTreeObj()
{
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }
  ::showTree( this, selObj );
  emit viewChanged();
  return true;
}


bool CmdView::testObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


