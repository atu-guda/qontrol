/***************************************************************************
                          qmo2doc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2013 by atu
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
// include files for Qt
#include <QtGui>
#include <QtXml>

// application specific includes
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qmo2view.h"

using namespace std;

QMo2Doc::QMo2Doc()
  : modified(false), m_title("?unknown"), m_filename(), 
    pViewList( new  QList<QMo2View> ),
    rootdata(nullptr), model(nullptr), is_nonamed(true),
    loaded_as_old(true)
{
  //pViewList->setAutoDelete( false );
}

QMo2Doc::~QMo2Doc()
{
  DBG1( "dbg: dtor" );
  delete pViewList; pViewList = 0;
  delete rootdata; rootdata = 0; model = 0; // model belong to rootdata
}

void QMo2Doc::addView( QMo2View * /*view*/ )
{
  /* TODO: REVIVE!!!
  pViewList->append( *view );
  changedViewList();
  */
}

void QMo2Doc::removeView( QMo2View * /*view*/ )
{
  /* TODO: REVIVE!!!
  pViewList->remove( *view );
  if( !pViewList->isEmpty() )
    changedViewList();
  else
    deleteContents();
  */
}

void QMo2Doc::changedViewList()
{
  //QMo2View *w;
  /*
  if( (int)pViewList->count() == 1 ) {
    w = &(pViewList->first());
    w->setCaption( m_title );
  } else {
    int i;
    for( i=1, w=&(pViewList->first());  w!=0;  i++, w = &(pViewList->next()) )
      w->setCaption( QString( m_title + ":%1" ).arg(i) );
  };
  */
}

bool QMo2Doc::isLastView() const
{
  return true; // now the only view exist
}


void QMo2Doc::updateAllViews( QMo2View * /*sender*/ )
{
  // TODO: REVIVE!!!
  /*
  QMo2View *w;
  if( pViewList == 0 ) 
    return;
  for( w=pViewList->first(); w!=0; w=pViewList->next() ) {
     w->update( sender );
  };
  */
}

void QMo2Doc::setPathName( const QString &name )
{
  m_filename = name;
  m_title = QFileInfo(name).fileName();
}

const QString& QMo2Doc::pathName() const
{
  return m_filename;
}

void QMo2Doc::setTitle( const QString &title )
{
  m_title = title;
}

const QString &QMo2Doc::title() const
{
  return m_title;
}


bool QMo2Doc::newDocument()
{
  if( rootdata ) {
    delete rootdata; rootdata = 0; model = 0;
  }
  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );
  model = qobject_cast<TModel*>(rootdata->add_obj( "TModel", "model" ));
  if( !model ) {
    QMessageBox::critical( 0, "QMo2Doc::newDocument", 
      QString("Fail to insert model to root: "), 0,0,0 );
    return false;
  }

  // rootdata->dumpStruct();
  modified = false; is_nonamed = true;
  loaded_as_old = true; // TODO: change to false when new model be created by default
  return true;
}

