/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
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
           :QObject( a_parent ),
	    flags(a_flags),
	    par(a_parent), 
	    target_name( obj_name )
{
  setObjectName( "_HO_" + obj_name ); // TODO: w/o "_HO_"
  a_parent->registerHolder( this );

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
  DBGx( "dbg: HolderData::~HolderData(): %s par: %p %s", 
      qP( objectName() ), par, qP( par->objectName() )  );
  par->unregisterHolder( this );
}

void HolderData::setParm( const QString &name, const QString &value )
{
  parms[name] = value;
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
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=\s*(\S+)$)" );
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
      DBGx( "warn: bad extra string part: \"%s\"", qP( s ) );
    }
  }
  if( getParm("dyn") == "1" )
    dyn = 1;
}

void HolderData::setElems( const QString &els )
{
  parms["list_elems"] = els;
  int n_nl = els.count( '\n' );
  if( ! els.isEmpty() ) { // "" => 0, "a" => 1, "a\nb" => 2
    ++n_nl;
  }
  setParm( "min", "0" );
  setParm( "max", QString::number( n_nl-1 ) );
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
HolderInfo HolderInt::holder_info = {
  "int", HolderInt::createPlus
};


HolderInt::HolderInt( int *p, const QString &obj_name, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new int; *val = 0 /* tmp bad */; dyn = 1;
  }
  ptr = val; tp=QVariant::Int;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "INT,SIMPLE" );
  }
  post_set();
}

HolderInt::HolderInt( const QString &obj_name, int v0, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
           v(v0),
	   val(nullptr) // to del
{
  if( !val ) {
    val = new int; *val = v0; dyn = 1;
  }
  ptr = val; tp=QVariant::Int;
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

HolderData* HolderInt::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderInt( 0, obj_name, v_name, a_parent, a_flags, 
                        a_descr, a_extra );
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
  int v_min { IMIN }, v_max { IMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() ) 
    v_min = s_min.toInt();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toInt();
  *val = qBound( v_min, *val, v_max );
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

REGISTER_SIMPLE_T(HolderInt)


// ---------------- HolderSwitch ---------
HolderInfo HolderSwitch::holder_info = {
  "switch", HolderSwitch::createPlus
};

HolderSwitch::HolderSwitch( int *p, const QString &obj_name, 
              const QString &v_name, TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,SWITCH" );
  }
  setParm("min","0"); setParm("max","1");
  post_set();
}

HolderSwitch::~HolderSwitch()
{
  // NOP
}

HolderData* HolderSwitch::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderSwitch( 0, obj_name, v_name, a_parent, a_flags, 
                           a_descr, a_extra );
}


void HolderSwitch::post_set()
{
  *val = (*val) ? 1 : 0;
}

QString HolderSwitch::getType() const
{
  return "switch";
}

REGISTER_SIMPLE_T(HolderSwitch)



// ---------------- HolderList ---------
HolderInfo HolderList::holder_info = {
  "list", HolderList::createPlus
};

HolderList::HolderList( int *p, const QString &obj_name, 
     const QString &v_name, TDataSet *a_parent, int a_flags,
     const QString &a_descr,
     const QString &a_extra,  const QString &a_elems  )
   :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  setParm("min","0"); setParm("max","0");
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

// FIXME: not to create dynamicaly: where to create list elems
HolderData* HolderList::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderList( 0, obj_name, v_name, a_parent, a_flags, a_descr, 
      a_extra, "First\nSecond\nThird\nFourth\nFifth"); // fake list
}


QString HolderList::getType() const
{
  return "list";
}


REGISTER_SIMPLE_T(HolderList)



// ---------------- HolderDouble ---------
HolderInfo HolderDouble::holder_info = {
  "double", HolderDouble::createPlus
};

HolderDouble::HolderDouble( double *p, const QString &obj_name,
              const QString &v_name,  TDataSet *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new double; *val = 0; dyn = 1;
  }
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "DOUBLE,SIMPLE" );
  }
  ptr = val; tp=QVariant::Double;
}

HolderDouble::~HolderDouble()
{
  if( dyn )
    delete val;
  val = 0; ptr = val; dyn = 0;
}

HolderData* HolderDouble::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderDouble( 0, obj_name, v_name, a_parent, a_flags, 
                           a_descr, a_extra);
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
  double v_min { DMIN }, v_max { DMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() ) 
    v_min = s_min.toDouble();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toDouble();
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


REGISTER_SIMPLE_T(HolderDouble)


// ---------------- HolderString ---------
HolderInfo HolderString::holder_info = {
  "string", HolderString::createPlus
};

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
  ptr = val; tp=QVariant::String;
}

HolderString::~HolderString()
{
  if( dyn )
    delete val;
  val = 0; ptr = val; dyn = 0;
}

HolderData* HolderString::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderString( 0, obj_name, v_name, a_parent, a_flags,
                           a_descr, a_extra);
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
  int v_max { IMAX };
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toInt();
  val->truncate( v_max );
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

REGISTER_SIMPLE_T(HolderString)


