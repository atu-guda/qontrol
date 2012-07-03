/***************************************************************************
                          qmo2doc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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
// elements TODO: delete all this:  load so
#include "myelems1.h"

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
  //static const TDataInfo model_inf = {
  //  dtpObj, CLASS_ID_TModel, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0, 0, "model",  
  //  "main model", "" 
  //};
  // QMessageBox::critical( 0, "Debug", "QMo2Doc::newDocument", 0,0,0 );
  qDebug( "DBG: QMo2Doc::newDocument() point 0 rootdata=%p", rootdata );
  rootdata = new TRootData( 0 );
  qDebug( "DBG: QMo2Doc::newDocument() point 1 rootdata=%p", rootdata );
  fillRoot();
  // k = rootdata->add_obj( &model_inf );
  qDebug( "DBG: QMo2Doc::newDocument() point 2 rootdata=%p", rootdata );
  void *xmodel = rootdata->add_obj( "TModel", "model" );
  qDebug( "DBG: QMo2Doc::newDocument() point 3 rootdata=%p", rootdata );
  if( !xmodel ) {
    QMessageBox::critical( 0, "QMo2Doc::newDocument", 
      QString("Fail to insert model to root: "), 0,0,0 );
    return false;
  }

  qDebug( "DBG: QMo2Doc::newDocument() point 4 rootdata=%p", rootdata );
  model = static_cast<TModel*>( rootdata->getObj( "model" ) );
  qDebug( "debug: QMo2Doc::newDocument: point 5 rootdata: %p model: %p xmodel: %p", 
      rootdata, model, xmodel );
  rootdata->dumpStruct();
  modified = false; is_nonamed = true;
  loaded_as_old = true; // TODO: change to false when new model be created by default
  return true;
}

bool QMo2Doc::openDocument(const QString &filename )
{
  int k;
  char buf[MAX_INPUTLEN];
  ifstream is;
  errno = 0;
  is.open( qPrintable( filename ) );
  if( ! is.good() ) {
    QMessageBox::critical( 0, "openDocument Error:",
       QString("Fail to open file: ") + filename + QString(" :\n") +
         QString( strerror(errno) ), 
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
  buf[0] = buf[MAX_INPUTLEN-1] = 0;
  int sign_len = strlen( FILE_SIGN );
  is.getline( buf, sizeof(buf) );
  if( is.gcount() < sign_len )
  if( strncmp( FILE_SIGN, buf, sign_len ) != 0 )
    return false;
  qDebug( "debug: QMo2Doc::openDocument:(1) rootdata: %p model: %p", rootdata, model );
  if( rootdata != 0 )
    delete rootdata;
  rootdata = new TRootData( 0 );
  fillRoot();
  model = 0;
  k = rootdata->loadDatas( &is );
  is.close();
  if( k != 0 ) {
    delete rootdata; rootdata = 0; model = 0; 
    QMessageBox::critical( 0, "openDocument Error:",
       QString("Fail to load datas from file: ") + filename +
         QString(" :\n") + QString( strerror(errno) ), 
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
  model = static_cast<TModel*> (rootdata->getObj( "model" )); // TODO many models
  if( model == 0 ) {
    delete rootdata; rootdata = 0; model = 0; 
    QMessageBox::critical( 0, "openDocument Error:",
       QString("Fail to detect model in file: ") + filename,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
  qDebug( "debug: QMo2Doc::openDocument:(2) rootdata: %p model: %p", rootdata, model );
  modified = false; is_nonamed = false; loaded_as_old = true;
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
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
  m_filename = filename;
  loaded_as_old = false;
  
  // TODO: real actions

  return false; // FOR now
}


bool QMo2Doc::saveDocument(const QString &filename )
{
  int k;
  ofstream os;
  if( rootdata == 0 )
    return false;

  if( ! loaded_as_old ) {
    QMessageBox::critical( 0, "saveDocument Error",
        QString("New format used fo this model. Cannot save os old 'mo2' file."),
	QMessageBox::Ok, QMessageBox::NoButton,  QMessageBox::NoButton );
    return false;
  }

  errno = 0;
  os.open( filename.toLocal8Bit().constData() );
  if( ! os.good() ) {
    QMessageBox::critical( 0, "saveDocument Error",
        QString("Fail to open for write file: ") + filename +
	    QString(":\n") + QString( strerror(errno) ), 
	QMessageBox::Ok, QMessageBox::NoButton,  QMessageBox::NoButton );
    return false;
  };
  os << FILE_SIGN << '\n';
  k = rootdata->saveDatas( &os );
  os.close();
  if( k == 0 ) {
    modified = false;
    m_filename = filename;
    m_title = QFileInfo(filename).fileName();
    is_nonamed = false;
  } else {
    QMessageBox::critical( 0, "Fail to save file", filename, 
	QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
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
  if( loaded_as_old ) {
    textData = makeXMLold();
  } else {
    textData = makeXML();
  }
  out << textData;

  QApplication::restoreOverrideCursor();
  // modified = false; // TODO: set to false when XML be a main format
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  is_nonamed = false;

  return true; // for now
}

QString QMo2Doc::makeXMLold() const
{
  if( ! rootdata || ! model )
    return QString();

  QDomDocument dd("QontrolLabML");
  QDomNode pre_node = dd.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
  dd.insertBefore( pre_node, dd.firstChild() );

  QDomElement dd_root = dd.createElement("QontrolLabML");
  dd.appendChild( dd_root );

  QDomElement de = rootdata->toDom( dd );
  // QDomElement de = model->toDom( dd );
  dd_root.appendChild( de );
  
  // defaults
  //QDomElement defs = dd.createElement("defaults");

  //QDomElement def_model = dd.createElement( "def_model" );
  //QDomText def_model_name = dd.createTextNode( "main" );
  //def_model.appendChild( def_model_name );
  //defs.appendChild( def_model );

  //QDomElement def_exp = dd.createElement( "def_exp" );
  //QDomText def_exp_name = dd.createTextNode( "run1" );
  //def_exp.appendChild( def_exp_name );
  //defs.appendChild( def_exp );
  
  // Experiments
  //QDomElement exps = dd.createElement("experimemts");
  //dd_root.appendChild( exps );

  //QDomElement exp1 = dd.createElement("experimemt");
  //exps.appendChild(exp1);
  //exp1.setAttribute( "name", "run1");
  // TODO: data from model
  //QDomText te1a = dd.createTextNode("run1 data will be here");
  //exp1.appendChild(te1a);

  //QDomElement schems = dd.createElement("schems");
  //dd_root.appendChild( schems );
  
  //QDomElement sch1 = dd.createElement("sch");
  //sch1.setAttribute( "name", "main");
  // TODO: data from model
  //schems.appendChild(sch1);
  
  //dd_root.appendChild( defs );
  
  
  return dd.toString();
}

QString QMo2Doc::makeXML() const
{
  if( ! rootdata )
    return QString();
  
  QDomDocument dd("QontrolLabML");
  QDomNode pre_node = dd.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
  dd.insertBefore( pre_node, dd.firstChild() );
  // TODO: real work
  return dd.toString();
}

void QMo2Doc::deleteContents()
{
  qDebug( "debug: QMo2Doc::deleteContents rootdata: %p model: %p", rootdata, model );
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
	     saveName = QFileDialog::getSaveFileName( QMo2Win::qmo2win , tr("Save file"), 
		 QString::null, "Model files (*.mo2);;All files(*)" );
             if(saveName.isEmpty())
          	return false;
	   } else {
  	     saveName = pathName();
	   };     
           if( ! saveDocument( saveName ) ) {
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
    qDebug( "debug: QMo2Doc::fillRoot: rootdata in null!!" );
    return;
  }
  // TODO: must be loaded from ld.so selected by config file
  rootdata->regClass( TModel::getStaticClassInfo() );
  rootdata->regClass( TOutArr::getStaticClassInfo() );
  rootdata->regClass( TGraph::getStaticClassInfo() );
  rootdata->regClass( TElmLink::getStaticClassInfo() );
  rootdata->regClass( TMiso::getStaticClassInfo() );
  rootdata->regClass( TLinear::getStaticClassInfo() );
  rootdata->regClass( TSource::getStaticClassInfo() );
  rootdata->regClass( TSourceLin::getStaticClassInfo() );
  rootdata->regClass( TMultipl::getStaticClassInfo() );
  rootdata->regClass( TPid::getStaticClassInfo() );
  rootdata->regClass( THyst::getStaticClassInfo() );
  rootdata->regClass( TFuncPoly::getStaticClassInfo() );
  rootdata->regClass( TFuncTrans::getStaticClassInfo() );
  rootdata->regClass( TFuncMisc::getStaticClassInfo() );
  rootdata->regClass( TDelay::getStaticClassInfo() );
  rootdata->regClass( TExtrLatch::getStaticClassInfo() );
  rootdata->regClass( TVibro::getStaticClassInfo() );
  rootdata->regClass( TFriction::getStaticClassInfo() );
  rootdata->regClass( TAdjGen::getStaticClassInfo() );
  rootdata->regClass( TRandTrigg::getStaticClassInfo() );
  rootdata->regClass( TIntegrator::getStaticClassInfo() );
  rootdata->regClass( TLinP1Q0::getStaticClassInfo() );
  rootdata->regClass( TInputAny::getStaticClassInfo() );
  rootdata->regClass( TOutputAny::getStaticClassInfo() );
  rootdata->regClass( TCriterion::getStaticClassInfo() );
  rootdata->regClass( TLatch::getStaticClassInfo() );
  rootdata->regClass( TSwitch::getStaticClassInfo() );
  rootdata->regClass( TTrigger::getStaticClassInfo() );
  rootdata->regClass( TLogic::getStaticClassInfo() );
  rootdata->regClass( TRand::getStaticClassInfo() );
  rootdata->regClass( TLorenz::getStaticClassInfo() );
  rootdata->regClass( TRossler::getStaticClassInfo() );
  rootdata->regClass( TCounter::getStaticClassInfo() );
  rootdata->regClass( TCorrAnalysis::getStaticClassInfo() );
  rootdata->regClass( TFourier::getStaticClassInfo() );
}

// end of qmo2doc.cpp

