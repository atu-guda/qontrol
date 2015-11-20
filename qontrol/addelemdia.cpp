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
  QString btp = pobj->getParm( QSL("bad_types") );
  cl_names = EFACT.goodTypeNames( allowed, btp ); // obj, data

  sel_type = QString();
  setupUi();
}

void SelectTypeDialog::setupUi()
{
  setWindowTitle( "Select object type" );

  auto lay = new QVBoxLayout( this );

  lw = new QListWidget( this );

  QSize def_sz( 80, 60 ); // TODO: calculate;

  bool first_add = true;
  for( QString cname : cl_names ) {
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

  auto dia = new QDialog( aparent );
  auto lay = new QVBoxLayout( dia );

  auto lbl_tp = new QLabel( QSL( "Creating object with type <b>") % tp % QSL("</b>"), dia );
  lay->addWidget( lbl_tp );
  auto fr = new QFrame( aparent );
  fr->setFrameStyle( QFrame::HLine );
  lay->addWidget( fr );

  auto lbl_name = new QLabel( "Name", dia );
  lay->addWidget( lbl_name );
  auto ed_name = new QLineEdit( aparent );
  ed_name->setValidator( new QRegExpValidator( QRegExp(RE_NAME), aparent ) );
  ed_name->setText( prm.name );
  lay->addWidget( ed_name );

  auto lbl_val = new QLabel( "Value(s)", dia );
  lay->addWidget( lbl_val );
  auto ed_val = new QTextEdit( aparent );
  ed_val->setText( prm.values );
  lay->addWidget( ed_val );

  auto lbl_descr = new QLabel( "Description", dia );
  lay->addWidget( lbl_descr );
  auto ed_descr = new QLineEdit( aparent );
  lay->addWidget( ed_descr );

  auto lbl_vis_name = new QLabel( "Visual name", dia );
  lay->addWidget( lbl_vis_name );
  auto ed_vis_name = new QLineEdit( aparent );
  lay->addWidget( ed_vis_name );

  auto lbl_sep = new QLabel( "End current", dia );
  lay->addWidget( lbl_sep );
  auto lws = new QComboBox( dia );
  lws->addItem( QSL("None"), QSL("") );
  lws->addItem( QSL("Column"), QSL("\nsep=col") );
  lws->addItem( QSL("Column after"), QSL("\nsep=col") );
  lws->addItem( QSL("Block"), QSL("\nsep=block") );
  lws->addItem( QSL("Block afer"), QSL("\nsep=blockend") );
  lws->addItem( QSL("Tab"), QSL("\nsep=tab") );
  lws->addItem( QSL("Tab afer"), QSL("\nsep=tabend") );
  lay->addWidget( lws );

  auto lbl_extra = new QLabel( "Extra", dia );
  lay->addWidget( lbl_extra );
  auto ed_extra = new QTextEdit( aparent );
  ed_extra->setText( prm.extra );
  lay->addWidget( ed_extra );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dia );
  lay->addWidget( bbox );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  prm.name = ed_name->text();
  prm.values = ed_val->toPlainText();
  prm.descr = ed_descr->text();
  prm.vis_name = ed_vis_name->text();
  if( prm.vis_name.isEmpty() ) {
    prm.vis_name = QSL("<div>") % prm.name % QSL("</div>" );
  }
  QString sep = lws->currentData().toString();
  prm.extra = ed_extra->toPlainText() % sep;

  delete dia;
  if( rc != QDialog::Accepted || prm.name.isEmpty() ) {
    return false;
  }
  return true;
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

  HolderString *o_dsc = ho->getObjT<HolderString*>( "descr" );
  if( o_dsc ) {
    o_dsc->set( prm.descr );
  } else {
    ho->setParm( "descr", prm.descr );
  }
  ho->setParm( "vis_name", prm.vis_name );
  ho->setParm( "extra", prm.extra );
  ho->extraToParm();
  ho->setDatas( prm.values );
  return ho;
}
