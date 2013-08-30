/***************************************************************************
                          addelemdia.cpp  - widgets for DataDialog
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <numeric>
#include "dataset.h"
#include "addelemdia.h"

using namespace std;


AddElemDialog::AddElemDialog( addElemInfo *a_aei, 
             TDataSet *a_pobj,  QWidget *aparent,
             const QString& allowed_types )
  : QDialog( aparent ), aei( a_aei ), allowed( a_pobj->allowTypes() ), pobj( a_pobj)
{
  if( ! allowed_types.isEmpty() ) {
    allowed = allowed_types;
  }
  setupUi();
}

void AddElemDialog::setupUi()
{
  if( !aei )
    return;
  
  QGridLayout *lay = new QGridLayout( this );

  QLabel *la_name = new QLabel( "Name", this );
  lay->addWidget( la_name, 0, 0 );

  ed_name = new QLineEdit( this );
  ed_name->setValidator( new QRegExpValidator( QRegExp(RE_NAME), this ) );
  ed_name->setText( aei->name );
  ed_name->setFocus();
  lay->addWidget( ed_name, 1, 0  );

  QLabel *la_order = new QLabel( "Order", this );
  lay->addWidget( la_order, 0, 1 );

  ed_order = new QLineEdit( this );
  ed_order->setText( QString::number( aei->order ) );
  lay->addWidget( ed_order, 1, 1 );
  ed_order->setValidator( new QIntValidator( 0, IMAX, this ) );
  
  QLabel *la_type = new QLabel( "Type", this );
  lay->addWidget( la_type, 2, 0 );

  lw = new QListWidget( this );

  QStringList cl_names = EFACT.goodTypeNames( allowed );
  QSize def_sz( 72, 50 ); // TODO: calculate;
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
    if( cname == "TLinear" ) {
      lw->setCurrentItem( lwi );
    }
  };
  lw->setViewMode( QListView::IconMode );
  lw->setUniformItemSizes( true );
  lw->setResizeMode ( QListView::Adjust );
  lay->addWidget( lw, 3, 0, 1, 2 );
  
  QDialogButtonBox *bbox 
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox, 5, 0, 1, 2 );
  connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
  resize( 720, 400 );
}

void AddElemDialog::accept()
{
  if( lw->currentItem() ) 
    aei->type = lw->currentItem()->text();
  aei->name = ed_name->text(); 
  aei->order = ed_order->text().toInt(); 
  QDialog::accept();
}

