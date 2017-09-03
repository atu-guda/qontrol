/***************************************************************************
                          datawidget.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include <QInputDialog>

#include "miscfun.h"
#include "datawidget.h"
#include "longvalidator.h"
#include "labowin.h"
#include "addelemdia.h"
#include "linkcompleter.h"

using namespace std;

// ------------------------------ DataLabel ---------------------------------------------------

DataLabel::DataLabel( HolderData &_ho, const QString &text, DataWidget *parent, Qt::WindowFlags f )
   : QLabel( text, parent, f ),
     ho( _ho ), dw( parent )
{
  if( ho.getModified() ) {
    setStyleSheet( QSL("QLabel { color : red; }") );
  }
}

void DataLabel::contextMenuEvent( QContextMenuEvent *ev )
{
  QMenu *mnu = new QMenu( this );
  auto act = mnu->addAction( "&Information" );
  connect( act, &QAction::triggered, dw, &DataWidget::infoObj );
  // act = mnu->addAction( "&What's this" );
  // connect( act, &QAction::triggered, dw, &DataWidget::showWhats );
  act = mnu->addAction( "&Copy object" );
  connect( act, &QAction::triggered, dw, &DataWidget::copyObj );
  if( ho.isDyn() ) {
    act = mnu->addAction( "&Delete object" );
    connect( act, &QAction::triggered, dw, &DataWidget::deleteObj );
    act = mnu->addAction( "&Propetries" );
    connect( act, &QAction::triggered, dw, &DataWidget::editPropsObj );
    act = mnu->addAction( "Rename" );
    connect( act, &QAction::triggered, dw, &DataWidget::renameObj );
  }
  if( !ho.isRoTree( efROAny ) ) {
    act = mnu->addAction( "Re&vert changes" );
    connect( act, &QAction::triggered, dw, &DataWidget::revertObj );
    act = mnu->addAction( "Default value" );
    connect( act, &QAction::triggered, dw, &DataWidget::defaultVal );
  }
  mnu->exec( ev->globalPos() );
  delete mnu;
}

// ------------------------------ DataWidget ---------------------------------------------------

static const int MAX_COLS_PER_WIDGET  = 20;
static const int MAX_WIDGETS_PER_COL  = 16;

DataWidget::DataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : QFrame( parent ), ho( h ), main_w( nullptr ),
    lbl( new DataLabel( h, QSL(""), this ) )
{
  DataDialog *ddia = nullptr; // find DataDialog among parents
  for( QObject *par = parent; par; par = par->parent() ) {
    ddia = qobject_cast<DataDialog*>( par );
    if( ddia ) {
      break;
    }
  }

  if( ddia ) {
    connect( this, &DataWidget::delMe     , ddia, &DataDialog::delObjByName   , Qt::QueuedConnection );
    connect( this, &DataWidget::bigChanges, ddia, &DataDialog::reactBigChanges, Qt::QueuedConnection );
  } else {
    qWarning() << "No DataDialog for " << ho.getFullName() << WHE;
  }
  if( ! hideLabel ) {
    // lbl->setMinimumWidth( 5 * LaboWin::Em() );
    lbl->setTextFormat( Qt::RichText );
    lbl->setText( tex2label( ho.getParm( QSL("vis_name") ) ) );
  } else {
    lbl->setFixedSize( 0, 0 );
  }
  saved_data = ho.toString();
  // setFrameStyle( QFrame::Panel | QFrame::Sunken );
}

QSize DataWidget::minimumSizeHint() const
{
  if( main_w ) {
    return  main_w->minimumSizeHint() + QSize( lbl->minimumSizeHint().width(), 0 );
  }
  qWarning() << "no main widget for " << ho.getType() <<  ho.objectName() << WHE;
  return QSize( 50, 20 ); // fallback value;
}


QSize DataWidget::sizeHint() const
{
  if( main_w ) {
    return  main_w->sizeHint() + QSize( lbl->sizeHint().width(), 0 );
  }
  qWarning() << "no main widget for " << ho.getType() << ho.objectName() << WHE;
  return QSize( 50, 20 ); // fallback value;
}

void DataWidget::infoObj()
{
  QString extr { QSL("; // ") };
  auto na = ho.arrSize();
  if( na != 1 ) {
    extr += QSL(" [") % QSN( na ) % QSL("] ");
  }
  if( ho.isDyn() ) {
    extr += QSL("dyn, ");
  }
  if( ho.getModified() ) {
    extr += QSL("mod");
  }

  QString s = QSL("<p><b> Object ") % ho.getFullName() % QSL("</b><hr/>\n<h3>")
            % ho.getType() % QSL(" ") % ho.objectName() % extr % QSL("</h3>\n")
            % ho.textVisual() % QSL("<hr/>\n")
            % ho.getParm( QSL("descr" ) ) % QSL("<hr/>")
            % flags2str( ho.getFlags() )
            % QSL("<hr/></p>\n<p>\n");

  const auto& parms = ho.getAllParms();
  for( auto p = parms.cbegin(); p != parms.cend(); ++p ) {
    s += p.key() % QSL(" = \"") % p.value().toHtmlEscaped() % QSL("\";<br/>\n");
  }
  s += QSL("<hr/></p>\n");

  QMessageBox::information( this, PACKAGE "Object info", s );
}

void DataWidget::showWhats()
{
}



void DataWidget::copyObj() const
{
  QString s = ho.toXML( true );
  setClipboardStr( s );
}

void DataWidget::deleteObj()
{
  if( ! isWriteAllowed( QSL("delete" ) ) ) {
    return;
  }
  if( confirmDelete( this, QSL("Object"), ho.objectName() ) ) {
    emit delMe( ho.objectName() );
  }
  // TODO: check!!!!
}

void DataWidget::renameObj()
{
  if( ! isWriteAllowed( QSL("rename" ) ) ) {
    return;
  }
  auto opar = ho.getParent();
  if( !opar ) { // dangling object?
    return;
  }
  QString old_name = ho.objectName();
  bool ok;
  QString new_name = QInputDialog::getText( this, "Rename:" + ho.getFullName(),
      "Enter new name:", QLineEdit::Normal, old_name, &ok );
  if( !ok || ! isGoodName( new_name ) || old_name == new_name ) {
    return;
  }
  opar->renameObj( old_name, new_name );
  ho.setParm( QSL("vis_name"), autoVisName( QString(), new_name ) );
  emit bigChanges();
}


void DataWidget::revertObj()
{
  if( isWriteAllowed( QSL("revert" ) ) ) {
    ho.suspendHandleStructChange();
    ho.delAllDyn(); // TODO: suppress errors during recreate.
    ho.fromString( saved_data );
    ho.setUnModified();
    ho.resumeHandleStructChange();
    obj2vis();
  }
}

void DataWidget::defaultVal()
{
  if( isWriteAllowed( QSL("make dafault" ) ) ) {
    ho.reset_dfl();
    obj2vis();
  }
}

void DataWidget::editPropsObj()
{
  if( !isWriteAllowed( QSL("edit properties" ) ) ) {
    return;
  }
  if( !ho.isDyn() ) {
    qWarning() << "attempt to edit non-dynamic object properties" << WHE;
    return;
  }

  const auto& parms = ho.getAllParms();
  QString s;
  for( auto p = parms.cbegin(); p != parms.cend(); ++p ) {
    const auto &ke = p.key();
    if( ke == QSL("props") || ke == QSL("extra") ) {
      continue;
    }
    s += p.key() % QSL("=") % p.value() % QSL("\n");
  }

  bool ok = false;
  s = QInputDialog::getMultiLineText( this, QSL("Edit properties"),
      ho.getFullName() % QSL(" properties"), s, &ok );

  if( ok ) {
    ho.setParm( QSL("extra"), s );
    ho.extraToParm();
    obj2vis();
    emit bigChanges();
  }

}

bool DataWidget::isWriteAllowed( const QString &actName )
{
  if( ho.isRoTree( efROAny ) ) {
    qWarning() << "Attempt to " << actName << " RO object " << ho.getFullName();
    return false;
  }
  return true;
}


// -------------  DummyDataWidget ----------------

DummyDataWidget::DummyDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
    lbl_d( new QLabel( this ) )
{
  main_w = lbl_d;
  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( lbl_d );
}

bool DummyDataWidget::obj2vis()
{
  lbl_d->setText( QSL("@ ") + ho.toString().left(20) );
  return true;
}

bool DummyDataWidget::vis2obj() const
{
  return true;
}

DW_REG_FUN_STD( DummyDataWidget, "" );


// ------------------- StringDataWidget ---------------------------

StringDataWidget::StringDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  le( new QLineEdit( this ) )
{
  main_w = le;
  if( h.isRoTree( efROAny ) ) {
    le->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( le );
  }

  int v_max = h.getParmInt( QSL("max"), 4096 ); // not IMAX - good limit for single-line string
  le->setMaxLength( v_max );

  QString mask = h.getParm( QSL("mask") );
  if( ! mask.isEmpty() ) {
    le->setInputMask( mask );
  }

  QString cmpl_targ = h.getParm( QSL("cmpl") );
  if( ! cmpl_targ.isEmpty() ) {
    auto cmpl = new LinkCompleter( this );
    QAbstractItemModel *cmpl_mdl = h.getComplModel( cmpl_targ, cmpl );
    cmpl->setModel( cmpl_mdl );
    le->setCompleter( cmpl );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le, 1 );
  setLayout( lay );
  setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred ); // TODO: check?
}

bool StringDataWidget::obj2vis()
{
  le->setText( ho.toString() );
  return true;
}

bool StringDataWidget::vis2obj() const
{
  ho.fromString( le->text() );
  return true;
}

DW_REG_FUN_STD( StringDataWidget, "STRING,SIMPLE" );


// ------------------- StringMLDataWidget ---------------------------

StringMLDataWidget::StringMLDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  te( new QTextEdit( this ) )
{
  main_w = te;
  if( h.isRoTree( efROAny ) ) {
    te->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( te );
  }

  // setMaxLength( h.getMax() ); ???
  // TODO: min size
  te->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
  te->setAcceptRichText( false );
  // te->setWordWrapMode( QTextEdit::NoWrap );

  auto lay =  new QVBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( te, 1 );
  setLayout( lay );
  setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
}

bool StringMLDataWidget::obj2vis()
{
  te->setPlainText( ho.toString() );
  return true;
}

bool StringMLDataWidget::vis2obj() const
{
  ho.fromString( te->toPlainText() );
  return true;
}

QSize StringMLDataWidget::minimumSizeHint() const
{
  return te->minimumSizeHint() + QSize( 0, lbl->minimumSizeHint().height() );
}


QSize StringMLDataWidget::sizeHint() const
{
  return te->sizeHint() + QSize( 0, lbl->sizeHint().height() );
}

DW_REG_FUN_STD( StringMLDataWidget, "STRING,MLINE" );

// ------------------- StringExtDataWidget ---------------------------

StringExtDataWidget::StringExtDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  pb( new QPushButton( this ) )
{
  main_w = pb;
  if( h.isRoTree( efROAny ) ) {
    pb->setDisabled( true );
  }
  pb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
  pb->setText( ho.objectName() % QSL(" [text]") );
  connect( pb, &QPushButton::clicked, this, &StringExtDataWidget::edit );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( pb, 1 );
  setLayout( lay );
}

bool StringExtDataWidget::obj2vis()
{
  ts = ho.toString();
  return true;
}

bool StringExtDataWidget::vis2obj() const
{
  ho.fromString( ts );
  return true;
}

void StringExtDataWidget::edit()
{
  // TODO: temporary dir
  QString ftempl = QSL( "qo_tmp_file_XXXXXXXX" )  %  ho.getParm( QSL("fileext"), QSL(".txt") );

  QTemporaryFile f;
  f.setFileTemplate( ftempl );
  // f.setAutoRemove( false ); // for debug
  if( !f.open() ) {
    return;
  }
  QString fn = f.fileName();
  SettingsData *sett = SETTINGS;
  QString cmd = sett->getDataD( QSL("editCmd"), QSL("gvim -f ") )  %  QSL(" ") % fn;

  {
    QTextStream os( &f );
    os << ts << endl;
    f.close();
  }

  QProcess proc;
  int rc = proc.execute( cmd );
  if( rc != 0 ) {
    qWarning() << "Command " << cmd << " returns status " << rc << WHE;
    return;
  }

  QFile fr( fn );
  if( !fr.open( QIODevice::ReadOnly ) ) {
    qWarning() << "Fail to reopen file " << fn << WHE;
    return;
  }
  QTextStream is( &fr );
  ts = is.readAll();
  f.close();
}

DW_REG_FUN_STD( StringExtDataWidget, "STRING,EXT,MLINE,LARGETEXT" );



// ------------------- IntDataWidget ---------------------------

IntDataWidget::IntDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  le( new QLineEdit( this ) )
{
  main_w = le;
  if( h.isRoTree( efROAny ) ) {
    le->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( le );
  }

  int v_min = h.getParmInt( QSL("min"), IMIN );
  int v_max = h.getParmInt( QSL("max"), IMAX );
  le->setValidator( new QIntValidator( v_min, v_max, le ) );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le, 1 );
  setLayout( lay );
}

bool IntDataWidget::obj2vis()
{
  le->setText( QSN( ho.get().toInt() ) );
  return true;
}

bool IntDataWidget::vis2obj() const
{
  bool ok;
  QVariant v = le->text().toInt( &ok, 0 );
  ho.set( v );
  return ok;
}

DW_REG_FUN_STD( IntDataWidget, "INT,SIMPLE" );


// ------------------- IntSpinDataWidget ---------------------------

IntSpinDataWidget::IntSpinDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  sb( new QSpinBox( this ) )
{
  main_w = sb;
  if( h.isRoTree( efROAny ) ) {
    sb->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( sb );
  }

  int v_min = h.getParmInt( QSL("min"), IMIN );
  int v_max = h.getParmInt( QSL("max"), IMAX );
  sb->setRange( v_min, v_max );

  QString prefix = h.getParm( QSL("prefix") );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }

  int step = h.getParmInt( QSL("step"), -1 );
  if( step > 0 ) {
    sb->setSingleStep( step );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( sb, 1 );
  setLayout( lay );

}

bool IntSpinDataWidget::obj2vis()
{
  sb->setValue(  ho.get().toInt() );
  return true;
}

bool IntSpinDataWidget::vis2obj() const
{
  QVariant v = sb->value();
  ho.set( v );
  return true;
}

DW_REG_FUN_STD( IntSpinDataWidget, "INT,SPIN" );


// ------------------- SwitchDataWidget ---------------------------

SwitchDataWidget::SwitchDataWidget( HolderData &h, QWidget *parent, bool /*hideLabel*/ )
  : DataWidget( h, parent, false ),
  cb( new QCheckBox( ho.getParm( QSL("vis_name")), this )  )
{
  lbl->setText( QSL(">") );
  main_w = cb;
  if( h.isRoTree( efROAny ) ) {
    cb->setDisabled( true );
  }

  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );

  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool SwitchDataWidget::obj2vis()
{
  cb->setChecked(  ho.get().toBool() );
  return true;
}

