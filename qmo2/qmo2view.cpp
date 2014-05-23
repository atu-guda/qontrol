/***************************************************************************
  qmo2view.cpp  -  description
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

#include <cmath>
// include files for Qt
#include <QtWidgets>

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
: QWidget( parent ), doc( pDoc ),
  root( doc->getRoot() ),
  model( doc->getModel() )
{
  QVBoxLayout *vlay = new QVBoxLayout( this );
  vlay->setContentsMargins( 2, 1, 2, 2 );

  QWidget *main_part = new QWidget( this );

  setAttribute(Qt::WA_DeleteOnClose);


  QGridLayout *grLay = new QGridLayout( main_part );
  grLay->setContentsMargins( 2, 2, 2, 2 );

  scrollArea = new QScrollArea( main_part );

  sview = new QStructView( doc, this, main_part );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  oview = new QOutView( doc, this, main_part );
  gview = new QGraphView( doc, this, main_part );

  stam = new QStatusModel( this, this );

  grLay->addWidget( scrollArea, 0, 0 );
  grLay->addWidget( oview, 0, 1 );
  grLay->addWidget( gview, 0, 2 );

  main_part->setLayout( grLay );

  vlay->addWidget( main_part );
  vlay->addWidget( stam );
  setLayout( vlay );

  setWindowTitle( doc->title() );

  connect( this, &QMo2View::viewChanged, this, &QMo2View::updateViews );
  connect( sview, &QStructView::sig_changeSel,   this, &QMo2View::changeSel );
  connect( sview, &QStructView::sig_changeLevel, this, &QMo2View::changeLevel );

}

QMo2View::~QMo2View()
{
  // DBGx( "dbg: view dtor, doc=%p", doc );
  delete doc; doc = nullptr;
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



void QMo2View::update()
{
  updateViews();
  QWidget::update();
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
        QMessageBox::Ok, QMessageBox::NoButton );
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
        QMessageBox::Ok, QMessageBox::NoButton );
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
  // treeView->update();
}


void QMo2View::changeSel( int x, int y, int rel )
{
  TMiso *ob;
  selObj = nullptr;
  switch( rel ) {
    case 0: sel_x = x; sel_y = y; break;
    case 1: sel_x += x; sel_y += y; break;
    case 2:
            ob = model->xy2Miso( sel_x, sel_y );
            if( !ob )
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
  aei.name = QString("obj_") + QSN( model->getNMiso() ) ;
  aei.order = model->hintOrd();
  AddElemDialog *dia = new AddElemDialog( &aei, model, this, "TMiso" );
                                          // limit to such elements here

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


void QMo2View::qlinkElm()
{
  QString toname;
  if( ! checkState( linkToCheck ) )
    return;

  if( !selObj || !markObj )
    return;
  toname = markObj->objectName();

  // TODO: its model action

  InputSimple *in = selObj->getInput( level );
  if( ! in )
    return;
  if( ! in->setData( "source", toname ) ) {
    DBGx( "warn: fail to set source in \"%s\" to \"%s\"",
          qP(in->objectName()), qP(toname) );
    return;
  }
  model->reportStructChanged();
  model->reset();
  model->setModified();
  emit viewChanged();
}


void QMo2View::qplinkElm()
{
  QString oldlink;
  if( ! checkState( linkToCheck ) )
    return;

  if( !selObj || !markObj  )
    return;
  QString toname = markObj->objectName();

  InputParams *pis = selObj->getElemT<InputParams*>("pis");
  if( !pis ) {
    DBG2q( "err: no pis object in ", selObj->getFullName() );
    return;
  }
  int n_pi = pis->getNumObj();
  QString pi_name = QString("pi_") + QSN(n_pi);
  InputParam *pi = pis->addObj<InputParam>( pi_name );
  if( !pi ) {
    return;
  }

  pi->setData( "source", toname );
  pi->setData( "line_w", 2 );
  pi->setData( "line_color", "red" );

  model->reportStructChanged();
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::unlinkElm()
{
  if( ! checkState( selCheck ) ) // no need marked to unlink
    return;

  QString lnkname;
  QString none("");
  int ni = selObj->inputsCount();
  for( int i=0; i<ni; ++i ) {
    InputSimple *in = selObj->getInput( i );
    if( ! in )
      continue;
    in->setData( "source", none );
  }

  InputParams *pis = selObj->getElemT<InputParams*>("pis");
  if( !pis ) {
    DBG2q( "err: no pis object in ", selObj->getFullName() );
    return;
  }
  qDeleteAll( pis->children() );

  model->reportStructChanged();
  model->reset(); model->setModified();
  emit viewChanged();
}

void QMo2View::lockElm()
{
  int lck;
  if( ! checkState( selCheck ) )
    return;

  selObj->getData( "locked", &lck );
  lck = !lck;
  selObj->setData( "locked", lck );

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
      tv->setItem( i, 1, new QTableWidgetItem(ds->getType()) );
      tv->setItem( i, 2, new QTableWidgetItem( ds->toString() ) );
    } else if( ob->inherits("HolderData" ) ) {
      HolderData *ho = qobject_cast<HolderData*>(ob);
      tv->setItem( i, 1, new QTableWidgetItem(ho->getType() ) );
      tv->setItem( i, 2, new QTableWidgetItem(ho->toString() ) );
      tv->setItem( i, 3, new QTableWidgetItem(ho->getParm("vis_name") + " \""
                    + ho->getParm("descr" ) + "\"" ) );
      tv->setItem( i, 4, new QTableWidgetItem( ho->objectName() ) );
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

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 720, 500 ); // TODO: adjust to inner table width
  dia->exec();
  delete dia;
  emit viewChanged();
}

void QMo2View::showTreeElm()
{
  if( ! checkState( selCheck ) )
    return;

  HolderModel *ho_elm = new HolderModel( selObj, this );

  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Element tree: ") + selObj->objectName() );

  QVBoxLayout *lay = new QVBoxLayout();


  QTreeView *treeView = new QTreeView( dia );
  treeView->setModel( ho_elm );

  // scroll is in view
  lay->addWidget( treeView );


  QPushButton *bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  dia->resize( 600, 400 ); // TODO: unmagic
  dia->exec();
  delete dia;
  delete ho_elm;
  emit viewChanged();
  return;
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

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  dia->resize( 600, 300 ); // TODO: unmagic
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
  while( model->getElem( elname ) && suff_n < 50 ) { // guess good name
    elname = elname_base + "_" + QSN( suff_n );
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
  oord_ed->setText( QSN(oord) );
  lay->addWidget( oord_ed, 1, 1 );

  QDialogButtonBox *bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 2, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

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
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

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
    x =  (*gi.dat[0])[i];
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

  sinf = QString( "n= " ) % QSN( gi.row )
       % QString( "; \nave= " ) % QSN( ave )
       % QString( "; \nave2= " ) % QSN( ave2 )
       % QString( "; \nsum= " ) % QSN( s )
       % QString( "; \nsum2= " ) % QSN( s2 )
       % QString( ";\nD= " ) % QSN( disp )
       % QString( "; \nsigm= " ) % QSN( msq )
       % QString( "; \nmin= " ) % QSN( vmin )
       % QString( "; \nmax= " ) % QSN( vmax );
  lab = new QLabel( sinf, dia );
  lab->setTextInteractionFlags( Qt::TextSelectableByMouse
       | Qt::TextSelectableByKeyboard);
  lay->addWidget( lab, 0, 1 );

  bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
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
  grnameq = QString("graph") + QSN( no );
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
  dia->setWindowTitle( QString("Graph data: ") + gi.title );
  lv = new QVBoxLayout( dia );

  dmod = new DoubleTableModel( &gi, dia );
  dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lv->addWidget( dtv );

  bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
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
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

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
    model->setModified();
    emit viewChanged();
  };
}


void QMo2View::showTreeModel()
{
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Model ") );

  QVBoxLayout *lay = new QVBoxLayout();

  HolderModel *ho_mo = new HolderModel( root, this );
  QTreeView *treeView = new QTreeView( dia );
  treeView->setModel( ho_mo );

  lay->addWidget( treeView );

  QPushButton *bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  dia->resize( 600, 400 ); // TODO: unmagic
  dia->exec();
  delete dia;
  delete ho_mo;
  emit viewChanged();
  return;
}

void QMo2View::runScript()
{
  if( ! doc || ! root ) {
    DBG1( "ERR: can run script w/o doc or root" );
    return;
  }
  // TODO: special class to edit js and view results

  auto *dia = new QDialog( this );
  dia->setWindowTitle( "Edit script" );
  QVBoxLayout *lv = new QVBoxLayout( dia );

  auto *ted = new QTextEdit( dia );
  ted->setText( scr );
  lv->addWidget( ted );

  auto bt_ok = new QPushButton( "&Ok", dia );
  // bt_ok->setDefault( true );
  lv->addWidget( bt_ok );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  dia->resize( 800, 600 );

  int rc = dia->exec();
  QString res;
  if( rc != QDialog::Accepted ) {
    delete dia; dia = nullptr;
    return;
  }
  scr = ted->toPlainText();
  delete dia; dia = nullptr;

  res = doc->runScript( scr );

  auto *dia1 = new QDialog( this );
  dia1->setWindowTitle( "Script result" );
  QVBoxLayout *lv1 = new QVBoxLayout( dia1 );

  auto *ted1 = new QTextEdit( dia1 );
  ted1->setText( res );
  ted1->setReadOnly( true );
  lv1->addWidget( ted1 );

  auto bt_ok1 = new QPushButton( "&Ok", dia1 );
  lv1->addWidget( bt_ok1 );

  connect( bt_ok1, &QPushButton::clicked, dia1, &QDialog::accept );
  dia1->resize( 800, 600 );
  dia1->exec();
  delete dia1; dia1 = nullptr;

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

void QMo2View::newSimul()
{
  bool ok;
  if( ! checkState( validCheck ) )
    return;
  QString simName = QString("sim") + QSN( sel );
  simName = QInputDialog::getText( this, "Creating new Simulation",
      "Enter name of new simulation:", QLineEdit::Normal,
      simName, &ok );
  if( ok ) {
    if( ! isGoodName( simName ) ) {
      QMessageBox::critical( this, "Error",
         QString("Bad simulation name: \"") + simName + "\"",
         QMessageBox::Ok, QMessageBox::NoButton );
    }
    model->newSimul( simName );
    emit viewChanged();
  };
}

void QMo2View::delSimul()
{
}

void QMo2View::editSimul()
{
}

void QMo2View::setActiveSimul()
{
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

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  dia->exec();
  delete dia;
}


// end of qmo2view.cpp


