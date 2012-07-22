/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <limits>
#include <QColor>
#include "miscfun.h"
#include "dataset.h"

using namespace std;


// ================================================================
// ---------------- HolderData .... ----------------------

HolderData::HolderData( const QString &obj_name, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
           :QObject( a_parent ), old_tp(0),
	    dyn(0), flags(a_flags),
	    v_min(DMIN), v_max(DMAX),
	    tp(QVariant::Invalid), ptr(0), par(a_parent), 
	    target_name( obj_name )
{
  setObjectName( "_HO_" + obj_name );
  par->registerHolder( this );

  if( v_name.isNull() )  {
    setParm( "vis_name", obj_name );
  } else {
    setParm( "vis_name", v_name );
  }
  setParm( "descr", a_descr );
  setParm( "extra", a_extra );
  extraToParm();
}

HolderData::~HolderData()
{
  par->unregisterHolder( this );
}

void HolderData::setParm( const QString &name, const QString &value )
{
  parms[name] = value;
  if( name == "min" ) { // may be not used - only string?
    v_min = value.toDouble();
  } else if ( name == "max" ) {
    v_max = value.toDouble();
  }
}


QString HolderData::getParm( const QString &name ) const
{
  if( parms.contains( name ) )
    return parms[name];
  return QString();
}

bool HolderData::isObject( const QString & /*cl_name*/  ) const
{
  return false;
}

void HolderData::extraToParm()
{
  QRegExp re( "^([_a-zA-Z][_a-zA-Z0-9]*)\\s*=\\s*(\\S+)$" );
  QStringList el = getParm("extra").split("\n");
  for( QString &s : el ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      setParm( nm, val );
    } else {
      qDebug( "warn: bad extra string part: \"%s\"", qPrintable( s ) );
    }
  }
  if( getParm("dyn") == "1" )
    dyn = 1;
}

void HolderData::setElems( const QString &els )
{
  parms["list_elems"] = els;
  elems = els.split("\n");
  v_max = elems.size() - 1;
}

QString HolderData::getType() const // = 0;
{
  return "None";
}

QDomElement HolderData::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "param" );
  de.setAttribute( "name", targetName() );
  if( dyn ) {
    de.setAttribute( "dyn", "1" );
    de.setAttribute( "otype", getType() );
  }
  QDomText tn = dd.createTextNode( toString() );
  de.appendChild( tn );
  return de;
}


// ---------------- HolderInt ---------

HolderInt::HolderInt( int *p, const QString &obj_name, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new int; *val = (int)(v_min); dyn = 1;
  }
  ptr = val; tp=QVariant::Int; old_tp = dtpInt;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "INT,SIMPLE" );
  }
  post_set();
}

HolderInt::~HolderInt()
{
  if( dyn )
    delete val;
  val = 0; ptr = 0; dyn = 0;
}

bool HolderInt::set( const QVariant & x )
{
  bool ok;
  *val = x.toInt( &ok );
  post_set();
  return ok;
}

QVariant HolderInt::get() const
{
  return QVariant( *val );
}

void HolderInt::post_set()
{
  int imin = ( v_min < (double)(IMIN) ) ? IMIN : (int)(v_min);
  int imax = ( v_max > (double)(IMAX) ) ? IMAX : (int)(v_max);
  *val = qBound( imin, *val, imax );
}

QString HolderInt::toString() const
{
  return QString::number( *val );
}

bool HolderInt::fromString( const QString &s )
{
  bool ok;
  int v;
  v = s.toInt( &ok, 0 ); // 0 = auto base
  if( ok ) {
    *val = v;
    post_set();
  }
  return ok;
}


QString HolderInt::getType() const
{
  return "int";
}

// ---------------- HolderSwitch ---------
HolderSwitch::HolderSwitch( int *p, const QString &obj_name, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,SWITCH" );
  }
  v_min = 0; v_max = 1;
  post_set();
}

HolderSwitch::~HolderSwitch()
{
  // NOP
}


void HolderSwitch::post_set()
{
  *val = (*val) ? 1 : 0;
}

QString HolderSwitch::getType() const
{
  return "switch";
}


// ---------------- HolderList ---------
HolderList::HolderList( int *p, const QString &obj_name, 
     const QString &v_name, TDataSet *a_parent, int a_flags,
     const QString &a_descr,
     const QString &a_extra,  const QString &a_elems  )
   :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  v_min = v_max = 0;
  setElems( a_elems );
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,LIST" );
  }
  post_set();
}

