/***************************************************************************
  qmo2view.cpp  -  description
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

#include <cmath>
#include <limits>
// include files for Qt
#include <QtGui>

// application specific includes
#include "miscfun.h"
#include "qmo2view.h"
#include "qmo2doc.h"
#include "qmo2win.h"
#include "qstructview.h"
#include "qoutview.h"
#include "qgraphview.h"
#include "qstatusmodel.h"
#include "qdoubletable.h"
#include "qrunview.h"
#include "qplotview.h"
#include "datawidget.h"
#include "holdermodel.h"
#include "addelemdia.h"

using namespace std;


QMo2View::QMo2View( QMo2Doc* pDoc, QWidget *parent )
: QWidget( parent ) 
{
  doc = pDoc;
  QSize p_size = parent->size();
  root = doc->getRoot();
  model = doc->getModel();
  ho_mo = new HolderModel( root, this );
  sel = mark = -1; sel_x = sel_y = 0; level = 0; 
  selObj = nullptr; markObj = nullptr;

  QVBoxLayout *vlay = new QVBoxLayout( this );

  QWidget *left_part = new QWidget( this );

  setAttribute(Qt::WA_DeleteOnClose);

  QSplitter *split = new QSplitter( this );
  
  QGridLayout *grLay = new QGridLayout( left_part );

  scrollArea = new QScrollArea( left_part );
  sview = new QStructView( doc, this, left_part );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 2 );
  scrollArea->setMidLineWidth( 2 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  oview = new QOutView( doc, this, left_part );
  gview = new QGraphView( doc, this, left_part );
  
  stam = new QStatusModel( this, this );

  treeView = new QTreeView( this );
  treeView->setModel( ho_mo );

  grLay->addWidget( scrollArea, 0, 0 );
  grLay->addWidget( oview, 0, 1 );
  grLay->addWidget( gview, 0, 2 );

  left_part->setLayout( grLay );

  split->addWidget( left_part );
  split->addWidget( treeView );

  vlay->addWidget( split );
  vlay->addWidget( stam );
  setLayout( vlay );
  
  QSize s_size ( sview->getElemsBound() );
  s_size += QSize( 2 * oview->width() + gview->width(), 0 );
  QSize n_size = s_size.boundedTo( p_size );
  resize( n_size );

  setWindowTitle( doc->title() );

  connect( model, SIGNAL(sigStructChanged()), treeView, SLOT(reset()) );
  connect( this, SIGNAL(viewChanged()), this, SLOT(updateViews()) );
  connect( sview, SIGNAL(sig_changeSel(int,int,int)), this, SLOT(changeSel(int,int,int)) );
  connect( sview, SIGNAL(sig_changeLevel(int)), this, SLOT(changeLevel(int)) );

}

QMo2View::~QMo2View()
{
}

QMo2Doc *QMo2View::getDocument() const
{
  return doc;
}

const QString& QMo2View::currentFile() const 
{ 
  return doc->pathName(); 
}

QSize QMo2View::svSize() const 
{ 
  return scrollArea->size();
}



void QMo2View::update( QMo2View* pSender )
{
  if( pSender != this )
    repaint();
}

void QMo2View::print()
{
  sview->printAll(); 
}

void QMo2View::closeEvent( QCloseEvent *e )
{
  if( doc == 0 || doc->canCloseFrame( this ) ) {
    e->accept();
  } else {
    e->ignore();
  }
}

void QMo2View::resizeEvent( QResizeEvent *e )
{
  updateViews();
  QWidget::resizeEvent( e );
}

int QMo2View::checkState( CheckType ctp ) 
{
  QString msg;
  int state;
  if( model == 0 || root == 0 ) {
    QMessageBox::critical( this, "Error", "Model or root don't exist!",
	QMessageBox::Ok, QMessageBox::NoButton	);
    return 0; 
  };
  switch( ctp ) {
    case validCheck: break;
    case selCheck: 
		     if( !selObj ) 
		       msg = "You must select object to do this"; 
		     break;
    case linkToCheck:
		     if( !selObj || !markObj || level < 0  || level >=4 )
		       msg = "You need selected and marked objects to link";
		     break;
    case noselCheck: if( selObj != nullptr )
		       msg = "You heed empty sell to do this";
		     break;
    case moveCheck: if( selObj != nullptr || !markObj )
		      msg = "You need marked object and empty cell to move";
		    break;
    case doneCheck:
		    state = model->getState();
		    if( state < stateDone ) {
		      msg = QString("Nothing to plot: state '%1', not 'Done' !").arg(
			  getStateString(state)  );
		    };
		    break;
    default: msg = "Unknown check?";
  };
  if( ! msg.isEmpty() ) {
    QMessageBox::warning( this, "Warning", msg,
	QMessageBox::Ok, QMessageBox::NoButton	);
    return 0;
  };
  return 1;
}


TRootData* QMo2View::getRoot(void)
{
  return root;
}

TModel*  QMo2View::getModel(void)
{
  return model;
}

void QMo2View::updateViews()
{
  sview->update();
  oview->update();
  gview->update();
  stam->update();
  // treeView->reset();
  treeView->update();
}  


void QMo2View::changeSel( int x, int y, int rel )
{
  int elnu;
  TMiso *ob;
  selObj = nullptr;
  switch( rel ) {
    case 0: sel_x = x; sel_y = y; break;
    case 1: sel_x += x; sel_y += y; break;
    case 2:
	    elnu = model->xy2elnu( sel_x, sel_y );
	    elnu++;
	    if( elnu >= model->getNMiso() )
	      elnu = 0;
	    ob = model->getMiso( elnu );
	    if( ob == 0 )
	      break;
	    ob->getData( "vis_x", &sel_x ); 
	    ob->getData( "vis_y", &sel_y );
	    break;
    default: break;
  };
  if( sel_x >= MODEL_MX ) sel_x = MODEL_MX-1; 
  if( sel_y >= MODEL_MY ) sel_y = MODEL_MY-1;
  if( sel_x < 0 ) sel_x = 0; 
  if( sel_y < 0 ) sel_y = 0;
  sel = model->xy2elnu( sel_x, sel_y );
  ob = model->xy2Miso( sel_x, sel_y );
  if( ob ) {
    selObj = ob;
  }
  QPoint seco = sview->getSelCoords();
  scrollArea->ensureVisible( seco.x(), seco.y() );
  emit viewChanged();
}


void QMo2View::changeLevel( int lev )
{
  level = lev;
  if( level < 0 || level >= 64 )
    level = 0;
  emit viewChanged();
}  

// ==== element related

void QMo2View::newElm()
{
  if( !checkState( noselCheck ) )
    return;

  addElemInfo aei;
  aei.name = QString("obj_") + QString::number( model->getNMiso() ) ;
  aei.order = model->hintOrd();
  AddElemDialog *dia = new AddElemDialog( &aei, clpElem, model, this );

  int rc = dia->exec();
  delete dia; dia = 0;
  if( rc != QDialog::Accepted ) {
    return;
  }; 
  if( rc != QDialog::Accepted || aei.type.isEmpty() )
    return;
  if( ! isGoodName( aei.name )  ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: bad object name \"") + aei.name + "\"", 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  
  const TClassInfo *ci = EFACT.getInfo( aei.type );
  if( !ci ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: class \"") + aei.type + "\" not found", 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  TMiso *ob = model->insElem( aei.type, aei.name, aei.order, sel_x, sel_y );
  if( !ob  ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: ") + aei.type + " " + aei.name, 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  changeSel( 0, 0, 1 ); // update sel
  editElm();
}

void QMo2View::delElm()
{
  int k;
  if( ! checkState( selCheck ) )
    return;	  
  
  QString oname = selObj->objectName();

  k = QMessageBox::information( this, PACKAGE " delete confirmation",
       QString("Do you really want to delete element \"") 
        + oname + QString("\" ?"),
       "&Yes", "&No", "Help", 0, 1 );
  if( k == 0 ) {
    model->delElem( oname );
    if( sel == mark ) {
      mark = -1;
      markObj = nullptr;
    }

    changeSel( 0, 0, 1 ); // update sel
    emit viewChanged();
  };
}

void QMo2View::editElm()
{
  if( ! checkState( selCheck ) )
    return;	  
  DataDialog *dia = new DataDialog( *selObj, this );
  int rc = dia->exec();
  delete dia;
  
  if( rc == QDialog::Accepted ) {
    model->reset();
    model->setModified();
  };
  emit viewChanged();
}

void QMo2View::linkElm()
{
  TElmLink *el;
  int rc;
  if( ! checkState( selCheck ) )
    return;	  

  el = static_cast<TElmLink*>( selObj->getObj( "links" ) );
  if( el == 0 ) {
    qDebug( "ERR: fail to find links for object %s", 
	qPrintable( selObj->getFullName() ) );
    return ;
  }
  DataDialog *dia = new DataDialog( *el, this );
  rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
  };
}

void QMo2View::qlinkElm()
{
  int k;
  QString toname;
  QString oldlink;
  if( ! checkState( linkToCheck ) )
    return;	  

  if( !selObj || !markObj )
    return;
  toname = markObj->objectName();

  QString lnkname( "links.inps" );
  lnkname += QString::number( level );
  k = selObj->getData( lnkname, oldlink );
  if( !k )
    return;
  k = selObj->setData( lnkname, toname );
  model->reset(); model->setModified();
  emit viewChanged();
}


void QMo2View::qplinkElm()
{
  int k;
  QString oldlink;
  if( ! checkState( linkToCheck ) )
    return;	  

  if( !selObj || !markObj  )
    return;
  QString toname = markObj->objectName();

  QString lnkname( "links.pinps" );
  lnkname += QString::number( level );
  k = selObj->getData( lnkname, oldlink );
  if( !k  || ! oldlink.isEmpty() )
    return;
  k = selObj->setData( lnkname, toname );
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::unlinkElm()
{
  int k;
  if( ! checkState( linkToCheck ) )
    return;	  
  
  QString lnkname;
  QString none("");
  for( k=0; k<4; k++ ) {
    lnkname = "links.inps" + QString::number(k);
    selObj->setData( lnkname, none );
  };

  for( k=0; k<4; k++ ) {
    lnkname = "links.pinps" + QString::number(k);
    selObj->setData( lnkname, none );
  };
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::lockElm()
{
  int lck;	
  if( ! checkState( selCheck ) )
    return;	  
  
  selObj->getData( "links.locked", &lck );
  lck = !lck;
  selObj->setData( "links.locked", lck );
  
  model->reset();
  model->setModified();
  emit viewChanged();
}

void QMo2View::ordElm()
{
  bool ok;
  int new_ord, old_ord;	
  if( ! checkState( selCheck ) )
    return;	  
  old_ord = -1;
  selObj->getData( "ord", &old_ord );
  new_ord = QInputDialog::getInt(this, "New element order", 
      "Input new element order", 
      old_ord, 0, IMAX, 1, &ok );
  if( ok ) {
    model->newOrder( selObj->objectName(), new_ord ); // reset implied
    emit viewChanged();
  }; 
}

void QMo2View::markElm()
{
  mark = sel;
  markObj = selObj;
  emit viewChanged();
}

void QMo2View::moveElm()
{
  if( ! checkState( moveCheck ) )
    return;	  
  model->moveElem( mark, sel_x, sel_y );
  emit viewChanged();
}

void QMo2View::infoElm()
{
  QString cbuf;
  QDialog *dia; 
  QPushButton *bt_ok;
  QVBoxLayout *lay;
  QTableWidget *tv;
  QString qs;
  if( ! checkState( selCheck ) )
    return;	  
  
  dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Structure of ") + selObj->getFullName() );

  lay = new QVBoxLayout();

  tv = new QTableWidget( 100, 6, dia );
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
      tv->setItem( i, 1, new QTableWidgetItem(ds->getClassName()) );
      tv->setItem( i, 2, new QTableWidgetItem( ds->toString() ) );
    } else if( ob->inherits("HolderData" ) ) {
      HolderData *ho = qobject_cast<HolderData*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ho->getType() ) );
      tv->setItem( i, 2, new QTableWidgetItem(ho->toString() ) );
      tv->setItem( i, 3, new QTableWidgetItem(ho->getParm("vis_name") + " \"" 
	            + ho->getParm("descr" ) + "\"" ) );
      tv->setItem( i, 4, new QTableWidgetItem( ho->targetName() ) );
      tv->setItem( i, 5, new QTableWidgetItem( flags2str(ho->getFlags()) ) );

    } else { // unknown
      tv->setItem( i, 1, new QTableWidgetItem("???unknown???" ) );
      tv->setItem( i, 2, new QTableWidgetItem(ob->metaObject()->className() ) );
    }
    ++i;
  }


  
  lay->addWidget( tv );

  bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  // dia->resize( 600, 300 );
  dia->exec();
  delete dia;
  emit viewChanged();
}

void QMo2View::testElm1()
{
  QString buf;
  if( ! checkState( selCheck ) )
    return;	  
  
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": test1 ") + selObj->objectName() );
  
  buf = selObj->toString();


  QVBoxLayout *lay = new QVBoxLayout();

  QLabel *la = new QLabel( dia );
  la->setText( buf );
  QScrollArea *scroll = new QScrollArea( dia );
  scroll->setWidget( la );
  lay->addWidget( scroll );


  QPushButton *bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->resize( 600, 300 );
  dia->exec();
  delete dia;
  emit viewChanged();
}

void QMo2View::testElm2()
{
  if( ! checkState( selCheck ) )
    return;	  
  if( selObj == 0 )
    return;
  return;
}

void QMo2View::cutElm()
{
  copyElm();
  delElm();
}

void QMo2View::copyElm()
{
  if( !selObj )
    return;
  QString s = selObj->toString();
  QClipboard *clp = QApplication::clipboard();
  if( clp ) {
    clp->setText( s );
  }
}

void QMo2View::pasteElm()
{
  if( selObj )
    return;
  QClipboard *clp = QApplication::clipboard();
  if( !clp )
    return;
  QString s = clp->text();
  int err_line, err_column;
  QString errstr;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    QMessageBox::warning(QMo2Win::qmo2win, tr( PACKAGE ),
                         tr("Cannot parse clipboard string:\n%2\nLine %3 column %4.")
                         .arg(errstr).arg(err_line).arg(err_column) );
    return;
  }
  QDomElement ee = x_dd.documentElement();
  
  QString tagname = ee.tagName();
  if( tagname != "obj" ) {
    QMessageBox::warning(QMo2Win::qmo2win, tr( PACKAGE ),
                 tr("element tag is not 'obj':  %2")
                         .arg( tagname ) );
    return;
  }
  
  QString eltype = ee.attribute( "otype" );
  QString elname = ee.attribute( "name" );
  QString elname_base = elname;
  int suff_n = 1;
  while( model->getHolder( elname ) && suff_n < 50 ) { // guess good name
    elname = elname_base + "_" + QString::number( suff_n );
    suff_n++;
    if( suff_n > 20 ) {
      elname += "_x";
    }
  }
  
  int oord = model->hintOrd();
  
  QDialog *dia = new QDialog( this );
  QGridLayout *lay = new QGridLayout( dia );

  QLabel *la_name = new QLabel( "&Name", dia );
  lay->addWidget( la_name, 0, 0 );

  QLineEdit *oname_ed = new QLineEdit( dia );
  oname_ed->setText( elname );
  oname_ed->setFocus();
  lay->addWidget( oname_ed, 1, 0  );

  QLabel *la_ord = new QLabel( "&Order", dia );
  lay->addWidget( la_ord, 0, 1 );

  QLineEdit *oord_ed = new QLineEdit( dia );
  oord_ed->setText( QString::number(oord) );
  lay->addWidget( oord_ed, 1, 1 );
  
  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));

  int rc = dia->exec();
  elname = oname_ed->text(); 
  oord = oord_ed->text().toInt();
  delete dia;
  
  if( rc != QDialog::Accepted ) {
    return;
  }; 
  
  if( ! isGoodName( elname )  ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: bad object name \"") + elname + "\"", 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }

  TMiso *ob = model->insElem( eltype, elname, oord, sel_x, sel_y) ; // reset() implied
  if( !ob  ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: ") + eltype + " " + elname, 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  if( !ob->fromDom( ee, errstr ) ) {
    QMessageBox::warning(QMo2Win::qmo2win, tr( PACKAGE ),
                 tr("fail to set params:  %1")
                         .arg( errstr ) );
  }
  ob->setData( "vis_x", sel_x );
  ob->setData( "vis_y", sel_y );
  ob->setData( "ord", oord );
  changeSel( 0, 0, 1 ); // update sel
  
}

// ==== outs related 

void QMo2View::newOut()
{
  int rc;
  QString onameq, enameq;
  QDialog *dia;
  QLineEdit *oname_ed, *ename_ed; QLabel *lab1, *lab2;
  QGridLayout *lay;
  if( ! checkState( validCheck ) )
    return;
  
  if( selObj  )
  {
    enameq = selObj->objectName();
    onameq = QString("out_") + QString(enameq);
  } else {
    enameq = ":t";
    onameq = QString("out_t");
  };

  dia = new QDialog( this );
  dia->setWindowTitle( "Creating new output array" );

  lay = new QGridLayout( dia );

  lab1 = new QLabel( "Output array name", dia );
  lay->addWidget( lab1, 0, 0 );

  oname_ed = new QLineEdit( dia );
  oname_ed->setMaxLength( MAX_NAMELEN-1 );
  oname_ed->setText( onameq ); oname_ed->setFocus();
  lay->addWidget( oname_ed, 0, 1 );

  lab2 = new QLabel( "Element name",  dia );
  lay->addWidget( lab2, 1, 0 );

  ename_ed = new QLineEdit( dia );
  ename_ed->setMaxLength( MAX_NAMELEN-1 );
  ename_ed->setText( enameq );
  lay->addWidget( ename_ed, 1, 1 );
  
  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));

  rc = dia->exec();
  onameq = oname_ed->text(); enameq = ename_ed->text();
  delete dia;
  if( rc != QDialog::Accepted ) 
    return;
    
  if( isGoodName( onameq ) ) {
    int irc = model->insOut( onameq, enameq );
    if( irc  ) {
      QMessageBox::critical( this, "Error", 
	 QString("Fail to add Output: ") + onameq, 
	 QMessageBox::Ok, QMessageBox::NoButton );
    }
    emit viewChanged();
    return;
  }
  QMessageBox::critical( this, "Error", 
     QString("Bad output name: ") + onameq, 
     QMessageBox::Ok, QMessageBox::NoButton );
}

void QMo2View::delOut()
{
  int k;
  if( ! checkState( validCheck ) )
    return;	  
  if( level < 0 || level >= model->getNOutArr() )
    return;
  TOutArr *arr= model->getOutArr( level );
  if( arr == 0 )
    return;
  k = QMessageBox::information( this, PACKAGE " delete confirmation",
      QString("Do you really want to delete output array \"" )
       + arr->objectName() + QString( "\" ?" ),
      "&Yes", "&No", "&Help", 0, 1 );
  if( k == 0 ) {
    model->delOut( level );
    emit viewChanged();
  };
}


void QMo2View::editOut()
{
  TOutArr *arr;
  int rc;
  if( ! checkState( validCheck ) )
    return;	

  if( level < 0 || level >= model->getNOutArr() )
    return;
  arr = model->getOutArr( level );
  if( arr == 0 )
    return;
  DataDialog *dia = new DataDialog( *arr, this );
  rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
  };
}

void QMo2View::editOut2()
{
  editOut(); // moved to mainstream
}


void QMo2View::showOutData() // TODO: special dialog (+ for many rows)
{
  QDialog *dia;
  DoubleTableModel *dmod;
  QTableView *dtv;
  QGridLayout *lay;
  QString fnq, sinf;
  QPushButton *bt_ok;
  QLabel *lab;
  TOutArr *arr;
  GraphInfo gi;
  int k;
  if( ! checkState( doneCheck ) )
    return;	
  arr = model->getOutArr( level );
  if( arr == 0 )
    return;
  k = arr->fillGraphInfo( &gi );
  if( k != 0 )
    return;

  // calculate statistical data
  double s = 0, s2 = 0, ave = 0, ave2 = 0, disp = 0, msq = 0, x;
  double vmin = DMAX, vmax = DMIN;
  for( int i=0; i<gi.row; i++ ) {
    x =  gi.dat[0][i];
    s += x; s2 += x * x;
    if( x < vmin ) vmin = x;
    if( x > vmax ) vmax = x;
  };
  ave = s / gi.row; ave2 = s2 / gi.row;
  disp = ave2 - ave * ave;
  msq = sqrt( disp );


  dia = new QDialog( this );
  dia->setWindowTitle( QString("Data array: ") + QString(gi.title) );
  lay = new QGridLayout( dia );

  dmod = new DoubleTableModel( &gi, dia );
  dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lay->addWidget( dtv, 0, 0 );

  sinf = QString( "n= " ) % QString::number( gi.row )
       % QString( "; \nave= " ) % QString::number( ave )
       % QString( "; \nave2= " ) % QString::number( ave2 ) 
       % QString( "; \nsum= " ) % QString::number( s )
       % QString( "; \nsum2= " ) % QString::number( s2 )
       % QString( ";\nD= " ) % QString::number( disp )
       % QString( "; \nsigm= " ) % QString::number( msq )
       % QString( "; \nmin= " ) % QString::number( vmin )
       % QString( "; \nmax= " ) % QString::number( vmax ); 
  lab = new QLabel( sinf, dia );
  lab->setTextInteractionFlags( Qt::TextSelectableByMouse 
       | Qt::TextSelectableByKeyboard);
  lay->addWidget( lab, 0, 1 );

  bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  lay->addWidget( bt_ok, 1, 0, 1, 2 );
  dia->setLayout( lay );

  dia->exec();
  delete dia;
}

void QMo2View::exportOut()
{
  QString fnq;
  TOutArr *arr;
  if( ! checkState( doneCheck ) )
    return;	
  arr = model->getOutArr( level );
  if( arr == 0 )
    return;
  fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() )
    return;
  arr->dump( qPrintable(fnq), ' ' ); // TODO: QString
}


// ==== graph related

void QMo2View::newGraph()
{
  int no;
  QString grnameq, aname;
  bool ok;
  if( ! checkState( validCheck ) )
    return;	
  no = model->getNGraph();
  grnameq = QString("graph") + QString::number( no ); 
  aname = QInputDialog::getText( this, "Creating new Graph descriptions",
      "Enter name of new Graph:", QLineEdit::Normal, 
      grnameq, &ok );
  if( ok ) {
    if( ! isGoodName( aname ) ) { 
      QMessageBox::critical( this, "Error", 
         QString("Bad graph name: \"") + aname + "\"", 
	 QMessageBox::Ok, QMessageBox::NoButton );
    }
    model->insGraph( aname );
    emit viewChanged();
  };
}


void QMo2View::delGraph()
{
  int k;
  if( ! checkState( validCheck ) )
    return;	
  if( level < 0 || level >= model->getNGraph() )
    return;
  TGraph *gra = model->getGraph( level );
  if( gra == 0 )
    return;
  k = QMessageBox::information( this, PACKAGE " delete confirmation",
      QString("Do you really want to delete graph description \"")
       + gra->objectName() + QString("\" ?") ,
      "&Yes", "&No", "&Help", 0, 1 );
  if( k == 0 ) {
    model->delGraph( level );
    emit viewChanged();
  };
}


void QMo2View::editGraph()
{
  TGraph *gra;
  int rc;
  if( ! checkState( validCheck ) )
    return;	

  if( level < 0 || level >= model->getNGraph() )
    return;
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  DataDialog *dia = new DataDialog( *gra, this );
  rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
  };
}

void QMo2View::editGraph2()
{
  return editGraph(); // new in upstream
}

void QMo2View::showGraph()
{
  int n_gra;
  QPlotView *pv; TGraph *gra;
  QMainWindow *plotWnd;
  if( ! checkState( doneCheck ) )
    return;	
  n_gra = model->getNGraph();
  if( level < 0 || level >= n_gra )
    return;
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  plotWnd = new QMainWindow( this );
  plotWnd->setWindowTitle( QString( PACKAGE ": Plot ") + gra->objectName() );
  pv = new QPlotView( doc, gra, plotWnd );
  plotWnd->setCentralWidget( pv );
  pv->setFocus();
  plotWnd->show();
}

void QMo2View::showGraphData()
{
  QDialog *dia;
  DoubleTableModel *dmod;
  QTableView *dtv;
  QVBoxLayout *lv;
  GraphInfo gi;
  QString fnq; QPushButton *bt_ok;
  TGraph *gra;
  int k;
  if( ! checkState( doneCheck ) )
    return;	
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  k = gra->fillGraphInfo( &gi );
  if( k != 0 )
    return;

  dia = new QDialog( this );
  dia->setWindowTitle( QString("Graph data: ") + QString(gi.title) );
  lv = new QVBoxLayout( dia );

  dmod = new DoubleTableModel( &gi, dia );
  dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lv->addWidget( dtv );

  bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  lv->addWidget( bt_ok );

  dia->exec();
  delete dia;

}

void QMo2View::exportGraphData()
{
  const char *fn;
  QString fnq;
  TGraph *gra;
  if( ! checkState( doneCheck ) )
    return;	
  if( level < 0 || level >= model->getNGraph() )
    return;
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() )
    return;
  fn = fnq.toLocal8Bit();
  gra->dump( fn, ' ' );
}

void QMo2View::gnuplotGraph()
{
  TGraph *gra;
  QDialog *dia;
  QLabel *lb1, *lb2, *lb3; QLineEdit *ed_pgm, *ed_dat, *ed_eps;
  QCheckBox *sw_x11;
  QGridLayout *lay; 
  QString f_pgm, f_dat, f_eps, cdir;
  int l, rc, use_x11;
  if( ! checkState( doneCheck ) )
    return;	
  if( level < 0 || level >= model->getNGraph() )
    return;
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  f_pgm = doc->pathName();
  QFileInfo doc_fi( f_pgm );
  cdir = QDir::currentPath();
  if( cdir == doc_fi.absolutePath() ) 
    f_pgm = doc_fi.fileName();
  if( f_pgm.length() < 1 ) { f_pgm = "gplot"; };
  l = f_pgm.length();
  if( doc_fi.suffix() == "mo2"  ) 
    f_pgm.truncate( l-4 );
  f_dat = f_pgm + ".dat"; f_eps = f_pgm + ".eps";
  f_pgm += ".gp";

  dia = new QDialog( this );
  dia->resize( 400, 260 );
  lay = new QGridLayout( dia );
  sw_x11 = new QCheckBox( "Output to &X11 window", dia );
  sw_x11->setChecked( false );
  lay->addWidget( sw_x11, 0, 0, 1, 2 );

  lb1 = new QLabel( "Output to EPS file:", dia );
  lay->addWidget( lb1, 1, 0, 1, 2 );
  ed_eps = new QLineEdit( dia );
  ed_eps->setText( f_eps );
  lay->addWidget( ed_eps, 2, 0, 1, 2 );

  lb2 = new QLabel( "Data file:", dia );
  lay->addWidget( lb2, 3, 0, 1, 2 );
  ed_dat = new QLineEdit( dia );
  ed_dat->setText( f_dat );
  lay->addWidget( ed_dat, 4, 0, 1, 2 );

  lb3 = new QLabel( "Gnuplot program file:", dia );
  lay->addWidget( lb3, 5, 0, 1, 2 );
  ed_pgm = new QLineEdit( dia );
  ed_pgm->setText( f_pgm );
  lay->addWidget( ed_pgm, 6, 0, 1, 2 );
  
  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 7, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), dia, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), dia, SLOT(reject()));

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    f_pgm = ed_pgm->text(); f_eps = ed_eps->text(); 
    f_dat = ed_dat->text(); use_x11 = sw_x11->isChecked();
    gra->gnuPlot( !use_x11, f_pgm.toLocal8Bit(), 0, 
	          f_eps.toLocal8Bit(), f_dat.toLocal8Bit() );
  };
  delete dia;
}

// ==== model related

void QMo2View::editModel()
{
  int rc;
  if( ! checkState( validCheck ) )
    return;	

  DataDialog *dia = new DataDialog( *model, this );
  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    model->reset();
    emit viewChanged();
  };
}


void QMo2View::showVars()
{
  QDialog *dia;
  DoubleTableModel *dmod;
  QTableView *dtv;
  QVBoxLayout *lv;
  GraphInfo gi;
  QString fnq; QPushButton *bt_ok;
  if( ! checkState( validCheck ) )
    return;	
  const double *vars = model->getVars();
  gi.row = MODEL_NVAR; gi.col = 1; gi.ny = gi.row;
  strcpy( gi.title, "Model vars" );
  gi.dat[0] = vars;
  strcpy( gi.label[0], "vars" );

  dia = new QDialog( this );
  dia->setWindowTitle( QString("Model vars: ") );
  lv = new QVBoxLayout( dia );
  
  dmod = new DoubleTableModel( &gi, dia );
  dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lv->addWidget( dtv );

  bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  lv->addWidget( bt_ok );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;
}

void QMo2View::editModel2()
{
  editModel();
}


// ==== run related

void QMo2View::runRun()
{
  QRunView *rv;
  if( ! checkState( validCheck ) )
    return;	
  rv = new QRunView( model, 0, this );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void QMo2View::runPrm()
{
  if( ! checkState( validCheck ) )
    return;	
  QRunView *rv;
  rv = new QRunView( model, 1, this );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void QMo2View::runPrm2()
{
  if( ! checkState( validCheck ) )
    return;	
  QRunView *rv;
  rv = new QRunView( model, 2, this );  // TODO remove 0
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void QMo2View::resetModel()
{
  if( ! checkState( validCheck ) )
    return;	
  model->reset();
  emit viewChanged();
}


// misc

const char QMo2View::helpstr[] = "<b>Hot keys:</b><br>\n"
"<b>Ctrl-w</b> - close <br>\n"
"<b>h/F1</b> - this help <br>\n"
"<b>{Left, Top, Right, Bottom, Home}</b> - move selected cell <br>\n"
"<b>0-9</b> - select out array / graph <br>\n"
;

void QMo2View::showHelp(void)
{
  QDialog *dia; QLabel *la; QPushButton *bt_ok;
  QVBoxLayout *lv;
  dia = new QDialog( this );
  dia->setWindowTitle( "Hot keys in structure window" );
  lv = new QVBoxLayout( dia );
  
  la = new QLabel( dia );
  la->setText( helpstr );
  lv->addWidget( la );
  
  bt_ok = new QPushButton( "&Ok", dia );
  bt_ok->setDefault( true );
  lv->addWidget( bt_ok );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;
}


// end of qmo2view.cpp


