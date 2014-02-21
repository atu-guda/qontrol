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
#include <QtWidgets>
#include <QtXml>

// application specific includes
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qmo2view.h"

using namespace std;

QMo2Doc::QMo2Doc()
{
}

QMo2Doc::~QMo2Doc()
{
  // DBG1( "dbg: dtor" );
  delete rootdata;
  rootdata = nullptr; model = nullptr; // model belong to rootdata
}


bool QMo2Doc::isLastView() const
{
  return true; // now the only view exist
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
    DBG1( "warn: non-null rootdata while creation new doc" );
    delete rootdata;
    rootdata = nullptr; model = nullptr;
  }

  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );
  model = rootdata->addObj<TModel>( "model" );
  if( !model ) {
    delete rootdata; rootdata = nullptr;
    QMessageBox::critical( 0, "QMo2Doc::newDocument",
      QString("Fail to insert model to root: "), 0,0,0 );
    return false;
  }

  if( ! createEmptySyms() ) {
    DBG1( "ERR: Fail to create sims - simulations container" );
    delete rootdata; rootdata = nullptr; model = nullptr;
    QMessageBox::critical( 0, "QMo2Doc::newDocument",
      QString("Fail to insert create sims in model: "), 0,0,0 );
    return false;
  }

  // rootdata->dumpStruct();
  modified = false; is_nonamed = true;
  loaded_as_old = true; // TODO: change to false when new model be created by default

  // add to engine
  initEngine();

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

  if( rootdata ) {
    DBG1( "warn: non-null rootdata while opening new doc" );
    delete rootdata;
    rootdata = nullptr; model = nullptr;
  }
  rootdata = new TRootData( "root", nullptr, 0, "root", "root of all objects" );

  model = 0;
  rootdata->suspendHandleStructChange();
  bool read_ok = rootdata->fromDom( obj_root, errstr );
  if( ! read_ok ) {
    // QString xxx = rootdata->toString();
    // DBG1q( xxx );
    delete rootdata;
    rootdata = nullptr;
    QMessageBox::critical( 0, "openDocumentXml Error:",
       QString("Fail parse file: ") + filename + " : " + errstr,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  }

  model = rootdata->getElemT<TModel*>( "model" );
  loaded_as_old = true;
  if( !model ) {
    delete rootdata; rootdata = 0; model = 0;
    QMessageBox::critical( 0, "openDocumentXML Error:",
       QString("Fail to detect model in file: ") + filename,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };

  if( ! migrateSumul() ) {
    delete rootdata; rootdata = 0; model = 0;
    QMessageBox::critical( 0, "openDocumentXML Error:",
       QString("Fail to migrate simulations in file: ") + filename,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };

  rootdata->resumeHandleStructChange();
  model->reset();
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  initEngine();

  return true;
}


bool QMo2Doc::saveDocumentXML( const QString &filename )
{
  if( rootdata == 0 )
    return false;

  QSaveFile file( filename );
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

QString QMo2Doc::runScript( const QString& script )
{
  QScriptValue res = eng.evaluate( script );
  QString r;
  if( eng.hasUncaughtException() ) {
    int line = eng.uncaughtExceptionLineNumber();
    r = "Error: uncaught exception at line " + QSN( line ) + " : \n";
  }
  r += res.toString();
  return r;
}

void QMo2Doc::initEngine()
{
  if( !rootdata || ! model ) {
    DBG1( "ERR: no root or model" );
  }
  QScriptValue eng_root = eng.newQObject( rootdata );
  eng.globalObject().setProperty("root", eng_root );
  QScriptValue eng_model = eng.newQObject( model );
  eng.globalObject().setProperty("model", eng_model );
}

bool QMo2Doc::migrateSumul()
{
  if( !rootdata || ! model ) {
    DBG1( "ERR: no root or model" );
    return false;
  }

  ContSimul *sims = model->getElemT<ContSimul*>("sims");
  if( sims )
    return true; // conversion not required

  DBG1( "info: migrate tmodel->simulations" );

  if( ! createEmptySyms() ) {
    return false;
  }

  sims = model->getElemT<ContSimul*>("sims");
  if( ! sims ) {
    DBG1( "ERR: simulations container not created!!!" ); // unlikely
    return false; //
  }

  Simulation *sim0 = sims->getElemT<Simulation*>("sim0");
  if( ! sim0 ) {
    DBG1( "ERR: default simulations not created!!!" ); // unlikely
    return false; //
  }

  double t = 100;
  int ti = 1000;
  // no check
  model->getData( "tt", &t );  sim0->setData( "T", t );
  model->getData( "nn", &ti );  sim0->setData( "N", ti );
  model->getData( "use_sync", &ti );  sim0->setData( "syncRT", ti );
  model->getData( "nl1", &ti );  sim0->setData( "N1", ti );
  model->getData( "nl2", &ti );  sim0->setData( "N2", ti );
  model->getData( "n_steps", &ti );  sim0->setData( "n_iosteps", ti );
  model->getData( "prm0s", &t );  sim0->setData( "prm0s", t );
  model->getData( "prm1s", &t );  sim0->setData( "prm1s", t );
  model->getData( "prm2s", &t );  sim0->setData( "prm2s", t );
  model->getData( "prm3s", &t );  sim0->setData( "prm3s", t );
  model->getData( "seed", &ti );  sim0->setData( "seed", ti );
  model->getData( "seedType", &ti );  sim0->setData( "seedType", ti );
  model->getData( "autoStart", &ti );  sim0->setData( "autoStart", ti );


  return true;
}

bool QMo2Doc::createEmptySyms()
{
  if( !rootdata || !model ) {
    DBG1( "ERR: no root or model" );
    return false;
  }

  ContSimul *sims = model->getElemT<ContSimul*>("sims");
  if( ! sims ) { // create only if not exist
    sims = model->addObj<ContSimul>( "sims" );
  }

  if( ! sims ) {
    DBG1( "ERR: Fail to create sims - simulatios container" );
    return false;
  }

  Simulation *sim0 = sims->getElemT<Simulation*>( "sim0" );
  if( sim0 )
    return true; // creation not required: all exits
  sim0 = sims->addObj<Simulation>( "sim0" );
  if( ! sim0 ) {
    DBG1( "ERR: Fail to create defaul simulation sim0" );
    return false;
  }
  return true;
}


// end of qmo2doc.cpp