bool SwitchDataWidget::vis2obj() const
{
  QVariant v = (int)cb->isChecked();
  ho.set( v );
  return true;
}

DW_REG_FUN_STD( SwitchDataWidget, "INT,SWITCH" );


// ------------------- ListDataWidget ---------------------------

ListDataWidget::ListDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  cb( new QComboBox( this ) )
{
  main_w = cb;
  if( h.isRoTree( efROAny ) ) {
    cb->setDisabled( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( cb );
  }

  QStringList sl = ho.getListStrings();
  QString noTeX = ho.getParm( QSL("noTeX") );
  if( noTeX != QSL("y") ) {
    for( auto &s : sl ) { s = tex2label( s, true ); }
  }
  cb->addItems( sl );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool ListDataWidget::obj2vis()
{
  cb->setCurrentIndex( ho.get().toInt() );
  return true;
}

bool ListDataWidget::vis2obj() const
{
  QVariant v = cb->currentIndex();
  ho.set( v );
  return true;
}

DW_REG_FUN_STD( ListDataWidget, "INT,LIST" );


// ------------------- LongDataWidget ---------------------------

LongDataWidget::LongDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  le( new QLineEdit( this ) )
{
  main_w = le;
  if( h.isRoTree( efROAny ) ) {
    le->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( le );
  }

  long v_min = h.getParmLong( QSL("min"), LMIN );
  long v_max = h.getParmLong( QSL("max"), LMAX );
  le->setValidator( new LongValidator( v_min, v_max, le ) );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le, 1 );
  setLayout( lay );
}

bool LongDataWidget::obj2vis()
{
  le->setText( QSN( ho.get().toLongLong() ) );
  return true;
}

bool LongDataWidget::vis2obj() const
{
  bool ok;
  QVariant v = (qlonglong)( le->text().toLong( &ok, 0 ) );
  ho.set( v );
  return ok;
}

DW_REG_FUN_STD( LongDataWidget, "LONG,SIMPLE" );


// ------------------- DoubleDataWidget ---------------------------

DoubleDataWidget::DoubleDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  le( new QLineEdit ( this ) )
{
  main_w = le;
  if( h.isRoTree( efROAny ) ) {
    le->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( le );
  }

  // int decimals = h.getParmInt( QSL("decimals"), DOUBLE_PREC );
  // double v_min = h.getParmDouble( QSL("min"), DMIN );
  // double v_max = h.getParmDouble( QSL("max"), DMAX );
  //
  // le->setValidator( new QDoubleValidator( v_min, v_max, decimals, le ) ); // TODO: revive

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le );
  setLayout( lay );
}

