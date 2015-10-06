/***************************************************************************
                          datawidget.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "miscfun.h"
#include "datawidget.h"
#include "labowin.h"
#include "addelemdia.h"
#include "linkcompleter.h"

using namespace std;

DataWidget::DataWidget( HolderData &h, QWidget *parent )
  : QFrame( parent ), ho( h ), main_w( nullptr ),
    lbl( new QLabel( tex2label(ho.getParm( "vis_name" )), this ) )
{
  lbl->setWhatsThis( ho.getType() + " " + ho.objectName() );
  lbl->setMinimumWidth( 5 * LaboWin::Em() );
  lbl->setTextFormat( Qt::RichText );
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

// -------------  DummyDataWidget ----------------

int DummyDataWidget::registered = DummyDataWidget::reg();

DummyDataWidget::DummyDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  lbl_d( new QLabel( this ) )
{
  main_w = lbl_d;
  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( lbl_d );
}

bool DummyDataWidget::set()
{
  lbl_d->setText( "@ " + ho.toString().left(20) );
  return true;
}

bool DummyDataWidget::get() const
{
  return true;
}


DataWidget* DummyDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new DummyDataWidget( h, parent );
}

int DummyDataWidget::reg()
{
  static DataWidgetProp p { create, "" };
  return FactoryDataWidget::theFactory().registerWidgetType( "DummyDataWidget", p );
}


// ------------------- StringDataWidget ---------------------------
int StringDataWidget::registered = StringDataWidget::reg();

StringDataWidget::StringDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit( this ) )
{
  main_w = le;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }
  lbl->setBuddy( le );

  int v_max { 4096 }; // not IMAX - good limit for single-line string
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toInt();
  }
  le->setMaxLength( v_max );

  QString mask = h.getParm( "mask" );
  if( ! mask.isEmpty() ) {
    le->setInputMask( mask );
  }

  QString cmpl_targ = h.getParm( "cmpl" );
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

bool StringDataWidget::set()
{
  le->setText( ho.toString() );
  return true;
}

bool StringDataWidget::get() const
{
  ho.fromString( le->text() );
  return true;
}

DataWidget* StringDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new StringDataWidget( h, parent );
}

int StringDataWidget::reg()
{
  static DataWidgetProp p { create, "STRING,SIMPLE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "StringDataWidget", p );
}


// ------------------- StringMLDataWidget ---------------------------
int StringMLDataWidget::registered = StringMLDataWidget::reg();

StringMLDataWidget::StringMLDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  te( new QTextEdit( this ) )
{
  main_w = te;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    te->setReadOnly( true );
  }
  // setMaxLength( h.getMax() ); ???
  // TODO: min size
  te->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );

  auto lay =  new QVBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( te, 1 );
  setLayout( lay );
  setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
}

bool StringMLDataWidget::set()
{
  te->setText( ho.toString() );
  return true;
}

bool StringMLDataWidget::get() const
{
  ho.fromString( te->toPlainText() );
  return true;
}

DataWidget* StringMLDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new StringMLDataWidget( h, parent );
}

int StringMLDataWidget::reg()
{
  static DataWidgetProp p { create, "STRING,MLINE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "StringMLDataWidget", p );
}

QSize StringMLDataWidget::minimumSizeHint() const
{
  return te->minimumSizeHint() + QSize( 0, lbl->minimumSizeHint().height() );
}


QSize StringMLDataWidget::sizeHint() const
{
  return te->sizeHint() + QSize( 0, lbl->sizeHint().height() );
}


// ------------------- StringExtDataWidget ---------------------------
int StringExtDataWidget::registered = StringExtDataWidget::reg();

StringExtDataWidget::StringExtDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  pb( new QPushButton( this ) )
{
  main_w = pb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    pb->setDisabled( true );
  }
  pb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
  pb->setText( ho.objectName() % " [text]" );
  connect( pb, &QPushButton::clicked, this, &StringExtDataWidget::edit );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( pb, 1 );
  setLayout( lay );
}

bool StringExtDataWidget::set()
{
  ts = ho.toString();
  return true;
}

bool StringExtDataWidget::get() const
{
  ho.fromString( ts );
  return true;
}

void StringExtDataWidget::edit()
{
  QTemporaryFile f;
  // f.setFileTemplate( "tmp_file_XXXXXXXX.js" );
  if( !f.open() ) {
    return;
  }
  QString fn = f.fileName();
  QString cmd = "gvim -f ";
  if( LaboWin::win() ) {
    Mo2Settings *sett = LaboWin::win()->getSettings();
    if( sett ) {
      cmd = sett->editCmd;
    }
  }
  cmd += " " + fn;

  {
    QTextStream os( &f );
    os << ts << endl;
    f.close();
  }

  QProcess proc;
  int rc = proc.execute( cmd );
  if( rc != 0 ) {
    return;
  }

  if( !f.open() ) {
    return;
  }
  QTextStream is( &f );
  ts = is.readAll();
  f.close();
}


DataWidget* StringExtDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new StringExtDataWidget( h, parent );
}

int StringExtDataWidget::reg()
{
  static DataWidgetProp p { create, "STRING,EXT,MLINE,LARGETEXT" };
  return FactoryDataWidget::theFactory().registerWidgetType( "StringExtDataWidget", p );
}





// ------------------- IntDataWidget ---------------------------
int IntDataWidget::registered = IntDataWidget::reg();

IntDataWidget::IntDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit( this ) )
{
  main_w = le;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }

  int v_min { IMIN }, v_max { IMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() ) {
    v_min = s_min.toInt();
  }
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toInt();
  }
  le->setValidator( new QIntValidator( v_min, v_max, le ) );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le, 1 );
  setLayout( lay );
}

bool IntDataWidget::set()
{
  le->setText( QSN( ho.get().toInt() ) );
  return true;
}

bool IntDataWidget::get() const
{
  bool ok;
  QVariant v = le->text().toInt( &ok, 0 );
  ho.set( v );
  return ok;
}

DataWidget* IntDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new IntDataWidget( h, parent );
}

int IntDataWidget::reg()
{
  static DataWidgetProp p { create, "INT,SIMPLE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "IntDataWidget", p );
}


// ------------------- IntSpinDataWidget ---------------------------
int IntSpinDataWidget::registered = IntSpinDataWidget::reg();

IntSpinDataWidget::IntSpinDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  sb( new QSpinBox( this ) )
{
  main_w = sb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    sb->setReadOnly( true );
  }

  int v_min { IMIN }, v_max { IMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() ) {
    v_min = s_min.toInt();
  }
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toInt();
  }
  sb->setRange( v_min, v_max );

  QString prefix = h.getParm( "prefix" );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }

  bool ok;
  int step = h.getParm( "step" ).toInt( &ok, 0 );
  if( ok ) {
    sb->setSingleStep( step );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( sb, 1 );
  setLayout( lay );

}

bool IntSpinDataWidget::set()
{
  sb->setValue(  ho.get().toInt() );
  return true;
}

bool IntSpinDataWidget::get() const
{
  QVariant v = sb->value();
  ho.set( v );
  return true;
}

DataWidget* IntSpinDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new IntSpinDataWidget( h, parent );
}

int IntSpinDataWidget::reg()
{
  static DataWidgetProp p { create, "INT,SPIN" };
  return FactoryDataWidget::theFactory().registerWidgetType( "IntSpinDataWidget", p );
}



// ------------------- SwitchDataWidget ---------------------------
int SwitchDataWidget::registered = SwitchDataWidget::reg();

SwitchDataWidget::SwitchDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  cb( new QCheckBox( " ", this )  )
{
  main_w = cb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true );
  }

  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );

  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool SwitchDataWidget::set()
{
  cb->setChecked(  ho.get().toBool() );
  return true;
}

bool SwitchDataWidget::get() const
{
  QVariant v = (int)cb->isChecked();
  ho.set( v );
  return true;
}

DataWidget* SwitchDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new SwitchDataWidget( h, parent );
}

int SwitchDataWidget::reg()
{
  static DataWidgetProp p { create, "INT,SWITCH" };
  return FactoryDataWidget::theFactory().registerWidgetType( "SwitchDataWidget", p );
}


// ------------------- ListDataWidget ---------------------------
int ListDataWidget::registered = ListDataWidget::reg();

ListDataWidget::ListDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  cb( new QComboBox( this ) )
{
  main_w = cb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true );
  }
  QString enum_name = ho.getParm( "enum" );
  QStringList sl;
  if( ! enum_name.isEmpty() ) {
    HolderData *par = h.getParent();
    if( par ) {
      sl = par->getEnumStrings( enum_name );
    }
  }
  QString noTeX = ho.getParm( "noTeX" );
  if( noTeX != "y" ) {
    for( auto &s : sl ) { s = tex2label( s, true ); }
  }
  cb->addItems( sl );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool ListDataWidget::set()
{
  cb->setCurrentIndex( ho.get().toInt() );
  return true;
}

bool ListDataWidget::get() const
{
  QVariant v = cb->currentIndex();
  ho.set( v );
  return true;
}

DataWidget* ListDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new ListDataWidget( h, parent );
}

int ListDataWidget::reg()
{
  static DataWidgetProp p { create, "INT,LIST" };
  return FactoryDataWidget::theFactory().registerWidgetType( "ListDataWidget", p );
}



// ------------------- DoubleDataWidget ---------------------------
int DoubleDataWidget::registered = DoubleDataWidget::reg();

DoubleDataWidget::DoubleDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit ( this ) )
{
  main_w = le;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }

  int decimals = 15;
  QString sdec =  h.getParm("decimals" );
  if( ! sdec.isEmpty() ) {
    decimals = sdec.toInt();
  }
  double v_min { DMIN }, v_max { DMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() )
    v_min = s_min.toDouble();
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() )
    v_max = s_max.toDouble();

  le->setValidator( new QDoubleValidator( v_min, v_max, decimals, le ) );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( le );
  setLayout( lay );
}

bool DoubleDataWidget::set()
{
  le->setText( QSN( ho.get().toDouble(), 'g', 15 ) );
  return true;
}

bool DoubleDataWidget::get() const
{
  bool ok;
  QVariant v = le->text().toDouble( &ok );
  ho.set( v );
  return ok;
}

DataWidget* DoubleDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new DoubleDataWidget( h, parent );
}

int DoubleDataWidget::reg()
{
  static DataWidgetProp p { create, "DOUBLE,SIMPLE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "DoubleDataWidget", p );
}


// ------------------- DoubleSpinDataWidget ---------------------------
int DoubleSpinDataWidget::registered = DoubleSpinDataWidget::reg();

DoubleSpinDataWidget::DoubleSpinDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
   sb( new QDoubleSpinBox( this ) )
{
  main_w = sb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    sb->setReadOnly( true );
  }

  double v_min { DMIN }, v_max { DMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() ) {
    v_min = s_min.toDouble();
  }
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toDouble();
  }
  sb->setRange( v_min, v_max );

  QString prefix = h.getParm( "prefix" );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }

  bool ok;
  double step = h.getParm( "step" ).toDouble( &ok );
  if( ok ) {
    sb->setSingleStep( step );
  }

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( sb, 1 );
  setLayout( lay );
}

bool DoubleSpinDataWidget::set()
{
  sb->setValue(  ho.get().toDouble() );
  return true;
}

bool DoubleSpinDataWidget::get() const
{
  QVariant v = sb->value();
  ho.set( v );
  return true;
}

DataWidget* DoubleSpinDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new DoubleSpinDataWidget( h, parent );
}

int DoubleSpinDataWidget::reg()
{
  static DataWidgetProp p { create, "DOUBLE,SPIN" };
  return FactoryDataWidget::theFactory().registerWidgetType( "DoubleSpinDataWidget", p );
}



// ------------------- ColorDataWidget ---------------------------
int ColorDataWidget::registered = ColorDataWidget::reg();

ColorDataWidget::ColorDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
   cb( new ColorBtn( this ) )
{
  main_w = cb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true ); // TODO: real read-only
  }
  cb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Expanding );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( cb, 1 );
  setLayout( lay );
}

bool ColorDataWidget::set()
{
  int c_i = ho.get().toInt();
  QRgb c_r( c_i );
  QColor c( c_r );
  cb->setColor( c ); // Int to Color inside button
  return true;
}

bool ColorDataWidget::get() const
{
  QColor c = cb->color();
  int c_i = c.rgba();
  QVariant v = c_i;
  ho.set( v );
  return true;
}

DataWidget* ColorDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new ColorDataWidget( h, parent );
}

int ColorDataWidget::reg()
{
  static DataWidgetProp p { create, "COLOR" };
  return FactoryDataWidget::theFactory().registerWidgetType( "ColorDataWidget", p );
}



// ------------------- IntArrayDataWidget ---------------------------
int IntArrayDataWidget::registered = IntArrayDataWidget::reg();

IntArrayDataWidget::IntArrayDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  pwi( new QWidget( this ) )
{
  main_w = pwi;
  bool ro = h.getFlags() & ( efRO | efRODial );
  int n = h.arrSize();
  les.reserve(n);
  QString vn = h.getParm("v_name");
  if( vn.isEmpty() ) {
    vn = h.objectName();
  }
  vn += '[';

  int v_min { IMIN }, v_max { IMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() ) {
    v_min = s_min.toInt();
  }
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toInt();
  }

  lbl->setText(""); // hack: hide common name
  auto lay = new QGridLayout( pwi );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( vn + QSN(i) + ']' );
    lay->addWidget( la, i, 0 );

    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    le->setValidator( new QIntValidator( v_min, v_max, le ) );
    lay->addWidget( le, i, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 0, 0, 0, 0 );
  pwi->setLayout( lay ); // ???
}

bool IntArrayDataWidget::set()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( QSN( ho.get(i).toInt() ) );
  }
  return true;
}

bool IntArrayDataWidget::get() const
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

DataWidget* IntArrayDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new IntArrayDataWidget( h, parent );
}

int IntArrayDataWidget::reg()
{
  static DataWidgetProp p { create, "ARRAY_INT,INLINE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "IntArrayDataWidget", p );
}


// ------------------- DoubleArrayDataWidget ---------------------------
int DoubleArrayDataWidget::registered = DoubleArrayDataWidget::reg();

DoubleArrayDataWidget::DoubleArrayDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  pwi( new QWidget( this ) )
{
  main_w = pwi;
  bool ro = h.getFlags() & ( efRO | efRODial );
  int n = h.arrSize();

  les.reserve(n);
  QString vn = h.getParm("v_name");
  if( vn.isEmpty() ) {
    vn = h.objectName();
  }
  vn += '[';

  double v_min { DMIN }, v_max { DMAX };
  QString s_min = h.getParm( "min" );
  if( ! s_min.isEmpty() ) {
    v_min = s_min.toDouble();
  }
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toDouble();
  }
  int decimals = 12;
  QString sdec =  h.getParm("decimals" );
  if( ! sdec.isEmpty() ) {
    decimals = sdec.toInt();
  }

  lbl->setText(""); // hack: hide common name
  auto lay = new QGridLayout( pwi );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( vn + QSN(i) + ']' );
    lay->addWidget( la, i, 0 );


    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    le->setValidator( new QDoubleValidator( v_min, v_max, decimals, le ) );
    lay->addWidget( le, i, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 0, 0, 0, 0 );
  pwi->setLayout( lay ); // ???
}

bool DoubleArrayDataWidget::set()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( QSN( ho.get(i).toDouble() ) );
  }
  return true;
}

bool DoubleArrayDataWidget::get() const
{
  bool ok = false;
  int n = les.size();
  QVariant v;
  for( int i=0; i<n; ++i ) {
    v = les[i]->text().toDouble( &ok );
    ho.set( v, i );
  }
  return ok;
}

DataWidget* DoubleArrayDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new DoubleArrayDataWidget( h, parent );
}

int DoubleArrayDataWidget::reg()
{
  static DataWidgetProp p { create, "ARRAY_DOUBLE,INLINE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "DoubleArrayDataWidget", p );
}


// ------------------- StringArrayDataWidget ---------------------------
int StringArrayDataWidget::registered = StringArrayDataWidget::reg();

StringArrayDataWidget::StringArrayDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  pwi( new QWidget( this ) )
{
  main_w = pwi;
  bool ro = h.getFlags() & ( efRO | efRODial );
  int n = h.arrSize();

  les.reserve(n);
  QString vn = h.getParm("v_name");
  if( vn.isEmpty() )
    vn = h.objectName();
  vn += '[';

  int len_max { 4096 }; // not IMAX - simple string in array limit
  QString s_max = h.getParm( "max" );
  if( ! s_max.isEmpty() ) {
    len_max = s_max.toInt();
  }
  QString mask = h.getParm( "mask" );

  lbl->setText(""); // hack: hide common name
  auto lay = new QGridLayout( pwi );

  for( int i=0; i<n; ++i ) {
    auto la = new QLabel( pwi );
    la->setText( vn + QSN(i) + ']' );
    lay->addWidget( la, i, 0 );

    auto le = new QLineEdit( pwi );
    le->setReadOnly( ro );
    le->setMaxLength( len_max );
    if( !mask.isEmpty() ) {
      le->setInputMask( mask );
    }
    lay->addWidget( le, i, 1 );
    les.push_back( le );
  }

  lay->setContentsMargins( 0, 0, 0, 0 );
  pwi->setLayout( lay ); // ???
}

bool StringArrayDataWidget::set()
{
  int n = les.size();
  for( int i=0; i<n; ++i ) {
    les[i]->setText( ho.get(i).toString() );
  }
  return true;
}

bool StringArrayDataWidget::get() const
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

DataWidget* StringArrayDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new StringArrayDataWidget( h, parent );
}

int StringArrayDataWidget::reg()
{
  static DataWidgetProp p { create, "ARRAY_STRING,INLINE" };
  return FactoryDataWidget::theFactory().registerWidgetType( "StringArrayDataWidget", p );
}


// ------------------- ObjDataWidget ---------------------------
int ObjDataWidget::registered = ObjDataWidget::reg();

ObjDataWidget::ObjDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
   pb( new QPushButton( this ) )
{
  main_w = pb;
  if( h.getFlags() & ( efRO | efRODial ) ) {
    pb->setDisabled( true ); // TODO: real read-only
  }
  pb->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  updateLabel();
  connect( pb, &QPushButton::clicked, this, &ObjDataWidget::edit );

  auto lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( lbl );
  lay->addWidget( pb, 1 );
  setLayout( lay );
}

bool ObjDataWidget::set()
{
  // TODO:
  return true;
}

bool ObjDataWidget::get() const
{
  // TODO:
  // QVariant v = (cb->color()).rgba();
  // ho.set( v );
  return true;
}

void ObjDataWidget::updateLabel()
{
  QString btn_text = ho.dataObj( 0, Qt::StatusTipRole ).toString();
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
    updateLabel();
  } else {
    qWarning() << "Fail to convert holder " <<  ho.getFullName() << " to HolderData " << WHE;
  }
}

DataWidget* ObjDataWidget::create( HolderData &h, QWidget *parent  )
{
  return new ObjDataWidget( h, parent );
}

int ObjDataWidget::reg()
{
  static DataWidgetProp p { create, "OBJ" };
  return FactoryDataWidget::theFactory().registerWidgetType( "ObjDataWidget", p );
}



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
  QStringList ho_p_list = ho.getParm("props").split(",");

  for( auto i = propMap.begin(); i!= propMap.end(); ++i ) { // no : need key
    int good = 0;
    QStringList w_p_list = QString( i.value().eprop ).split(",");

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
    name = "DummyDataWidget";
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
  : QDialog( parent ), ds( a_ds)
{
  QString s = ds.getType()  %  ' ' %  ds.getFullName();
  if( ds.getModified() ) {
    s += " *";
  }
  setWindowTitle( s );
  createWidgets();
  getAll();

}

int DataDialog::getAll() // from object to wigets
{
  int ng = 0;

  for( auto w : dwm  ) {
    w->set();
    ++ng;
  }

  return ng;
}

int DataDialog::setAll() // from widgets to object
{
  int ns = 0;

  for( auto w : dwm ) {
    w->get();
    ++ns;
  }
  ds.post_set();
  // ds.setModified(); // TODO: check: must be set by low-level elements

  return ns;
}

void DataDialog::accept()
{
  setAll();
  QDialog::accept();
}

void DataDialog::checkData()
{
  // TODO:
}

void DataDialog::showHelp()
{
  // TODO: file
  return showSimpleHelp();
}


void DataDialog::showSimpleHelp()
{
  const char *help;
  if( (help = ds.getHelp()) == 0 )
    return;
  QString help_str( "<p><a name=\"help_head\"></a> </p><hr/>\n" );

  help_str += help;
  help_str += "<hr/>\n";

  for( auto ho : ds.TCHILD(HolderData*) ) {
    help_str += "<p>" % ho->getType()
      % " <b> " % ho->objectName() % " </b>; // "
      % ho->getParm("descr")  % " ("
      % ho->getParm("vis_name") % ")</p>\n";
  }

  auto dia = new QDialog( this );
  dia->setMinimumSize( 500, 480 );
  dia->setWindowTitle( PACKAGE ": Help on element" );

  auto lay = new QGridLayout;

  auto brow = new QTextBrowser( this );
  brow->insertHtml( help_str );
  lay->addWidget( brow, 0, 0 );

  auto bt_ok = new QPushButton( "&Ok", dia );
  lay->addWidget( bt_ok, 1, 0 );
  dia->setLayout( lay );

  connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );
  brow->scrollToAnchor( "help_head" );
  dia->exec();
  delete dia;

}

void DataDialog::addParam()
{
  QStringList prm_clss = EFACT.goodTypeNames( ds.allowTypes(), true, false ); // no_obj, data
  if( prm_clss.isEmpty() ) {
    return;
  }

  auto dia = new QDialog( this );
  auto lay = new QGridLayout( dia );

  auto lbl_type = new QLabel( "Type", dia );
  lay->addWidget( lbl_type, 0, 0 );
  auto lw = new QListWidget( dia );

  bool first_add = true;
  for( QString ptype : prm_clss ) {
    auto lwi = new QListWidgetItem( ptype );
    lw->addItem( lwi );
    if( first_add  ||  ptype == "double" ) {
      lw->setCurrentItem( lwi );
    }
    first_add = false;
  }
  lay->addWidget( lw, 1, 0, 3, 1 );

  auto lbl_name = new QLabel( "Name", dia );
  lay->addWidget( lbl_name, 0, 1 );
  auto ed_name = new QLineEdit( this );
  ed_name->setValidator( new QRegExpValidator( QRegExp(RE_NAME), this ) );
  lay->addWidget( ed_name, 1, 1 );

  auto lbl_val = new QLabel( "Value", dia );
  lay->addWidget( lbl_val, 2, 1 );
  auto ed_val = new QLineEdit( this );
  lay->addWidget( ed_val, 3, 1 );

  auto lbl_descr = new QLabel( "Description", dia );
  lay->addWidget( lbl_descr, 4, 0 );
  auto ed_descr = new QLineEdit( this );
  lay->addWidget( ed_descr, 5, 0, 1, 2 );

  auto lbl_vis_name = new QLabel( "Visual name", dia );
  lay->addWidget( lbl_vis_name, 6, 0 );
  auto ed_vis_name = new QLineEdit( this );
  lay->addWidget( ed_vis_name, 7, 0, 1, 2 );

  auto lbl_extra = new QLabel( "Extra", dia );
  lay->addWidget( lbl_extra, 8, 0 );
  auto ed_extra = new QLineEdit( this );
  lay->addWidget( ed_extra, 9, 0, 1, 2 );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dia );
  lay->addWidget( bbox, 10, 0, 1, 2 );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  QString nm = ed_name->text();
  QString val = ed_val->text();
  QString descr = ed_descr->text();
  QString vis_name = ed_vis_name->text();
  QString extra = ed_extra->text();
  QString ptype;
  if( lw->currentItem() ) {
    ptype = lw->currentItem()->text();
  }

  delete dia;
  if( rc != QDialog::Accepted || nm.isEmpty() || ptype.isEmpty() ) {
    return;
  }
  HolderData *ho = ds.add_param( ptype, nm );
  if( !ho ) {
    handleError( this, QSL("Fail to add parameter: ") + ptype + " " + nm );
    return;
  }

  ho->fromString( val );
  ho->setParm( "descr", descr );
  ho->setParm( "vis_name", vis_name );
  ho->setParm( "extra", extra );
  ho->extraToParm();

  createWidgets(); // recreate iface
  getAll();
  update();
}

void DataDialog::addObj()
{
  addElemInfo aei;
  aei.name = QString("obj_") + QSN( ds.size() ) ;
  aei.order = 0;
  auto dia = new AddElemDialog( &aei, &ds, this );

  int rc = dia->exec();
  delete dia; dia = 0;

  if( rc != QDialog::Accepted || aei.type.isEmpty() ) {
    return;
  }

  if( ! isGoodName( aei.name )  ) {
    handleError( this, QSL( "Fail to add Elem: bad object name \"") + aei.name + "\"" );
    return;
  }

  const TClassInfo *ci = EFACT.getInfo( aei.type );
  if( !ci ) {
    handleError( this, QSL( "Fail to add Elem: class \"") + aei.type + "\" not found" );
    return;
  }

  HolderData *ob = ds.add_obj( aei.type, aei.name );
  if( !ob  ) {
    handleError( this, QSL( "Fail to add Elem: ") + aei.type + " " + aei.name );
    return;
  }

  createWidgets(); // recreate iface
  getAll();
  update();
}

void DataDialog::delSome( bool is_obj )
{
  QStringList sl;
  for( auto ho :  ds.TCHILD(HolderData*) ) {
    if( ! ho->isDyn() ) {
      continue;
    }
    if( (is_obj ^ ho->isObject()) ) {
      continue;
    }
    sl << ho->objectName();
  }
  if( sl.isEmpty() ) {
    return;
  }

  auto dia = new QDialog( this );
  auto lay = new QVBoxLayout( dia );

  auto la = new QLabel( "Delete param", dia );
  lay->addWidget( la );

  auto lw = new QListWidget( dia );
  for( QString ob_name : sl ) {
    lw->addItem( ob_name );
  };
  lay->addWidget( lw );

  auto bbox
    = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  lay->addWidget( bbox );
  connect( bbox, &QDialogButtonBox::accepted, dia, &QDialog::accept );
  connect( bbox, &QDialogButtonBox::rejected, dia, &QDialog::reject );

  int rc = dia->exec();
  QString ob_name;
  if( lw->currentItem() ) {
    ob_name = lw->currentItem()->text();
  }
  delete dia;

  if( rc != QDialog::Accepted || ob_name.isEmpty() ) {
    return;
  }

  ds.del_obj( ob_name );
  createWidgets();
  getAll();
  update();
}

void DataDialog::delParam()
{
  delSome( false );
}

void DataDialog::delObj()
{
  delSome( true );
}

int DataDialog::createWidgets()
{
  int nr = 0, nc = 0, nr_max = 0, nr_block = 0;
  int was_block = 0, was_col = 0;

  // remove existent- if recreate need
  for( QObject *child : children() ) {
    delete child;
  }
  dwm.clear();

  auto lay1 = new QVBoxLayout;

  auto lay2 = new QGridLayout;
  lay1->addLayout( lay2 );

  DataWidget *w;

  for( auto ho :  ds.TCHILD(HolderData*) ) {
    if( ho->getFlags() & efNoDial ) {
      continue;
    }

    int ncol = 1; // number of columns per widget
    QString ncol_str = ho->getParm("ncol");
    if( ! ncol_str.isEmpty() ) {
      ncol = ncol_str.toInt();
      ncol = qBound( -1, ncol, 20 );
    }

    if( ho->getParm("sep") == "col" || was_col ) {
      nr = nr_block; ++nc;
    }

    if( ho->getParm("sep") == "block" || was_block) {
      auto fr = new QFrame( this );
      fr->setFrameStyle( QFrame::HLine );

      lay2->addWidget( fr, nr_max, 0, 1, -1 );
      ++nr_max;
      nr_block = nr_max; nr = nr_block; nc = 0;
    }

    QString name = ho->objectName();

    w = FactoryDataWidget::theFactory().createDataWidget( *ho, this );
    if( !w ) {
      qWarning() << "not found edit widget for object " << name << WHE;
      continue;
    }

    dwm[name] = w;
    QString whats = ho->dataObj( 0, Qt::WhatsThisRole ).toString();
    w->setWhatsThis( whats );
    // w->setStatusTip( whats ); // TODO: just for test
    // w->setToolTip( whats ); // TODO: just for test
    lay2->addWidget( w, nr, nc, 1, ncol );

    ++nr;
    if( nr > nr_max ) {
      nr_max = nr;
    }

    was_col = was_block = 0;
    if( ho->getParm("sep") == "colend" ||  nr >= 20 ) { // 20: max elems in column
      was_col = 1;
    }
    if( ho->getParm("sep") == "blockend" ) {
      was_block = 1;
    }
  } // -------------- end item looo

  // final line and buttons
  auto frb = new QFrame( this );
  frb->setFrameStyle( QFrame::HLine );
  lay1->addWidget( frb );

  auto lay_btn2 = new QHBoxLayout;

  auto btn_addParam = new QPushButton( "Add parameter" );
  connect( btn_addParam, &QPushButton::clicked, this, &DataDialog::addParam );
  lay_btn2->addWidget( btn_addParam );


  bool can_add_params = false;
  QStringList prm_clss = EFACT.goodTypeNames( ds.allowTypes(), true, false ); // no_obj, data
  if( ! prm_clss.isEmpty() ) {
    can_add_params = true;
  }
  if( !can_add_params ) {
     btn_addParam->setEnabled( false );
  }

  bool can_add_objs = false;
  QStringList obj_clss = EFACT.goodTypeNames( ds.allowTypes(), false, true ); // obj, no_data
  if( ! obj_clss.isEmpty() ) {
    can_add_objs = true;
  }

  auto btn_addObj = new QPushButton( "Add object" );
  connect( btn_addObj, &QPushButton::clicked, this, &DataDialog::addObj);
  lay_btn2->addWidget( btn_addObj );
  if( ! can_add_objs ) {
    btn_addObj->setEnabled( false );
  }

  auto btn_delParam = new QPushButton( "Delete param" );
  connect( btn_delParam, &QPushButton::clicked, this, &DataDialog::delParam );
  lay_btn2->addWidget( btn_delParam );
  if( !can_add_params ) {
    btn_delParam->setEnabled( false );
  }
  auto btn_delObj = new QPushButton( "Delete object" );
  connect( btn_delObj, &QPushButton::clicked, this, &DataDialog::delObj);
  lay_btn2->addWidget( btn_delObj );
  if( ! can_add_objs ) {
    btn_delObj->setEnabled( false );
  }
  lay1->addLayout( lay_btn2 );

  auto lay_btn = new QHBoxLayout;
  auto btn_ok = new QPushButton( QIcon::fromTheme("dialog-ok"),"Ok" );
  btn_ok->setDefault( true );
  connect( btn_ok, &QPushButton::clicked, this, &DataDialog::accept);
  lay_btn->addWidget( btn_ok );
  //
  auto btn_cancel = new QPushButton( QIcon::fromTheme("dialog-cancel"), "Cancel" );
  connect( btn_cancel, &QPushButton::clicked, this, &DataDialog::reject);
  lay_btn->addWidget( btn_cancel );
  auto btn_check = new QPushButton( "Check?" );
  connect( btn_check, &QPushButton::clicked, this, &DataDialog::checkData); // TODO:
  lay_btn->addWidget( btn_check );
  auto btn_help = new QPushButton( QIcon::fromTheme("help-contents"), "Help" );
  connect( btn_help, &QPushButton::clicked, this, &DataDialog::showHelp);
  lay_btn->addWidget( btn_help );
  lay1->addLayout( lay_btn );

  setLayout( lay1 );

  return nr;
}

