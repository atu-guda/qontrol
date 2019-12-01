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

  model = new DoubleTableModel( di, this );
  view = new QTableView( this );
  view->setModel( model );
  lay->addWidget( view, 0, 0 );

  int n_col = model->columnCount();
  int w_width = 110;

  for( int i=0; i<n_col; ++i ) {
    w_width += view->columnWidth( i );
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
    return findData();
  } else if ( button == btn_copy ) {
    return copySel();
  }
  return;
}

void DoubleDialog::findData()
{
  QMessageBox::information( this, QSL("Find"), QSL("Find called") );
}

void DoubleDialog::copySel()
{
  QString s;
  int old_row = -1;
  auto indexes = view->selectionModel()->selection().indexes();

  for( const auto& index : indexes ) {
    if( ! index.isValid() ) {
      continue;
    }
    // s += QSL("( ") % QSN( index.row() ) % QSL(" , ") % QSN( index.column() ) % QSL(" )\n");
    if( old_row != -1 && old_row != index.row() ) {
      s += '\n';
    }
    s += index.data().toString() + '\t' ;
    old_row = index.row();
  }
  setClipboardStr( s );
  // QMessageBox::information( this, QSL("Copy"), s );
}