bool DoubleDataWidget::obj2vis()
{
  le->setText( QSND( ho.getDouble() ) );
  return true;
}

bool DoubleDataWidget::vis2obj() const
{
  bool ok;
  QVariant v = toDoubleEx(le->text(), &ok );
  if( ok ) {
    ho.set( v );
  }
  return ok;
}

DW_REG_FUN_STD( DoubleDataWidget, "DOUBLE,SIMPLE" );


// ------------------- DoubleSpinDataWidget ---------------------------

DoubleSpinDataWidget::DoubleSpinDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
   sb( new QDoubleSpinBox( this ) )
{
  main_w = sb;
  if( h.isRoTree( efROAny ) ) {
    sb->setReadOnly( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( sb );
  }

  double v_min = h.getParmDouble( QSL("min"), DMIN );
  double v_max = h.getParmDouble( QSL("max"), DMAX );
  sb->setRange( v_min, v_max );

  QString prefix = h.getParm( QSL("prefix") );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }

  double step = h.getParmDouble( QSL("step"), -1 );
  if( step > 0 ) {
    sb->setSingleStep( step );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( sb, 1 );
  setLayout( lay );
}

bool DoubleSpinDataWidget::obj2vis()
{
  sb->setValue(  ho.get().toDouble() );
  return true;
}

bool DoubleSpinDataWidget::vis2obj() const
{
  QVariant v = sb->value();
  ho.set( v );
  return true;
}

DW_REG_FUN_STD( DoubleSpinDataWidget, "DOUBLE,SPIN" );


// ------------------- ColorDataWidget ---------------------------

ColorDataWidget::ColorDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
   cb( new ColorBtn( this ) )
{
  main_w = cb;
  if( h.isRoTree( efROAny ) ) {
    cb->setDisabled( true ); // TODO: real read-only
  }
  cb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Expanding );

  if( !hideLabel ) {
    lbl->setBuddy( cb );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool ColorDataWidget::obj2vis()
{
  QColor c;
  QVariant v = ho.get();
  switch( v.type() ) {
    case QVariant::Color:
      c = v.value<QColor>();
      break;
    case QVariant::String:
      c = QColor( v.toString() );
      break;
    default:
      int c_i = v.toInt();
      QRgb c_r( c_i );
      c = QColor( c_r );
      break;
  };

  cb->setColor( c ); // Int to Color inside button
  return true;
}

bool ColorDataWidget::vis2obj() const
{
  QColor c = cb->color();
  // int c_i = c.rgba();
  QVariant v = c;
  ho.set( c );
  return true;
}

DW_REG_FUN_STD( ColorDataWidget, "COLOR" );


// ------------------- FontDataWidget ---------------------------

FontDataWidget::FontDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
   cb( new FontBtn( this ) )
{
  main_w = cb;
  if( h.isRoTree( efROAny ) ) {
    cb->setDisabled( true );
  }
  cb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Expanding );

  if( !hideLabel ) {
    lbl->setBuddy( cb );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool FontDataWidget::obj2vis()
{
  cb->set_Font( ho.get().value<QFont>() );
  return true;
}

bool FontDataWidget::vis2obj() const
{
  QFont c = cb->font();
  ho.set( c );
  return true;
}

DW_REG_FUN_STD( FontDataWidget, "FONT" );


// ------------------- DateDataWidget ---------------------------

DateDataWidget::DateDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  de( new QDateEdit( this ) )
{
  main_w = de;
  if( h.isRoTree( efROAny ) ) {
    de->setDisabled( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( de );
  }

  QString s_min = h.getParm( QSL("min") );
  if( ! s_min.isEmpty() ) {
    de->setMinimumDate( QDate::fromString( s_min, DATE_FORMAT ) );
  }
  QString s_max = h.getParm( QSL("max") );
  if( ! s_max.isEmpty() ) {
    de->setMaximumDate( QDate::fromString( s_max, DATE_FORMAT ) );
  }

  de->setCalendarPopup( true );
  de->setDisplayFormat( DATE_FORMAT );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( de, 1 );
  setLayout( lay );

}

bool DateDataWidget::obj2vis()
{
  de->setDate( ho.get().toDate() );
  return true;
}

bool DateDataWidget::vis2obj() const
{
  ho.set( de->date() );
  return true;
}

DW_REG_FUN_STD( DateDataWidget, "DATE" );



// ------------------- TimeDataWidget ---------------------------

TimeDataWidget::TimeDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
  te( new QTimeEdit( this ) )
{
  main_w = te;
  if( h.isRoTree( efROAny ) ) {
    te->setDisabled( true );
  }

  if( !hideLabel ) {
    lbl->setBuddy( te );
  }

  QString s_min = h.getParm( QSL("min") );
  if( ! s_min.isEmpty() ) {
    te->setMinimumTime( QTime::fromString( s_min, TIME_FORMAT ) );
  }
  QString s_max = h.getParm( QSL("max") );
  if( ! s_max.isEmpty() ) {
    te->setMaximumTime( QTime::fromString( s_max, TIME_FORMAT ) );
  }

  te->setDisplayFormat( TIME_FORMAT );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( te, 1 );
  setLayout( lay );

}

bool TimeDataWidget::obj2vis()
{
  te->setTime( ho.get().toTime() );
  return true;
}

bool TimeDataWidget::vis2obj() const
{
  ho.set( te->time() );
  return true;
}

DW_REG_FUN_STD( TimeDataWidget, "TIME" );



// ------------------- IntArrayDataWidget ---------------------------

IntArrayDataWidget::IntArrayDataWidget( HolderData &h, QWidget *parent, bool /*hideLabel*/ )
  : DataWidget( h, parent, false ),
  pwi( new QFrame( this ) )
{
  main_w = pwi;
  pwi->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
  bool ro = h.isRoTree( efROAny );
  int n = h.arrSize();
  les.reserve(n);

  int v_min = h.getParmInt( QSL("min"), IMIN );
  int v_max = h.getParmInt( QSL("max"), IMAX );

  auto lay = new QGridLayout( pwi );

  lay->addWidget( lbl, 0, 1 );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( QSL("[") % QSN(i) + QSL("]") );
    lay->addWidget( la, i+1, 0 );

    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    le->setValidator( new QIntValidator( v_min, v_max, le ) );
    lay->addWidget( le, i+1, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 1, 1, 1, 1 );
  pwi->setLayout( lay );
}

bool IntArrayDataWidget::obj2vis()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( QSN( ho.get(i).toInt() ) );
  }
  return true;
}

bool IntArrayDataWidget::vis2obj() const
{
  bool ok = false;
  int n = les.size();
  QVariant v;
  for( int i=0; i<n; ++i ) {
    v = les[i]->text().toInt( &ok, 0 );
    ho.set( v, i );
  }
  return ok;
}

DW_REG_FUN_STD( IntArrayDataWidget, "ARRAY_INT,INLINE" );


// ------------------- DoubleArrayDataWidget ---------------------------

DoubleArrayDataWidget::DoubleArrayDataWidget( HolderData &h, QWidget *parent, bool /*hideLabel*/ )
  : DataWidget( h, parent, false ),
  pwi( new QFrame( this ) )
{
  main_w = pwi;
  pwi->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
  bool ro = h.isRoTree( efROAny );
  int n = h.arrSize();

  les.reserve(n);

  // double v_min = h.getParmDouble( QSL("min"), DMIN );
  // double v_max = h.getParmDouble( QSL("max"), DMAX );
  // int decimals = h.getParmInt( QSL("decimals"), DOUBLE_PREC );

  auto lay = new QGridLayout( pwi );

  lay->addWidget( lbl, 0, 1 );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( QSL("[") % QSN(i) + QSL("]") );
    lay->addWidget( la, i+1, 0 );

    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    // le->setValidator( new QDoubleValidator( v_min, v_max, decimals, le ) ); TODO: revive! DoubleValidatorEx
    lay->addWidget( le, i+1, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 1, 1, 1, 1 );
  pwi->setLayout( lay ); // ???
}

bool DoubleArrayDataWidget::obj2vis()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( QSND( ho.getDouble(i) ) );
  }
  return true;
}