// ---------------- HolderColor ---------
HolderInfo HolderColor::holder_info = {
  "color", HolderColor::createPlus
};

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
  ptr = val; tp=QVariant::Color;
}

HolderColor::~HolderColor()
{
  if( dyn )
    delete val;
  val = 0; ptr = val;
}

HolderData* HolderColor::createPlus( const QString &obj_name, 
         const QString &v_name, TDataSet *a_parent, int a_flags,
	 const QString &a_descr, const QString &a_extra )
{
  return new HolderColor( 0, obj_name, v_name, a_parent, a_flags,
                           a_descr, a_extra);
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

REGISTER_SIMPLE_T(HolderColor)


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
    DBGx( "ERR: *** p=0 obj_name=%s !", qP( obj_name ) );
  }
  // post_set();
  ptr = obj; tp=QVariant::UserType;
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
 {  "TDataSet",  TDataSet::create, 0, helpstr, 0 };


const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

TDataSet::TDataSet( TDataSet* aparent )
         :QObject( aparent ),
	  par(aparent)
{
}

TDataSet::~TDataSet()
{
  DBGx( "dbg:  %p %s", this, qP( getFullName() ));
  // deletion is done by holders dtors
  state = stateBad; guard = 0;
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
    DBGx( "ERR: %s i=%d out of range", qP( getFullName() ), i );
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
    DBGx( "ERR: fail to add %s to %s: exists",
            qP( ho->targetName() ), qP( getFullName() ) );
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
    DBGx( "ERR: fail to find %s in %s",
            qP( ho->targetName() ), qP( getFullName() ) );
    return false;
  }
  ho_map.erase( iter );

  auto i2 = find( ho_vect.begin(), ho_vect.end(), ho );
  if( i2 == ho_vect.end() ) {
    DBGx( "ERR: fail to ptr to find %s in %s",
            qP( ho->targetName() ), qP( getFullName() ) );
    return false;
  }
  ho_vect.erase( i2 );
  return true;
}

TDataSet* TDataSet::create( TDataSet* apar )
{
  DBG1( "warn: pure TDataSet created" );
  return new TDataSet( apar );
}



const char *TDataSet::getHelp(void) const
{
  return helpstr;
}


TDataSet* TDataSet::getObj( const QString &ename, const QString &cl_name )
{
  HolderData *ho = getHolder( ename );
  if( !ho ) {
    DBGx( "ERR: not found element \"%s\"", qP(ename) );
    return nullptr;
  }
  if( ! ho->isObject( cl_name ) ){
    DBGx( "ERR: element \"%s\" in %s has type \"%s\", need \"%s\"",
	qP(ename), qP(getFullName()), qP(ho->getType()), qP(cl_name) );
    return nullptr;
  }
  HolderObj *hob = qobject_cast<HolderObj*>(ho);
  if( !hob ) {
    DBG2q( "ERR: qobject_cast failed for", ename );
    return nullptr; // unlikely
  }
  return hob->getObj();
}


