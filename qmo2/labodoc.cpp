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
#include "scheme.h"

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

bool LaboDoc::migrateSumul() // TODO: remove after migration
{
  DBGx( "dbg: start..." );
  if( !rootdata || ! model ) {
    DBG1( "ERR: no root or model" );
    return false;
  }

  // migrate elements
  ContScheme *schems = model->getElemT<ContScheme*>("schems");
  if( !schems ) {
    DBGx( "err: schems not exist!" );
    return false;
  }

  Scheme *main_s = schems->getElemT<Scheme*>("main_s");
  if( !schems ) {
    DBGx( "err: main scheme not exist!" );
    return false;
  }

  int n_el = 0;
  for( auto o : main_s->children() ) {
    TMiso *el = qobject_cast<TMiso*>(o);
    if( el ) {
      ++n_el;
      DBGx( "dbg: found \"%s\" elem in main, not migrating", qP( el->objectName() ) );
      break;
    }
  }

  DBGx( "dbg: n_el = %d size: %d", n_el, main_s->size() );

  if( n_el == 0 ) {
    for( auto c : model->children() ) {
      TMiso *ob = qobject_cast<TMiso*>(c);
      if( !ob ) {
        continue;
      }
      DBGx( "dbg: migrating \"%s\" element", qP( ob->objectName() ) );
      QString s;

      s = ob->toString();
      HolderData *ob_new = main_s->add_obj( ob->getType(), ob->objectName() );
      if( !ob_new ) {
        DBGx( "warn: fail to migrate elem \"%s\" type \"%s\"",
            qP(ob->objectName()), qP(ob->getType()) );
        continue;
      }
      ob_new->fromString( s );
    };
  }

  // migrate outputs
  ContOut *outs =  model->getElemT<ContOut*>("outs");
  if( !outs ) {
    DBGx( "err: outs not exist!" );
    return false;
  }

  if( outs->size() == 0 ) {
    for( auto c : model->children() ) {
      HolderData *ob = qobject_cast<HolderData*>(c);
      QString tp = ob->getType();
      if( ob->isChildOf("TOutArr") ) {
        QString s = ob->toString();
        TOutArr *out_n = outs->addObj<TOutArr>( ob->objectName() );
        if( out_n ) {
          out_n->fromString( s );
        }
      }
    };

  }


  // migrate plots
  ContGraph *plots =  model->getElemT<ContGraph*>("plots");
  if( !plots ) {
    DBGx( "err: plots not exist!" );
    return false;
  }

  if( plots->size() == 0 ) {
    for( auto c : model->children() ) {
      HolderData *ob = qobject_cast<HolderData*>(c);
      QString tp = ob->getType();
      if( ob->isChildOf("TGraph") ) {
        QString s = ob->toString();
        TGraph *gr_n = plots->addObj<TGraph>( ob->objectName() );
        if( gr_n ) {
          gr_n->fromString( s );
        }
      }
    };

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
  if( descr.isEmpty() ) {

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
    model->getData( "prm0d", &t );  sim0->setData( "prm0d", t );
    model->getData( "prm1d", &t );  sim0->setData( "prm1d", t );
    model->getData( "seed", &ti );  sim0->setData( "seed", ti );
    model->getData( "seedType", &ti );  sim0->setData( "seedType", ti );
    model->getData( "autoStart", &ti );  sim0->setData( "autoStart", ti );
    model->getData( "long_descr", descr );  sim0->setData( "descr", descr );
    sim0->post_set();
  }

  // remove old elements from model
  QStringList deads;
  for( auto c : model->children() ) {
    HolderData *ob = qobject_cast<HolderData*>(c);
    QString tp = ob->getType();
    if( ob->isChildOf("TMiso") || ob->isChildOf("TOutArr") || ob->isChildOf("TGraph") ) {
      deads << ob->objectName();
    }
  };
  for( auto nm : deads ) {
    model->del_obj( nm );
  }

  return true;
}



// end of labodoc.cpp