bool DoubleArrayDataWidget::vis2obj() const
{
  bool ok = false;
  int n = les.size();
  QVariant v;
  for( int i=0; i<n; ++i ) {
    v = toDoubleEx( les[i]->text(), &ok );
    ho.set( v, i );
  }
  return ok;
}

DW_REG_FUN_STD( DoubleArrayDataWidget, "ARRAY_DOUBLE,INLINE" );


// ------------------- StringArrayDataWidget ---------------------------

StringArrayDataWidget::StringArrayDataWidget( HolderData &h, QWidget *parent, bool /*hideLabel*/ )
  : DataWidget( h, parent, false ),
  pwi( new QFrame( this ) )
{
  main_w = pwi;
  pwi->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
  bool ro = h.isRoTree( efROAny );
  int n = h.arrSize();

  les.reserve(n);

  int len_max = h.getParmInt( QSL("max"), 4096 ); // not IMAX - good limit for single-line string
  QString mask = h.getParm( QSL("mask") );

  auto lay = new QGridLayout( pwi );

  lay->addWidget( lbl, 0, 1 );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( QSL("[") % QSN(i) + QSL("]") );
    lay->addWidget( la, i+1, 0 );

    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    le->setMaxLength( len_max );
    if( !mask.isEmpty() ) {
      le->setInputMask( mask );
    }
    lay->addWidget( le, i+1, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 1, 1, 1, 1 );
  pwi->setLayout( lay );
}

