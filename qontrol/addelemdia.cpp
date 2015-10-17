/***************************************************************************
                          addelemdia.cpp  - widgets for DataDialog
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <memory>
#include "dataset.h"
#include "addelemdia.h"

using namespace std;



// ---------------------------- SelectTypeDialog --------------------------------------

SelectTypeDialog::SelectTypeDialog( HolderData *a_pobj,  QWidget *aparent,
             const QString& allowed_types )
  : QDialog( aparent ), allowed( a_pobj->allowTypes() ), pobj( a_pobj )
{
  if( ! allowed_types.isEmpty() ) {
    allowed = allowed_types;
  }
  QString btp = pobj->getParm( "bad_types" );
  cl_names = EFACT.goodTypeNames( allowed, false, true, btp ); // obj, no_data

  sel_type = QString();
  setupUi();
}

void SelectTypeDialog::setupUi()
{
  setWindowTitle( "Select object type" );

  auto lay = new QVBoxLayout( this );

  lw = new QListWidget( this );

  QSize def_sz( 80, 50 ); // TODO: calculate;

  bool first_add = true;
  for( QString cname : cl_names ) {
    QString iconName = QString( ":icons/elm_" )
      + cname.toLower()
      + ".png";
    QListWidgetItem *lwi;
    QFileInfo fi( iconName );
    if( fi.isFile() ) {
      lwi = new QListWidgetItem( QIcon(iconName), cname );
    } else {
      lwi = new QListWidgetItem( QIcon(":icons/elm_unknown.png"), cname );
    }
    lwi->setSizeHint( def_sz );
    lwi->setToolTip( cname );
    lw->addItem( lwi );
    if( first_add  ||  cname == "TLinear" ) {
      lw->setCurrentItem( lwi );
    }
    first_add = false;
  };
  lw->setViewMode( QListView::IconMode );
  lw->setUniformItemSizes( true );
  lw->setResizeMode ( QListView::Adjust );
  lay->addWidget( lw );

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
  lay->addWidget( bbox );
  connect( bbox, &QDialogButtonBox::accepted, this, &SelectTypeDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, this, &SelectTypeDialog::reject );
  resize( 720, 400 );
}

void SelectTypeDialog::accept()
{
  if( lw->currentItem() ) {
    sel_type = lw->currentItem()->text();
  }
  QDialog::accept();
}

QString SelectTypeDialog::getType( HolderData *pobj, QWidget *aparent,
           const QString& allowed_types )
{
  QString tp;
  unique_ptr<SelectTypeDialog> dia( new SelectTypeDialog( pobj, aparent, allowed_types ) );
  int nt = dia->countTypes();

  if( nt < 1 ) {
    return tp;
  }
  if( nt == 1 ) {
    tp = dia->getTypeN( 0 );
    return tp;
  }

  int rc = dia->exec();
  if( rc == QDialog::Accepted ) {
    tp = dia->getSelType();
  };
  return tp;
}

QString SelectTypeDialog::getTypeAndName( HolderData *pobj, QWidget *aparent, QString &nm,
    const QString& allowed_types )
{
  QString tp = getType( pobj, aparent, allowed_types );
  if( tp.isEmpty() ) {
    return tp;
  }

  QString nm1 = pobj->hintName( tp );
  bool ok;
  QString objName = QInputDialog::getText( aparent, "Creating new Object",
      "Enter name of new element:", QLineEdit::Normal,
      nm1, &ok );
  if( !ok ) { return QString(); }

  nm = objName;
  return tp;
}

