/***************************************************************************
     paramsdialog.cpp - dialog to ask params before run
                             -------------------
    begin                : 2016.07.04
    copyright            : GPL (C) 2012-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "miscfun.h"
#include "paramsdialog.h"

using namespace std;

ParamsDialog::ParamsDialog( QList<AskedParam> &_plist, QWidget *_parent )
     : QDialog( _parent ), plist( _plist )
{
  setupUi();
}



void ParamsDialog::setupUi()
{
  setWindowTitle( QSL("Parameters:") );

  auto lay = new QGridLayout( this );

  int nr = 0;
  for( auto &p : plist ) {
    QLabel *lbl = new QLabel( p.name, this );
    lay->addWidget( lbl, nr, 0 );
    QLineEdit *ed = new QLineEdit( this );
    // ed->setValidator( new QDoubleValidator );
    ed->setText( QSND( p.saved ) );
    lay->addWidget( ed, nr, 1 );
    eds[ p.name ] = ed;
    ++nr;
  }

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  lay->addWidget( bbox, nr, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, this, &ParamsDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, this, &ParamsDialog::reject );

}

void ParamsDialog::accept()
{
  for( auto &p : plist ) {
    QLineEdit *ed = eds[ p.name ];
    if( !ed ) { continue; }
    p.cur = toDoubleEx( ed->text() );
  }
  eds.clear();
  QDialog::accept();
}