bool StringArrayDataWidget::obj2vis()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( ho.get(i).toString() );
  }
  return true;
}

bool StringArrayDataWidget::vis2obj() const
{
  bool ok = false;
  int n = les.size();
  QVariant v;
  for( int i=0; i<n; ++i ) {
    v = les[i]->text();
    ho.set( v, i );
  }
  return ok;
}

DW_REG_FUN_STD( StringArrayDataWidget, "ARRAY_STRING,INLINE" );


// ------------------- ObjDataWidget ---------------------------

ObjDataWidget::ObjDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
   pb( new QPushButton( this ) )
{
  lbl->setText( QSL(">") );
  lbl->setBuddy( pb );
  main_w = pb; // no R/O handling here
  if( hideLabel ) {
    int w0 = 3 * LaboWin::Em() + 6;
    pb->setFixedWidth( w0 );
    setFixedWidth( w0+2 );
    pb->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
  } else {
    pb->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  }
  updateLabel();
  connect( pb, &QPushButton::clicked, this, &ObjDataWidget::edit );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( pb, 1 );
  setLayout( lay );
}

bool ObjDataWidget::obj2vis()
{
  // TODO:
  return true;
}

bool ObjDataWidget::vis2obj() const
{
  // TODO:
  return true;
}

void ObjDataWidget::updateLabel()
{
  QString btn_text = ho.dataObj( 1, Qt::StatusTipRole ).toString(); // col 1: for button
  pb->setText( btn_text );
  pb->setShortcut( QKeySequence() );
  pb->setIcon( ho.getIcon() );
}

void ObjDataWidget::edit()
{
  HolderData *obj = qobject_cast<HolderData*>(&ho);
  if( obj ) {
    auto dia = new DataDialog( *obj,  this );
    dia->exec();
    emit editingFinished();
    updateLabel();
  } else {
    qWarning() << "Fail to convert holder " <<  ho.getFullName() << " to HolderData " << WHE;
  }
}

DW_REG_FUN_STD( ObjDataWidget, "OBJ" );


// ------------------- InputDataWidget ---------------------------

InputDataWidget::InputDataWidget( HolderData &h, QWidget *parent, bool hideLabel )
  : DataWidget( h, parent, hideLabel ),
   le( new QLineEdit( this ) ),
   ow( new ObjDataWidget( h, this, true ) )
{
  main_w = ow;
  if( h.isRoTree( efROAny ) ) {
    le->setReadOnly( true );
  }
  // ow->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );

  if( !hideLabel ) {
    lbl->setBuddy( le );
  }

  le->setMinimumWidth( 6 * LaboWin::Em() );
  le->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  auto cmpl = new LinkCompleter( this );
  QAbstractItemModel *cmpl_mdl = h.getComplModel( QSL("in"), cmpl );
  cmpl->setModel( cmpl_mdl );
  le->setCompleter( cmpl );

  auto lay =  new QHBoxLayout( this );
  const int msz = 0;
  lay->setContentsMargins( msz, msz, msz, msz );
  lay->addWidget( lbl, 0 );
  lay->addWidget( le,  5 );
  lay->addWidget( ow,  0 );
  auto frr = new QFrame( this );
  frr->setFrameStyle( QFrame::VLine );
  lay->addWidget( frr );
  setLayout( lay );
  setFrameStyle( QFrame::Panel | QFrame::Sunken );
  setMinimumWidth( lbl->width() + (6+3) * LaboWin::Em() ); // hack

  connect( le, &QLineEdit::editingFinished, this, &InputDataWidget::lineToObj );
  connect( ow, &ObjDataWidget::editingFinished, this, &InputDataWidget::objToLine );
}

