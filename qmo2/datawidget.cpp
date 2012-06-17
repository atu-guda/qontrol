/***************************************************************************
                          datawidget.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "datawidget.h"

DataWidget::DataWidget( HolderData &h, QWidget *parent )
  : QWidget( parent ), ho( h )
{
}

// -------------  DummyDataWidget ----------------

int DummyDataWidget::registered = DummyDataWidget::reg();

DummyDataWidget::DummyDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  lbl( new QLabel( this ) )
{
}

bool DummyDataWidget::set()
{
  lbl->setText( ho.toString() );
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

QSize DummyDataWidget::minimumSizeHint() const
{
  return lbl->minimumSizeHint();
}


QSize DummyDataWidget::sizeHint() const
{
  return lbl->sizeHint();
}


// ------------------- StringDataWidget ---------------------------
int StringDataWidget::registered = StringDataWidget::reg();

StringDataWidget::StringDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }
  le->setMaxLength( h.getMax() );
  QString mask = h.getParm( "mask" );
  if( ! mask.isEmpty() ) {
    le->setInputMask( mask );
  }
  // TODO: r/o, size, mask ....
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

QSize StringDataWidget::minimumSizeHint() const
{
  return le->minimumSizeHint();
}


QSize StringDataWidget::sizeHint() const
{
  return le->sizeHint();
}



// ------------------- StringMLDataWidget ---------------------------
int StringMLDataWidget::registered = StringMLDataWidget::reg();

StringMLDataWidget::StringMLDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  te( new QTextEdit( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    te->setReadOnly( true );
  }
  // setMaxLength( h.getMax() ); ???
  // TODO: min size
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
  static DataWidgetProp p { create, "STRING,ML" };
  return FactoryDataWidget::theFactory().registerWidgetType( "StringMLDataWidget", p );
}

QSize StringMLDataWidget::minimumSizeHint() const
{
  return te->minimumSizeHint();
}


QSize StringMLDataWidget::sizeHint() const
{
  return te->sizeHint();
}



// ------------------- IntDataWidget ---------------------------
int IntDataWidget::registered = IntDataWidget::reg();

IntDataWidget::IntDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }
  le->setValidator( new QIntValidator( h.getMin(), h.getMax(), le ) );
  // TODO: r/o
}

bool IntDataWidget::set()
{
  le->setText( QString::number( ho.get().toInt() ) ); 
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

QSize IntDataWidget::minimumSizeHint() const
{
  return le->minimumSizeHint();
}


QSize IntDataWidget::sizeHint() const
{
  return le->sizeHint();
}


// ------------------- IntSpinDataWidget ---------------------------
int IntSpinDataWidget::registered = IntSpinDataWidget::reg();

IntSpinDataWidget::IntSpinDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  sb( new QSpinBox( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    sb->setReadOnly( true );
  }
  sb->setRange( h.getMin(), h.getMax() );
  QString prefix = h.getParm( "prefix" );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }
  bool ok;
  int step = h.getParm( "step" ).toInt( &ok, 0 );
  if( ok ) {
    sb->setSingleStep( step );
  }
  
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

QSize IntSpinDataWidget::minimumSizeHint() const
{
  return sb->minimumSizeHint();
}


QSize IntSpinDataWidget::sizeHint() const
{
  return sb->sizeHint();
}



// ------------------- SwitchDataWidget ---------------------------
int SwitchDataWidget::registered = SwitchDataWidget::reg();

SwitchDataWidget::SwitchDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  cb( new QCheckBox( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true );
  }
  
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

QSize SwitchDataWidget::minimumSizeHint() const
{
  return cb->minimumSizeHint();
}


QSize SwitchDataWidget::sizeHint() const
{
  return cb->sizeHint();
}


// ------------------- ListDataWidget ---------------------------
int ListDataWidget::registered = ListDataWidget::reg();

ListDataWidget::ListDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  cb( new QComboBox( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true );
  }
  cb->addItems( ho.getElems() );
  
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

QSize ListDataWidget::minimumSizeHint() const
{
  return cb->minimumSizeHint();
}


QSize ListDataWidget::sizeHint() const
{
  return cb->sizeHint();
}



// ------------------- DoubleDataWidget ---------------------------
int DoubleDataWidget::registered = DoubleDataWidget::reg();

DoubleDataWidget::DoubleDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
  le( new QLineEdit ( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    le->setReadOnly( true );
  }
  le->setValidator( new QDoubleValidator( h.getMin(), h.getMax(), 12, le ) );
  //setNotation( ScientificNotation );  // is by default
  // TODO: notation, precision
}

bool DoubleDataWidget::set()
{
  le->setText( QString::number( ho.get().toDouble() ) ); 
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

QSize DoubleDataWidget::minimumSizeHint() const
{
  return le->minimumSizeHint();
}


QSize DoubleDataWidget::sizeHint() const
{
  return le->sizeHint();
}


// ------------------- DoubleSpinDataWidget ---------------------------
int DoubleSpinDataWidget::registered = DoubleSpinDataWidget::reg();

DoubleSpinDataWidget::DoubleSpinDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
   sb( new QDoubleSpinBox( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    sb->setReadOnly( true );
  }
  sb->setRange( h.getMin(), h.getMax() );
  QString prefix = h.getParm( "prefix" );
  if( ! prefix.isEmpty() ) {
    sb->setPrefix( prefix );
  }
  bool ok;
  double step = h.getParm( "step" ).toDouble( &ok );
  if( ok ) {
    sb->setSingleStep( step );
  }
  
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

QSize DoubleSpinDataWidget::minimumSizeHint() const
{
  return sb->minimumSizeHint();
}


QSize DoubleSpinDataWidget::sizeHint() const
{
  return sb->sizeHint();
}


// ------------------- ColorDataWidget ---------------------------
int ColorDataWidget::registered = ColorDataWidget::reg();

ColorDataWidget::ColorDataWidget( HolderData &h, QWidget *parent )
  : DataWidget( h, parent ),
   cb( new QColorBtn( this ) )
{
  if( h.getFlags() & ( efRO | efRODial ) ) {
    cb->setDisabled( true ); // TODO: real read-only
  }
}

bool ColorDataWidget::set()
{
  cb->setColor( ho.get().toInt() ); // Int to Color inside button
  return true;
}

bool ColorDataWidget::get() const
{
  QVariant v = (cb->color()).rgba();
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

QSize ColorDataWidget::minimumSizeHint() const
{
  return cb->minimumSizeHint();
}


QSize ColorDataWidget::sizeHint() const
{
  return cb->sizeHint();
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
  static QString name;
  int max_good = IMIN;
  QStringList ho_p_list = ho.getProps().split(",");
  DwPropMap::const_iterator i;
  for( i = propMap.begin(); i!= propMap.end(); ++i ) {
    int good = 0;
    QStringList w_p_list = QString( i.value().eprop ).split(",");

    for( int i=0; i<ho_p_list.size(); ++i ) {
      if( w_p_list.contains( ho_p_list.at(i) ) ) {
	good += 10;
      }
    }

    for( int i=0; i<w_p_list.size(); ++i ) {
      if( ! ho_p_list.contains( w_p_list.at(i) ) ) {
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

  if( lev != 0 )
    *lev = max_good;

  return name;
}

DataWidget* FactoryDataWidget::createDataWidget( HolderData &ho, QWidget *parent ) const
{
  QString name = findForHolder( ho );
  if( name.isEmpty() ) {
    return 0;
  }
  DwPropMap::const_iterator i = propMap.find( name );
  if( i == propMap.end() )
    return 0;
  return i.value().creator( ho, parent );
}

bool FactoryDataWidget::registerWidgetType( const QString &wname, 
    const DataWidgetProp &prop )
{
  if( propMap.contains( wname ) )
    return false;
  propMap.insert( wname, prop );
  // updateHints();
  return true;
}

bool FactoryDataWidget::unregisterWidgetType( const QString &wname )
{
  DwPropMap::iterator i = propMap.find( wname );
  if( i == propMap.end() )
    return false;
  propMap.erase( i );
  // updateHints();
  return true;
}

// ======================= DataDialog ===========================

DataDialog::DataDialog( TDataSet &a_ds, QWidget *parent )
  : QDialog( parent ), ds( a_ds) 
{
  QString s = L8B( ds.getClassName() );
  s += ' ';
  char nm[MAX_INPUTLEN];
  ds.getFullName( nm );
  s += L8B( nm );
  setWindowTitle( s );
  createWidgets();
  getAll();
  
}

int DataDialog::getAll()
{
  int ng = 0;
  DataWidget *w;

  for(DaWiMap::const_iterator i = dwm.begin(); i != dwm.end(); ++i  ) {
    w = i.value();
    w->set();
    ++ng;
  }

  return ng;
}

int DataDialog::setAll()
{
  int ns = 0;
  DataWidget *w;

  for(DaWiMap::const_iterator i = dwm.begin(); i != dwm.end(); ++i  ) {
    w = i.value();
    w->get();
    ++ns;
  }

  return ns;

}

void DataDialog::accept()
{
  setAll();
  QDialog::accept();
}

int DataDialog::createWidgets()
{
  int nw = 0;
  QVBoxLayout *lay1 = new QVBoxLayout;
  
  QWidget *ce = new QWidget( this );
  QScrollArea *sa = new QScrollArea( this );
  sa->setWidget( ce );
  lay1->addWidget( sa );
  ce->setMinimumSize( 40, 50 );

  QDialogButtonBox *bBox = 
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
	Qt::Horizontal, this );
  connect(bBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bBox, SIGNAL(rejected()), this, SLOT(reject()));
  lay1->addWidget( bBox );
  
  QGridLayout *lay2 = new QGridLayout( ce );
  ce->setLayout( lay2 );

  DataWidget *w;
  QObjectList childs = ds.children();
  for( QObjectList::iterator o = childs.begin(); o != childs.end(); ++o ) {
    QObject *xo = *o;
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    if( !ho )
      continue; // but how?
    if( ho->getFlags() & efNoDial )
      continue;
    QString name = ho->objectName();
    QString visName = ho->getVisName();
    int lev;
    QString wtp = FactoryDataWidget::theFactory().findForHolder( *ho, &lev );
    wtp += '_';
    wtp += QString::number( lev );
    w = FactoryDataWidget::theFactory().createDataWidget( *ho, ce );
    if( !w ) {
      qDebug( "not found edit widget for object %s", name.toLocal8Bit().constData() );
      continue;
    }
    
    dwm[name] = w;
    w->setWhatsThis( ho->getDescr() );
    QLabel *la = new QLabel( visName, ce );
    la->setWhatsThis( ho->getType() + " " + name + " (" + wtp + ")" );
    lay2->addWidget( la, nw, 0 );
    lay2->addWidget( w, nw, 1 );
    // lay2->addWidget( new QLabel( wtp, ce ), nw, 2 );
    ++nw;
  }

  setLayout( lay1 );

  QSize sz2 = lay2->sizeHint();
  
  ce->setMinimumSize( sz2 );

  return nw;
}
