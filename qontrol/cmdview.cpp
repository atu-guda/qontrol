/***************************************************************************
                      cmdview.cpp  -  common base for views vith std actions
                             -------------------
    begin                : 2015.10.27
    copyright            : (C) 2015-2020 by atu
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

#include "prog_opts.h"
#include "dataset.h"
#include "cmdview.h"
#include "commonsubwin.h"
#include "miscfun.h"
#include "addelemdia.h"
#include "pastedialog.h"
#include "stringtablemodel.h"
#include "datatableview.h"

CmdView::CmdView( CommonSubwin *a_par, HolderData *a_storage )
  : QWidget( a_par ), storage( a_storage ), par( a_par )
{
}

void CmdView::focusInEvent( QFocusEvent *ev )
{
  emit focusChanged();
  QWidget::focusInEvent( ev );
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
  QString new_name = QInputDialog::getText( this, QSL("Rename:") % selObj->getFullName(),
      QSL("Enter new name:"), QLineEdit::Normal, old_name, &ok );

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
  new_name = QInputDialog::getText( this, QSL("Object:") + selObj->getFullName(),
      QSL("Enter new name:"), QLineEdit::Normal, new_name, &ok );

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

  setClipboardStr( selObj->toString() );
  return true;
}


bool CmdView::pasteObj()
{
  if( storage->isRoTree() ) {
    return false;
  }

  QString s = getClipboardStr();
  int err_line, err_column;
  QString errstr;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    HANDLE_WARN( QSL("Cannot parse clipboard string:\n%2\nLine %3 column %4.").arg( errstr, QSN(err_line), QSN(err_column) ) );
    return false;
  }
  QDomElement ee = x_dd.documentElement();

  QString tagname = ee.tagName();
  if( tagname != QSL("obj") ) {
    HANDLE_WARN( QSL("element tag is not 'obj':  %2").arg( tagname ) );
    return false;
  }

  QString otype = ee.attribute( QSL("otype") );
  QString oname = ee.attribute( QSL("name") );

  PasteObjParams pp;
  pp.old_name = oname % QSL(" type(") % otype % QSL(")");
  pp.new_name = storage->hintName( otype, oname );

  if( ! PasteDialog::getPasteData( pp, this ) ) {
    return false;
  }
  oname = pp.new_name;

  HolderData *ob = storage->addObjP( otype, oname );
  if( !ob  ) {
    HANDLE_ERROR( QSL("Fail to add Obj: %1 %2").arg( otype, oname ) );
    return false;
  }
  lastObjName = oname;
  if( !ob->fromDom( ee, errstr ) ) {
    HANDLE_WARN( QSL("fail to set params:  %1").arg( errstr ) );
  }

  // TMP: TODO: use postPaste
  LinkedObj *el = qobject_cast<LinkedObj*>( ob );
  if( el && pp.iterateSrc ) {
    el->iterateSources( pp.dn );
  }
  return true;
}

bool CmdView::iterateSources()
{
  LinkedObj *el = qobject_cast<LinkedObj*>( getSelObj() );
  if( !el ) {
    return false;
  }
  bool ok = false;
  int dn = QInputDialog::getInt( this, QSL("Input source step"), QSL("dN"), 1, -2147483647, 2147483647, 1, &ok );
  if( ok ) {
    el->iterateSources( dn );
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
  dia->setWindowTitle( QSL( PACKAGE ": Structure of " ) + selObj->getFullName() );

  QStringList hlabels;
  hlabels << QSL("Name") << QSL("Type") << QSL("Value") << QSL("Description") << QSL("Flags"); // << QSL("x")

  QList<QStringList> d;

  QObjectList childs = selObj->children();

  for( auto ob :  childs ) {
    HolderData *ho = qobject_cast<HolderData*>(ob);
    QStringList oi;
    oi << ob->objectName();
    if( ho ) {
      oi << ho->getType()
         << ho->textVisual()
         << ( ho->getParm( QSL("vis_name") ) + QSL(" \"") + ho->getParm( QSL("descr") ) + QSL("\"") )
         <<  flags2str(ho->getFlags());

    } else { // unknown
      oi << QSL("???unknown???")
         << ob->metaObject()->className()
         << QSL("?")
         << QSL("?");
    }
    d.append( oi );
  }


  QFontMetrics fm( dia->font() );
  int em = fm.horizontalAdvance( QSL("W") );

  auto lay = new QVBoxLayout();

  auto mod = new StringTableModel( &d, &hlabels, this );

  auto tv = new DataTableView( this );
  tv->setModel( mod );

  lay->addWidget( tv );

  auto bt_ok = new QPushButton( QSL("Done"), dia );
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 85*em, 50*em );
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
  dia->setWindowTitle( QSL( PACKAGE ": test1 " ) + selObj->objectName() );

  QFontMetrics fm( dia->font() );
  int em = fm.horizontalAdvance( QSL("W") );

  buf = selObj->toString();

  auto lay = new QVBoxLayout();

  auto la = new QLabel( dia );
  la->setText( buf );
  auto scroll = new QScrollArea( dia );
  scroll->setWidget( la );
  lay->addWidget( scroll );

  auto bt_ok = new QPushButton( QSL("Done"), dia);
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
  qWarning() << QSL("Unimplemented") << WHE;
  return false;
}

bool CmdView::showDataObj()
{
  qWarning() << QSL("Unimplemented") << WHE;
  return false;
}

bool CmdView::exportObj()
{
  qWarning() << QSL("Unimplemented") << WHE;
  return false;
}


