/***************************************************************************
                          addelemdia.cpp  - widgets for DataDialog
                             -------------------
    begin                : Jul 30 2012
    copyright            : GPL (C) 2012-2019 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <memory>
#include "dataset.h"
#include "addelemdia.h"
#include "miscfun.h"

using namespace std;



// ---------------------------- SelectTypeDialog --------------------------------------

SelectTypeDialog::SelectTypeDialog( HolderData *a_pobj,  QWidget *aparent,
             const QString& allowed_types )
  : QDialog( aparent ), allowed( a_pobj->allowTypes() ), pobj( a_pobj )
{
  if( ! allowed_types.isEmpty() ) {
    allowed = allowed_types;
  }
  QString btp = pobj->getParm( QSL("bad_types") );
  cl_names = EFACT.goodTypeNames( allowed, btp ); // obj, data

  sel_type = QString();
  setupUi();
}

void SelectTypeDialog::setupUi()
{
  setWindowTitle( QSL("Select object type") );

  auto lay = new QVBoxLayout( this );

  lw = new QListWidget( this );

  QSize def_sz( 80, 60 ); // TODO: calculate;

  bool first_add = true;
  for( const QString &cname : qAsConst(cl_names) ) {
    QString fcname = cname.toLower();
    if( fcname.endsWith( QSL("[]" ) ) ) {
      fcname.chop( 2 );
      fcname += QSL("_arr");
    }
    QString iconName = QSL( ":icons/elm_" ) % fcname % QSL(".png" );
    QListWidgetItem *lwi;
    QFileInfo fi( iconName );
    if( ! fi.isFile() ) {
      iconName = QSL(":icons/elm_unknown.png");
    }
    lwi = new QListWidgetItem( QIcon(iconName), cname );
    lwi->setSizeHint( def_sz );
    lwi->setToolTip( cname );
    lw->addItem( lwi );
    if( first_add  ||  cname == QSL("TLinear")  || cname == QSL("double") ) {
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

  resize( def_sz.width()*9, def_sz.height()*8 );
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

  if( dia->exec() == QDialog::Accepted ) {
    tp = dia->getSelType();
  };
  return tp;
}


bool SelectTypeDialog::getTypeAndParams( HolderData *pobj, QWidget *aparent, AddObjParams &prm,
    const QString& allowed_types )
{
  if( !pobj ) { return false; }
  QString tp = getType( pobj, aparent, allowed_types );
  if( tp.isEmpty() ) {
    return false;
  }

  prm.tp = tp;
  prm.name = pobj->hintName( tp, prm.name );

  return( NameAndParamsDialog::getNameAndParams( aparent, prm ) && ! prm.name.isEmpty() );
}

HolderData* SelectTypeDialog::askAndCreateObj( HolderData *pobj, QWidget *aparent, AddObjParams &prm,
    const QString& allowed_types )
{
  if( !pobj ) { return nullptr; }
  if( ! SelectTypeDialog::getTypeAndParams( pobj, aparent, prm, allowed_types ) ) {
    return nullptr;
  }

  HolderData *ho = pobj->addObjP( prm.tp, prm.name );
  if( !ho ) {
    qWarning() <<  QSL("Fail to add object: type ") << prm.tp << QSL(" name ") << prm.name
       << QSL(" in " ) << pobj->getFullName() << WHE;
    return nullptr;
  }

  HolderString *o_dsc = ho->getObjT<HolderString*>( QSL("descr") );
  if( o_dsc ) {
    o_dsc->set( prm.descr );
  } else {
    ho->setParm( QSL("descr"), prm.descr );
  }
  ho->setParm( QSL("vis_name"), prm.vis_name );
  ho->setParm( QSL("extra"), prm.extra );
  ho->extraToParm();
  ho->reset_dfl();
  if( !prm.values.isEmpty() ) {
     ho->setDatas( prm.values );
  }
  return ho;
}

// -------------------------------------- NameAndParamsDialog ---------

NameAndParamsDialog::NameAndParamsDialog( QWidget *aparent, AddObjParams &aprm )
  : QDialog( aparent ), prm( aprm )
{
  setupUi();
}

void NameAndParamsDialog::setupUi()
{
  auto lay_h = new QHBoxLayout( this );

  auto lay_m = new QVBoxLayout; // main mart
  lay_h->addLayout( lay_m );

  auto lay_ex = new QVBoxLayout; // extra part
  lay_h->addLayout( lay_ex );

  auto lay_btn = new QVBoxLayout; // right button part
  lay_h->addLayout( lay_btn );

  setWindowTitle( QSL( "Creating object with type ") % prm.tp );

  auto lbl_name = new QLabel( QSL("Name"), this );
  lay_m->addWidget( lbl_name );
  ed_name = new QLineEdit( this );
  ed_name->setValidator( new QRegExpValidator( QRegExp(QSL(RE_NAME)), this ) );
  ed_name->setText( prm.name );
  lay_m->addWidget( ed_name );

  auto lbl_val = new QLabel( QSL("Value(s)"), this );
  lay_m->addWidget( lbl_val );
  ed_val = new QTextEdit( this );
  ed_val->setText( prm.values );
  lay_m->addWidget( ed_val );

  auto lbl_descr = new QLabel( QSL("Description"), this );
  lay_m->addWidget( lbl_descr );
  ed_descr = new QLineEdit( this );
  lay_m->addWidget( ed_descr );

  auto lbl_vis_name = new QLabel( QSL("Visual name"), this );
  lay_m->addWidget( lbl_vis_name );
  ed_vis_name = new QLineEdit( this );
  lay_m->addWidget( ed_vis_name );


  auto lbl_sep = new QLabel( QSL("End current"), this );
  lay_ex->addWidget( lbl_sep );
  lws = new QComboBox( this );
  lws->addItem( QSL("None"), QES );
  lws->addItem( QSL("Column"), QSL("\nsep=col") );
  lws->addItem( QSL("Column after"), QSL("\nsep=col") );
  lws->addItem( QSL("Row"), QSL("\nsep=row") );
  lws->addItem( QSL("Block"), QSL("\nsep=block") );
  lws->addItem( QSL("Block after"), QSL("\nsep=blockend") );
  lws->addItem( QSL("Tab"), QSL("\nsep=tab") );
  lws->addItem( QSL("Tab after"), QSL("\nsep=tabend") );
  lay_ex->addWidget( lws );

  auto lbl_extra = new QLabel( QSL("Extra"), this );
  lay_ex->addWidget( lbl_extra );
  ed_extra = new QTextEdit( this );
  ed_extra->setText( prm.extra );
  lay_ex->addWidget( ed_extra );

  auto bbox
    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this );
  lay_btn->addWidget( bbox );
  lay_btn->addStretch( 1 );

  connect( bbox, &QDialogButtonBox::accepted,    this, &NameAndParamsDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected,    this, &NameAndParamsDialog::reject );
  connect( ed_name, &QLineEdit::editingFinished, this, &NameAndParamsDialog::suggestVisName );
}

bool NameAndParamsDialog::getNameAndParams( QWidget *aparent, AddObjParams &aprm )
{
  auto dia = new NameAndParamsDialog( aparent, aprm );
  return( dia->exec() == QDialog::Accepted );
}

void NameAndParamsDialog::accept()
{
  prm.name = ed_name->text();
  prm.values = ed_val->toPlainText();
  prm.descr = ed_descr->text();
  prm.vis_name = autoVisName( ed_vis_name->text(), prm.name );
  QString sep = lws->currentData().toString();
  prm.extra = ed_extra->toPlainText() % sep;
  QDialog::accept();
}

void NameAndParamsDialog::suggestVisName()
{
  if( ! ed_vis_name->text().isEmpty()  ) {
    return;
  }
  QString t = ed_name->text();
  ed_vis_name->setText( autoVisName( QString(), t ) );
}

