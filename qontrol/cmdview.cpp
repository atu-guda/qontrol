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
#include "commonsubwin.h"
#include "miscfun.h"
#include "addelemdia.h"

CmdView::CmdView( CommonSubwin *a_par, HolderData *a_storage )
  : QWidget( a_par ), storage( a_storage ), par( a_par )
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
  if( storage->isRoTree() ) {
    return false;
  }

  QString objName = sugg_name;
  sugg_name = QString(); // reset at once

  AddObjParams prm;
  prm.name = objName;

  HolderData *ho = SelectTypeDialog::askAndCreateObj( storage, this, prm );
  if( !ho ) {
    return false;
  }

  lastObjName = prm.name;
  return true;
}


bool CmdView::delObj()
{
  if( storage->isRoTree() ) {
    return false;
  }

  HolderData *ob = getSelObj();
  if( ! ob ) {
    return false;
  }

  QString oname = ob->objectName();

  if( confirmDelete( this, ob->getType(), oname ) ) {
    storage->delObj( oname );
    lastObjName = oname;
    handleSelChange();
    emit viewChanged();
    return true;
  };
  return false;
}


bool CmdView::editObj()
{
  // allow to view
  // if( storage->isRoTree() ) {
  //   return false;
  // }

  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }
  bool ok = ::editObj( this, selObj );
  if( ok ) {
    lastObjName = selObj->objectName();
    emit viewChanged();
    return true;
  }
  return false;
}


bool CmdView::renameObj()
{
  if( storage->isRoTree() ) {
    return false;
  }

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
      lastObjName = new_name;
      emit viewChanged();
      return true;
    }
  }
  return false;
}


bool CmdView::cloneObj()
{
  if( storage->isRoTree() ) {
    return false;
  }

  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }

  QString nm = selObj->objectName();
  QString new_name = storage->hintName( selObj->getType(), nm );
  bool ok;
  new_name = QInputDialog::getText( this, "Object:" + selObj->getFullName(),
      "Enter new name:", QLineEdit::Normal, new_name, &ok );

  if( !ok ) {
    return false;
  }

  if( storage->cloneObj( nm, new_name ) ) {
    lastObjName = new_name;
    // TODO: post_clone( obj )
    // model->reset();
    //
    emit viewChanged();
    return true;
  }

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
  if( storage->isRoTree() ) {
    return false;
  }

  QClipboard *clp = QApplication::clipboard();
  if( !clp ) {
    return false;
  }
  QString s = clp->text();
  int err_line, err_column;
  QString errstr;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    handleWarn( this, tr("Cannot parse clipboard string:\n%2\nLine %3 column %4.")
                .arg(errstr).arg(err_line).arg(err_column) );
    return false;
  }
  QDomElement ee = x_dd.documentElement();

  QString tagname = ee.tagName();
  if( tagname != "obj" ) {
    handleWarn( this, tr("element tag is not 'obj':  %2").arg( tagname ) );
    return false;
  }

  QString otype = ee.attribute( "otype" );
  QString oname = ee.attribute( "name" );
  oname = storage->hintName( otype, oname );

  bool ok;
  oname = QInputDialog::getText( this, "Object: " + oname,
      "Enter new name:", QLineEdit::Normal, oname, &ok );
  if( !ok ) {
    return false;
  }

  HolderData *ob = storage->addObjP( otype, oname );
  if( !ob  ) {
    handleError( this, QString("Fail to add Obj: %1 %2").arg(otype).arg(oname) );
    return false;
  }
  lastObjName = oname;
  if( !ob->fromDom( ee, errstr ) ) {
    handleWarn( this, tr("fail to set params:  %1").arg( errstr ) );
  }
  return true;
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
      tv->setItem( i, 3, new QTableWidgetItem(ho->getParm( QSL("vis_name") ) + " \""
                    + ho->getParm( QSL("descr") ) + "\"" ) );
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
  HolderData *selObj = getSelObj();
  if( !selObj ) {
    return false;
  }
  QString buf;

  auto dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": test1 ") + selObj->objectName() );

  QFontMetrics fm( dia->font() );
  int em = fm.width( 'W' );

  buf = selObj->toString();

  auto lay = new QVBoxLayout();

  auto la = new QLabel( dia );
  la->setText( buf );
  auto scroll = new QScrollArea( dia );
  scroll->setWidget( la );
  lay->addWidget( scroll );

  auto bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 60*em, 30*em ); // TODO: unmagic
  dia->exec();
  delete dia;
  lastObjName = selObj->objectName();
  emit viewChanged();
  return true;
}


bool CmdView::showObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}

bool CmdView::showDataObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}

bool CmdView::exportObj()
{
  qWarning() << "Unimplemented " << WHE;
  return false;
}


