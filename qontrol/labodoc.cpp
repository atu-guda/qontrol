/***************************************************************************
                          labodoc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2015 by atu
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
  rootdata = nullptr; model = nullptr; // model belong to rootdata
}


bool LaboDoc::isLastView() const
{
  return true; // now the only view exist
}


void LaboDoc::setPathName( const QString &name )
{
  m_filename = name;
  m_title = QFileInfo(name).fileName();
}

const QString& LaboDoc::pathName() const
{
  return m_filename;
}

void LaboDoc::setTitle( const QString &title )
{
  m_title = title;
}

const QString &LaboDoc::title() const
{
  return m_title;
}

void LaboDoc::showError( const QString &s )
{
  handleError( LaboWin::win(), s ); // nullptr is ok - batch
}

void LaboDoc::showWarn( const QString &s )
{
  handleWarn( LaboWin::win(), s );
}



bool LaboDoc::newDocument()
{
  if( rootdata ) {
    qWarning() << "non-null rootdata while creation new doc" << WHE;
    delete rootdata;
    rootdata = nullptr; model = nullptr;
  }

  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );
  model = rootdata->getElemT<TModel*>( "model" );
  if( !model ) {
    delete rootdata; rootdata = nullptr;
    showError( QSL( "Fail to find model in root: " ) );
    return false;
  }


  // rootdata->dumpStruct();
  modified = false; is_nonamed = true;

  return true;
}

bool LaboDoc::openDocumentXML(const QString &filename )
{
  QFile file( filename );
  if( !file.open( QFile::ReadOnly) ) {
    showError( tr("Cannot read file %1: %2.").arg(filename).arg(file.errorString() ) );
    return false;
  }
  QTextStream in( &file );
  QString textData = in.readAll();

  QString errstr;
  QDomDocument dd;
  int err_line, err_column;

  if( ! dd.setContent( textData, false, &errstr, &err_line, &err_column ) ) {
    showError( tr("Cannot parse file %1:\n%2\nLine %3 column %4.")
                         .arg(filename).arg(errstr).arg(err_line).arg(err_column) );
    m_filename = "";
    return false;
  }
  QDomElement domroot = dd.documentElement();
  QDomElement obj_root;

  QDomNode cnode = domroot.firstChild();

  while( ! cnode.isNull() ) {
    if ( cnode.isElement()) {
      QDomElement ee = cnode.toElement();
      QString elname = ee.attribute( "name" );
      QString tagname = ee.tagName();
      if( elname == "root" || tagname == "obj" ) {
        obj_root = ee;
        break;
      }
      showError( tr("Bad first element: %1 %2 ").arg(tagname).arg(elname) );
      return false;
    }
    cnode = cnode.nextSibling();
  }

  m_filename = filename;

  if( rootdata ) {
    qWarning() << "non-null rootdata while opening new doc" << WHE;
    delete rootdata;
    rootdata = nullptr; model = nullptr;
  }
  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );

  model = nullptr;
  rootdata->suspendHandleStructChange();
  bool read_ok = rootdata->fromDom( obj_root, errstr );
  if( ! read_ok ) {
    delete rootdata;
    rootdata = nullptr;
    showError( QSL("Fail to parse file: ") % filename % " : " % errstr );
    return false;
  }

  model = rootdata->getElemT<TModel*>( "model" );

  if( !model ) {
    delete rootdata; rootdata = 0; model = 0;
    showError( QSL("Fail to detect model in file: ") % filename );
    return false;
  };

  rootdata->resumeHandleStructChange();
  model->reset();
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true;
}


bool LaboDoc::saveDocumentXML( const QString &filename )
{
  if( rootdata == 0 ) {
    return false;
  }

  QSaveFile file( filename );
  if ( ! file.open( QFile::WriteOnly )) {
    showError( tr("Cannot write file %1:\n%2.").arg(filename).arg(file.errorString()) );
    return false;
  }
  file.setPermissions( QFileDevice::ReadUser | QFileDevice::WriteUser
           | QFileDevice::ReadGroup | QFileDevice::ReadOther );
  QTextStream out(&file);
  out.setCodec("UTF-8");
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString textData;
  textData = makeXML();
  out << textData;
  QApplication::restoreOverrideCursor();

  if( ! file.commit() ) {
    return false;
  }

  modified = false;
  model->setUnModified();
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true;
}

QString LaboDoc::makeXML() const
{
  if( ! rootdata  )
    return QString();

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


void LaboDoc::deleteContents()
{
  delete rootdata; rootdata = 0; model = 0;
}

bool LaboDoc::canCloseFrame( LaboView* pFrame )
{
  if( prog_opts.batch || !isModified()  ||  !isLastView() ) {
    return true;
  }

  bool ret = false;
  QString saveName;
  switch( QMessageBox::information( pFrame, title(),
        tr("The current file has been modified.\n"
          "Do you want to save it?"),
        QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel )) {
    case QMessageBox::Yes:
      if( is_nonamed ) {
        saveName = QFileDialog::getSaveFileName( LaboWin::win(), tr("Save model"),
            QString::null, "Model *.qol files (*.qol);;All files(*)" );
        if( saveName.isEmpty() ) {
          return false;
        }
      } else {
        saveName = pathName();
      };

      ret = false;
      if( ! saveDocumentXML( saveName ) ) {
        if( QMessageBox::critical( pFrame, tr("I/O Error !"),
              tr( "Could not save the current document !\n" "Close anyway ?" ),
              QMessageBox::Yes ,QMessageBox::No ) == QMessageBox::Yes ) {
          ret=true;
        };
      } else {
        ret=true;
      };
      break;
    case QMessageBox::No:
      ret=true;
      break;
    case QMessageBox::Cancel:
    default:
      ret=false;
      break;
  }; // switch

  return ret;
}

TModel* LaboDoc::getModel() const
{
  return model;
}

TRootData* LaboDoc::getRoot() const
{
  return rootdata;
}

bool LaboDoc::isModified() const
{
  int mmd;
  if( !rootdata  ||  !model  ) {
    return false;
  }
  mmd = model->getModified();
  return ( mmd & 1 );
}

void LaboDoc::fillRoot()
{
  if( rootdata == 0 ) {
    qCritical() << "rootdata is null!!" << WHE;
    return;
  }
}


// end of labodoc.cpp

