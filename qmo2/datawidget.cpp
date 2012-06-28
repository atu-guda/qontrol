/***************************************************************************
                          datawidget.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "datawidget.h"
#include "qmo2win.h"

DataWidget::DataWidget( HolderData &h, QWidget *parent )
  : QFrame( parent ), ho( h )
{
  // setFrameStyle( QFrame::Panel | QFrame::Sunken );
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
  lbl->setText( ho.toString().left(20) );
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
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( le, 1 );
  setLayout( lay );
  // TODO: r/o, size, mask ....
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

QSize StringDataWidget::minimumSizeHint() const
{
  return le->minimumSizeHint();
}


QSize StringDataWidget::sizeHint() const
{
  // return le->sizeHint();
  return QSize( 220, 20 );
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
  te->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Preferred );
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
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
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( le, 1 );
  setLayout( lay );
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

  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
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
  
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
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
  
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
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
  
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( le, 1 );
  setLayout( lay );
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
  
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
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
  cb->setSizePolicy( QSizePolicy::Expanding,  QSizePolicy::Expanding );
  
  QHBoxLayout *lay =  new QHBoxLayout( this );
  lay->setContentsMargins( 0, 0, 0, 0 );
  lay->addWidget( cb, 1 );
  setLayout( lay );
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
  QString name;
  int max_good = IMIN;
  QStringList ho_p_list = ho.getParm("props").split(",");

  for( auto i = propMap.begin(); i!= propMap.end(); ++i ) {
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
  char nm[MAX_INPUTLEN];
  ds.getFullName( nm );
  QString s = L8B( ds.getClassName() )  %  ' '  %  L8B( nm );
  setWindowTitle( s );
  createWidgets();
  getAll();
  
}

int DataDialog::getAll()
{
  int ng = 0;

  for( auto w : dwm  ) {
    w->set();
    ++ng;
  }

  return ng;
}

int DataDialog::setAll()
{
  int ns = 0;

  for( auto w : dwm ) {
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

void DataDialog::checkData()
{
  // TODO: 
}

void DataDialog::showHelp()
{
  QString cl_name = L8B( ds.getClassName() );
  if( cl_name.isEmpty() )
    return showSimpleHelp();
  QString resname = L8B( "elems/" );
  resname += cl_name;
  resname += L8B( "/index.html" );
  QString helpfile = QMo2Win::qmo2win->findRes( resname );
  // TODO: rich browser here or in next function
  //if( helpfile.isEmpty() )  
  //  return showSimpleHelp();
  return showSimpleHelp();
}


void DataDialog::showSimpleHelp(void)
{
  QDialog *dia;
  QTextBrowser *brow;
  QPushButton *bt_ok;
  QGridLayout *lay;
  const char *help;
  if( (help = ds.getHelp()) == 0 )
    return;
  dia = new QDialog( this );
  dia->resize( 500, 480 );
  dia->setWindowTitle( PACKAGE ": Help on element" );
  
  lay = new QGridLayout;
  
  brow = new QTextBrowser( this );
  brow->insertHtml( help );
  lay->addWidget( brow, 0, 0 );

  bt_ok = new QPushButton( "&Ok", dia );
  lay->addWidget( bt_ok, 1, 0 );
  dia->setLayout( lay );

  connect( bt_ok, SIGNAL(clicked()), dia, SLOT(accept()) );
  dia->exec();
  delete dia;

}
int DataDialog::createWidgets()
{
  int nr = 0, nc = 0, nr_max = 0, nr_block = 0;
  int was_block = 0, was_col = 0;
  QVBoxLayout *lay1 = new QVBoxLayout;
  
  QGridLayout *lay2 = new QGridLayout;
  lay1->addLayout( lay2 );

  DataWidget *w;
  QObjectList childs = ds.children();
  // for( QObjectList::iterator o = childs.begin(); o != childs.end(); ++o ) {
  for( auto o :  childs ) {
    QObject *xo = o;
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    if( !ho )
      continue; // but how?
    if( ho->getFlags() & efNoDial )
      continue;

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
      QFrame *fr = new QFrame( this );
      fr->setFrameStyle( QFrame::HLine );

      lay2->addWidget( fr, nr_max, 0, 1, -1 );
      ++nr_max;
      nr = nr_max;
      nr_block = nr_max; nr = nr_block; nc = 0;
    }

    QString name = ho->objectName();
    QString visName = ho->getParm( "vis_name" );
    //qDebug( "DBG: createWidgets %s at (%d,%d+%d)", 
    //	    qPrintable(name), nr, nc, ncol  );

    int lev = 0;
    QString wtp = 
      " ( " % QString::number( ho->getMin() ) % " ; " % QString::number( ho->getMax() ) % " ) " 
      % FactoryDataWidget::theFactory().findForHolder( *ho, &lev )
      % '_'   % QString::number( lev ) % ' ' %  ho->getParm( "props" ) ;
    w = FactoryDataWidget::theFactory().createDataWidget( *ho, this );
    if( !w ) {
      qDebug( "not found edit widget for object %s", qPrintable(name) );
      continue;
    }
    
    dwm[name] = w;
    w->setWhatsThis( ho->getParm("descr") );
    QLabel *la = new QLabel( visName, this );
    la->setWhatsThis( ho->getType() + " " + name + " (" + wtp + ")" );
    lay2->addWidget( la, nr, nc*2 );
    lay2->addWidget( w, nr, nc*2+1, 1, ncol );
    ++nr;
    if( nr > nr_max )
      nr_max = nr;
    
    was_col = was_block = 0;
    if( ho->getParm("sep") == "colend" ||  nr >= 40 ) {
      was_col = 1;
    }
    if( ho->getParm("sep") == "blockend" ) {
      was_block = 1;
    }
  } // -------------- end item looo

  // final line and buttons
  QFrame *frb = new QFrame( this );
  frb->setFrameStyle( QFrame::HLine );
  lay1->addWidget( frb );

  QHBoxLayout *lay_btn = new QHBoxLayout;
  QPushButton *btn_ok = new QPushButton( QIcon::fromTheme("dialog-ok"),"Ok" );
  btn_ok->setDefault( true );
  connect(btn_ok, SIGNAL(clicked()), this, SLOT(accept()));
  lay_btn->addWidget( btn_ok );
  // 
  QPushButton *btn_cancel = new QPushButton( QIcon::fromTheme("dialog-cancel"), "Cancel" );
  connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
  lay_btn->addWidget( btn_cancel );
  QPushButton *btn_check = new QPushButton( "Check?" );
  connect(btn_check, SIGNAL(clicked()), this, SLOT(checkData())); // TODO:
  lay_btn->addWidget( btn_check );
  QPushButton *btn_help = new QPushButton( QIcon::fromTheme("help-contents"), "Help" );
   connect(btn_help, SIGNAL(clicked()), this, SLOT(showHelp()));
  lay_btn->addWidget( btn_help );
  lay1->addLayout( lay_btn );


  setLayout( lay1 );

  return nr;
}

