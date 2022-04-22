/***************************************************************************
                          labodoc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2022 by atu
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

#include <cerrno>
#include <fstream>
#include <iostream>

#include <QtWidgets>
#include <QtXml>

#include "miscfun.h"
#include "prog_opts.h"
#include "labodoc.h"
#include "labowin.h"
#include "laboview.h"
#include "scheme.h"

using namespace std;

LaboDoc::LaboDoc()
{
}

LaboDoc::~LaboDoc()
{
  delete rootdata;
  rootdata = nullptr;
}


void LaboDoc::setPathName( const QString &name )
{
  m_filename = name;
  // m_title = QFileInfo(name).fileName();
}

const QString& LaboDoc::pathName() const
{
  return m_filename;
}

QString LaboDoc::getFilePath() const
{
  return QFileInfo( m_filename ).canonicalFilePath();
}

QString LaboDoc::getFileBase() const
{
  return QFileInfo( m_filename ).baseName();
}



bool LaboDoc::newDocument()
{
  if( rootdata ) {
    qWarning() << QSL("non-null rootdata while creation new doc") << WHE;
    delete rootdata;
    rootdata = nullptr;
  }

  rootdata = new TRootData( QSL("root"), nullptr, 0, QSL("root"), QSL("root of all objects") );
  rootdata->setDoc( this );

  // rootdata->dumpStruct();
  is_nonamed = true;
  rootdata->setUnModified();

  return true;
}

bool LaboDoc::openDocument( const QString &filename )
{
  QFile file( filename );
  if( !file.open( QFile::ReadOnly) ) {
    HANDLE_ERROR_W( MAINWIN, QSL("Cannot read file %1: %2.").arg( filename, file.errorString() ) );
    return false;
  }

  QString errstr;
  QDomDocument dd;
  int err_line, err_column;

  if( ! dd.setContent( &file, &errstr, &err_line, &err_column ) ) {
    HANDLE_ERROR_W( MAINWIN, QSL("Cannot parse file %1:\n%2\nLine %3 column %4.")
                         .arg( filename, errstr, QSN(err_line), QSN(err_column) ) );
    m_filename = QES;
    return false;
  }
  QDomElement domroot = dd.documentElement();
  QDomElement obj_root;

  QDomNode cnode = domroot.firstChild();

  while( ! cnode.isNull() ) {
    if ( cnode.isElement()) {
      QDomElement ee = cnode.toElement();
      QString elname = ee.attribute( QSL("name") );
      QString tagname = ee.tagName();
      if( elname == QSL("root") || tagname == QSL("obj") ) {
        obj_root = ee;
        break;
      }
      HANDLE_ERROR_W( MAINWIN, QSL("Bad first element: %1 %2 ").arg( tagname, elname ) );
      return false;
    }
    cnode = cnode.nextSibling();
  }

  m_filename = filename;

  if( rootdata ) {
    qWarning() << QSL("non-null rootdata while opening new doc") << WHE;
    delete rootdata;
    rootdata = nullptr;
  }
  rootdata = new TRootData( QSL("root"), nullptr, 0, QSL("root"), QSL("root of all objects") );
  rootdata->setDoc( this );

  rootdata->suspendHandleStructChange();
  bool read_ok = rootdata->fromDom( obj_root, errstr );
  if( ! read_ok ) {
    delete rootdata;
    rootdata = nullptr;
    HANDLE_ERROR_W( MAINWIN, QSL("Fail to parse file: ") % filename % " : " % errstr );
    return false;
  }

  rootdata->postFileRead();
  rootdata->resumeHandleStructChange();
  rootdata->setUnModified();
  rootdata->reset();

  m_filename = filename;
  // m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true;
}


bool LaboDoc::saveDocument( bool forceNewName )
{
  if( !rootdata ) {
    return false;
  }
  QWidget *mwin = MAINWIN;

  QString fn = m_filename;
  bool isNewName = false;
  if( ( is_nonamed || forceNewName )  &&  ! prog_opts.batch ) {
    fn = QFileDialog::getSaveFileName( mwin, tr("Save model"),
           QString(), model_files_sel );
    if( fn.isEmpty() ) {
      return false;
    }
    QFileInfo fi( fn );
    QString pfn = fi.fileName();
    if( ! pfn.contains('.') ) {
      fn += model_file_suff;
    }
    isNewName = true;
  }

  if( isNewName ) {
    QFileInfo fi( fn );
    if( fi.exists() ) {
      auto rc =  QMessageBox::information( mwin, "Save model",
          tr("The file \"%1\" already exists.\n"
            "Do you want to overwrite it it?").arg( fn ),
          QMessageBox::Yes, QMessageBox::No );

      if( rc == QMessageBox::No ) { // exit w/o save
        return false;
      }
    }
  }

  QSaveFile file( fn );
  if ( ! file.open( QFile::WriteOnly )) {
    HANDLE_ERROR_W( MAINWIN, QSL("Cannot write file %1:\n%2.").arg( fn, file.errorString() ) );
    return false;
  }
  auto perms =  file.permissions() | QFileDevice::ReadUser | QFileDevice::WriteUser;
  perms &= ~( QFileDevice::ExeUser | QFileDevice::ExeGroup | QFileDevice::ExeOther );
  file.setPermissions( perms );
  QTextStream out( &file );
  out.setCodec("UTF-8");
  QApplication::setOverrideCursor( Qt::WaitCursor );

  QString textData;
  textData = toString();
  out << textData;
  QApplication::restoreOverrideCursor();

  if( ! file.commit() ) {
    HANDLE_ERROR_W( MAINWIN, QSL("Cannot write file %1:\n%2.").arg( fn, file.errorString() ) );
    return false;
  }

  rootdata->setUnModified();
  m_filename = fn;
  // m_title = QFileInfo( fn ).fileName();
  is_nonamed = false;

  return true;
}

QString LaboDoc::toString() const
{
  if( ! rootdata  ) {
    return QString();
  }

  QDomDocument dd("QontrolLabML");
  QDomNode pre_node = dd.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
  dd.insertBefore( pre_node, dd.firstChild() );

  QDomElement dd_root = dd.createElement("QontrolLabML");
  dd.appendChild( dd_root );

  if( rootdata ) {
    QDomElement de = rootdata->toDom( dd );
    dd_root.appendChild( de );
  }

  return dd.toString();
}


bool LaboDoc::canCloseFrame( LaboView* pFrame )
{
  if( prog_opts.batch || !isModified()  ) {
    return true;
  }

  bool ret = false;
  auto rc =  QMessageBox::information( pFrame, "Close model",
        tr("The current file has been modified.\n"
          "Do you want to save it?"),
        QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel );

  if( rc == QMessageBox::No ) { // exit w/o save
    return true;
  }
  if( rc == QMessageBox::Cancel ) { // no exit
    return false;
  }

  ret = false;
  if( ! saveDocument( false ) ) {
    if( QMessageBox::critical( pFrame, QSL("I/O Error !"),
          QSL( "Could not save the current document !\n" "Close anyway ?" ),
          QMessageBox::Yes ,QMessageBox::No ) == QMessageBox::Yes ) {
      ret=true;
    };
  } else {
    ret=true;
  };

  return ret;
}


bool LaboDoc::isModified() const
{
  if( !rootdata ) {
    return false;
  }
  int mmd = rootdata->getModified();
  return mmd;
}


// end of labodoc.cpp