HolderList::~HolderList()
{
  // NOP
}


QString HolderList::getType() const
{
  return "list";
}



// ---------------- HolderDouble ---------

HolderDouble::HolderDouble( double *p, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new double; *val = v_min; dyn = 1;
  }
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "DOUBLE,SIMPLE" );
  }
  ptr = val; tp=QVariant::Double; old_tp = dtpDouble;
}

HolderDouble::~HolderDouble()
{
  if( dyn )
    delete val;
  val = 0; ptr = val; dyn = 0;
}

bool HolderDouble::set( const QVariant & x )
{
  bool ok;
  *val = x.toDouble( &ok );
  post_set();
  return ok;
}

QVariant HolderDouble::get() const
{
  return QVariant( *val );
}

void HolderDouble::post_set()
{
  *val = qBound( v_min, *val, v_max );
}

QString HolderDouble::toString() const
{
  return QString::number( *val, 'g', 16 );
}

bool HolderDouble::fromString( const QString &s )
{
  bool ok;
  double v;
  v = s.toDouble( &ok ); 
  if( ok ) {
    *val = v;
    post_set();
  }
  return ok;
}


QString HolderDouble::getType() const
{
  return "double";
}


// ---------------- HolderString ---------

HolderString::HolderString( QString *p, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new QString; dyn = 1;
  }
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "STRING,SIMPLE" );
  }
  if( v_min < 0 ) {
    v_min = 0;
  }
  if( v_max > 100000 ) {
    v_max = 100000;
  }
  ptr = val; tp=QVariant::String; old_tp = dtpString;
}

HolderString::~HolderString()
{
  if( dyn )
    delete val;
  val = 0; ptr = val; dyn = 0;
}

bool HolderString::set( const QVariant & x )
{
  *val = x.toString();
  post_set();
  return true;
}

QVariant HolderString::get() const
{
  return QVariant( *val );
}

void HolderString::post_set()
{
  val->truncate( (int)(v_max) );
}

QString HolderString::toString() const
{
  return *val;
}

bool HolderString::fromString( const QString &s )
{
  *val = s; 
  post_set();
  return true;
}


QString HolderString::getType() const
{
  return "string";
}

// ---------------- HolderStringArr ---------

HolderStringArr::HolderStringArr( QString *p, int an, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   n(an), val(p)
{
  if( n < 1 )
    n = 1;
  if( !val ) {
    val = new QString [n]; dyn = 1;
  }
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "STRINGARR" );
  }
  if( v_min < 0 ) {
    v_min = 0;
  }
  if( v_max > 100000 ) {
    v_max = 100000;
  }
  ptr = val; tp=QVariant::StringList; old_tp = dtpStringArr;
}

HolderStringArr::~HolderStringArr()
{
  if( dyn )
    delete[] val;
  val = 0; ptr = val; dyn = 0; n = 0;
}

bool HolderStringArr::set( const QVariant & x )
{
  QStringList sl = x.toStringList();
  for( int i=0; i<n; ++i ) {
    val[i] = sl[i];
  }
  post_set();
  return true;
}

QVariant HolderStringArr::get() const
{
  QStringList sl;
  for( int i=0; i<n; ++i ) {
    sl << val[i];
  }
  return QVariant( sl );
}

void HolderStringArr::post_set()
{
  for( int i=0; i<n; ++i ) {
    val[i].truncate( (int)(v_max) );
  }
}

QString HolderStringArr::toString() const
{
  QString s;
  for( int i=0; i<n; ++i ) {
    QString t = val[i];
    t.replace( "\\", "\\\\" );
    t.replace( "\"", "\\\"" );
    s += "\"" + t + "\",";
  }
  return s;
}

bool HolderStringArr::fromString( const QString &s )
{
  QStringList sl = s.split("\",\""); // TODO: overbad!
  for( int i=0; i<n; ++i ) {
    val[i] = sl[i];
  }
  post_set();
  return true;
}


QString HolderStringArr::getType() const
{
  return "string[]";
}

// ---------------- HolderColor ---------

HolderColor::HolderColor( QColor *p, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new QColor( Qt::red ); dyn = 1;
  }
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "COLOR,INT" );
  }
  ptr = val; tp=QVariant::Color; old_tp = dtpInt;
}

HolderColor::~HolderColor()
{
  if( dyn )
    delete val;
  val = 0; ptr = val;
}

bool HolderColor::set( const QVariant & x )
{
  QRgb rgba = x.toInt();
  val->setRgba( rgba );
  post_set();
  return true;
}

