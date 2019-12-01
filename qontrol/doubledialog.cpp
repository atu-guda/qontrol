/***************************************************************************
     doubledialog.cpp - for data table represenration
                             -------------------
    begin                : 2019.12.01
    copyright            : GPL (C) 2019-2019 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "doubledialog.h"
#include "doubletable.h"
#include "miscfun.h"

using namespace std;

DoubleDialog::DoubleDialog( const DatasInfo &_di,  const QString &_stat_str, QWidget *_parent )
     : QDialog( _parent ), di( _di ), stat_str( _stat_str )
{
  setupUi();
}



void DoubleDialog::setupUi()
{
  setWindowTitle( QSL("Data array: ") % di.title );

  auto lay = new QGridLayout( this );

  auto dmod = new DoubleTableModel( di, this );
  auto dtv = new QTableView( this );
  dtv->setModel( dmod );
  lay->addWidget( dtv, 0, 0 );

  int n_col = dmod->columnCount();
  int w_width = 110;

  for( int i=0; i<n_col; ++i ) {
    w_width += dtv->columnWidth( i );
  }


  if( ! stat_str.isEmpty() ) {
    auto l_stat = new QLabel( stat_str, this );
    l_stat->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard );
    lay->addWidget( l_stat, 0, 1 );
    w_width += 170;
  }

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Close );
  btn_find =  bbox->addButton( QSL("Find"), QDialogButtonBox::ActionRole );
  btn_copy =  bbox->addButton( QSL("Copy"), QDialogButtonBox::ActionRole );
  lay->addWidget( bbox, 1, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::clicked,  this, &DoubleDialog::btn_clicked );
  connect( bbox, &QDialogButtonBox::rejected, this, &QDialog::reject );

  QFontMetrics fm( this->font() );
  int ex = fm.capHeight();
  resize( w_width, ex*60 );

}

void DoubleDialog::btn_clicked( QAbstractButton *button )
{
  if( button == btn_find ) {
    return find();
  } else if ( button == btn_copy ) {
    return copy();
  }
  return;
}

void DoubleDialog::find()
{
  QMessageBox::information( this, QSL("Find"), QSL("Find called") );
}

void DoubleDialog::copy()
{
  QMessageBox::information( this, QSL("Copy"), QSL("Copy called") );
}

