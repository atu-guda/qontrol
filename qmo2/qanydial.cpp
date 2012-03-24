/***************************************************************************
                          qanydial.cpp  -  description
                             -------------------
    begin                : Mon Jun 26 2000
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

#include <QString>
#include <QtGui>

#include "../config.h"
#include "qcolorbtn.h"
#include "qmo2win.h"
#include "qanydial.h"

QAnyDialog::QAnyDialog( TDataSet *ads, QWidget *parent ) 
           : QDialog( parent )
{
  elms = 0; ds = ads; 
  initDialog();
}

QAnyDialog::~QAnyDialog()
{
  delete[] elms;
}

void  QAnyDialog::initDialog(void)
{
  int i, ne, end, x, y, h, w, k;
  QLineEdit *qle;
  QCheckBox *qch; QComboBox *qcb; 
  QTextEdit *te;
  QLabel *qla; QGroupBox *qgb; QPushButton *qpb; QColorBtn *qcob;
  QString qs;
  char oname[MAX_INPUTLEN];
  ne = ds->getN(); end = 0;
  elms = new QWidget* [ne+2];
  const TDataInfo *di;
  for( i=0; i<ne && (!end); i++ ) {
    di = ds->getDataInfo( i );
    if( !di ) break;
    if( (di->flags) & efNoDial ) continue; // hidden element
    x = di->dlg_x; y = di->dlg_y;  w = di->dlg_w; h = di->dlg_h;
    switch( di->tp ) {
      case dtpEnd:  end = 1; break;
      case dtpInt:  switch( di->subtp ) {
                     case dtpsInt:
		          qle = new QLineEdit( this );
		          qle->setMaxLength( 12 );
			  qle->setGeometry( x, y, w, h );
			  //qle->setEnabled( !(di->flags & efRODial) );
			  if( di->v_max < di->v_min )
  			    qle->setValidator( new QIntValidator(this) );
			  else
			    qle->setValidator( new QIntValidator(  int( di->v_min ),
			                       int( di->v_max ), this )  );
			  elms[i] = qle;			
		          break;
		     case dtpsSwitch: 
		          qch = new QCheckBox( this );
			  qch->setGeometry( x, y, w, h );
			  // qch->setEnabled( !(di->flags & efRODial) );
			  qch->setText( di->listdata );
			  elms[i] = qch;
		          break;
		     case dtpsList:  
		          qcb = new QComboBox( this );
			  qcb->setGeometry( x, y, w, h );
			  //qcb->setEnabled( !(di->flags & efRODial) );
			  fillComboBox( qcb, di->listdata, di->max_len);
			  elms[i] = qcb;
		          break;
		     case dtpsColor:
		          qcob = new QColorBtn( this );
		          qcob->setGeometry( x, y, w, h );
		          elms[i] = qcob;
		          break;
                    }; break;
      case dtpDbl: qle = new QLineEdit( this ); 
		   qle->setMaxLength( 20 );
 		   qle->setGeometry( x, y, w, h );
		   //qle->setEnabled( !(di->flags & efRODial) );
		   if( di->v_max <= di->v_min )
  		     qle->setValidator( new QDoubleValidator( this ) );
 		   else     
		     qle->setValidator( new QDoubleValidator( di->v_min, 
		                              di->v_max, 12, this ) );
  		   elms[i] = qle; 
                   break;
      case dtpStr:  switch( di->subtp ) {
                      case dtpsStr: 
		        qle = new QLineEdit( this ); 
     		        qle->setMaxLength( di->max_len - 1 );
 		        qle->setGeometry( x, y, w, h );
		        //qle->setEnabled( !(di->flags & efRODial) );
		        elms[i] = qle; 
		        break;
		      case dtpsMline: 
		        te = new QTextEdit( this ); 
     		        // te->setMaxLength( di->max_len ); // ??
 		        te->setGeometry( x, y, w, h );
		        te->setEnabled( !(di->flags & efRODial) );
		        elms[i] = te; 
		        break;	
                    }; break;
      case dtpDial:  resize( w, h );
                     qs = QString( PACKAGE ": " ) + QString( di->listdata )
                        + QString( ": " );
                     k = ds->getFullName( oname );
		     if( k == 0 )
		       qs += oname;
                     setWindowTitle( qs );
                     break;  
      case dtpLabel: qla = new QLabel( this );
                     qla->setGeometry( x, y, w, h );
		     qla->setText( di->listdata );
		     elms[i] = qla;
		     break;
      case dtpGroup: qgb = new QGroupBox( this );
                     qgb->setGeometry( x, y, w, h );
		     qgb->setTitle( di->listdata );
		     elms[i] = qgb; 
                     break;
      case dtpButton: qpb = new QPushButton( this );
                     qpb->setGeometry( x, y, w, h );
		     qpb->setText( di->listdata );
		     switch( di->subtp ) {
		       case dtpsOK:  
			    qpb->setDefault( true );
		            connect( qpb, SIGNAL(clicked()), this, SLOT(accept()) );
		            break; 
		       case dtpsCancel:  
		            connect( qpb, SIGNAL(clicked()), this, SLOT(reject()) );
		            break; 
		       case dtpsHelp:  
			    connect( qpb, SIGNAL(clicked()), this, SLOT(showHelp()) );
			    break; 	    	    
		     };
		     elms[i] = qpb;  
                     break;
      default: break;
    }; // end switch( di->tp )
  }; // end loop on elements
  
}

int QAnyDialog::exec_trans(void)
{
  int i, rc, ne, iv, k, ena;
  double dv;
  QString sv;
  QTextEdit *te;
  QString qs;
  const TDataInfo *di;
  // fill from vars
  ne = ds->getN();
  for( i=0; i<ne; i++ ) {
     di = ds->getDataInfo( i );
     if( !di ) break;
     if( (di->flags) & efNoDial ) continue;
     ena = !(di->flags & efRODial);
     switch( di->tp ) {
       case dtpEnd: break;
       case dtpInt:
            k = ds->getDataII( i, &iv, 1 );
            if( k != 0 ) continue;
            switch( di->subtp ) {
	      case dtpsInt:
                   qs.setNum( iv );
                   ((QLineEdit*)(elms[i]))->setText( qs );
                   ((QLineEdit*)(elms[i]))->setEnabled( ena );
	           break;
	      case dtpsSwitch:
                   ((QCheckBox*)(elms[i]))->setChecked( iv != 0 );
                   ((QCheckBox*)(elms[i]))->setEnabled( ena );
	           break;   
	      case dtpsList:
                   ((QComboBox*)(elms[i]))->setCurrentIndex( iv );
                   ((QComboBox*)(elms[i]))->setEnabled( ena );
	           break;
	      case dtpsColor:
	           ((QColorBtn*)(elms[i]))->setColor( iv );
	           ((QColorBtn*)(elms[i]))->setEnabled( ena );
	           break;
	    };
            break;
       case dtpDbl: k = ds->getDataID( i, &dv, 1 );
            if( k != 0 ) continue;
            qs.setNum( dv, 'g', 8 );
            ((QLineEdit*)(elms[i]))->setText( qs );
            ((QLineEdit*)(elms[i]))->setEnabled( ena );
            break;
       case dtpStr:
            k = ds->getDataIS( i, &sv, di->max_len, 1 );
            qs = sv;
            if( k != 0 ) { continue; };
            switch( di->subtp ) {
	      case dtpsStr: ((QLineEdit*)(elms[i]))->setText( qs );
	           ((QLineEdit*)(elms[i]))->setEnabled( ena );
	           break;
	      case dtpsMline: 
		   te = (QTextEdit*)(elms[i]);
		   te->clear();
		   te->insertPlainText( qs );
	           te->setEnabled( ena );
	           break;	   
	    };
            break;        
       default: continue;
     };
  }; // for( ne )
  rc = exec();
  if( rc != Accepted ) 
     return rc;
  for( i=0; i<ne; i++ ) {
    // to vars
    di = ds->getDataInfo( i );
    if( !di ) break;
    if( (di->flags) & efNoDial || (di->flags) & efRODial  ) continue;
    switch( di->tp ) {
     case dtpEnd: break;
     case dtpInt:
          switch( di->subtp ) {
	   case dtpsInt: 
              qs = ((QLineEdit*)(elms[i]))->text();
              iv = qs.toInt();
	      break;
	   case dtpsSwitch:
	      iv = ((QCheckBox*)(elms[i]))->isChecked();
	      break;
	   case dtpsList: 
	      iv = ((QComboBox*)(elms[i]))->currentIndex();
	      break;
	   case dtpsColor:
	      iv = ((QColorBtn*)(elms[i]))->colorInt();
	      break;
	  };
	  ds->setDataII( i, iv, 1 );
          break;
     case dtpDbl: qs = ((QLineEdit*)(elms[i]))->text();
          dv = qs.toDouble();
          ds->setDataID( i, dv, 1 );
          break;
     case dtpStr:
          switch( di->subtp ) {
             case dtpsStr: qs = ((QLineEdit*)(elms[i]))->text();
                           ds->setDataIS( i, &qs, 1 );
                  break;
             case dtpsMline: qs = ((QTextEdit*)(elms[i]))->toPlainText();
                             ds->setDataIS( i, &qs, 1 );
                  break;	
          };
          break;
     default: continue;
    }; // switch( tp )
  }; // for( ne )
  return rc;
}

int fillComboBox( QComboBox *qb, const char *s, int mn )
{
  int i, j, k, k1, l;
  char buf[MAX_INPUTLEN];
  j = 0; l = strlen( s );
  for( i=0; i<mn; i++ ) {
    buf[0] = 0; k1 = 0;
    for( k=j; k<=l; k++ ) {
      if( s[k] == '\n' || s[k] == 0 ) {
        j = k+1; buf[k1] = 0;
        qb->addItem( QString::fromLocal8Bit(buf) );
        break;
      };
      buf[k1++] = s[k];
    };
  };
  return 0;
}

void QAnyDialog::showHelp(void)
{
  if( ds == 0 )
    return;
  QString cl_name = QString::fromLocal8Bit(ds->getClassName());
  if( cl_name.isEmpty() )
    return showSimpleHelp();
  QString resname = QString::fromLocal8Bit( "elems/" );
  resname += cl_name;
  resname += QString::fromLocal8Bit( "/index.html" );
  QString helpfile = QMo2Win::qmo2win->findRes( resname );
  // TODO: rich browser here or in next function
  //if( helpfile.isEmpty() )  
  //  return showSimpleHelp();
  return showSimpleHelp();
}

void QAnyDialog::showSimpleHelp(void)
{
  QDialog *dia;
  QTextBrowser *brow;
  QPushButton *bt_ok;
  QGridLayout *lay;
  const char *help;
  if( ds == 0 || (help = ds->getHelp()) == 0 )
    return;
  dia = new QDialog( this );
  dia->resize( 500, 480 );
  dia->setWindowTitle( PACKAGE ": Help on element" );
  
  lay = new QGridLayout;
  
  brow = new QTextBrowser( this );
  brow->insertHtml( help );
  lay->addWidget( brow, 0, 0 );

  bt_ok = new QPushButton( "&Ok", dia );
  lay->addWidget( bt_ok, 1, 0 );
  dia->setLayout( lay );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;

}
// end of qanydial.cpp

 
