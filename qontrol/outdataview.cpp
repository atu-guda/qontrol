/***************************************************************************
                          outdataview.cpp - view for outs
                             -------------------
    begin                : 2014.11.15
    copyright            : (C) 2014-2016 by atu
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

#include "dataset.h"
#include "labowin.h"
#include "addelemdia.h"
#include "doubletable.h"
#include "miscfun.h"
#include "outdataview.h"
#include "commonsubwin.h"
#include "tgraph.h"
#include "longvalidator.h"


OutDataView::OutDataView( HolderData *a_mod, CommonSubwin *a_par )
  : CmdListView( a_mod, a_par )
{
  init_actions();

  QPalette s_pal = palette();
  s_pal.setColor( QPalette::Base, QColor( 120,220,252 ) );
  setPalette( s_pal );
}

void OutDataView::init_actions()
{
  auto a = new QAction( QIcon::fromTheme(QSL("arrow-right")), QSL("&Add to plot"), this );
  lv->addAction( a  );
  connect( a, SIGNAL(triggered()), this, SLOT(addToPlot()) );

  a = new QAction( QIcon::fromTheme(QSL("document-export")), QSL("Export"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(exportObj()) );

  a = new QAction( QIcon::fromTheme(QSL("document-Import")), QSL("Import as text"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(importTxt()) );

  a = new QAction( QSL("Fill"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(fill()) );

  a = new QAction(  QIcon( QSL(":icons/showoutdata.png") ), QSL("&Show data"), this );
  lv->addAction( a );
  connect( a, SIGNAL(triggered()), this, SLOT(showDataObj()) );
}

bool OutDataView::addObj()
{
  QString tgt_nm = par->getSelNameInView( QSL("sview") );
  QString out_nm;

  if( !tgt_nm.isEmpty() ) {
    out_nm = QSL("out_") % tgt_nm % QSL("!");
  } else {
    tgt_nm = QSL("t");
    out_nm = QSL("out_t!");
  };
  sugg_name = out_nm;

  if( ! CmdView::addObj() ) {
    return false;
  }

  QString nm = getLastObjName();
  HolderData *ob = storage->getObj( nm );
  if( !ob ) {
    return false;
  }

  ob->setData( QSL("name.source"), tgt_nm );
  ob->setData( QSL("label"), tgt_nm );
  ::editObj( this, ob );
  ob->reset();
  emit viewChanged();
  return true;
}

bool OutDataView::showDataObj()
{
  DatasInfo di;

  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  int k = arr->fillDatasInfo( &di );
  if( !k ) {
    qWarning() << "fail to fill info about output array " <<  arr->getFullName() << WHE;
    return false;
  }

  // calculate statistical data TODO: separate struct and func (or/and in TOutArr)
  arr->calc_stat( true, true ); // ensure all and fresh data
  QString sinf = arr->getAllStats( QSL(";\n") );


  auto dia = new QDialog( this );
  dia->setWindowTitle( QSL("Data array: ") % di.title );
  auto lay = new QGridLayout( dia );

  auto dmod = new DoubleTableModel( di, dia );
  auto dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lay->addWidget( dtv, 0, 0 );

  auto lab = new QLabel( sinf, dia );
  lab->setTextInteractionFlags( Qt::TextSelectableByMouse
       | Qt::TextSelectableByKeyboard );
  lay->addWidget( lab, 0, 1 );

  auto bt_ok = new QPushButton( QSL("Done"), dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  lay->addWidget( bt_ok, 1, 0, 1, 2 );
  dia->setLayout( lay );

  dia->exec();
  delete dia;
  return true;
}

bool OutDataView::exportObj()
{
  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  QString fnq = QFileDialog::getSaveFileName( this,
      QSL("Export data from \"") % arr->objectName() % QSL("\""),
      arr->objectName() % data_file_suff, data_files_sel );
  if( fnq.isEmpty() ) {
    return false;
  }
  arr->dump( fnq, QSL(" ") );
  return true;
}

bool OutDataView::addToPlot()
{
  QString nm_o = getSelName();
  if( nm_o.isEmpty() ) {
    return false;
  }

  TGraph *gra = qobject_cast<TGraph*>( par->getSelectedInView( QSL("plots_view") ) );
  if( !gra ) {
    qWarning() << "No selected plot found for " << nm_o << WHE;
    return false;
  }

  int rc = gra->addOutArr( nm_o ); // complex fun
  if( !rc ) {
    qWarning() << "Fail to add output array " << nm_o << " to plot " << gra->objectName() << WHE;
    return false;
  }
  // TODO: model reset
  emit viewChanged();
  return true;
}


bool OutDataView::importTxt()
{
  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  QString fnq = QFileDialog::getOpenFileName( this,
       QSL("Import text data from file"),
       QSL("."), data_files_sel );
  if( fnq.isEmpty() ) {
     return false;
  }

  int col = 0; long n_start = 0, n_max = 100000000; char sep = ' ';

  auto dia = new QDialog( this );
  dia->setWindowTitle( QSL("Import data array: ") % arr->getFullName() );
  auto lay = new QGridLayout( dia );

  auto lbl_col = new QLabel( QSL("column"), dia );
  lay->addWidget( lbl_col, 0, 0 );

  auto ed_col = new QLineEdit( this );
  ed_col->setText( QSN( col  ) );
  ed_col->setValidator( new QIntValidator( 0, INT_MAX, this ) );
  lay->addWidget( ed_col, 0, 1 );

  auto lbl_start = new QLabel( QSL("start"), dia );
  lay->addWidget( lbl_start, 1, 0 );

  auto ed_start = new QLineEdit( this );
  ed_start->setText( QSL("0") );
  ed_start->setValidator( new LongValidator( 0, LONG_MAX, this ) );
  lay->addWidget( ed_start, 1, 1 );

  auto lbl_max = new QLabel( QSL("max"), dia );
  lay->addWidget( lbl_max, 2, 0 );

  auto ed_max = new QLineEdit( this );
  ed_max->setText( QSN( n_max ) );
  ed_max->setValidator( new LongValidator( 1, LONG_MAX, this ) );
  lay->addWidget( ed_max, 2, 1 );

  auto lbl_sep = new QLabel( QSL("separator"), dia );
  lay->addWidget( lbl_sep, 3, 0 );

  auto ed_sep = new QLineEdit( this );
  ed_sep->setText( QSL(" ") );
  lay->addWidget( ed_sep, 3, 1 );

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  lay->addWidget( bbox, 4, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  dia->setLayout( lay );

  if ( dia->exec() == QDialog::Accepted ) {
    col  = ed_col->text().toInt();
    n_start = ed_start->text().toLong();
    n_max   = ed_max->text().toLong();
    QString ssep = ed_sep->text();
    if( ssep.length() > 0 ) {
      sep = ssep[0].toLatin1();
    }
    long n_new = arr->importTxt( fnq, col, n_start, n_max, sep );
    qWarning() << "Imported " << n_new << "rows to " << arr->getFullName() << WHE;
  };

  delete dia;

  return arr->getN() > 0;
}


bool OutDataView::fill()
{
  TOutArr *arr = qobject_cast<TOutArr*>( getSelObj() );
  if( !arr ) {
    return false;
  }

  long a_nn = arr->getN();
  double start = 0, step = 0.1;

  auto dia = new QDialog( this );
  dia->setWindowTitle( QSL("Fill data array: ") % arr->getFullName() );
  auto lay = new QGridLayout( dia );

  auto lbl_n = new QLabel( QSL("n"), dia );
  lay->addWidget( lbl_n, 0, 0 );

  auto ed_n = new QLineEdit( this );
  ed_n->setText( QSN( a_nn  ) );
  ed_n->setValidator( new LongValidator( 1, LONG_MAX, this ) );
  lay->addWidget( ed_n, 0, 1 );

  auto lbl_start = new QLabel( QSL("start"), dia );
  lay->addWidget( lbl_start, 1, 0 );

  auto ed_start = new QLineEdit( this );
  ed_start->setText( QSL("0.0") );
  ed_start->setValidator( new QDoubleValidator( this ) );
  lay->addWidget( ed_start, 1, 1 );

  auto lbl_step = new QLabel( QSL("step"), dia );
  lay->addWidget( lbl_step, 2, 0 );

  auto ed_step = new QLineEdit( this );
  ed_step->setText( QSL("0.1") );
  ed_step->setValidator( new QDoubleValidator( this ) );
  lay->addWidget( ed_step, 2, 1 );

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  lay->addWidget( bbox, 3, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  dia->setLayout( lay );

  if ( dia->exec() == QDialog::Accepted ) {
    a_nn  = ed_n->text().toLong();
    start = ed_start->text().toDouble();
    step  = ed_step->text().toDouble();
    arr->fill( a_nn, start, step );
  };

  delete dia;

  return true;
}

