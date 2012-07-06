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
#include "qstructview.h"
#include "qoutview.h"
#include "qgraphview.h"
#include "qstatusmodel.h"
#include "qdoubletable.h"
#include "qrunview.h"
#include "qplotview.h"
#include "datawidget.h"


QMo2View::QMo2View( QMo2Doc* pDoc, QWidget *parent )
: QWidget( parent ) 
{
  doc = pDoc;
  QSize p_size = parent->size();
  root = doc->getRoot();
  model = doc->getModel();
  sel = mark = -1; sel_x = sel_y = 0; level = 0;

  setAttribute(Qt::WA_DeleteOnClose);
  
  QGridLayout *grLay = new QGridLayout;

  scrollArea = new QScrollArea( this );
  sview = new QStructView( doc, this, this );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 2 );
  scrollArea->setMidLineWidth( 2 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  oview = new QOutView( doc, this, this );
  gview = new QGraphView( doc, this, this );
  
  stam = new QStatusModel( this, this );

  grLay->addWidget( scrollArea, 0, 0 );
  grLay->addWidget( oview, 0, 1 );
  grLay->addWidget( gview, 0, 2 );
  grLay->addWidget( stam, 1, 0, 1, 3 );

  setLayout( grLay );
  
  QSize s_size ( sview->getElemsBound() );
  s_size += QSize( 2 * oview->width() + gview->width(), 0 );
  QSize n_size = s_size.boundedTo( p_size );
  resize( n_size );

  setWindowTitle( doc->title() );

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
		     if( sel < 0 ) 
		       msg = "You must select object to do this"; 
		     break;
    case linkToCheck:
		     if( sel < 0 || mark < 0 || level < 0  || level >=4 )
		       msg = "You need selected and marked objects to link";
		     break;
    case noselCheck: if( sel >= 0 )
		       msg = "You heed empty sell to do this";
		     break;
    case moveCheck: if( sel >= 0 || mark < 0 )
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
}  


