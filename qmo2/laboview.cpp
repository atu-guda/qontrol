/***************************************************************************
  laboview.cpp  -  description
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

#include <cmath>
// include files for Qt
#include <QtWidgets>

// application specific includes
#include "miscfun.h"
#include "laboview.h"
#include "labodoc.h"
#include "labowin.h"
#include "structview.h"
#include "outdataview.h"
#include "graphdataview.h"
#include "statusmodel.h"
#include "doubletable.h"
#include "runview.h"
#include "plotview.h"
#include "simulview.h"
#include "datawidget.h"
#include "addelemdia.h"

using namespace std;


LaboView::LaboView( LaboDoc* pDoc, QWidget *parent )
: QWidget( parent ), doc( pDoc ),
  root( doc->getRoot() ),
  model( doc->getModel() ),
  schems( model->getElemT<ContScheme*>( "schems" ) ),
  sch_main( schems->getElemT<Scheme*>("main") ),
  outs( model->getElemT<ContOut*>( "outs" ) ),
  plots( model->getElemT<ContGraph*>( "plots" ) ),
  sims( model->getElemT<ContSimul*>( "sims" ) )
{

  QVBoxLayout *vlay = new QVBoxLayout( this );
  vlay->setContentsMargins( 2, 1, 2, 2 );

  QWidget *main_part = new QWidget( this );

  setAttribute(Qt::WA_DeleteOnClose);


  QGridLayout *grLay = new QGridLayout( main_part );
  grLay->setContentsMargins( 2, 2, 2, 2 );

  scrollArea = new QScrollArea( main_part );


  sview = new StructView( sch_main, this );
  scrollArea->setWidget( sview );
  scrollArea->setLineWidth( 1 );
  scrollArea->setMidLineWidth( 1 );
  scrollArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
  scrollArea->setFocusProxy( sview );

  outs_view = new OutDataView( outs, this );

  plots_view = new GraphDataView( plots, this );

  sims_view = new SimulView( sims, this );


  stam = new StatusModel( this, this );

  grLay->addWidget( scrollArea, 0, 0 );
  grLay->addWidget( outs_view, 0, 1 );
  grLay->addWidget( plots_view, 0, 2 );
  grLay->addWidget( sims_view, 0, 3 );

  main_part->setLayout( grLay );

  vlay->addWidget( main_part );
  vlay->addWidget( stam );
  setLayout( vlay );

  setWindowTitle( doc->title() );

  connect( this, &LaboView::viewChanged, this, &LaboView::updateViews );
  connect( sview, &StructView::sig_changeSel,   this, &LaboView::changeSel );
  connect( sview, &StructView::sig_changeLevel, this, &LaboView::changeLevel );

}

LaboView::~LaboView()
{
  // DBGx( "dbg: view dtor, doc=%p", doc );
  delete doc; doc = nullptr; // BUG: is here?
}

LaboDoc *LaboView::getDocument() const
{
  return doc;
}

const QString& LaboView::currentFile() const
{
  return doc->pathName();
}

bool LaboView::confirmDelete( const QString &obj, const QString &nm )
{
  int rpl = QMessageBox::question( this, PACKAGE " delete confirmation",
       QString("Do you really want to delete %1 \"%2\"?").arg(obj).arg(nm) );
  return rpl == QMessageBox::Yes;
}

void LaboView::showError( const QString &s )
{
  QMessageBox::critical( this, PACKAGE ": Error", s,  QMessageBox::Ok );
}

void LaboView::showWarn( const QString &s )
{
  QMessageBox::warning( this, PACKAGE ": Warning", s, QMessageBox::Ok );
}


bool LaboView::editObj( HolderData *obj )
{
  if( !obj ) {
    return false;
  }

  DataDialog *dia = new DataDialog( *obj, this );
  int rc = dia->exec();
  delete dia;
  if( rc == QDialog::Accepted ) {
    model->reset(); model->setModified();
    emit viewChanged();
    return true;
  };
  return false;
}

QString LaboView::getSelName( QAbstractItemView *view )
{
  if( !view  ||  ! checkState( validCheck ) ) {
    return QString();
  }

  QItemSelectionModel *selMod = view->selectionModel();
  if( !selMod ) {
    return QString();
  }

  QModelIndex cs = selMod->currentIndex();
  if( !cs.isValid() ) {
    return QString();
  }
  return cs.data( Qt::DisplayRole ).toString();
}

QSize LaboView::svSize() const
{
  return scrollArea->size();
}



void LaboView::update()
{
  updateViews();
  QWidget::update();
}

void LaboView::print()
{
  sview->printAll();
}

void LaboView::closeEvent( QCloseEvent *e )
{
  if( doc == 0 || doc->canCloseFrame( this ) ) {
    e->accept();
  } else {
    e->ignore();
  }
}

void LaboView::resizeEvent( QResizeEvent *e )
{
  updateViews();
  QWidget::resizeEvent( e );
}

int LaboView::checkState( CheckType ctp )
{
  QString msg;
  int state;
  if( !model  || !root  || !schems  || !outs || !plots || !sims ) {
    showError( "Some part of model don't exist!" );
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
                    state = sch_main->getState();
                    if( state < stateDone ) {
                      msg = QString("Nothing to plot: state '%1', not 'Done' !").arg(
                          getStateString(state)  );
                    };
                    break;
    default: msg = "Unknown check?";
  };
  if( ! msg.isEmpty() ) {
    showWarn( msg );
    return 0;
  };
  return 1;
}


TRootData* LaboView::getRoot(void)
{
  return root;
}

TModel*  LaboView::getModel(void)
{
  return model;
}

void LaboView::updateViews()
{
  sview->update();
  outs_view->update();
  plots_view->update();
  sims_view->update();
  stam->update();
}


void LaboView::changeSel( int x, int y, int rel )
{
  TMiso *ob;

  selObj = nullptr;
  switch( rel ) {
    case 0: sel_x = x; sel_y = y; break;
    case 1: sel_x += x; sel_y += y; break;
    case 2:
            ob = sch_main->xy2Miso( sel_x, sel_y );
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
  sel = -1;
  ob = sch_main->xy2Miso( sel_x, sel_y );
  if( ob ) {
    selObj = ob;
    sel = ob->getMyIndexInParent();
  }
  QPoint seco = sview->getSelCoords();
  scrollArea->ensureVisible( seco.x(), seco.y() );
  emit viewChanged();
}


void LaboView::changeLevel( int lev )
{
  level = lev;
  if( level < 0 || level >= 64 )
    level = 0;
  emit viewChanged();
}

// ==== element related

void LaboView::newElm()
{
  if( !checkState( noselCheck ) )
    return;

  addElemInfo aei;
  aei.name = QString("obj_"); // + QSN( sch_main->getNMiso() ) ; // TODO: fix
  aei.order = sch_main->hintOrd();
  AddElemDialog *dia = new AddElemDialog( &aei, sch_main, this, "TMiso" );
                                          // limit to such elements here

  int rc = dia->exec();
  delete dia; dia = 0;

  if( rc != QDialog::Accepted || aei.type.isEmpty() ) {
    return;
  }
  if( ! isGoodName( aei.name )  ) {
    showError( QString("Fail to add Elem: bad object name \"%1\"").arg(aei.name) );
    return;
  }

  TMiso *ob = sch_main->insElem( aei.type, aei.name, aei.order, sel_x, sel_y );
  if( !ob  ) {
    showError( QString("Fail to add Elem: type \"%1\" \"%2\"").arg(aei.type).arg(aei.name) );
    return;
  }
  changeSel( 0, 0, 1 ); // update sel
  editElm();
}

void LaboView::delElm()
{
  if( ! checkState( selCheck ) )
    return;

  QString oname = selObj->objectName();

  bool sel_is_mark = ( selObj == markObj );

  if( confirmDelete( "element", oname) ) {
    sch_main->delElem( oname );
    if( sel_is_mark ) {
      markObj = nullptr;
    }

    changeSel( 0, 0, 1 ); // update sel
    emit viewChanged();
  };
}

void LaboView::editElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }
  editObj( selObj );
}


void LaboView::qlinkElm()
{
  QString toname;
  if( ! checkState( linkToCheck ) ) {
    return;
  }

  if( !selObj || !markObj ) {
    return;
  }
  toname = markObj->objectName();

  // TODO: its model action. really? model dont know about selected and marked.

  InputSimple *in = selObj->getInput( level );
  if( ! in )
    return;
  if( ! in->setData( "source", toname ) ) {
    DBGx( "warn: fail to set source in \"%s\" to \"%s\"",
          qP(in->objectName()), qP(toname) );
    return;
  }
  sch_main->reportStructChanged();
  sch_main->reset();
  sch_main->setModified();
  emit viewChanged();
}


void LaboView::qplinkElm()
{
  QString oldlink;
  if( ! checkState( linkToCheck ) ) {
    return;
  }

  if( !selObj || !markObj  ) {
    return;
  }

  QString toname = markObj->objectName();

  InputParams *pis = selObj->getElemT<InputParams*>("pis");
  if( !pis ) {
    DBG2q( "err: no pis object in ", selObj->getFullName() );
    return;
  }
  int n_pi = pis->size();
  QString pi_name = QString("pi_") + QSN(n_pi);
  InputParam *pi = pis->addObj<InputParam>( pi_name );
  if( !pi ) {
    return;
  }

  pi->setData( "source", toname );
  pi->setData( "line_w", 2 );
  pi->setData( "line_color", "red" );

  sch_main->reportStructChanged();
  sch_main->reset(); sch_main->setModified();
  emit viewChanged();
}

void LaboView::unlinkElm()
{
  if( ! checkState( selCheck ) ) { // no need marked to unlink
    return;
  }

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

  sch_main->reportStructChanged();
  sch_main->reset(); sch_main->setModified();
  emit viewChanged();
}

void LaboView::lockElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  int lck = 0;
  selObj->getData( "locked", &lck );
  lck = !lck;
  selObj->setData( "locked", lck );

  sch_main->reset();
  sch_main->setModified();
  emit viewChanged();
}

void LaboView::ordElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  bool ok;
  int new_ord = -1, old_ord;
  selObj->getData( "ord", &old_ord );
  new_ord = QInputDialog::getInt(this, "New element order",
      "Input new element order",
      old_ord, 0, IMAX, 1, &ok );
  if( ok ) {
    sch_main->newOrder( selObj->objectName(), new_ord ); // reset implied
    emit viewChanged();
  };
}

void LaboView::markElm()
{
  markObj = selObj;
  emit viewChanged();
}

void LaboView::moveElm()
{
  if( ! checkState( moveCheck )  ||  !markObj  ) {
    return;
  }

  sch_main->moveElem( markObj->objectName(), sel_x, sel_y );
  emit viewChanged();
}

void LaboView::infoElm()
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

  int em = LaboWin::labowin->getEm();

  dia->resize( 72*em, 50*em ); // TODO: adjust to inner table width
  dia->exec();
  delete dia;
  emit viewChanged();
}

void LaboView::showTreeElm()
{
  if( ! checkState( selCheck ) ) {
    return;
  }

  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Element tree: ") + selObj->objectName() );

  QVBoxLayout *lay = new QVBoxLayout();


  QTreeView *treeView = new QTreeView( dia );
  treeView->setModel( selObj );
  lay->addWidget( treeView );


  QPushButton *bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  int em = LaboWin::labowin->getEm();
  dia->resize( 80*em, 50*em ); // TODO: unmagic
  dia->exec();
  delete dia;
  emit viewChanged();
  return;
}


void LaboView::testElm1()
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

  int em = LaboWin::labowin->getEm();
  dia->resize( 60*em, 30*em ); // TODO: unmagic
  dia->exec();
  delete dia;
  emit viewChanged();
}

void LaboView::testElm2()
{
  if( ! checkState( selCheck ) )
    return;
  if( selObj == 0 )
    return;

  return;
}

void LaboView::cutElm()
{
  copyElm();
  delElm();
}

void LaboView::copyElm()
{
  if( !selObj )
    return;
  QString s = selObj->toString();
  QClipboard *clp = QApplication::clipboard();
  if( clp ) {
    clp->setText( s );
  }
}

void LaboView::pasteElm()
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
    showWarn( tr("Cannot parse clipboard string:\n%2\nLine %3 column %4.")
                .arg(errstr).arg(err_line).arg(err_column) );
    return;
  }
  QDomElement ee = x_dd.documentElement();

  QString tagname = ee.tagName();
  if( tagname != "obj" ) {
    showWarn( tr("element tag is not 'obj':  %2").arg( tagname ) );
    return;
  }

  QString eltype = ee.attribute( "otype" );
  QString elname = ee.attribute( "name" );
  QString elname_base = elname;
  int suff_n = 1;
  while( sch_main->getElem( elname ) && suff_n < 50 ) { // guess good name
    elname = elname_base + "_" + QSN( suff_n );
    suff_n++;
    if( suff_n > 20 ) {
      elname += "_x";
    }
  }

  int oord = sch_main->hintOrd();

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
    showError( QString("Fail to add Elem: bad object name \"%1\"").arg(elname) );
    return;
  }

  TMiso *ob = sch_main->insElem( eltype, elname, oord, sel_x, sel_y) ; // reset() implied
  if( !ob  ) {
    showError( QString("Fail to add Elem: %1 %2").arg(eltype).arg(elname) );
    return;
  }
  if( !ob->fromDom( ee, errstr ) ) {
    showWarn( tr("fail to set params:  %1").arg( errstr ) );
  }
  ob->setData( "vis_x", sel_x );
  ob->setData( "vis_y", sel_y );
  ob->setData( "ord", oord );
  changeSel( 0, 0, 1 ); // update sel

}

// ==== outs related

void LaboView::newOut()
{
  int rc;
  QString onameq, enameq;
  QDialog *dia;
  QLineEdit *oname_ed, *ename_ed; QLabel *lab1, *lab2;
  QGridLayout *lay;
  if( ! checkState( validCheck ) ) {
    return;
  }

  if( selObj  )
  {
    enameq = selObj->objectName();
    onameq = QString("out_") + QString(enameq);
  } else {
    enameq = "t";
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
      showError( QString("Fail to add Output: \"%1\"").arg(onameq) );
    }
    emit viewChanged();
    return;
  }
  showError( QString("Bad output name: \"%1\"").arg(onameq) );
}

void LaboView::delOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( "output array", nm ) ) {
    model->delOut( nm );
    emit viewChanged();
  }
}


void LaboView::editOut()
{
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  TOutArr *arr = model->getOutArr( nm );
  editObj( arr );
}

void LaboView::selectOut()
{
  if( ! checkState( validCheck ) )
    return;

  QItemSelectionModel *selMod = outs_view->selectionModel();
  if( !selMod ) {
    return;
  }

  QModelIndex s_idx = outs->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}


void LaboView::showOutData() // TODO: special dialog (+ for many rows)
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

  if( ! checkState( doneCheck ) ) {
    return;
  }
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }
  arr = model->getOutArr( nm );
  if( !arr ) {
    return;
  }

  int k = arr->fillGraphInfo( &gi );
  if( k != 0 ) {
    return;
  }

  // calculate statistical data TODO: separate struct and func (or/and in TOutArr)
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

void LaboView::exportOut()
{
  if( ! checkState( doneCheck ) ) {
    return;
  }
  QString nm = getSelName( outs_view );
  if( nm.isEmpty() ) {
    return;
  }

  TOutArr *arr = model->getOutArr( nm );
  if( !arr ) {
    return;
  }

  QString fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() ) {
    return;
  }
  arr->dump( qP(fnq), ' ' );
}


// ==== graph related

void LaboView::newGraph()
{
  QString grnameq, aname;
  bool ok;
  if( ! checkState( validCheck ) )
    return;
  // int no = model->getNGraph();
  grnameq = QString("plot_"); // + QSN( no ); TODO: what?
  aname = QInputDialog::getText( this, "Creating new plot",
      "Enter name of new plot:", QLineEdit::Normal,
      grnameq, &ok );
  if( ok ) {
    if( ! isGoodName( aname ) ) {
      showError( QString("Bad plot name: \"%1\"").arg(aname) );
    }
    model->insGraph( aname );
    emit viewChanged();
  };
}


void LaboView::delGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( "plot", nm ) ) {
    model->delGraph( nm );
    emit viewChanged();
  }
}


void LaboView::editGraph()
{
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *plot = model->getGraph( nm );

  editObj( plot );
}

void LaboView::selectGraph()
{
  if( ! checkState( validCheck ) )
    return;

  QItemSelectionModel *selMod = plots_view->selectionModel();
  if( !selMod ) {
    return;
  }

  QModelIndex s_idx = plots->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}



void LaboView::showGraph()
{
  if( ! checkState( doneCheck ) ) {
    return;
  }

  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );
  if( !gra ) {
    return;
  }

  QMainWindow *plotWnd = new QMainWindow( this );
  plotWnd->setWindowTitle( QString( PACKAGE ": Plot ") + gra->objectName() );
  PlotView *pv = new PlotView( doc, gra, plotWnd );
  plotWnd->setCentralWidget( pv );
  pv->setFocus();
  plotWnd->show();
}

void LaboView::showGraphData()
{
  if( ! checkState( doneCheck ) ) {
    return;
  }

  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }
  TGraph *gra = model->getGraph( nm );
  if( !gra ) {
    return;
  }

  GraphInfo gi;
  int k = gra->fillGraphInfo( &gi );
  if( k != 0 ) {
    return;
  }

  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString("Plot data: ") + gi.title );
  QVBoxLayout *lv = new QVBoxLayout( dia );

  DoubleTableModel *dmod = new DoubleTableModel( &gi, dia );
  QTableView *dtv = new QTableView( dia );
  dtv->setModel( dmod );
  lv->addWidget( dtv );

  QPushButton *bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  lv->addWidget( bt_ok );

  dia->exec();
  delete dia;

}

void LaboView::exportGraphData()
{
  if( ! checkState( doneCheck ) ) {
    return;
  }
  QString nm = getSelName( plots_view );
  if( nm.isEmpty() ) {
    return;
  }

  TGraph *gra = model->getGraph( nm );
  if( !gra  ) {
    return;
  }
  QString fnq = QFileDialog::getSaveFileName( this, tr("Export data"), "",
      "Data files (*.txt *.dat *.csv);;All files (*)" );
  if( fnq.isEmpty() ) {
    return;
  }

  gra->dump( fnq.toLocal8Bit(), ' ' );
}

void LaboView::gnuplotGraph()
{
  showError( "Unimplemented for now" );
  // TGraph *gra;
  // QDialog *dia;
  // QLabel *lb1, *lb2, *lb3; QLineEdit *ed_pgm, *ed_dat, *ed_eps;
  // QCheckBox *sw_x11;
  // QGridLayout *lay;
  // QString f_pgm, f_dat, f_eps, cdir;
  // int l, rc, use_x11;
  // if( ! checkState( doneCheck ) )
  //   return;
  // if( level < 0 || level >= model->getNGraph() )
  //   return;
  // gra = model->getGraph( level );
  // if( !gra )
  //   return;
  //
  // f_pgm = doc->pathName();
  // QFileInfo doc_fi( f_pgm );
  // cdir = QDir::currentPath();
  // if( cdir == doc_fi.absolutePath() )
  //   f_pgm = doc_fi.fileName();
  // if( f_pgm.length() < 1 ) { f_pgm = "gplot"; };
  // l = f_pgm.length();
  // if( doc_fi.suffix() == "qol"  )
  //   f_pgm.truncate( l-4 );
  // f_dat = f_pgm + ".dat"; f_eps = f_pgm + ".eps";
  // f_pgm += ".gp";
  //
  // dia = new QDialog( this );
  //
  // int em = LaboWin::labowin->getEm();
  // dia->resize( 50*em, 30*em );
  // lay = new QGridLayout( dia );
  // sw_x11 = new QCheckBox( "Output to &X11 window", dia );
  // sw_x11->setChecked( false );
  // lay->addWidget( sw_x11, 0, 0, 1, 2 );
  //
  // lb1 = new QLabel( "Output to EPS file:", dia );
  // lay->addWidget( lb1, 1, 0, 1, 2 );
  // ed_eps = new QLineEdit( dia );
  // ed_eps->setText( f_eps );
  // lay->addWidget( ed_eps, 2, 0, 1, 2 );
  //
  // lb2 = new QLabel( "Data file:", dia );
  // lay->addWidget( lb2, 3, 0, 1, 2 );
  // ed_dat = new QLineEdit( dia );
  // ed_dat->setText( f_dat );
  // lay->addWidget( ed_dat, 4, 0, 1, 2 );
  //
  // lb3 = new QLabel( "Gnuplot program file:", dia );
  // lay->addWidget( lb3, 5, 0, 1, 2 );
  // ed_pgm = new QLineEdit( dia );
  // ed_pgm->setText( f_pgm );
  // lay->addWidget( ed_pgm, 6, 0, 1, 2 );
  //
  // QDialogButtonBox *bbox
  //   = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  // lay->addWidget( bbox, 7, 0, 1, 2 );
  // connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  // connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );
  //
  // rc = dia->exec();
  // if( rc == QDialog::Accepted ) {
  //   f_pgm = ed_pgm->text(); f_eps = ed_eps->text();
  //   f_dat = ed_dat->text(); use_x11 = sw_x11->isChecked();
  //   gra->gnuPlot( !use_x11, f_pgm.toLocal8Bit(), 0,
  //                 f_eps.toLocal8Bit(), f_dat.toLocal8Bit() );
  // };
  // delete dia;
}

// ==== simulation related

void LaboView::newSimul()
{
  bool ok;
  if( ! checkState( validCheck ) )
    return;
  QString simName = QString("sim") + QSN( model->getNSimul() );
  simName = QInputDialog::getText( this, "Creating new Simulation",
      "Enter name of new simulation:", QLineEdit::Normal,
      simName, &ok );
  if( ok ) {
    if( ! isGoodName( simName ) ) {
      showError( QString("Bad simulation name: \"%1\"").arg(simName) );
    }
    ok = model->newSimul( simName );
    if( !ok ) {
      showError( QString("Fail to create simulation: \"%1\"").arg(simName) );
    }
    emit viewChanged();
  };
}

void LaboView::delSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  if( confirmDelete( "simulation", nm ) ) {
    model->delSimul( nm );
    emit viewChanged();
  }
}

void LaboView::editSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }
  Simulation *sim = model->getSimul( nm );

  editObj( sim );
}

void LaboView::selectSimul() // TODO: propagate to all
{
  if( ! checkState( validCheck ) ) {
    return;
  }

  QItemSelectionModel *selMod = sims_view->selectionModel();
  if( !selMod ) {
    return;
  }

  QModelIndex s_idx = sims->index( level, 0, QModelIndex() );

  selMod->clear();
  selMod->select( s_idx, QItemSelectionModel::Select );
  selMod->setCurrentIndex( s_idx, QItemSelectionModel::Select );
  emit viewChanged();
}

void LaboView::setActiveSimul()
{
  QString nm = getSelName( sims_view );
  if( nm.isEmpty() ) {
    return;
  }

  sims->setActiveElem( nm );
  emit viewChanged();
}


// ==== model related

void LaboView::editModel()
{
  if( ! checkState( validCheck ) )
    return;

  editObj( model );
}


void LaboView::showTreeModel()
{
  QDialog *dia = new QDialog( this );
  dia->setWindowTitle( QString( PACKAGE ": Model ") );

  QVBoxLayout *lay = new QVBoxLayout();

  QTreeView *treeView = new QTreeView( dia );
  treeView->setModel( root );

  lay->addWidget( treeView );

  QPushButton *bt_ok = new QPushButton( tr("Done"), dia);
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  int em = LaboWin::labowin->getEm();
  dia->resize( 80*em, 50*em ); // TODO: unmagic
  dia->exec();
  delete dia;
  // delete ho_mo;
  emit viewChanged();
  return;
}

// TODO: 2-pane dialog + script pool
void LaboView::runScript()
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
  lv->addWidget( bt_ok );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  int em = LaboWin::labowin->getEm();
  dia->resize( 80*em, 60*em );

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

  dia1->resize( 80*em, 60*em );
  dia1->exec();
  delete dia1; dia1 = nullptr;

}



// ==== run related

void LaboView::runRun()
{
  RunView *rv;
  if( ! checkState( validCheck ) )
    return;
  // Scheme *sch = model->getElemT<Scheme*>( "schems.main" ); // TODO: from simul

  ContSimul *sims = model->getElemT<ContSimul*>( "sims" );
  if( !sims ) {
    return;
  }

  Simulation *sim = sims->getActiveElemT<Simulation*>();
  if( !sim ) {
    showError( "No active simulations" );
    return;
  }

  rv = new RunView( sch_main, sim, this );
  rv->exec();
  emit viewChanged();
  sview->setFocus();
}

void LaboView::runPrm()
{
  // TODO: remove: use simulation
}

void LaboView::runPrm2()
{
  // TODO: remove: use simulation
}

void LaboView::resetModel()
{
  if( ! checkState( validCheck ) )
    return;
  model->reset();
  emit viewChanged();
}

// misc

const char LaboView::helpstr[] = "<b>Hot keys:</b><br>\n"
"<b>Ctrl-w</b> - close <br>\n"
"<b>h/F1</b> - this help <br>\n"
"<b>{Left, Top, Right, Bottom, Home}</b> - move selected cell <br>\n"
"<b>0-9</b> - select out array / graph <br>\n"
;

void LaboView::showHelp(void)
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


// end of laboview.cpp


