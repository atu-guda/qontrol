/***************************************************************************
  qmo2view.cpp  -  description
  begin                : Mon Jul 31 16:51:57 EEST 2000
  copyright            : (C) 2000-2003 by atu
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
#include <qdir.h>
#include <qfileinfo.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qscrollview.h>

// application specific includes
#include "miscfun.h"
#include "qmo2view.h"
#include "qmo2doc.h"
#include "qstructview.h"
#include "qoutview.h"
#include "qgraphview.h"
#include "qstatusmodel.h"
#include "qanydial.h"
#include "qdoubletable.h"
#include "qrunview.h"
#include "qplotview.h"


QMo2View::QMo2View( QMo2Doc* pDoc, QWidget *parent,
    const char* name, int wflags )
: QMainWindow(parent, name, wflags)
{
  QHBox *hb; QVBox *vb; 
  doc = pDoc;
  QSize p_size = parent->size();
  root = doc->getRoot();
  model = doc->getModel();
  sel = mark = -1; sel_x = sel_y = 0; level = 0;
  vb = new QVBox( this, "vbox" ); // main field(hb=3views) + status(stab)
  hb = new QHBox( vb, "hbox" );
  sv = new QScrollView( hb, "scrollview" );
  sv->setVScrollBarMode( QScrollView::AlwaysOn );
  sview = new QStructView( doc, this, sv->viewport(), "structure_view" );
  sv->addChild( sview );
  oview = new QOutView( doc, this, hb, "out_view" );
  gview = new QGraphView( doc, this, hb, "graph_view" );
  stam = new QStatusModel( this, vb, "status_model" );

  QSize s_size ( sview->getElemsBound() );
  s_size += QSize( 2 * oview->width() + gview->width(), 0 );
  QSize n_size = s_size.boundedTo( p_size );
  resize( n_size );

  setCentralWidget( vb );
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


QSize QMo2View::svSize() const 
{ 
  return QSize( sv->visibleWidth(), sv->visibleHeight() ); 
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
  // LEAVE THIS EMPTY: THE EVENT FILTER IN THE QMo2Win CLASS
  // TAKES CARE FOR CLOSING
  // QWidget closeEvent must be prevented.
  // atu: no, i prefer do do it here, so event filter method is ill-brained
  if( doc == 0 || doc->canCloseFrame( this ) )
    e->accept();
  else
    e->ignore();
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
    QMessageBox::warning( this, "Error", "Model or root don't exist!",
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
	    ob->getDataSI( "vis_x", &sel_x, 0 ); 
	    ob->getDataSI( "vis_y", &sel_y, 0 );
	    break;
    default: break;
  };
  if( sel_x >= MODEL_MX ) sel_x = MODEL_MX-1; 
  if( sel_y >= MODEL_MY ) sel_y = MODEL_MY-1;
  if( sel_x < 0 ) sel_x = 0; 
  if( sel_y < 0 ) sel_y = 0;
  sel = model->xy2elnu( sel_x, sel_y );
  QPoint seco = sview->getSelCoords();
  sv->ensureVisible( seco.x(), seco.y() );
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
  TDataInfo di;
  QDialog *seld; 
  QComboBox *cb; QLabel *la;
  QPushButton *bt_ok, *bt_can;
  QLineEdit *oname_ed, *oord_ed;
  QVBoxLayout *main_lay, *type_lay;
  QHBoxLayout *btn_lay;
  QGridLayout *name_lay;
  QSpacerItem *spa1;

  const TClassInfo *ci;
  int i, nc, rc, oord;
  const char *cnm, *oname;
  QString cnmq, onameq, oordq;
  if( !checkState( noselCheck ) )
    return;

  seld = new QDialog( this, "sel_dial", true );

  main_lay = new QVBoxLayout( seld, 10, 10, "newElm_main_layout" );
  name_lay = new QGridLayout( main_lay, 2, 2,  -1, "name_grid" );
  type_lay = new QVBoxLayout( main_lay, -1, "type_lay" );
  btn_lay  = new QHBoxLayout( main_lay, -1, "btn_lay" );

  la = new QLabel( seld, "l1" );
  la->setText( "Name" );
  name_lay->addWidget( la, 0, 0 );

  oname_ed = new QLineEdit( seld, "oname_ed" );
  oname_ed->setText( QString("obj_")
      + QString::number( model->getNMiso() ) );
  oname_ed->setFocus();
  name_lay->addWidget( oname_ed, 1, 0  );

  la = new QLabel( seld, "l2" );
  la->setText( "Order" );
  name_lay->addWidget( la, 0, 1 );

  oord = model->hintOrd();
  oord_ed = new QLineEdit( seld, "oord_ed" );
  oord_ed->setText( QString::number(oord) );
  name_lay->addWidget( oord_ed, 1, 1 );


  la = new QLabel( seld, "l3" );
  la->setText( "Type" );
  type_lay->addWidget( la );

  cb = new QComboBox( seld, "combo" );
  nc = root->getNClasses( -1,  0 );
  for( i=0; i<nc; i++ ) {
    ci = root->classInfoByNum( i );
    if( ci == 0 ) continue;
    if( root->isHisParent( ci->id, CLASS_ID_TMiso, 0 ) ) {
      cb->insertItem( ci->className );
    };
  };
  type_lay->addWidget( cb );
  spa1 = new QSpacerItem( 1, 200, 
      QSizePolicy::Minimum, QSizePolicy::Expanding );
  type_lay->addItem( spa1 );


  bt_ok = new QPushButton( seld, "bt_ok" );
  bt_ok->setText( "Ok" ); bt_ok->setDefault( true );
  btn_lay->addWidget( bt_ok );
  connect( bt_ok, SIGNAL(clicked()), seld, SLOT(accept() ) );
  bt_can = new QPushButton( seld, "bt_can" );
  bt_can->setText( "Cancel" );
  btn_lay->addWidget( bt_can );
  connect( bt_can, SIGNAL(clicked()), seld, SLOT(reject() ) );
  rc = seld->exec();
  if( rc == QDialog::Accepted ) {
    cnmq = cb->currentText();
    onameq = oname_ed->text(); 
    oordq = oord_ed->text(); 
  }; 
  delete seld;
  if( rc != QDialog::Accepted )
    return;
  cnm = cnmq.latin1(); oname = onameq.latin1(); oord = oordq.toInt();
  if( ! isGoodName( oname )  )
    return;
  i = root->findClass( cnm );
  if( i < 1 ) return;
  ci = root->classInfoByNum( i );
  di.tp = dtpObj; di.subtp = ci->id;
  di.max_len = 0; di.dlg_x = di.dlg_y = di.dlg_w = di.dlg_h = 0;
  di.hval = di.dyna = 0; di.flags = 0; di.v_min = di.v_max = 0;
  di.name[0] = 0;
  strncat( di.name, oname, MAX_NAMELEN-1 );
  di.descr = ""; 
  di.listdata = "";
  model->insElem( &di, oord, sel_x, sel_y) ;
  changeSel( 0, 0, 1 ); // update sel
  editElm();
}

void QMo2View::delElm()
{
  int k;
  if( ! checkState( selCheck ) )
    return;	  
  k = QMessageBox::information( this, PACKAGE " delete confirmation",
      "Do you really want to delete this element ?",
      "&Yes", "&No", "&Help", 0, 1 );
  if( k == 0 ) {
    model->delElem( sel );
    if( sel == mark )
      mark = -1;
    sel = model->xy2elnu( sel_x, sel_y );
    emit viewChanged();
  };
}

void QMo2View::editElm()
{
  QAnyDialog *adi;
  TMiso *ob;
  int rc;	
  if( ! checkState( selCheck ) )
    return;	  
  model->reset();
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  adi = new QAnyDialog( ob, this, "edit_dial" );
  rc = adi->exec_trans();
  if( rc == QDialog::Accepted ) {
    model->reset();
    model->setModified();
  };
  emit viewChanged();
  delete adi;
}

void QMo2View::linkElm()
{
  QAnyDialog *adi;
  TMiso *ob;
  TElmLink *el;
  int rc;
  if( ! checkState( selCheck ) )
    return;	  
  model->reset();
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return ;
  adi = new QAnyDialog( el, this, "link_dial" );
  rc = adi->exec_trans();
  delete adi;
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
  const char *toname;
  char oldlink[MAX_NAMELEN], lnkname[MAX_NAMELEN];
  if( ! checkState( linkToCheck ) )
    return;	  
  model->reset();
  ob = model->getMiso( sel ); toob = model->getMiso( mark );
  if( ob == 0 || toob == 0 )
    return;
  toname = toob->getName();

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return;
  strcpy( lnkname, "inps0" );
  lnkname[4] = char( '0' + level );
  oldlink[0] = 0;
  k = el->getDataSS( lnkname, oldlink, MAX_NAMELEN, 0 );
  if( k != 0 || oldlink[0] != 0 )
    return;
  k = el->setDataSS( lnkname, toname, 0 );
  model->reset(); model->setModified();
  emit viewChanged();
}


void QMo2View::qplinkElm()
{
  TMiso *ob, *toob;
  TElmLink *el;
  int k;
  const char *toname;
  char oldlink[MAX_NAMELEN], lnkname[MAX_NAMELEN];
  if( ! checkState( linkToCheck ) )
    return;	  
  model->reset();
  ob = model->getMiso( sel ); toob = model->getMiso( mark );
  if( ob == 0 || toob == 0 )
    return;
  toname = toob->getName();

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return;
  strcpy( lnkname, "pinps0" );
  lnkname[5] = char( '0' + level );
  oldlink[0] = 0;
  k = el->getDataSS( lnkname, oldlink, MAX_NAMELEN, 0 );
  if( k != 0 || oldlink[0] != 0 )
    return;
  k = el->setDataSS( lnkname, toname, 0 );
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
  model->reset();
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;

  el = static_cast<TElmLink*>( ob->getObj( "links" ) );
  if( el == 0 ) return;
  strcpy( lnkname, "inps0" );
  for( k=0; k<4; k++ ) {
    lnkname[4] = char( '0' + k );
    el->setDataSS( lnkname, "", 0 );
  };
  strcpy( lnkname, "pinps0" );
  for( k=0; k<4; k++ ) {
    lnkname[5] = char( '0' + k );
    el->setDataSS( lnkname, "", 0 );
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
  model->reset();
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  ob->getDataSI( "links.locked", &lck, 0 );
  lck = !lck;
  ob->setDataSI( "links.locked", lck, 0 );
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
  ob->getDataSI( "ord", &old_ord, 0 );
  new_ord = QInputDialog::getInteger("New element order", 
      "Input new element order", 
      old_ord, 0, 2147483647, 1, &ok, this, "order_dialog");
  if( ok ) {
    model->newOrder( sel, new_ord );
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
  const TDataInfo *di;
  int i, j, nelm, ibuf;
  char cbuf[256];
  double dbuf;
  QDialog *dia; QListView *lv; QPushButton *bt_ok;
  QVBoxLayout *lay;
  QString qs;
  static const char *lbl[7] = { "MR", "NRC", "ND", "ROD", "NS", "RO" , "?1" };
  if( ! checkState( selCheck ) )
    return;	  
  ob = model->getMiso( sel );
  if( ob == 0 )
    return;
  nelm = ob->getN();
  dia = new QDialog( this, "info_elm_dia", true );
  dia->setCaption( QString( PACKAGE ": Structure of ") + ob->getName() );

  lay = new QVBoxLayout( dia, 10, 10, "info_vboxlay" );

  lv = new QListView( dia, "list" );
  lv->addColumn( "N", 24 ); lv->addColumn( "Name", 80 );
  lv->addColumn( "Type", 38 ); lv->addColumn( "Value", 150 );
  lv->addColumn( "Descr", 120 ); lv->addColumn( "Flags", 100 );
  lv->setSorting( -1 );
  for( i=nelm-1; i>=0; i-- ) {
    di = ob->getDataInfo( i );
    if( di == 0 ) continue;
    qs = "";
    for( j=0; j<7; j++ ) {
      if( di->flags & ( 1 << j ) ) {
	qs += lbl[j]; qs += ",";
      };
    };
    ibuf = 0; dbuf = 0; cbuf[0] = 0;
    switch( di->tp ) {
      case dtpInt: ob->getDataII( i, &ibuf, 0 );
		   (void) new QListViewItem( lv, QString::number(i), 
					     di->name, "i/" + QString::number( di->subtp ),
					     QString::number( ibuf ),
					     QString::fromLocal8Bit(di->descr), qs );
		   break;
      case dtpDbl: ob->getDataID( i, &dbuf, 0 );
		   (void) new QListViewItem( lv, QString::number(i), 
					     di->name, "d", QString::number( dbuf ), 
					     QString::fromLocal8Bit(di->descr), qs );
		   break;
      case dtpStr: ob->getDataIS( i, cbuf, sizeof(cbuf), 0 );
		   (void) new QListViewItem( lv, QString::number(i), di->name,
					     QString("s") + QString::number( di->max_len ),
					     QString::fromLocal8Bit( cbuf ), 
					     QString::fromLocal8Bit(di->descr), qs );
		   break;
      default: (void) new QListViewItem( lv, QString::number(i), di->name,
					 QString::number(di->tp), di->listdata, 
					 QString::fromLocal8Bit(di->descr), qs );
    };
  };
  lay->addWidget( lv );

  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Done" );
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;
  emit viewChanged();
}


// ==== outs related 

void QMo2View::newOut()
{
  int idx, rc, no;
  const TDataInfo *di;
  QString onameq, enameq;
  const char *outname, *elmname;
  QDialog *dia; QPushButton *bt_ok, *bt_can;
  QLineEdit *oname_ed, *ename_ed; QLabel *lab1, *lab2;
  QVBoxLayout *main_lay;
  QGridLayout *grid_lay;
  QHBoxLayout *btn_lay;
  if( ! checkState( validCheck ) )
    return;
  if( sel >=0 && (idx = model->elnu2idx( sel )) >=0
      && (di = model->getDataInfo( idx )) != 0 )
  {
    enameq = di->name;
  } else {
    enameq = ":t";
  };
  no = model->getNOutArr();
  onameq = QString("out") + QString::number( no );

  dia = new QDialog( this, "newout_dial", true );
  dia->setCaption( "Creating new output array" );

  main_lay = new QVBoxLayout( dia, 10, 10, "main_newout_lay" );
  grid_lay = new QGridLayout( main_lay, 2, 2, -1, "grig_newout" );
  btn_lay =  new QHBoxLayout( main_lay, -1, "btn_newout" );

  lab1 = new QLabel( dia, "l_oname" );
  lab1->setText( "Output array name" );
  grid_lay->addWidget( lab1, 0, 0 );

  oname_ed = new QLineEdit( dia, "oname_ed" );
  oname_ed->setMaxLength( MAX_NAMELEN-1 );
  oname_ed->setText( onameq ); oname_ed->setFocus();
  grid_lay->addWidget( oname_ed, 0, 1 );

  lab2 = new QLabel( dia, "l_ename" );
  lab2->setText( "Element name" );
  grid_lay->addWidget( lab2, 1, 0 );

  ename_ed = new QLineEdit( dia, "ename_ed" );
  ename_ed->setMaxLength( MAX_NAMELEN-1 );
  ename_ed->setText( enameq );
  grid_lay->addWidget( ename_ed, 1, 1 );


  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Ok" ); bt_ok->setDefault( true );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept() ) );
  btn_lay->addWidget( bt_ok );


  bt_can = new QPushButton( dia, "bt_can" );
  bt_can->setText( "Cancel" );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject() ) );
  btn_lay->addWidget( bt_can );

  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    onameq = oname_ed->text(); enameq = ename_ed->text();
    outname = onameq.latin1(); elmname = enameq.latin1();
    if( isGoodName( outname ) ) {
      model->insOut( outname, elmname );
      emit viewChanged();
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
  k = QMessageBox::information( this, PACKAGE " delete confirmation",
      "Do you really want to delete this output array ?",
      "&Yes", "&No", "&Help", 0, 1 );
  if( k == 0 ) {
    model->delOut( level );
    emit viewChanged();
  };
}


void QMo2View::editOut()
{
  QAnyDialog *adi;
  TOutArr *arr;
  int rc;
  if( ! checkState( validCheck ) )
    return;	
  model->reset();
  if( level < 0 || level >= model->getNOutArr() )
    return;
  arr = model->getOutArr( level );
  if( arr == 0 )
    return;
  adi = new QAnyDialog( arr, this, "out_dial" );
  rc = adi->exec_trans();
  delete adi;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
  };
}

void QMo2View::showOutData()
{
  QDialog *dia;
  QDoubleTable *dv;
  QVBoxLayout *lv;
  QHBoxLayout *lg;
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
  double vmin = numeric_limits<double>::max(), vmax = numeric_limits<double>::min();
  for( int i=0; i<gi.row; i++ ) {
    x =  gi.dat[0][i];
    s += x; s2 += x * x;
    if( x < vmin ) vmin = x;
    if( x > vmax ) vmax = x;
  };
  ave = s / gi.row; ave2 = s2 / gi.row;
  disp = ave2 - ave * ave;
  msq = sqrt( disp );


  dia = new QDialog( this, "data_dial", 1 );
  dia->setCaption( QString("Data array: ") + gi.title );
  lv = new QVBoxLayout( dia, 10, 6, "showOutData_vbox_layout" );

  lg = new QHBoxLayout( lv, -1, "showOutData_gbox_layout" );

  dv = new QDoubleTable( &gi,  dia, "dv" );
  lg->addWidget( dv );

  lab = new QLabel( dia, "showOutData_lab" );
  sinf = QString( "n= " ) + QString::number( gi.row );
  sinf += QString( "; \nave= " ) + QString::number( ave ); 
  sinf += QString( "; \nave2= " ) + QString::number( ave2 ); 
  sinf += QString( "; \nsum= " ) + QString::number( s ); 
  sinf += QString( "; \nsum2= " ) + QString::number( s2 ); 
  sinf += QString( ";\nD= " ) + QString::number( disp ); 
  sinf += QString( "; \nsigm= " ) + QString::number( msq ); 
  sinf += QString( "; \nmin= " ) + QString::number( vmin ); 
  sinf += QString( "; \nmax= " ) + QString::number( vmax ); 
  lab->setText( sinf );
  lg->addWidget(lab);

  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Done" ); bt_ok->setDefault( true );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  lv->addWidget( bt_ok );

  dia->exec();
  delete dia;
}

void QMo2View::exportOut()
{
  const char *fn;
  QString fnq;
  TOutArr *arr;
  if( ! checkState( doneCheck ) )
    return;	
  arr = model->getOutArr( level );
  if( arr == 0 )
    return;
  fnq = QFileDialog::getSaveFileName( 0, 
      "Data files (*.txt *.dat *.csv);;All files (*)", this );
  if( fnq.isEmpty() )
    return;
  fn = fnq.latin1();
  arr->dump( fn, ' ' );
}


// ==== graph related

void QMo2View::newGraph()
{
  int no;
  QString grnameq, aname;
  bool ok;
  const char *grname;
  if( ! checkState( validCheck ) )
    return;	
  no = model->getNGraph();
  grnameq = QString("graph") + QString::number( no ); 
  aname = QInputDialog::getText( "Creating new Graph descriptions",
      "Enter name of new Graph:", QLineEdit::Normal, 
      grnameq, &ok, this, "newgraph_dialog" );
  if( ok ) {
    grname = aname.latin1();
    if( isGoodName( grname ) ) {
      model->insGraph( grname );
      emit viewChanged();
    };
  };
}


void QMo2View::delGraph()
{
  int k;
  if( ! checkState( validCheck ) )
    return;	
  if( level < 0 || level >= model->getNGraph() )
    return;
  k = QMessageBox::information( this, PACKAGE " delete confirmation",
      "Do you really want to delete this graph description?",
      "&Yes", "&No", "&Help", 0, 1 );
  if( k == 0 ) {
    model->delGraph( level );
    emit viewChanged();
  };
}


void QMo2View::editGraph()
{
  QAnyDialog *adi;
  TGraph *gra;
  int rc;
  if( ! checkState( validCheck ) )
    return;	
  model->reset();
  if( level < 0 || level >= model->getNGraph() )
    return;
  gra = model->getGraph( level );
  if( gra == 0 )
    return;
  adi = new QAnyDialog( gra, this, "graph_dial" );
  rc = adi->exec_trans();
  delete adi;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
  };
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
  plotWnd = new QMainWindow( this, "plot_wnd1", 
      WType_TopLevel | WDestructiveClose );
  plotWnd->setCaption( QString( PACKAGE ": Plot ") + QString(gra->getName()) );
  pv = new QPlotView( doc, gra, plotWnd, "plot_view" );
  plotWnd->setCentralWidget( pv );
  pv->setFocus();
  plotWnd->show();
}

void QMo2View::showGraphData()
{
  QDialog *dia;
  QDoubleTable *dv;
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

  dia = new QDialog( this, "graphdata_dial", 1 );
  dia->setCaption( QString("Graph data: ") + gi.title );
  lv = new QVBoxLayout( dia, 10, 6, "showFraphData_vbox_layout" );

  dv = new QDoubleTable( &gi, dia,  "dv" );
  lv->addWidget( dv );

  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Done" ); bt_ok->setDefault( true );
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
  fnq = QFileDialog::getSaveFileName( 0, 
      "Data files (*.txt *.dat *.csv);;All files (*)", this );
  if( fnq.isEmpty() )
    return;
  fn = fnq.latin1();
  gra->dump( fn, ' ' );
}

void QMo2View::gnuplotGraph()
{
  TGraph *gra;
  QDialog *dia;
  QLabel *lb1, *lb2, *lb3; QLineEdit *ed_pgm, *ed_dat, *ed_eps;
  QCheckBox *sw_x11; QPushButton *bt_ok, *bt_can;
  QVBoxLayout *lv; QHBoxLayout *lg;
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
  cdir = QDir::currentDirPath();
  if( cdir == doc_fi.dirPath() ) 
    f_pgm = doc_fi.fileName();
  if( f_pgm.length() < 1 ) { f_pgm = "gplot"; };
  l = f_pgm.length();
  if( doc_fi.extension(false) == "mo2"  ) 
    f_pgm.truncate( l-4 );
  f_dat = f_pgm + ".dat"; f_eps = f_pgm + ".eps";
  f_pgm += ".gpl";

  dia = new QDialog( this, "gnuplot_dia", true );
  dia->resize( 400, 260 );
  lv = new QVBoxLayout( dia, 10, 6, "gnuplot_vbox_layouot" );
  sw_x11 = new QCheckBox( dia, "sw_x11" );
  sw_x11->setText( "Output to &X11 window" );
  sw_x11->setChecked( false );
  lv->addWidget( sw_x11 );
  lb1 = new QLabel( dia, "l1" );
  lb1->setText( "Output to EPS file:" );
  lv->addWidget( lb1 );
  ed_eps = new QLineEdit( dia, "ed_eps" );
  ed_eps->setText( f_eps );
  lv->addWidget( ed_eps );
  lb2 = new QLabel( dia, "l2" );
  lb2->setText( "Data file:" );
  lv->addWidget( lb2 );
  ed_dat = new QLineEdit( dia, "ed_dat" );
  ed_dat->setText( f_dat );
  lv->addWidget( ed_dat );
  lb3 = new QLabel( dia, "l3" );
  lb3->setText( "Gnuplot program file:" );
  lv->addWidget( lb3 );
  ed_pgm = new QLineEdit( dia, "ed_pgm" );
  ed_pgm->setText( f_pgm );
  lv->addWidget( ed_pgm );
  lg = new QHBoxLayout( lv, 10, "gnuplot_btn" );
  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Ok" ); bt_ok->setDefault( true );
  lg->addWidget( bt_ok );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept() ) );
  bt_can = new QPushButton( dia, "bt_can" );
  bt_can->setText( "Cancel" );
  lg->addWidget( bt_can );
  connect( bt_can, SIGNAL(clicked()), dia, SLOT(reject() ) );
  rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    f_pgm = ed_pgm->text(); f_eps = ed_eps->text(); 
    f_dat = ed_dat->text(); use_x11 = sw_x11->isChecked();
    gra->gnuPlot( !use_x11, f_pgm.local8Bit(), 0, f_eps.local8Bit(), f_dat.local8Bit() );
  };
  delete dia;
}

// ==== model related

void QMo2View::editModel()
{
  QAnyDialog *adi;
  int rc;
  if( ! checkState( validCheck ) )
    return;	
  model->reset();
  adi = new QAnyDialog( model, this, "mod_dial" );
  rc = adi->exec_trans();
  if( rc == QDialog::Accepted ) {
    model->reset();
    emit viewChanged();
  };
}


void QMo2View::showVars()
{
  QDialog *dia;
  QDoubleTable *dv;
  QVBoxLayout *lv;
  GraphInfo gi;
  const double *vars;
  QString fnq; QPushButton *bt_ok;
  if( ! checkState( validCheck ) )
    return;	
  if( (vars = model->getVars()) == 0 )
    return;
  gi.row = MODEL_NVAR; gi.col = 1; gi.ny = gi.row;
  strcpy( gi.title, "Model vars" );
  gi.dat[0] = vars;
  strcpy( gi.label[0], "vars" );

  dia = new QDialog( this, "vars_dial", 1 );
  dia->setCaption( QString("Model vars: ") );
  lv = new QVBoxLayout( dia, 10, 6, "showVars_vbox_layouot" );
  dv = new QDoubleTable( &gi, dia,  "dv" );
  lv->addWidget( dv );
  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "Done" ); bt_ok->setDefault( true );
  lv->addWidget( bt_ok );
  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;
}

// ==== run related

void QMo2View::runRun()
{
  QRunView *rv;
  if( ! checkState( validCheck ) )
    return;	
  rv = new QRunView( model, 0, this, "run_view", 
      WType_Dialog | WDestructiveClose );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void QMo2View::runPrm()
{
  if( ! checkState( validCheck ) )
    return;	
  QRunView *rv;
  rv = new QRunView( model, 1, 0, "run_view",
      WType_Dialog | WDestructiveClose );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void QMo2View::runPrm2()
{
  if( ! checkState( validCheck ) )
    return;	
  QRunView *rv;
  rv = new QRunView( model, 2, 0, "run_view", 
      WType_Dialog | WDestructiveClose );
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
  dia = new QDialog( this, "help_dia", true );
  dia->setCaption( "Hot keys in structure window" );
  lv = new QVBoxLayout( dia, 10, 6, "showHelp_vbox_layouot" );
  
  la = new QLabel( dia, "helplabel" );
  la->setText( helpstr );
  lv->addWidget( la );
  
  bt_ok = new QPushButton( dia, "bt_ok" );
  bt_ok->setText( "&Ok" );
  bt_ok->setDefault( true );
  lv->addWidget( bt_ok );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;
}


// end of qmo2view.cpp