void QMo2View::changeSel( int x, int y, int rel )
{
  int elnu;
  TMiso *ob;
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
  QDialog *seld; 
  QLabel *la;
  QPushButton *bt_ok, *bt_can;
  QLineEdit *oname_ed, *oord_ed;
  QListWidget *lw;
  QGridLayout *lay;

  const TClassInfo *ci;
  int rc, oord;
  QString cnmq, onameq, oordq;
  if( !checkState( noselCheck ) )
    return;

  seld = new QDialog( this );

  lay = new QGridLayout( seld );

  la = new QLabel( "Name", seld );
  lay->addWidget( la, 0, 0 );

  oname_ed = new QLineEdit( seld );
  oname_ed->setText( QString("obj_")
      + QString::number( model->getNMiso() ) );
  oname_ed->setFocus();
  lay->addWidget( oname_ed, 1, 0  );

  la = new QLabel( "Order", seld );
  lay->addWidget( la, 0, 1 );

  oord = model->hintOrd();
  oord_ed = new QLineEdit( seld );
  oord_ed->setText( QString::number(oord) );
  lay->addWidget( oord_ed, 1, 1 );

  la = new QLabel( "Type", seld );
  lay->addWidget( la, 2, 0 );

  lw = new QListWidget( seld );
  QStringList cl_names = ElemFactory::theFactory().allTypeNames();
  for( QString cname : cl_names ) {
    ci = ElemFactory::theFactory().getInfo( cname );
    if( ci == 0 ) 
      continue;
    if( ! ( ci->props & clpElem ) )
      continue;
    QString iconName = QString( ":icons/elm_" )
      + cname.toLower() 
      + ".png";
    lw->addItem( new QListWidgetItem( QIcon(iconName), cname ) );
  };
  lw->setViewMode( QListView::IconMode );
  // lw->setUniformItemSizes( true );
  lw->setResizeMode ( QListView::Adjust );
  lay->addWidget( lw, 3, 0, 1, 2 );

  bt_ok = new QPushButton( "Ok", seld );
  bt_ok->setText( "Ok" ); bt_ok->setDefault( true );
  lay->addWidget( bt_ok, 5, 0 );
  seld->setLayout( lay );

  connect( bt_ok, SIGNAL(clicked()), seld, SLOT(accept() ) );
  bt_can = new QPushButton( "Cancel", seld );
  lay->addWidget( bt_can, 5, 1 );
  connect( bt_can, SIGNAL(clicked()), seld, SLOT(reject() ) );

  seld->resize( 600, 400 );

  rc = seld->exec();
  if( rc == QDialog::Accepted ) {
    if( lw->currentItem() ) 
      cnmq = lw->currentItem()->text();
    onameq = oname_ed->text(); 
    oordq = oord_ed->text(); 
  }; 
  delete seld;
  if( rc != QDialog::Accepted || cnmq.isEmpty() )
    return;
  oord = oordq.toInt();
  if( ! isGoodName( onameq )  ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: bad object name \"") + onameq + "\"", 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  
  ci = ElemFactory::theFactory().getInfo( cnmq );
  if( ! ci ) {
    QMessageBox::critical( this, "Error", 
       QString("Fail to add Elem: class \"") + cnmq + "\" not found", 
       QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }
  model->insElem( cnmq, onameq, oord, sel_x, sel_y) ; // reset() implied
  changeSel( 0, 0, 1 ); // update sel
  editElm();
}

void QMo2View::delElm()
{
  int k;
  if( ! checkState( selCheck ) )
    return;	  
  
  TMiso *ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  
  QString oname = ob->objectName();

  k = QMessageBox::information( this, PACKAGE " delete confirmation",
       QString("Do you really want to delete element \"") 
        + oname + QString("\" ?"),
       "&Yes", "&No", "Help", 0, 1 );
  if( k == 0 ) {
    model->reset();
    model->delElem( sel );
    if( sel == mark )
      mark = -1;
    sel = model->xy2elnu( sel_x, sel_y );
    emit viewChanged();
  };
}

void QMo2View::editElm()
{
  if( ! checkState( selCheck ) )
    return;	  
  TMiso *ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  DataDialog *dia = new DataDialog( *ob, this );
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

  TMiso *ob = model->getMiso( sel );
  if( ob == 0 )
    return;

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) {
    qDebug( "ERR: fail to find links for object %s", 
	qPrintable( ob->getFullName() ) );
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
  TMiso *ob, *toob;
  TElmLink *el;
  int k;
  QString toname;
  QString oldlink;
  char lnkname[MAX_NAMELEN];
  if( ! checkState( linkToCheck ) )
    return;	  

  ob = model->getMiso( sel ); toob = model->getMiso( mark );
  if( ob == 0 || toob == 0 )
    return;
  toname = toob->objectName();

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return;
  strcpy( lnkname, "inps0" );
  lnkname[4] = char( '0' + level );
  k = el->getDataSS( lnkname, &oldlink, MAX_NAMELEN, 0 );
  if( k != 0 )
    return;
  k = el->setDataSS( lnkname, &toname, 0 );
  model->reset(); model->setModified();
  emit viewChanged();
}


void QMo2View::qplinkElm()
{
  TMiso *ob, *toob;
  TElmLink *el;
  int k;
  QString toname;
  QString oldlink[MAX_NAMELEN];
  char lnkname[MAX_NAMELEN];
  if( ! checkState( linkToCheck ) )
    return;	  

  ob = model->getMiso( sel ); toob = model->getMiso( mark );
  if( ob == 0 || toob == 0 )
    return;
  toname = toob->objectName();

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return;
  strcpy( lnkname, "pinps0" );
  lnkname[5] = char( '0' + level );
  k = el->getDataSS( lnkname, oldlink, MAX_NAMELEN, 0 );
  if( k != 0 || oldlink[0] != 0 )
    return;
  k = el->setDataSS( lnkname, &toname, 0 );
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::unlinkElm()
{
  TMiso *ob;
  TElmLink *el;
  int k;
  char lnkname[MAX_NAMELEN];
  if( ! checkState( linkToCheck ) )
    return;	  
  
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) 
    return;

  strcpy( lnkname, "inps0" );
  QString t("");
  for( k=0; k<4; k++ ) {
    lnkname[4] = char( '0' + k );
    el->setDataSS( lnkname, &t, 0 );
  };
  strcpy( lnkname, "pinps0" );
  for( k=0; k<4; k++ ) {
    lnkname[5] = char( '0' + k );
    el->setDataSS( lnkname, &t, 0 );
  };
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::lockElm()
{
  TMiso *ob;
  int lck;	
  if( ! checkState( selCheck ) )
    return;	  
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  
  ob->getData( "links.locked", &lck );
  lck = !lck;
  ob->setDataSI( "links.locked", lck, 0 );
  
  model->reset();
  model->setModified();
  emit viewChanged();
}

void QMo2View::ordElm()
{
  TMiso *ob;
  bool ok;
  int new_ord, old_ord;	
  if( ! checkState( selCheck ) )
    return;	  
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  old_ord = -1;
  ob->getData( "ord", &old_ord );
  new_ord = QInputDialog::getInt(this, "New element order", 
      "Input new element order", 
      old_ord, 0, 2147483647, 1, &ok );
  if( ok ) {
    model->newOrder( sel, new_ord ); // reset implied
    emit viewChanged();
  }; 
}

void QMo2View::markElm()
{
  mark = sel;
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
  TMiso *ob; 
  QString cbuf;
  QDialog *dia; 
  QPushButton *bt_ok;
  QVBoxLayout *lay;
  QTableWidget *tv;
  QString qs;
  if( ! checkState( selCheck ) )
    return;	  
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  
  dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Structure of ") + ob->objectName() );

  lay = new QVBoxLayout();

  tv = new QTableWidget( 100, 6, dia );
  QStringList hlabels;
  hlabels << "Name" << "Type" << "Value" << "Descr" << "Target"<< "Flags";
  tv->setHorizontalHeaderLabels( hlabels );
  
  QObjectList childs = ob->children();
  
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
  int nelm;
  QString buf;
  if( ! checkState( selCheck ) )
    return;	  
  TMiso *ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  nelm = ob->getN();
  
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": test1 ") + ob->objectName() );
  
  buf = ob->toString();
  buf += "# nelm= " + QString::number( nelm ) + "\n";

  //QObjectList childs = ob->children();
  //for( QObjectList::iterator o = childs.begin(); o != childs.end(); ++o ) {
  //  QObject *xo = *o;
  //  buf += QString(xo->metaObject()->className()) + " " + xo->objectName();
  //  if( ! xo->inherits("HolderData" )) {
  //    buf += " -\n";
  //    continue;
  //  }
  //  HolderData *ho = qobject_cast<HolderData*>(xo);
  //  buf += QString("=\"") + ho->toString() + "\"";
  //
  //  buf += " \n";
  //}


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
  TMiso *ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  return;
}