bool InputDataWidget::obj2vis()
{
  QString s = ho.getDataD( QSL("source"), QSL("") );
  le->setText( s );
  // other is done by ObjDataWidget::edit
  return true;
}

bool InputDataWidget::vis2obj() const
{
  // TODO:
  return true;
}

void InputDataWidget::lineToObj()
{
  QString s = le->text();
  ho.setData( QSL("source"), s );
  ho.handleStructChanged();
  ow->updateLabel();
}

void InputDataWidget::objToLine()
{
  QString s = ho.getDataD(  QSL("source"), QSL("") );
  le->setText( s );
}


DW_REG_FUN_STD( InputDataWidget, "OBJ,INPUT" );


// ============================================================
// ==================== FactoryDataWidget ====================

FactoryDataWidget::FactoryDataWidget()
{
}

FactoryDataWidget& FactoryDataWidget::theFactory()
{
  static FactoryDataWidget instance;
  return instance;
}


QString FactoryDataWidget::findForHolder( const HolderData &ho, int *lev ) const
{
  QString name;
  int max_good = IMIN;
  QStringList ho_p_list = ho.getParm( QSL("props") ).split( QSL(",") );

  for( auto i = propMap.begin(); i!= propMap.end(); ++i ) { // no : need key
    int good = 0;
    QStringList w_p_list = QString( i.value().eprop ).split(QSL(","));

    for( int j=0; j<ho_p_list.size(); ++j ) {
      if( w_p_list.contains( ho_p_list.at(j) ) ) {
        good += 10;
      }
    }

    for( int j=0; j<w_p_list.size(); ++j ) {
      if( ! ho_p_list.contains( w_p_list.at(j) ) ) {
        good -= 2;
      }
    }

    if( good > max_good ) {
      max_good = good;
      name = i.key();
    }
  }

  if( max_good < 0 ) { // TODO ????
    name = QSL("DummyDataWidget");
    max_good = 0;
  }

  if( lev != nullptr ) {
    *lev = max_good;
  }

  return name;
}

DataWidget* FactoryDataWidget::createDataWidget( HolderData &ho, QWidget *parent ) const
{
  QString name = findForHolder( ho );
  if( name.isEmpty() ) {
    return nullptr;
  }

  const auto i = propMap.find( name );
  if( i == propMap.end() ) {
    return nullptr;
  }

  return i.value().creator( ho, parent );
}

bool FactoryDataWidget::registerWidgetType( const QString &wname,
    const DataWidgetProp &prop )
{
  if( propMap.contains( wname ) ) {
    return false;
  }

  propMap.insert( wname, prop );
  // updateHints();
  return true;
}

bool FactoryDataWidget::unregisterWidgetType( const QString &wname )
{
  auto i = propMap.find( wname );
  if( i == propMap.end() ) {
    return false;
  }

  propMap.erase( i );
  // updateHints();
  return true;
}

// ======================= DataDialog ===========================

DataDialog::DataDialog( HolderData &a_ds, QWidget *parent )
  : QDialog( parent ), ds( a_ds ), ro( ds.isRoTree( efROAny ) )
{
  setStyleSheet( "*[readOnly=\"true\"] {"
      "color: #000000;"
      "background-color: #C0C0C0;"
      "border: 1px solid #909090;"
      "border-radius: 2px; }");

  ds.post_set(); // BUG: may be not here, but sometimes post_set not work as required
  obj2visAll();
  saved_data = ds.toString();
  was_modified = ds.getModified();
}

int DataDialog::obj2visAll()
{
  QString s = ds.getType()  %  ' ' %  ds.getFullName();
  if( ds.getModified() ) {
    s += QSL(" *");
  }
  setWindowTitle( s );

  createWidgets(); // recreate iface

  int ng = 0;
  for( auto w : dwm  ) {
    w->obj2vis();
    ++ng;
  }

  return ng;
}

int DataDialog::vis2objAll() // from widgets to object
{
  if( ro ) {
    return 0;
  }
  int ns = 0;

  for( auto w : dwm ) {
    w->vis2obj();
    ++ns;
  }
  ds.post_set();
  return ns;
}

void DataDialog::accept()
{
  vis2objAll();
  QDialog::accept();
}

void DataDialog::reject()
{
  if( was_modified || ds.getModified() ) {
    revertData();
  }
  QDialog::reject();
}


void DataDialog::checkData()
{
  // TODO:
}

void DataDialog::refreshData()
{
  vis2objAll();
  obj2visAll();
}

void DataDialog::revertData()
{
  if( ro ) {
    return;
  }
  ds.suspendHandleStructChange();
  ds.delAllDyn(); // TODO: suppress errors during recreate.
  ds.fromString( saved_data );
  if( ! was_modified ) {
    ds.setUnModified();
  }
  ds.resumeHandleStructChange();
  obj2visAll();
}

void DataDialog::defaultData()
{
  if( ro ) {
    return;
  }
  ds.suspendHandleStructChange();
  for( auto ho : ds.TCHILD(HolderData*) ) {
    if( !ho->isRoTree( efROAny ) ) {
      ho->reset_dfl();
    }
  }
  ds.resumeHandleStructChange();
  obj2visAll();
}


void DataDialog::showHelp()
{
  // TODO: file
  return showSimpleHelp();
}


void DataDialog::showSimpleHelp()
{
  const char *help;
  if( (help = ds.getHelp()) == 0 ) {
    return;
  }
  QString help_str = QSL( "<p><a name=\"help_head\"></a> </p><hr/>\n" ) % help % QSL("<hr/>\n");

  for( auto ho : ds.TCHILD(HolderData*) ) {
    help_str += QSL("<p>") % ho->getType()
      % QSL(" <b> ") % ho->objectName() % QSL(" </b>; // ")
      % ho->getParm( QSL("descr") )  % QSL(" (")
      % ho->getParm( QSL("vis_name") ) % QSL(")</p>\n");
  }

  auto dia = new QDialog( this );
  int em = LaboWin::Em();
  dia->setMinimumSize( 60*em, 50*em );
  dia->setWindowTitle( PACKAGE ": Help for element" );

  auto lay = new QGridLayout;

  auto brow = new QTextBrowser( this );
  brow->insertHtml( help_str );

  brow->setStyleSheet( QSL("QTextEdit { background-color: white; }") );
  lay->addWidget( brow, 0, 0 );

  auto bt_ok = new QPushButton( QSL("&Ok"), dia );
  lay->addWidget( bt_ok, 1, 0 );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  brow->scrollToAnchor( QSL("help_head") );
  dia->exec();
  delete dia;

}