QVariant HolderColor::get() const
{
  return QVariant( (int)(val->rgba()) );
}

void HolderColor::post_set()
{
  if( !val->isValid() )
    *val = QColor(Qt::red);
}

QString HolderColor::toString() const
{
  return QString::number( (int)(val->rgba()) );
}

bool HolderColor::fromString( const QString &s )
{
  QRgb rgba = s.toInt();
  val->setRgba( rgba );
  post_set();
  return true;
}


QString HolderColor::getType() const
{
  return "color";
}


// ---------------- HolderObj ---------

HolderObj::HolderObj( TDataSet *p, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   obj(p)
{
  // no create? what to do if p == 0 ?
  if( !p ) {
    qDebug( "ERR: *** HolderObj::HolderObj p = 0 obj_name=%s !",
	qPrintable( obj_name ) );
  }
  // post_set();
  ptr = obj; tp=QVariant::UserType; old_tp = dtpObj; 
  if( getParm("props").isEmpty() ) {
    setParm( "props", "OBJ" );
  }
  obj->setObjectName( obj_name );
  obj->holderOfMe = this;
}

HolderObj::~HolderObj()
{
  if( dyn )
    delete obj;
  obj = 0; ptr = 0;
}

bool HolderObj::isObject( const QString &cl_name  ) const
{
  if( cl_name.isEmpty() ) // any object
    return true;
  const QMetaObject *mob = obj->metaObject();
  while( mob ) {
    if( cl_name == mob->className() )
      return true;
    mob = mob->superClass(); 
  }
  return false;
}

bool HolderObj::set( const QVariant & x )
{
  return obj->set( x );
}

QVariant HolderObj::get() const
{
  return obj->get();
}

void HolderObj::post_set()
{
  obj->post_set();
}

QString HolderObj::toString() const
{
  return obj->toString();
}

bool HolderObj::fromString( const QString &s )
{
  return obj->fromString( s );
}

QString HolderObj::getType() const
{
  return L8B( obj->getClassName() );
}

QDomElement HolderObj::toDom( QDomDocument &dd ) const
{
  return obj->toDom( dd );
}


// ---------------- TDataSet ------------------------

TClassInfo TDataSet::class_info = 
 {  CLASS_ID_TDataSet, "TDataSet",  TDataSet::create, 0, helpstr, 0 };


const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

TDataSet::TDataSet( TDataSet* aparent )
         :QObject( aparent ),
	  guard(guard_val), par(aparent), holderOfMe(nullptr),
	  state(stateGood), modified(0)
{
}

TDataSet::~TDataSet()
{
  // deletion is done by holders dtors
  state = stateBad;
}

const char* TDataSet::getClassName(void) const 
{ 
  return metaObject()->className();
}

const TClassInfo* TDataSet::getClassInfo(void) const
{
  return &class_info;
}

QString TDataSet::getFullName() const
{
  const TDataSet *cob;
  QString res = objectName();
  QString tn;
  cob = par; // TODO? is first dot need 
  while( cob != 0 ) {
    tn = cob->objectName();
    tn += '.';
    res.prepend( tn );
    cob = cob->par;
  };
  return res;
}

int TDataSet::getNumObj() const
{
  return ho_vect.size();
}

QStringList TDataSet::elemNames() const
{
  return ho_map.keys();
}

QVector<HolderData*> TDataSet::holders() const
{
  return ho_vect;
}

HolderData* TDataSet::getHolder( int i ) const
{
  if( i < 0 || i >= ho_vect.size() ) {
    qDebug( "ERR: TDataSet::getHolder(int i): %s i=%d out of range",
	qPrintable( getFullName() ), i );
    return nullptr;
  }
  return ho_vect[i];
}

HolderData* TDataSet::getHolder( const QString &oname ) const
{
  auto i = ho_map.find( oname );
  if( i == ho_map.end() )
    return nullptr;
  return i.value();
}

int TDataSet::indexOfHolder( HolderData *ho ) const
{
  return ho_vect.indexOf( ho );
}

bool TDataSet::registerHolder( HolderData *ho )
{
  if( !ho )
    return false;
  if( ho_map.contains( ho->targetName() ) ) {
    qDebug( "ERR: TDataSet::registerHolder: fail to add %s to %s: exists",
            qPrintable( ho->targetName() ), qPrintable( getFullName() ) );
    return false;
  }
  ho_map.insert( ho->targetName(), ho );
  ho_vect.push_back( ho );
  return true;
}

bool TDataSet::unregisterHolder( HolderData *ho )
{
  if( !ho )
    return false;
  auto iter = ho_map.find( ho->targetName() );
  if( iter == ho_map.end() ) {
    qDebug( "ERR: TDataSet::unregisterHolder: fail to find %s in %s",
            qPrintable( ho->targetName() ), qPrintable( getFullName() ) );
    return false;
  }
  ho_map.erase( iter );

  auto i2 = find( ho_vect.begin(), ho_vect.end(), ho );
  if( i2 == ho_vect.end() ) {
    qDebug( "ERR: TDataSet::unregisterHolder: fail to ptr to find %s in %s",
            qPrintable( ho->targetName() ), qPrintable( getFullName() ) );
    return false;
  }
  ho_vect.erase( i2 );
  return true;
}

TDataSet* TDataSet::create( TDataSet* apar )
{
  qDebug( "warn: pure TDataSet created" );
  return new TDataSet( apar );
}


TDataSet* TDataSet::createObj( const QString &cl_name, 
      const QString &nm, TDataSet* apar )
{
  TDataSet *ob = ElemFactory::theFactory().createElem( cl_name, nm, apar );
  if( !ob ) {
    qDebug("Fail to create obj \"%s\" class  \"%s\"",
	qPrintable(nm), qPrintable(cl_name) );
    return nullptr;
  }
  return ob;
}


const char *TDataSet::getHelp(void) const
{
  return helpstr;
}


TDataSet* TDataSet::getObj( const QString &ename, const QString &cl_name )
{
  HolderData *ho = getHolder( ename );
  if( !ho ) {
    qDebug( "ERR: TDataSet::getObj: not found element \"%s\"",
	qPrintable(ename) );
    return nullptr;
  }
  if( ! ho->isObject( cl_name ) ){
    qDebug( "ERR: TDataSet::getObj: element \"%s\" in %s "
	    "has type \"%s\", need \"%s\"",
	qPrintable(ename), qPrintable(getFullName()),
	qPrintable(ho->getType()), qPrintable(cl_name) );
    return nullptr;
  }
  HolderObj *hob = qobject_cast<HolderObj*>(ho);
  if( !hob )
    return nullptr; // unlikely
  return hob->getObj();
}


int TDataSet::getData( const QString &nm, QVariant &da ) const
{
  if( nm.isEmpty() )
    return 0;
  QString first, rest;
  int nm_type = splitName( nm, first, rest );
  if( nm_type == -1 ) {
    qDebug( "TDataSet::getData(Variant): bad target name \"%s\"",
	qPrintable( nm ) );
    return 0;
  }
  
  HolderData *ho = getHolder( first );
  if( !ho ) {
    qDebug( "TDataSet::getData(Variant): fail to find name \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  if( nm_type == 1 ) { // first only 
    da = ho->get();
    return 1;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    qDebug( "TDataSet::getData(Variant): compound name required \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  
  TDataSet* ds= static_cast<TDataSet*>( ho->getPtr() );
  return ds->getData( rest, da );
}


int TDataSet::getData( const QString &nm, int *da ) const
{
  if( !da )
    return 0;
  QVariant vda;
  int rc = getData( nm, vda );
  if( rc == 0 )
    return 0;
  *da = vda.toInt();
  return 1;
}

int TDataSet::getData( const QString &nm, double *da ) const
{
  if( !da )
    return 0;
  QVariant vda;
  int rc = getData( nm, vda );
  if( rc == 0 )
    return 0;
  *da = vda.toDouble();
  return 1;
}


int TDataSet::getData( const QString &nm, QString &da ) const
{
  QVariant vda;
  int rc = getData( nm, vda );
  if( rc == 0 )
    return 0;
  da = vda.toString();
  return 1;
}


int TDataSet::setData( const QString &nm, const QVariant &da )
{
  if( nm.isEmpty() )
    return 0;
  QString first, rest;
  int nm_type = splitName( nm, first, rest );
  if( nm_type == -1 ) {
    qDebug( "TDataSet::setData(Variant): bad target name \"%s\"",
	qPrintable( nm ) );
    return 0;
  }
  
  HolderData *ho = getHolder( first );
  if( !ho ) {
    qDebug( "TDataSet::setData(Variant): fail to find name \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  if( nm_type == 1 ) { // first only 
    return ho->set( da );
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    qDebug( "TDataSet::setData(Variant): compound name required \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  
  TDataSet* ds= static_cast<TDataSet*>( ho->getPtr() );
  return ds->setData( rest, da );
}


int TDataSet::checkData( int /* ni */ )
{
  check_guard();
  return 0;
}

const double* TDataSet::getDoublePtr( const QString &nm, ltype_t *lt, int lev ) const
{
  if( nm.isEmpty() ) {
    if( lt )
      *lt = LinkNone;
    return 0;
  }
  QString nmf = nm, first, rest;
  
  if( nm[0] == ':' ) { // handle old ':name' from TModel
    nmf.remove( 0, 1 );
  }

  int nm_type = splitName( nmf, first, rest );
  if( nm_type == -1 ) {
    if( lt )
      *lt = LinkBad;
    qDebug( "TDataSet::setDoublePtr: bad target name \"%s\"",
	qPrintable( nmf ) );
    return 0;
  }

  HolderData *ho = getHolder( first );
  
  if( !ho ) {
    if( lt )
      *lt = LinkBad;
    qDebug( "TDataSet::getDoublePtr: fail to find name \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  
  if( nm_type == 1 ) { // first only
    if( ho->isObject() ) {
      HolderObj *hob= qobject_cast<HolderObj*>(ho);
      return hob->getObj()->getDoublePtr( "out0", lt, lev+1 );
    }
    if( ho->getOldTp() == dtpDouble ) {
      if( lt ) {
	*lt = ( lev == 1 ) ? LinkElm : LinkSpec;
      }
      return static_cast<const double*>( ho->getPtr() );
    }
    return 0;
  }

  // both part of name exists
  if( ! ho->isObject() ) {
    if( lt )
      *lt = LinkBad;
    return 0;
  }
  HolderObj *hob= qobject_cast<HolderObj*>(ho);
  return hob->getObj()->getDoublePtr( rest, lt, lev+1 );
  
}

double* TDataSet::getDoublePrmPtr( const QString &nm, int *flg )
{
  if( nm.isEmpty() )
    return 0;
  
  HolderData *ho = getHolder( nm );
  
  if( !ho ) {
    qDebug( "TDataSet::getDoublePrmPtr: fail to find name \"%s\"",
	qPrintable( nm ) );
    return 0;
  }
  
  if( ho->getOldTp() == dtpDouble ) {
    if( flg )
      *flg = ho->getFlags();
    return static_cast<double*>( ho->getPtr() );
  }
  
  return 0;
}


void* TDataSet::add_obj( const QString &cl_name, const QString &ob_name )
{
  if( !allow_add )
    return nullptr;
  if( getHolder( ob_name ) != nullptr ) {
    qDebug( "ERR: TDataSet::add_obj: name \"%s\" exist in %s!",
	qPrintable(ob_name), qPrintable( getFullName() ) );
    return nullptr;
  }
  // TODO: no simple types for now!
  TDataSet *ob = ElemFactory::theFactory().createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  ob->check_guard();
  // const TClassInfo *cl = ElemFactory::theFactory().getInfo( cl_name );
  
  return ob;
}

int TDataSet::del_obj( const QString &ob_name )
{
  HolderData *ho = getHolder( ob_name );
  if( !ho ) {
    qDebug( "TDataSet::del_obj: not found element \"%s\"", qPrintable( ob_name ) );
    return 0;
  }
  if( ! ho->isDyn() ) {
    qDebug( "TDataSet::del_obj: obj \"%s\" is not created dynamicaly",
	    qPrintable( ob_name ) );
    return 0;
  }
  delete ho; // auto remove object and from parent lists
  return 1;
}


int TDataSet::isValidType(  const QString & /*cl_name*/  ) const
{
  return 1;
}

bool TDataSet::isChildOf( const QString &cname )
{
  check_guard();
  const QMetaObject *mob = metaObject();
  while( mob ) {
    if( cname == mob->className() )
      return true;
    mob = mob->superClass(); 
  }
  return false;
}


// FIXME: implement 
bool TDataSet::set( const QVariant & x )
{
  check_guard();
  return fromString( x.toString() );
}

QVariant TDataSet::get() const
{
  check_guard();
  return QVariant( this->toString() );// TODO:
}

void TDataSet::post_set()
{
  check_guard();
// TODO:
}

QString TDataSet::toString() const
{
  check_guard();
  QString buf;
  buf.reserve(4096); // TODO ?
  QTextStream tstr( &buf, QIODevice::WriteOnly );
  QDomDocument dd_tmp;
  QDomElement el_tmp = dd_tmp.createElement("tmp_xxx");
  QDomElement dom = toDom( dd_tmp );
  dom.save( tstr, 4 );
  
  return buf; 
}

bool TDataSet::fromString( const QString &s )
{
  QString errstr;
  int err_line, err_column;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    qDebug( "ERR: TDataSet::fromString: fail to parse str: line %d col %d", 
	err_line, err_column );
    return false; 
  }
  QDomElement domroot = x_dd.documentElement();

  return fromDom( domroot, errstr ); 
}

QDomElement TDataSet::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "obj" );
  de.setAttribute( "name", objectName() );
  de.setAttribute( "otype", getClassName() );
  
  for( HolderData *ho : ho_vect ) {
    if( !ho )
      continue; // but how?
    if( ho->getFlags() & efNoSave )
      continue;
    QDomElement cde = ho->toDom( dd );
    de.appendChild( cde );
  }
  
  return de;
}

static QString getDomText( QDomNode &p )
{
  QString r;
  for( QDomNode no = p.firstChild(); !no.isNull() ; no = no.nextSibling() ) {
    if ( ! no.isText() ) 
      continue;
    QDomText t = no.toText();
    r += t.data();
  }
  return r;
}

bool TDataSet::fromDom( QDomElement &de, QString &errstr )
{
  for( QDomNode no = de.firstChild(); !no.isNull() ; no = no.nextSibling() ) {
    
    if ( ! no.isElement() ) 
      continue;
    
    QDomElement ee = no.toElement();
    QString elname = ee.attribute( "name" );
    QString tagname = ee.tagName();
    //qDebug( "dbg: TDataSet::fromDom: obj: %s tag: \"%s\" name: \"%s\"",  
    //  qPrintable(getFullName()), qPrintable(tagname), qPrintable(elname) );
    
    if( tagname == "obj" ) {  // ------------------------------- object
      QString cl_name = ee.attribute("otype");
      HolderData *ho = getHolder( elname );
      if( ho && ! ho->isObject() ) {
        errstr = QString("TDataSet::fromDom: elem: \"%1\" is not a element: \"%2\"")
	         .arg(elname).arg(tagname); 
	return false;
      }
      if( !ho ) { // name not found
	if( ! allow_add ) {
	  qDebug( "WARN: TDataSet::fromDom: creating disallowed in: \"%s\"",
		   qPrintable( objectName() ) );
	  continue;
	}
	if( ! add_obj( cl_name, elname ) ) {
	  errstr = QString("TDataSet::fromDom: fail to create obj %1 %2 ")
		   .arg(cl_name).arg(elname); 
	  return false;
	}
      }
      ho = getHolder( elname );
      if( !ho || !ho->isObject( cl_name ) ) {
	errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 ")
		 .arg(cl_name).arg(elname); 
	return false;
      }
      HolderObj *hob = qobject_cast<HolderObj*>(ho);
      TDataSet *ob = hob->getObj();
      if( ! ob->fromDom( ee, errstr ) ) {
	return false;
      }
    
    } else if( tagname == "param" ) {  // ---------------- simple param
      HolderData *ho = getHolder( elname );
      if( !ho ) {       //  TODO: create param dyn, if allowed
	qDebug( "WARN: TDataSet::fromDom: unknown param: \"%s\"",
		 qPrintable(elname) );
	continue;
      }
      if( ho->isObject() ) {
	errstr = QString("TDataSet::fromDom: param %1 is a element type %2 ")
		 .arg(elname).arg(ho->getType()); 
	return false;
      }
      ho->set( getDomText(ee) );

    } else { // ----------- unknown element
      errstr = QString("TDataSet::fromDom: bad element %1 %2 ")
	       .arg(tagname).arg(elname); 
      return false;
    }
  }

  return true;
}

void TDataSet::check_guard() const
{
  if( guard != guard_val )  {
    qDebug( "Guard value!!!");
    abort();
  }
}

void TDataSet::dumpStruct() const
{
  static int dump_lev = -1;
  ++dump_lev;
  qDebug( "*%d struct of %s %s this=%p", 
         dump_lev, getClassName(), qPrintable(objectName()), this );
  // new part
  QObjectList childs = children();
  int i = 0;
  for( auto o : childs ) {
    QObject *xo = o;
    qDebug( "*# [%d] (%p) %s %s ", 
	i, xo, xo->metaObject()->className(), qPrintable(xo->objectName()) );
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    qDebug( "*#    = %s", qPrintable( ho->toString() ) );
    ++i;
  }
  qDebug( "*%d END", dump_lev );
  --dump_lev;
}

// end of dataset.cpp