// ==== outs related 

void QMo2View::newOut()
{
  int idx, rc;
  const TDataInfo *di;
  QString onameq, enameq;
  QDialog *dia; QPushButton *bt_ok, *bt_can;
  QLineEdit *oname_ed, *ename_ed; QLabel *lab1, *lab2;
  QGridLayout *lay;
  if( ! checkState( validCheck ) )
    return;
  
  if( sel >=0 && (idx = model->elnu2idx( sel )) >=0
      && (di = model->getDataInfo( idx )) != 0 )
  {
    enameq = di->name;
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


  bt_ok = new QPushButton( "Ok", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept() ) );
  lay->addWidget( bt_ok, 2, 0 );


  bt_can = new QPushButton( "Cancel", dia );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject() ) );
  lay->addWidget( bt_can, 2, 1 );
  dia->setLayout( lay );

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    onameq = oname_ed->text(); enameq = ename_ed->text();
    
    // TODO: all must be QString
    if( isGoodName( onameq ) ) {
      int irc = model->insOut( qPrintable(onameq), qPrintable(enameq) );
      if( irc  ) {
	QMessageBox::critical( this, "Error", 
	   QString("Fail to add Output: ") + onameq, 
	   QMessageBox::Ok, QMessageBox::NoButton );
      }
      emit viewChanged();
    } else {
      QMessageBox::critical( this, "Error", 
         QString("Bad output name: ") + onameq, 
	 QMessageBox::Ok, QMessageBox::NoButton );
    };
  };
  delete dia;
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
  fn = fnq.toLatin1();
  gra->dump( fn, ' ' );
}

void QMo2View::gnuplotGraph()
{
  TGraph *gra;
  QDialog *dia;
  QLabel *lb1, *lb2, *lb3; QLineEdit *ed_pgm, *ed_dat, *ed_eps;
  QCheckBox *sw_x11; QPushButton *bt_ok, *bt_can;
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

  bt_ok = new QPushButton( "&Ok", dia );
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok, 7, 0 );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept() ) );

  bt_can = new QPushButton( "Cancel", dia );
  lay->addWidget( bt_can, 7, 1 );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject() ) );
  
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


