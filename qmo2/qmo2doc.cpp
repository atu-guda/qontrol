/***************************************************************************
                          qmo2doc.cpp  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000 by atu
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
#include <qcstring.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qwidget.h>
#include <qmsgbox.h>
#include <qfiledialog.h>


// application specific includes
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qmo2view.h"
// elements TODO: delete all this:  load so
#include "myelems1.h"

using namespace std;

QMo2Doc::QMo2Doc()
{
  pViewList = new QList<QMo2View>;
  pViewList->setAutoDelete( false );
  rootdata = 0; model = 0; is_nonamed = true;
}

QMo2Doc::~QMo2Doc()
{
  delete pViewList; pViewList = 0;
  delete rootdata; rootdata = 0; model = 0; // model belong to rootdata
}

void QMo2Doc::addView( QMo2View *view )
{
  pViewList->append( view );
  changedViewList();
}

void QMo2Doc::removeView( QMo2View *view )
{
  pViewList->remove( view );
  if( !pViewList->isEmpty() )
    changedViewList();
  else
    deleteContents();
}

void QMo2Doc::changedViewList()
{	
  QMo2View *w;
  if( (int)pViewList->count() == 1 ) {
    w = pViewList->first();
    w->setCaption( m_title );
  } else {
    int i;
    for( i=1, w=pViewList->first();  w!=0;  i++, w = pViewList->next() )
      w->setCaption( QString( m_title + ":%1" ).arg(i) );	
  };
}

bool QMo2Doc::isLastView() const
{
  int k;
  if( pViewList == 0 ) 
    return 0;
  k = (int) (pViewList->count());
  return ( k == 1 );
}


void QMo2Doc::updateAllViews( QMo2View *sender )
{
  QMo2View *w;
  if( pViewList == 0 ) 
    return;
  for( w=pViewList->first(); w!=0; w=pViewList->next() ) {
     w->update( sender );
  };
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


void QMo2Doc::closeDocument()
{
  QMo2View *w;
  if( pViewList == 0 )
    return;
  if( !isLastView() ) {
    for( w = pViewList->first();  w != 0;  w = pViewList->next() ) {
     if( ! w->close() )
 	break;
    };
  };
  if( isLastView() ) {
    w = pViewList->first();
    w->close();
  };
}

bool QMo2Doc::newDocument()
{
  int k;
  static const TDataInfo model_inf = {
    dtpObj, CLASS_ID_TModel, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0, 0, "model",  
    "main model", "" 
  };
  // QMessageBox::critical( 0, "Debug", "QMo2Doc::newDocument", 0,0,0 );
  rootdata = new TRootData( 0 );
  fillRoot();
  k = rootdata->add_obj( &model_inf );
  if( k != 0 )
  QMessageBox::critical( 0, "QMo2Doc::newDocument", 
    QString("Fail to insert model to root: ") 
    + QString::number(k), 0,0,0 );
  model = static_cast<TModel*>( rootdata->getObj( "model" ) );
  modified = false; is_nonamed = true;
  return true;
}

bool QMo2Doc::openDocument(const QString &filename, const char * /*format */)
{
  int i, n, k;
  char buf[MAX_INPUTLEN];
  const TDataInfo *inf;
  QCString fn;
  ifstream is;
  fn = filename.local8Bit();
  //QMessageBox::critical( 0, "Debug: QMo2Doc::openDocument",
  //                       filename + ": " + QString(fn), 0,0,0 );
  errno = 0;
  is.open( fn );
  if( ! is.good() ) {
    cerr << "fail to open file " << fn << ": " << strerror(errno) << '\n';
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
  n = rootdata->getN();
  for( i=0; i<n; i++ ) {
    inf = rootdata->getDataInfo( i );
    if( inf->tp == dtpObj && inf->subtp == CLASS_ID_TModel ) {
      model = static_cast<TModel*> (rootdata->getObj( i ));
      break;
    };
  };
  if( model == 0 ) {
    delete rootdata; rootdata = 0; model = 0; 
    QMessageBox::critical( 0, "openDocument Error:",
       QString("Fail to detect model in file: ") + filename,
       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
    return false;
  };
  modified = false; is_nonamed = false;
  m_filename = filename;
  m_title = QFileInfo(filename).fileName();
  return true;
}

bool QMo2Doc::saveDocument(const QString &filename, const char * /*format */)
{
  int k;
  QCString fn;
  ofstream os;
  if( rootdata == 0 )
    return false;
  fn = filename.local8Bit();
  //QMessageBox::critical( 0, "Debug: QMo2Doc::saveDocument",
  //                       filename + ": " + QString(fn), 0,0,0 );
  errno = 0;
  os.open( fn );
  if( ! os.good() ) {
    cerr << "Fail to open for write file " << fn << ": "
         << strerror(errno) << '\n';
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

void QMo2Doc::deleteContents()
{
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
	     saveName = QFileDialog::getSaveFileName(0, 0, pFrame);
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
  if( rootdata == 0 )
    return;
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
  rootdata->regClass( TCounter::getStaticClassInfo() );
  rootdata->regClass( TCorrAnalysis::getStaticClassInfo() );
  rootdata->regClass( TFourier::getStaticClassInfo() );
}

// end of qmo2doc.cpp

