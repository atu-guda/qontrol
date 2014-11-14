/***************************************************************************
                          labodoc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2014 by atu
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
#include "labodoc.h"
#include "labowin.h"
#include "laboview.h"

using namespace std;

LaboDoc::LaboDoc()
{
}

LaboDoc::~LaboDoc()
{
  // DBG1( "dbg: dtor" );
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


bool LaboDoc::newDocument()
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
    QMessageBox::critical( 0, "LaboDoc::newDocument",
      QString("Fail to insert model to root: "), 0,0,0 );
    return false;
  }


  // rootdata->dumpStruct();
  modified = false; is_nonamed = true;
  loaded_as_old = true; // TODO: change to false when new model be created by default

  // add to engine
  initEngine();

  return true;
}

bool LaboDoc::openDocumentXML(const QString &filename )
{
  QFile file( filename );
  if( !file.open( QFile::ReadOnly) ) {
    QMessageBox::warning(LaboWin::labowin, tr( PACKAGE ),
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
    QMessageBox::warning(LaboWin::labowin, tr( PACKAGE ),
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
      QMessageBox::critical( LaboWin::labowin, tr( PACKAGE ),
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


bool LaboDoc::saveDocumentXML( const QString &filename )
{
  if( rootdata == 0 )
    return false;

  QSaveFile file( filename );
  if ( ! file.open( QFile::WriteOnly )) {
    QMessageBox::warning( LaboWin::labowin, tr(PACKAGE),
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
  DBGx( "debug: rootdata: %p model: %p", rootdata, model );
  delete rootdata; rootdata = 0; model = 0;
}

bool LaboDoc::canCloseFrame( LaboView* pFrame )
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
             saveName = QFileDialog::getSaveFileName( LaboWin::labowin, tr("Save model"),
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

TModel* LaboDoc::getModel(void) const
{
  return model;
}

TRootData* LaboDoc::getRoot(void) const
{
  return rootdata;
}

bool LaboDoc::isModified() const
{
  int mmd;
  if( rootdata == 0 || model == 0  )
    return 0;
  mmd = model->getModified();
  return ( mmd & 1 );
}

void LaboDoc::fillRoot(void)
{
  if( rootdata == 0 ) {
    DBG1( "ERR: rootdata in null!!" );
    return;
  }
}

QString LaboDoc::runScript( const QString& script )
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

void LaboDoc::initEngine()
{
  if( !rootdata || ! model ) {
    DBG1( "ERR: no root or model" );
  }
  QScriptValue eng_root = eng.newQObject( rootdata );
  eng.globalObject().setProperty("root", eng_root );
  QScriptValue eng_model = eng.newQObject( model );
  eng.globalObject().setProperty("model", eng_model );
}

bool LaboDoc::migrateSumul()
{
  DBGx( "dbg: start..." );
  if( !rootdata || ! model ) {
    DBG1( "ERR: no root or model" );
    return false;
  }
  model->linkNames(); // to get corrent number of outputs and plots

  // migrate outputs
  ContOut *outs =  model->getElemT<ContOut*>("outs");
  if( !outs ) {
    DBGx( "err: outs not exist!" );
    return false;
  }

  if( outs->size() == 0 ) {
    int n_out = model->getNOutArr();
    DBGx( "dbg: migrate outputs (%d)", n_out );

    for( int i=0; i<n_out; ++i ) {
      TOutArr *arr_o = model->getOutArr( i );
      if( !arr_o ) {
        continue;
      }
      QString nm = arr_o->objectName();
      DBGx( "dbg: migrate (1) TOutArr \"%s\" %d", qP(nm), i );
      QString s = arr_o->toString();
      TOutArr *arr_n = outs->addObj<TOutArr>( nm );
      if( arr_n ) {
        arr_n->fromString( s );
        DBGx( "dbg: migrate (2) TOutArr \"%s\" %d", qP(nm), i );
      }
    }

  }


  // migrate plots
  ContGraph *plots =  model->getElemT<ContGraph*>("plots");
  if( !plots ) {
    DBGx( "err: plots not exist!" );
    return false;
  }

  if( plots->size() == 0 ) {
    int n_pl = model->getNGraph();

    for( int i=0; i<n_pl; ++i ) {
      TGraph *gr_o = model->getGraph( i );
      if( !gr_o ) {
        continue;
      }
      QString s = gr_o->toString();
      TGraph *gr_n = plots->addObj<TGraph>( gr_o->objectName() );
      if( gr_n ) {
        gr_n->fromString( s );
      }
    }

  }


  ContSimul *sims = model->getElemT<ContSimul*>("sims");
  if( ! sims ) {
    DBG1( "ERR: simulation array not exist!!!" ); // unlikely
    return false;
  }


  Simulation *sim0 = sims->getElemT<Simulation*>("sim0");
  if( ! sim0 ) {
    DBG1( "ERR: default simulations not created!!!" ); // unlikely
    return false;
  }

  // not so good indicator for conversion, but no silver bullet
  QString descr;
  sim0->getData( "descr", descr );
  if( ! descr.isEmpty() ) {
    return true;
  }

  DBG1( "info: migrate tmodel->simulations" );

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
  model->getData( "long_descr", descr );  sim0->setData( "descr", descr );

  return true;
}



// end of labodoc.cpp