bool QMo2Doc::openDocumentXML(const QString &filename )
{
  QFile file( filename );
  if( !file.open( QFile::ReadOnly) ) {
    QMessageBox::warning(QMo2Win::qmo2win, tr( PACKAGE ),
                         tr("Cannot read file %1:\n%2.")
                         .arg(filename)
                         .arg(file.errorString()));
    return false;
  }
  QTextStream in( &file );
  QString textData = in.readAll();

  QString errstr;
  QDomDocument dd;
  int err_line, err_column;

  if( ! dd.setContent( textData, false, &errstr, &err_line, &err_column ) ) {
    QMessageBox::warning(QMo2Win::qmo2win, tr( PACKAGE ),
                         tr("Cannot parse file %1:\n%2\nLine %3 column %4.")
                         .arg(filename)
                         .arg(errstr).arg(err_line).arg(err_column) );
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
      //DBGx( "dbg: Element tagname: \"%s\" name: \"%s\"", qP(tagname), qP(elname) );
      if( elname == "root" || tagname == "obj" ) {
        obj_root = ee;
        break;
      }
      QMessageBox::critical( QMo2Win::qmo2win, tr( PACKAGE ),
        tr("Bad first element: %1 %2 ").arg(tagname).arg(elname) );
      return false;
    }
    cnode = cnode.nextSibling();
  }

  m_filename = filename;

  if( rootdata != 0 )
    delete rootdata;
  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );

  model = 0;
  bool read_ok = rootdata->fromDom( obj_root, errstr );
  if( ! read_ok ) {
    QMessageBox::critical( 0, "openDocumentXml Error:",
       QString("Fail parse file: ") + filename + " : " + errstr,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  }
  
  model = static_cast<TModel*> (rootdata->getObj( "model", "TModel" )); 
  if( model ) {
    loaded_as_old = true; 
  } else if ( (model = static_cast<TModel*>(rootdata->getObj( "schems.main", "TModel" ))) != nullptr ) {
    loaded_as_old = false;
    // and many more actions
  } else {
    delete rootdata; rootdata = 0; model = 0; 
    QMessageBox::critical( 0, "openDocumentXML Error:",
       QString("Fail to detect model in file: ") + filename,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
  model->reset();
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true; 
}


bool QMo2Doc::saveDocumentXML( const QString &filename )
{
  if( rootdata == 0 )
    return false;
  
  QFile file( filename );
  if ( ! file.open( QFile::WriteOnly )) {
    QMessageBox::warning( QMo2Win::qmo2win, tr(PACKAGE),
                         tr("Cannot write file %1:\n%2.")
                         .arg(filename)
                         .arg(file.errorString()));
    return false;
  }
  QTextStream out(&file);
  out.setCodec("UTF-8");
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString textData;
  textData = makeXML();
  out << textData;

  QApplication::restoreOverrideCursor();
  modified = false;
  model->setUnModified();
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true;
}

QString QMo2Doc::makeXML() const
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


void QMo2Doc::deleteContents()
{
  DBGx( "debug: rootdata: %p model: %p", rootdata, model );
  delete rootdata; rootdata = 0; model = 0;
}

bool QMo2Doc::canCloseFrame( QMo2View* pFrame ) 
{
  if( !isLastView()  )
    return true;
  bool ret = false;
  if( isModified() ) {
    QString saveName;
    switch( QMessageBox::information(pFrame, title(), 
             tr("The current file has been modified.\n"
                 "Do you want to save it?"), 
	     QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel )) {
      case QMessageBox::Yes:
  	   if( is_nonamed ) {
             saveName = QFileDialog::getSaveFileName( QMo2Win::qmo2win, tr("Save model"),
                 QString::null, "Model *.qol files (*.qol);;All files(*)" );
             if(saveName.isEmpty())
          	return false;
	   } else {
  	     saveName = pathName();
	   };     
           if( ! saveDocumentXML( saveName ) ) {
 	     switch(QMessageBox::critical(pFrame, tr("I/O Error !"), 
	             tr("Could not save the current document !\n" "Close anyway ?"),
		     QMessageBox::Yes ,QMessageBox::No)) {
 			case QMessageBox::Yes:
 			  ret=true;
 			case QMessageBox::No:
 			  ret=false;
 	     }; // switch
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
  } else {
    ret=true;
  }; // if(isModified())

  return ret;
}

TModel* QMo2Doc::getModel(void) const
{
  return model;
}

TRootData* QMo2Doc::getRoot(void) const
{
  return rootdata;
}

bool QMo2Doc::isModified() const
{ 
  int mmd;
  if( rootdata == 0 || model == 0  )
    return 0;
  mmd = model->getModified();
  return ( mmd & 1 );
}

void QMo2Doc::fillRoot(void)
{
  if( rootdata == 0 ) {
    DBG1( "ERR: rootdata in null!!" );
    return;
  }
}

// end of qmo2doc.cpp