int TDataSet::getData( const QString &nm, QVariant &da ) const
{
  if( nm.isEmpty() )
    return 0;
  QString first, rest;
  int nm_type = splitName( nm, first, rest );
  if( nm_type == -1 ) {
    DBGx( "warn: bad target name \"%s\"", qP( nm ) );
    return 0;
  }
  
  HolderData *ho = getHolder( first );
  if( !ho ) {
    DBGx( "warn: fail to find name \"%s\"", qP( first ) );
    return 0;
  }
  if( nm_type == 1 ) { // first only 
    da = ho->get();
    return 1;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    DBG2q( "warn: compound name required ", first );
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
    DBG2q( "warn: bad target name", nm );
    return 0;
  }
  
  HolderData *ho = getHolder( first );
  if( !ho ) {
    DBG2q( "warn: fail to find name", first );
    return 0;
  }
  if( nm_type == 1 ) { // first only 
    return ho->set( da );
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    DBG2q( "warn: compound name required ", first );
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
    DBG2q( "warn: bad target name", nmf );
    return 0;
  }

  HolderData *ho = getHolder( first );
  
  if( !ho ) {
    if( lt )
      *lt = LinkBad;
    DBG2q( "warn: fail to find name ", first );
    return 0;
  }
  
  if( nm_type == 1 ) { // first only
    if( ho->isObject() ) {
      HolderObj *hob= qobject_cast<HolderObj*>(ho);
      return hob->getObj()->getDoublePtr( "out0", lt, lev+1 );
    }
    if( ho->getTp() == QVariant::Double ) {
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
    DBG2q( "warn: fail to find name", nm );
    return 0;
  }
  
  if( ho->getTp() == QVariant::Double ) {
    if( flg )
      *flg = ho->getFlags();
    return static_cast<double*>( ho->getPtr() );
  }
  
  return 0;
}


TDataSet* TDataSet::add_obj( const QString &cl_name, const QString &ob_name )
{
  if( ! ( allow_add & allowObject ) )
    return nullptr;
  if( getHolder( ob_name ) != nullptr ) {
    DBGx( "ERR: name \"%s\" exist in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    DBGx( "WARN: type \"%s\" not allowed in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  TDataSet *ob = EFACT.createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  ob->check_guard();
  structChanged();
  
  return ob;
}

int TDataSet::del_obj( const QString &ob_name )
{
  HolderData *ho = getHolder( ob_name );
  if( !ho ) {
    DBG2q( "warn: not found element", ob_name );
    return 0;
  }
  if( ! ho->isDyn() ) {
    DBG2q( " object  is not created dynamicaly: ", ob_name );
    return 0;
  }
  delete ho; // auto remove object and from parent lists
  structChanged();
  return 1;
}

HolderData* TDataSet::add_param( const QString &tp_name, const QString &ob_name )
{
  if( ! ( allow_add & allowParam ) )
    return nullptr;
  if( getHolder( ob_name ) ) {
    DBGx( "ERR: name \"%s\" exist in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  HolderData *ho = EFACT.createParam( tp_name, ob_name, this );
  if( !ho ) {
    return nullptr;
  }
  structChanged();
  return ho;
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


bool TDataSet::set( const QVariant & x )
{
  check_guard();
  return fromString( x.toString() );
}

QVariant TDataSet::get() const
{
  check_guard();
  return QVariant( this->toString() );
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
    DBGx( "ERR: fail to parse str: line %d col %d str: %s", 
        err_line, err_column, qP(errstr) );
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
    // DBGx( "dbg: TDataSet::fromDom: obj: %s tag: \"%s\" name: \"%s\"",  
    //   qP(getFullName()), qP(tagname), qP(elname) );
    
    if( tagname == "obj" ) {  // ------------------------------- object
      QString cl_name = ee.attribute("otype");
      HolderData *ho = getHolder( elname );
      if( ho && ! ho->isObject() ) {
        errstr = QString("TDataSet::fromDom: elem: \"%1\" is not a element: \"%2\"")
	         .arg(elname).arg(tagname); 
	return false;
      }
      if( !ho ) { // name not found
	if( ! ( allow_add & allowObject) ) {
	  DBG2q( "WARN: creating disallowed in: %s", objectName() );
	  continue;
	}
	if( ! add_obj( cl_name, elname ) ) {
	  errstr = QString("TDataSet::fromDom: fail to create obj %1 %2 ")
		   .arg(cl_name).arg(elname); 
          DBG1q( errstr );
	  return false;
	}
      }
      ho = getHolder( elname );
      if( !ho || !ho->isObject( cl_name ) ) {
	errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 ")
		 .arg(cl_name).arg(elname); 
        DBG1q( errstr );
	return false;
      }
      HolderObj *hob = qobject_cast<HolderObj*>(ho);
      TDataSet *ob = hob->getObj();
      if( ! ob->fromDom( ee, errstr ) ) {
	return false;
      }
    
    } else if( tagname == "param" ) {  // ---------------- simple param
      QString tp_name = ee.attribute("otype");
      HolderData *ho = getHolder( elname );
      if( ho && ho->isObject() ) {
	errstr = QString("TDataSet::fromDom: param %1 is a element type %2 ")
		 .arg(elname).arg(ho->getType()); 
        DBG1q( errstr );
	return false;
      }
      if( !ho ) {
	DBG2q( "WARN: unknown param:", elname );
	if( ! ( allow_add & allowParam ) ) {
	  DBG2q( "WARN: creating param disallowed in:", objectName() );
	  continue;
	}
	ho =  add_param( tp_name, elname );
	if( !ho  ) {
	  errstr = QString("TDataSet::fromDom: fail to create param %1 %2 ")
		   .arg(tp_name).arg(elname); 
	  DBG1q( errstr );
	  return false;
	}
      }
      ho->set( getDomText(ee) );

    } else { // ----------- unknown element
      errstr = QString("TDataSet::fromDom: bad element %1 %2 ")
	       .arg(tagname).arg(elname); 
      DBG1q( errstr );
      return false;
    }
  }
  structChanged();

  return true;
}

void TDataSet::check_guard() const
{
  if( guard != guard_val )  {
    DBG1( "ERR!!!!!!!!!! Guard value!!!");
    abort();
  }
}


void TDataSet::structChanged()
{
  do_structChanged();
  emit sigStructChanged();
}

void TDataSet::do_structChanged()
{
}

void TDataSet::dumpStruct() const
{
  static int dump_lev = -1;
  ++dump_lev;
  DBGx( "* %d struct of %s %s this=%p", 
         dump_lev, getClassName(), qP(objectName()), this );
  // new part
  QObjectList childs = children();
  int i = 0;
  for( auto o : childs ) {
    QObject *xo = o;
    DBGx( "*# [%d] (%p) %s %s ", 
	i, xo, xo->metaObject()->className(), qP(xo->objectName()) );
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    DBGx( "*#    = %s", qP( ho->toString() ) );
    ++i;
  }
  DBGx( "*%d END", dump_lev );
  --dump_lev;
}

// end of dataset.cpp