bool DataDialog::addObj()
{
  if( ro ) {
    return false;
  }
  AddObjParams prm;

  HolderData *ho = SelectTypeDialog::askAndCreateObj( &ds, this, prm );
  if( !ho ) {
    return false;
  }

  reactBigChanges();
  return true;
}

bool DataDialog::delObj()
{
  if( ro ) {
    return false;
  }
  QStringList sl;
  for( auto ho :  ds.TCHILD(HolderData*) ) {
    if( ! ho->isDyn() || ho->hasAnyFlags( efImmutable ) ) {
      continue;
    }
    sl << ho->objectName();
  }
  if( sl.isEmpty() ) {
    return false;
  }

  bool ok;
  QString ob_name = QInputDialog::getItem( this, QSL("Delete object"),
      QSL("Select object to delete"), sl, 0, false, &ok );
  if( !ok ) {
    return false;
  }
  return delObjByName( ob_name );
}

bool DataDialog::delObjByName( const QString &name )
{
  if( ro ) {
    return false;
  }
  bool ok = ds.delObj( name );
  reactBigChanges();
  return ok;
}

bool DataDialog::copyOne()
{
  QStringList sl = ds.elemNames();
  if( sl.isEmpty() ) {
    return false;
  }

  bool ok;
  QString ob_name = QInputDialog::getItem( this, QSL("Copy object"),
      QSL("Select object to copy"), sl, 0, false, &ok );
  if( !ok ) {
    return false;
  }

  HolderData *ho = ds.getObj( ob_name );
  if( !ho ) {
    qWarning() << QSL("Fail to find object ") << ob_name << QSL(" to copy") << WHE;
    return false;
  }

  QString s = ho->toXML( true );
  setClipboardStr( s );
  return true;
}

bool DataDialog::copyAll()
{
  QString s = ds.toXML( true );
  setClipboardStr( s );
  return true;
}

bool DataDialog::pasteOne() // like CmdView::pasteObj() TODO: move common code
{
  if( ro ) {
    return false;
  }
  QString s = getClipboardStr();

  int err_line, err_column;
  QString errstr;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    handleWarn( this, tr("Cannot parse clipboard string:\n%2\nLine %3 column %4.")
                .arg(errstr).arg(err_line).arg(err_column) );
    return false;
  }
  QDomElement ee = x_dd.documentElement();

  QString tagname = ee.tagName();
  // if( tagname != "obj" ) {
  //   handleWarn( this, tr("element tag is not 'obj':  %2").arg( tagname ) );
  //   return false;
  // }

  QString otype = ee.attribute( QSL("otype") );
  QString oname = ee.attribute( QSL("name") );
  oname = ds.hintName( otype, oname );

  bool ok;
  oname = QInputDialog::getText( this, "Object: " + oname,
      QSL("Enter new name (type ") % otype % QSL("):"), QLineEdit::Normal, oname, &ok );
  if( !ok ) {
    return false;
  }

  HolderData *ob = ds.addObjP( otype, oname );
  if( !ob  ) {
    handleError( this, QString("Fail to add Obj: %1 %2").arg(otype).arg(oname) );
    return false;
  }
  if( !ob->fromDom( ee, errstr ) ) {
    handleWarn( this, tr("fail to set params:  %1").arg( errstr ) );
  }
  reactBigChanges();
  return true;
}

bool DataDialog::pasteAll()
{
  if( ro ) {
    return false;
  }
  QString s = getClipboardStr();

  bool ok =  ds.fromString( s );
  if( !ok ) {
    return false;
  }

  reactBigChanges();
  return true;
}

bool DataDialog::reactBigChanges()
{
  auto t_idx = tw->currentIndex();
  createWidgets();
  obj2visAll();
  tw->setCurrentIndex( t_idx ); // another tw, object, index may be the same
  return true;
}


