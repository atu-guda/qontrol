/***************************************************************************
     pastedialog.h - struct and dialog for paste and clone objects
                             -------------------
    begin                : 2016.1220
    copyright            : GPL (C) 2016-2016 by atu
    email                : atu@nmetau.edu.ua
*/

#include <memory>
#include "defs.h"
#include "pastedialog.h"

using namespace std;

PasteDialog::PasteDialog( PasteObjParams &_pp, QWidget *_parent )
     : QDialog( _parent ), pp( _pp )
{
  setupUi();
}



void PasteDialog::setupUi()
{
  setWindowTitle( QSL("Paste object[s]:") );

  auto lay = new QGridLayout( this );
  int nl = 0;

  auto lbl = new QLabel( QSL("New name"), this );
  lay->addWidget( lbl, nl, 0 );
  ed_new_name = new QLineEdit( this );
  ed_new_name->setText( pp.new_name );
  lay->addWidget( ed_new_name, nl, 1 );
  ++nl;

  lbl = new QLabel( QSL("Old name"), this );
  lay->addWidget( lbl, nl, 0 );
  lbl = new QLabel( pp.old_name, this );
  lay->addWidget( lbl, nl, 1 );
  ++nl;

  cb_iter = new QCheckBox( QSL("Iterate"), this );
  cb_iter->setChecked( pp.iterateSrc );
  lay->addWidget( cb_iter, nl, 0 );
  ++nl;

  lbl = new QLabel( QSL("N"), this );
  lay->addWidget( lbl, nl, 0 );
  ed_n = new QLineEdit( this );
  ed_n->setText( QSN( pp.n ) );
  ed_n->setValidator( new QIntValidator );
  lay->addWidget( ed_n, nl, 1 );
  ++nl;

  lbl = new QLabel( QSL("dN"), this );
  lay->addWidget( lbl, nl, 0 );
  ed_dn = new QLineEdit( this );
  ed_dn->setText( QSN( pp.dn ) );
  ed_dn->setValidator( new QIntValidator );
  lay->addWidget( ed_dn, nl, 1 );
  ++nl;

  lbl = new QLabel( QSL("dX"), this );
  lay->addWidget( lbl, nl, 0 );
  ed_dx = new QLineEdit( this );
  ed_dx->setText( QSN( pp.dx ) );
  ed_dx->setValidator( new QIntValidator );
  lay->addWidget( ed_dx, nl, 1 );
  ++nl;

  lbl = new QLabel( QSL("dY"), this );
  lay->addWidget( lbl, nl, 0 );
  ed_dy = new QLineEdit( this );
  ed_dy->setText( QSN( pp.dy ) );
  ed_dy->setValidator( new QIntValidator );
  lay->addWidget( ed_dy, nl, 1 );
  ++nl;

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  lay->addWidget( bbox, nl, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, this, &PasteDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, this, &PasteDialog::reject );
}

void PasteDialog::accept()
{
  pp.new_name = ed_new_name->text();
  pp.iterateSrc = cb_iter->isChecked();
  pp.n = ed_n->text().toInt();
  pp.dn = ed_dn->text().toInt();
  pp.dx = ed_dx->text().toInt();
  pp.dy = ed_dy->text().toInt();
  QDialog::accept();
}

bool PasteDialog::getPasteData( PasteObjParams &_pp, QWidget *aparent )
{
  unique_ptr<PasteDialog> dia( new PasteDialog( _pp, aparent ) );
  bool ok = false;
  if( dia->exec() == QDialog::Accepted ) {
    ok = true;
  };
  return ok;
}