int DataDialog::createWidgets()
{
  int nr = 0, nc = 0, nr_max = 0, n_tab = 0;
  bool was_block = 0, was_col = 0, was_tab = 0;

  if( !lay_main ) {
    lay_main = new QVBoxLayout( this );
    setLayout( lay_main );
  }

  createButtons();

  if( tw ) {
    delete tw; // and all sub-widgets
  }
  dwm.clear();
  tw = new QTabWidget( this );
  lay_main->insertWidget( 0, tw );

  tw->setTabBarAutoHide( true );

  QWidget *wmain = nullptr;
  QGridLayout *lay_gr = nullptr;
  QVBoxLayout *lay_wv = nullptr;
  QString tabname = QSL("Main");
  was_tab = true;

  for( auto ho :  ds.TCHILD(HolderData*) ) {

    QString sep =  ho->getParm( QSL("sep") );
    QStringList seps = sep.split( QSL(",") );
    QString ctn =  ho->getParm( QSL("tabname") );
    if( !ctn.isEmpty() ) {
      tabname = ctn;
    }
    int ncol =  ho->getParmInt( QSL("ncol"), 1 ); // number of columns per widget
    ncol = vBound( -1, ncol, MAX_COLS_PER_WIDGET );

    if( seps.contains( QSL("tab") ) || was_tab ) {
      if( lay_gr ) {
        addFinalSpace( lay_gr );
      }
      nc = nr = nr_max = 0;
      wmain = new QWidget();
      tw->addTab( wmain, tabname );

      lay_wv = new QVBoxLayout( wmain );

      was_block = true; lay_gr = nullptr;
      ++n_tab;
      tabname = QSL("Tab &") % QSN( n_tab ); // next tab, if not overrided
    }

    if( seps.contains( QSL("block") ) || was_block ) {

      if( lay_gr ) {
        auto fr = new QFrame( wmain );
        fr->setFrameStyle( QFrame::HLine );
        lay_gr->addWidget( fr, nr_max, 0, 1, -1 );
      }
      lay_gr = new QGridLayout; lay_gr->setSpacing( 2 );
      lay_wv->addLayout( lay_gr );
      nr = nr_max = nc = 0;
    }

    if( seps.contains( QSL("col") ) || was_col ) {
      nr = 0; ++nc;
    }

    if( seps.contains( QSL("row") ) ) {
      nr = nr_max; nc = 0;
    }

    if( ! ho->hasAnyFlags( efNoDial ) ) {
      QString name = ho->objectName();

      DataWidget *w = FactoryDataWidget::theFactory().createDataWidget( *ho, tw );
      if( !w ) {
        qWarning() << "not found edit widget for object " << name << WHE;
        continue;
      }

      dwm[name] = w;
      QString whats = ho->getType() % QSL(" ") % ho->objectName() % QSL("; ")
        % ho->dataObj( 0, Qt::WhatsThisRole ).toString();
      w->setWhatsThis( whats );
      // w->setStatusTip( whats ); // TODO: just for test
      // w->setToolTip( whats ); // TODO: just for test
      lay_gr->addWidget( w, nr, nc, 1, ncol );

      ++nr;
      if( nr > nr_max ) {
        nr_max = nr;
      }
    }

    was_col = was_block = was_tab = false;
    if( seps.contains( QSL("colend") ) ||  nr >= MAX_WIDGETS_PER_COL ) {
      was_col = true;
    }
    if( seps.contains( QSL("blockend") ) ) {
      was_block = true;
    }
    if( seps.contains( QSL("tabend") ) ) {
      was_tab = true;
    }
  } // -------------- end item loop
  if( lay_gr ) { // last tab: add stretch
    addFinalSpace( lay_gr );
  }

  return nr;
}

void DataDialog::addFinalSpace( QGridLayout *lay )
{
  int xnr = lay->rowCount();
  auto frb = new QFrame( this );
  // frb->setFrameStyle( QFrame::HLine );
  // frb->setFrameStyle( QFrame::Box );
  frb->setMinimumSize( 1, 1 );
  frb->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
  lay->addWidget( frb, xnr, 0, 1, -1 );
  lay->setRowMinimumHeight( xnr, 1 );
  lay->setRowStretch( xnr, 2 );
}

void DataDialog::createButtons()
{
  if( buttons_created ) {
    return;
  }
  if( !lay_main ) { return; }

  auto frb = new QFrame( this );
  frb->setFrameStyle( QFrame::HLine );
  lay_main->insertWidget( 1, frb );

  bool can_add_objs = false;
  QStringList obj_clss = EFACT.goodTypeNames( ds.allowTypes() );
  if( ! obj_clss.isEmpty() && !ro ) {
    can_add_objs = true;
  }

  if( can_add_objs ) {
    auto lay_btn2 = new QHBoxLayout;
    auto btn_addObj = new QPushButton( QIcon::fromTheme(QSL("list-add")), QSL("Add object") );
    connect( btn_addObj, &QPushButton::clicked, this, &DataDialog::addObj );
    lay_btn2->addWidget( btn_addObj );

    auto btn_delObj = new QPushButton( QIcon::fromTheme(QSL("edit-delete")), QSL("Delete object") );
    connect( btn_delObj, &QPushButton::clicked, this, &DataDialog::delObj );
    lay_btn2->addWidget( btn_delObj );

    auto btn_pasteOne = new QPushButton( QSL("Paste obj") );
    connect( btn_pasteOne, &QPushButton::clicked, this, &DataDialog::pasteOne );
    lay_btn2->addWidget( btn_pasteOne );

    auto btn_pasteAll = new QPushButton( QSL("Paste all") );
    connect( btn_pasteAll, &QPushButton::clicked, this, &DataDialog::pasteAll );
    lay_btn2->addWidget( btn_pasteAll );

    lay_main->addLayout( lay_btn2 );
  }

  auto lay_btn = new QHBoxLayout;
  auto btn_ok = new QPushButton( QIcon::fromTheme(QSL("dialog-ok")),QSL("Ok") );
  if( ro ) {
    btn_ok->setDisabled( true );
  } else {
    btn_ok->setDefault( true );
    connect( btn_ok, &QPushButton::clicked, this, &DataDialog::accept);
  }
  lay_btn->addWidget( btn_ok );
  //
  auto btn_cancel = new QPushButton( QIcon::fromTheme(QSL("dialog-cancel")), QSL("Cancel") );
  connect( btn_cancel, &QPushButton::clicked, this, &DataDialog::reject);
  lay_btn->addWidget( btn_cancel );
  //
  auto btn_refresh = new QPushButton( QIcon::fromTheme(QSL("view-refresh")), QSL("Refresh") );
  if( ro ) {
    btn_refresh->setDisabled( true );
  } else {
    connect( btn_refresh, &QPushButton::clicked, this, &DataDialog::refreshData );
  }
  lay_btn->addWidget( btn_refresh );

  //
  auto btn_more = new QPushButton( QSL("More...") );
  QMenu *mnu = new QMenu( this );
  //
  auto act = mnu->addAction( "Copy Object" );
  connect( act, &QAction::triggered, this, &DataDialog::copyOne );
  //
  act = mnu->addAction( "Copy All" );
  connect( act, &QAction::triggered, this, &DataDialog::copyAll );
  //
  if( !ro ) {
    act = mnu->addAction( QIcon::fromTheme(QSL("document-revert")), QSL("Revert") );
    connect( act, &QAction::triggered, this, &DataDialog::revertData );
    act = mnu->addAction( QSL("reset to Default") );
    connect( act, &QAction::triggered, this, &DataDialog::defaultData );
  }
  //
  btn_more->setMenu( mnu );
  lay_btn->addWidget( btn_more );
  //


  auto btn_help = new QPushButton( QIcon::fromTheme(QSL("help-contents")), QSL("Help") );
  connect( btn_help, &QPushButton::clicked, this, &DataDialog::showHelp );
  lay_btn->addWidget( btn_help );

  lay_main->insertLayout( 2, lay_btn );

  buttons_created = true;
}
