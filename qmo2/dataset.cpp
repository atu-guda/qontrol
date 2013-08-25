/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QColor>
#include "miscfun.h"
#include "dataset.h"

using namespace std;


// ================================================================
// ---------------- HolderData .... ----------------------
STD_CLASSINFO(HolderData,clpSpecial);

HolderData::HolderData( ARGS_CTOR_MIN )
     :QObject( a_parent ),
      flags(a_flags),
      par(a_parent)
{
  setObjectName( obj_name );

  if( a_v_name.isNull() )  {
    setParm( "vis_name", obj_name );
  } else {
    setParm( "vis_name", a_v_name );
  }
  setParm( "descr", a_descr );
  setParm( "extra", a_extra );
  extraToParm();
}

HolderData::~HolderData()
{
  /*
  DBGx( "dbg:  %s par: %p %s", 
      qP( objectName() ), par, 
      par ? qP( par->objectName() ) : "NONE" );
  */
}

HolderData* HolderData::create( const QString &, TDataSet *, 
         int, const QString &, const QString &, const QString & )
{
  DBG1( "Attempt to create abstrace class" );
  return nullptr;
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
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );
  QStringList el = getParm("extra").split( "\n", QString::SkipEmptyParts );
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
  setParm( "max", QSN( n_nl-1 ) );
}

QString HolderData::getType() const // = 0;
{
  return "None";
}

QDomElement HolderData::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "param" );
  de.setAttribute( "name", objectName() );
  if( dyn ) {
    de.setAttribute( "dyn", "1" );
    de.setAttribute( "otype", getType() );
  }
  QDomText tn = dd.createTextNode( toString() );
  de.appendChild( tn );
  return de;
}

QString HolderData::getFullName() const
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

bool HolderData::getData( const QString &nm, QVariant &da ) const
{
  if( ! nm.isEmpty() ) { // only w/o name
    DBGx( "warn: complex name \"%s\" req for simple obj %s", 
        qP(nm), qP(objectName()) )
    return false;
  }
  da = get();
  return true;
}


bool HolderData::getData( const QString &nm, int *da ) const
{
  if( !da )
    return false;
  QVariant vda;
  bool rc = getData( nm, vda );
  if( ! rc )
    return false;
  *da = vda.toInt();
  return true;
}

bool HolderData::getData( const QString &nm, double *da ) const
{
  if( !da )
    return 0;
  QVariant vda;
  bool rc = getData( nm, vda );
  if( ! rc )
    return false;
  *da = vda.toDouble();
  return 1;
}


bool HolderData::getData( const QString &nm, QString &da ) const
{
  QVariant vda;
  bool rc = getData( nm, vda );
  if( ! rc )
    return false;
  da = vda.toString();
  return true;
}


bool HolderData::setData( const QString &nm, const QVariant &da )
{
  if( ! nm.isEmpty() ) { // only w/o name
    DBGx( "warn: complex name \"%s\" req for simple obj %s", 
        qP(nm), qP(objectName()) )
    return false;
  }
  return set( da );
}


const char* HolderData::helpstr { "Abstract data holder" };



// ---------------- HolderInt ---------
STD_CLASSINFO_ALIAS(HolderInt,clpData,int);

CTOR(HolderInt,HolderData) , v(0)
{
  tp=QVariant::Int;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "INT,SIMPLE" );
  }
  post_set();
}

HolderInt::~HolderInt()
{
  v = 0; dyn = 0;
}

void HolderInt::reset_dfl()
{
  v = 0;
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = QString2IntEx( s );
  }
  post_set();
}


bool HolderInt::set( const QVariant & x, int /* idx */ )
{
  bool ok;
  if( x.type() == QVariant::Int ) {
    v = x.toInt( &ok );
    post_set();
    return ok;
  }
  v = QString2IntEx( x.toString(), &ok );
  return ok;
}

QVariant HolderInt::get( int /* idx */ ) const
{
  return QVariant( v );
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
  v = qBound( v_min, v, v_max );
}

QString HolderInt::toString() const
{
  return QSN( v );
}

bool HolderInt::fromString( const QString &s )
{
  bool ok = true;
  int vc = QString2IntEx( s, &ok );
  if( ok ) {
    v = vc;
    post_set();
  }
  return ok;
}


QString HolderInt::getType() const
{
  return "int";
}

const char* HolderInt::helpstr { "Contains integer data" };


DEFAULT_FUNCS_REG(HolderInt);


// ---------------- HolderSwitch ---------
STD_CLASSINFO_ALIAS(HolderSwitch,clpData,switch);

CTOR(HolderSwitch,HolderInt)
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

void HolderSwitch::post_set()
{
  v = v ? 1 : 0;
}

QString HolderSwitch::getType() const
{
  return "switch";
}

const char* HolderSwitch::helpstr { "Contains integer (bin) data - switch iface" };

DEFAULT_FUNCS_REG(HolderSwitch);


// ---------------- HolderList ---------
STD_CLASSINFO_ALIAS(HolderList,clpData,list);

HolderList::HolderList( ARGS_CTOR_MIN, const QString &a_elems  )
   : HolderInt( ARGS_CTOR_NAMES )
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

void HolderList::post_set()
{
  HolderInt::post_set();
}


QString HolderList::getType() const
{
  return "list";
}

const char* HolderList::helpstr { "Contains integer data - list iface" };

DEFAULT_FUNCS_REG(HolderList);



// ---------------- HolderDouble ---------
STD_CLASSINFO_ALIAS(HolderDouble,clpData,double);

CTOR(HolderDouble,HolderData), v(0)
{
  tp=QVariant::Double;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "DOUBLE,SIMPLE" );
  }
}

HolderDouble::~HolderDouble()
{
  v = 0; dyn = 0;
}

void HolderDouble::reset_dfl()
{
  v = 0;
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = s.toDouble();
  }
  post_set();
}


bool HolderDouble::set( const QVariant & x, int /* idx */ )
{
  bool ok;
  v = x.toDouble( &ok );
  post_set();
  return ok;
}

QVariant HolderDouble::get( int /* idx */ ) const
{
  return QVariant( v );
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
  v = qBound( v_min, v, v_max );
}

QString HolderDouble::toString() const
{
  return QSN( v, 'g', 16 ); // TODO? format
}

bool HolderDouble::fromString( const QString &s )
{
  bool ok;
  double vx = s.toDouble( &ok ); 
  if( ok ) {
    v = vx;
    post_set();
  }
  return ok;
}


QString HolderDouble::getType() const
{
  return "double";
}

const char* HolderDouble::helpstr { "Contains double data" };

DEFAULT_FUNCS_REG(HolderDouble);


// ---------------- HolderString ---------
STD_CLASSINFO_ALIAS(HolderString,clpData,QString);

CTOR(HolderString,HolderData)
{
  tp=QVariant::String;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "STRING,SIMPLE" );
  }
}

HolderString::~HolderString()
{
  dyn = 0;
}

void HolderString::reset_dfl()
{
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = s;
  } else {
    v = "";
  }
  post_set();
}

bool HolderString::set( const QVariant & x, int /* idx */  )
{
  v = x.toString();
  post_set();
  return true;
}

QVariant HolderString::get( int /* idx */ ) const
{
  return QVariant( v );
}

void HolderString::post_set()
{
  int v_max { IMAX };
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toInt();
  v.truncate( v_max );
}

QString HolderString::toString() const
{
  return v;
}

bool HolderString::fromString( const QString &s )
{
  v = s; 
  post_set();
  return true;
}


QString HolderString::getType() const
{
  return "string";
}

const char* HolderString::helpstr { "Contains QString data" };

DEFAULT_FUNCS_REG(HolderString);


// ---------------- HolderColor ---------
STD_CLASSINFO_ALIAS(HolderColor,clpData,QColor);

CTOR(HolderColor,HolderData)
{
  tp=QVariant::Color;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "COLOR,INT" );
  }
}

HolderColor::~HolderColor()
{
}

void HolderColor::reset_dfl()
{
  v=QColor();
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = QColor( s );
    if( ! v.isValid() ) {
      v = QColor(s.toInt());
    }
  }
  post_set();
}


bool HolderColor::set( const QVariant & x, int /* idx */  )
{
  QRgb rgba = x.toInt();
  v.setRgba( rgba );
  post_set();
  return true;
}

QVariant HolderColor::get( int /* idx */ ) const
{
  return QVariant( (int)(v.rgba()) );
}

void HolderColor::post_set()
{
  if( ! v.isValid() )
    v = QColor(Qt::red);
}

QString HolderColor::toString() const
{
  return QSN( (int)(v.rgba()) );
}

bool HolderColor::fromString( const QString &s )
{
  QRgb rgba = s.toInt();
  v.setRgba( rgba );
  post_set();
  return true;
}


QString HolderColor::getType() const
{
  return "color";
}

const char* HolderColor::helpstr { "Contains QColor data" };

DEFAULT_FUNCS_REG(HolderColor);

// ---------------- HolderIntArray ---------
STD_CLASSINFO_ALIAS(HolderIntArray,clpData|clpArray,int[]);

CTOR(HolderIntArray,HolderData)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_INT" );
  }
  // all done by reset_dfl
}

HolderIntArray::~HolderIntArray()
{
  dyn = 0;
}

void HolderIntArray::reset_dfl()
{
  int n = 1, v0 = 0;
  QString s = getParm("N");
  if( ! s.isEmpty() )
    n = s.toInt();
  s = getParm("def");
  v.assign( n, v0 );

  s = getParm("defs");
  if( ! s.isEmpty() ) {
    QStringList sl = s.split( " ", QString::SkipEmptyParts );
    if( sl.size() > (int)v.size() )
      v.assign( sl.size(), v0 );
  
    int vc, i = 0;
    bool ok;
    for( auto cs : sl ) {
      vc = cs.toInt( &ok, 0 ); // 0 = auto base
      if( ok )
        v[i] = vc;
      ++i;
    }
  }

  post_set();
}


bool HolderIntArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( idx < 0 || (unsigned)(idx) >= v.size() ) // slow, but safe - not for fast code
    return false;
  v[idx] = x.toInt( &ok );
  post_set();
  return ok;
}

QVariant HolderIntArray::get( int idx ) const
{
  if( idx < 0 || (unsigned)idx >= v.size() ) // slow, but safe - not for fast code
    return QVariant();
  return QVariant( v[idx] );
}

void HolderIntArray::post_set()
{
  int v_min { IMIN }, v_max { IMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() ) 
    v_min = s_min.toInt();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toInt();
  for( int& vc : v ) {
    vc = qBound( v_min, vc, v_max );
  }
}

QString HolderIntArray::toString() const
{
  QString s, sep = "";
  for( int vc : v ) {
    s += sep + QSN( vc );
    sep = " ";
  }
  return s;
}

bool HolderIntArray::fromString( const QString &s )
{
  bool ok;
  QStringList sl = s.split(" ", QString::SkipEmptyParts );
  v.clear(); v.reserve( sl.size() );
  int vc;

  for( auto s : sl ) {
    vc = s.toInt( &ok, 0 ); // 0 = auto base
    v.push_back( vc );
  }

  post_set();
  return ok; // ? only last
}


QString HolderIntArray::getType() const
{
  return "int[]";
}

const char* HolderIntArray::helpstr { "Contains vector of integer data" };


DEFAULT_FUNCS_REG(HolderIntArray);


// ---------------- HolderDoubleArray ---------
STD_CLASSINFO_ALIAS(HolderDoubleArray,clpData|clpArray,double[]);

CTOR(HolderDoubleArray,HolderData)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_DOUBLE" );
  }
}

HolderDoubleArray::~HolderDoubleArray()
{
  dyn = 0;
}

void HolderDoubleArray::reset_dfl()
{
  int n = 1;
  double v0 = 0;
  QString s = getParm("N");
  if( ! s.isEmpty() )
    n = s.toInt();
  s = getParm("def");
  v.assign( n, v0 );

  s = getParm("defs");
  if( ! s.isEmpty() ) {
    QStringList sl = s.split( " ", QString::SkipEmptyParts );
    if( sl.size() > (int)v.size() )
      v.assign( sl.size(), v0 );
  
    double vc;
    int i = 0;
    bool ok;
    for( auto cs : sl ) {
      vc = cs.toDouble( &ok );
      if( ok )
        v[i] = vc;
      ++i;
    }
  }

  post_set();
}


bool HolderDoubleArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( idx < 0 || (unsigned)(idx) >= v.size() ) // slow, but safe - not for fast code
    return false;
  v[idx] = x.toDouble( &ok );
  post_set();
  return ok;
}

QVariant HolderDoubleArray::get( int idx ) const
{
  if( idx < 0 || (unsigned)idx >= v.size() ) // slow, but safe - not for fast code
    return QVariant();
  return QVariant( v[idx] );
}

void HolderDoubleArray::post_set()
{
  double v_min { DMIN }, v_max { DMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() ) 
    v_min = s_min.toDouble();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) 
    v_max = s_max.toDouble();
  for( double& vc : v ) {
    vc = qBound( v_min, vc, v_max );
  }
}

QString HolderDoubleArray::toString() const
{
  QString s, sep = "";
  for( double vc : v ) {
    s += sep + QSN( vc );
    sep = " ";
  }
  return s;
}

bool HolderDoubleArray::fromString( const QString &s )
{
  bool ok;
  QStringList sl = s.split(" ", QString::SkipEmptyParts );
  v.clear(); v.reserve( sl.size() );
  double vc;

  for( auto s : sl ) {
    vc = s.toDouble( &ok );
    v.push_back( vc );
  }

  post_set();
  return ok; // ? only last
}


QString HolderDoubleArray::getType() const
{
  return "double[]";
}

const char* HolderDoubleArray::helpstr { "Contains vector of double data" };


DEFAULT_FUNCS_REG(HolderDoubleArray);


// ---------------- HolderStringArray ---------
STD_CLASSINFO_ALIAS(HolderStringArray,clpData|clpArray,string[]);

CTOR(HolderStringArray,HolderData)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_STRING" );
  }
}

HolderStringArray::~HolderStringArray()
{
  dyn = 0;
}

void HolderStringArray::reset_dfl()
{
  int n = 1;
  QString s = getParm("N");
  if( ! s.isEmpty() )
    n = s.toInt();
  QString sd = getParm("def");

  QStringList sl;
  s = getParm("defs");
  if( ! s.isEmpty() ) {
    sl = s.split( '\x01', QString::KeepEmptyParts ); // keep here
    if( sl.size() > n )
      n = sl.size();
  }
  
  v.clear();
  int nds = sl.size();
  for( int i=0; i<n; ++i ) {
    QString vc;
    if( i < nds )
      vc = sl[i];
    else
      vc = sd;
    v.append( vc );
  }

  post_set();
}


bool HolderStringArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( idx < 0 || idx >= v.size() ) // slow, but safe - not for fast code
    return false;
  v[idx] = x.toString();
  post_set();
  return ok;
}

QVariant HolderStringArray::get( int idx ) const
{
  if( idx < 0 || idx >= v.size() ) // slow, but safe - not for fast code
    return QVariant();
  return QVariant( v[idx] );
}

void HolderStringArray::post_set()
{
  int len_max = IMAX;
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) {
    len_max = s_max.toInt();
    for( QString& vc : v ) {
      vc.truncate(len_max);
    }
  }
}

QString HolderStringArray::toString() const
{
  return v.join("\x01");
}

bool HolderStringArray::fromString( const QString &s )
{
  v = s.split('\x01', QString::KeepEmptyParts );

  post_set();
  return !v.isEmpty(); 
}


QString HolderStringArray::getType() const
{
  return "string[]";
}

const char* HolderStringArray::helpstr { "Contains QStringList" };


DEFAULT_FUNCS_REG(HolderStringArray);



// ---------------- TDataSet ------------------------
STD_CLASSINFO(TDataSet,clpSpecial);


const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

CTOR(TDataSet,HolderData)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "OBJ" );
  }
}

TDataSet::~TDataSet()
{
  // DBGx( "dbg:  %p %s", this, qP( getFullName() ));
  state = stateBad; guard = 0;
}

bool TDataSet::isObject( const QString & cl_name  ) const
{
  if( cl_name.isEmpty() )
    return true;
  if( isChildOf( cl_name ) )
    return true;

  return false;
}

void TDataSet::reset_dfl()
{
  for( auto c : children() ) {
    HolderData *o = qobject_cast<HolderData*>( c );
    if( o ) {
      o->reset_dfl();
    } else {
      DBGx( "warn: non-holder element \"%s\" in \"%s\"", 
          qP( c->objectName()), qP( objectName() ) );
    }
  }
}



QString TDataSet::getType() const
{ 
  return metaObject()->className();
}

DEFAULT_FUNCS_REG(TDataSet);


QStringList TDataSet::elemNames() const
{
  QStringList cl;
  for( const auto c : children() ) {
    cl << c->objectName();
  }
  return cl;
}

HolderData* TDataSet::getElem( const QString &oname ) const
{
  // bad, but Qt provide only recusive search FIXME: Qt >= 5
  for( auto c : children() ) {
    if( c->objectName() == oname )
      return qobject_cast<HolderData*>( c );
  }
  return nullptr;
}

HolderData* TDataSet::getElem( int i ) const
{
  auto cs = children()[i];
  return qobject_cast<HolderData*>(cs);
}

int TDataSet::indexOfHolder( HolderData *ho ) const
{
  return children().indexOf( ho );
}


HolderData* TDataSet::getObj( const QString &ename, const QString &cl_name )
{
  HolderData *ho = getElem( ename );
  if( !ho ) {
    DBGx( "ERR: not found element \"%s\"", qP(ename) );
    return nullptr;
  }
  if( ! ho->isObject( cl_name ) ){
    DBGx( "ERR: element \"%s\" in %s has type \"%s\", need \"%s\"",
	qP(ename), qP(getFullName()), qP(ho->getType()), qP(cl_name) );
    return nullptr;
  }
  return ho;
}


bool TDataSet::getData( const QString &nm, QVariant &da ) const
{
  if( nm.isEmpty() )
    return 0;
  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    DBGx( "warn: bad target name \"%s\"", qP( nm ) );
    return 0;
  }
  
  HolderData *ho = getElem( first );
  if( !ho ) {
    DBGx( "warn: fail to find name \"%s\"", qP( first ) );
    return 0;
  }
  if( nm_type == simpleName ) { // first only 
    da = ho->get( idx ); // TODO: check for slimple data?
    return 1;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType ) {
    DBGx( "warn: compound name required (1). first: \"%s\" rest: \"%s\" tp: %d", 
        qP(first), qP(rest), (int)(ho->getTp()) );
    return 0;
  }
  if( ! ho->isObject() ) {
    DBGx( "warn: compound name required (2). first: \"%s\" rest: \"%s\" obj \"%s\" type \"%s\"", 
        qP(first), qP(rest), qP(ho->objectName()), qP(ho->getType()) );
    return 0;
  }
  
  return ho->getData( rest, da );
}


bool TDataSet::getData( const QString &nm, int *da ) const
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

bool TDataSet::getData( const QString &nm, double *da ) const
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


bool TDataSet::getData( const QString &nm, QString &da ) const
{
  QVariant vda;
  int rc = getData( nm, vda );
  if( rc == 0 )
    return 0;
  da = vda.toString();
  return 1;
}


bool TDataSet::setData( const QString &nm, const QVariant &da )
{
  if( nm.isEmpty() )
    return 0;
  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    DBG2q( "warn: bad target name", nm );
    return 0;
  }
  
  HolderData *ho = getElem( first );
  if( !ho ) {
    DBG2q( "warn: fail to find name", first );
    return 0;
  }
  if( nm_type == simpleName ) { // first only 
    return ho->set( da, idx );
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    DBG2q( "warn: compound name required ", first );
    return 0;
  }
  
  return ho->setData( rest, da );
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

  int idx;
  NameType nm_type = splitName( nmf, first, rest, idx );
  if( nm_type == badName ) {
    if( lt )
      *lt = LinkBad;
    DBG2q( "warn: bad target name", nmf );
    return 0;
  }

  HolderData *ho = getElem( first );
  
  if( !ho ) {
    if( lt )
      *lt = LinkBad;
    DBGx( "warn: fail to find name \"%s\" in \"%s\"", qP(first), qP(objectName()) );
    return 0;
  }
  
  if( nm_type == simpleName ) { // first only
    if( ho->isObject() ) {
      TDataSet *ds= qobject_cast<TDataSet*>(ho);
      return ds->getDoublePtr( "out0", lt, lev+1 );
    }
    if( ho->getTp() == QVariant::Double ) {
      HolderDouble *hod = qobject_cast<HolderDouble*>(ho);
      if( !hod )
        return nullptr;
      if( lt ) {
	*lt = ( lev == 1 ) ? LinkElm : LinkSpec;
      }
      return hod->caddr();
    }
    // TODO: DoubleArray
    return nullptr;
  }

  // both part of name exists
  if( ! ho->isObject() ) {
    if( lt )
      *lt = LinkBad;
    return 0;
  }
  TDataSet *ds = qobject_cast<TDataSet*>(ho);
  return ds->getDoublePtr( rest, lt, lev+1 );
  
}

double* TDataSet::getDoublePrmPtr( const QString &nm, int *flg ) // TODO: why not const?
{
  if( nm.isEmpty() )
    return 0;
  
  HolderData *ho = getElem( nm );
  
  if( !ho ) {
    DBG2q( "warn: fail to find name", nm );
    return 0;
  }
  
  if( ho->getTp() == QVariant::Double ) {
    HolderDouble *hod = qobject_cast<HolderDouble*>(ho);
    if( !hod )
      return nullptr;
    if( flg )
      *flg = ho->getFlags();
    return hod->addr();
  }
  
  return 0;
}


// TODO: more args
HolderData* TDataSet::add_obj( const QString &cl_name, const QString &ob_name )
{
  if( ! ( allow_add & allowObject ) )
    return nullptr;
  HolderData *el = getElem( ob_name );
  if( el != nullptr ) {
    DBGx( "ERR: name \"%s\" (%s)  exist in \"%s\"!", 
          qP(ob_name), qP(el->getType()), qP( getFullName() ) );
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    DBGx( "WARN: type \"%s\" not allowed in \"%s\"!", 
          qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  HolderData *ob = EFACT.createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  reportStructChanged();
  
  return ob;
}

bool TDataSet::add_obj_param( const QString &cl_name, const QString &ob_name, 
     const QString &params )
{
  HolderData *ho = add_obj( cl_name, ob_name );
  if( ! ho )
    return false;
  
  QStringList sl = params.split( "\n", QString::SkipEmptyParts);
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );
  
  for( QString &s : sl ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      ho->setData( nm, val );
    } else {
      DBGx( "warn: bad param string part: \"%s\"", qP( s ) );
    }
  }
  return true;
}

int TDataSet::del_obj( const QString &ob_name )
{
  HolderData *ho = getElem( ob_name );
  if( !ho ) {
    DBG2q( "warn: not found element", ob_name );
    return 0;
  }
  if( ! ho->isDyn() ) {
    DBG2q( " object  is not created dynamicaly: ", ob_name );
    return 0;
  }
  delete ho; // auto remove object and from parent lists
  reportStructChanged();
  return 1;
}

HolderData* TDataSet::add_param( const QString &tp_name, const QString &ob_name )
{
  if( ! ( allow_add & allowParam ) )
    return nullptr;
  if( getElem( ob_name ) ) {
    DBGx( "ERR: name \"%s\" exist in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  HolderData *ho = EFACT.createElem( tp_name, ob_name, this );
  if( !ho ) {
    return nullptr;
  }
  reportStructChanged();
  return ho;
}



int TDataSet::isValidType(  const QString & /*cl_name*/  ) const
{
  return 1;
}

bool TDataSet::isChildOf( const QString &cname ) const
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


bool TDataSet::set( const QVariant & x, int /* idx */  )
{
  check_guard();
  return fromString( x.toString() );
}

QVariant TDataSet::get( int /* idx */ ) const
{
  check_guard();
  return QVariant( this->toString() );
}

void TDataSet::post_set()
{
  check_guard();
  for( auto e: children() ) { // propagate to childs
    HolderData* ho = qobject_cast<HolderData*>(e);
    if( !ho )
      continue;
    ho->post_set();
  }
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
  de.setAttribute( "otype", getType() );
  
  for( auto ch : children() ) {
    HolderData *ho = qobject_cast<HolderData*>(ch);
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
      if( cl_name.isEmpty() ) {
        errstr = QString( "err: element \"%1\" without type" ).arg(elname);
        DBG1q( errstr );
	return false;
      }
      HolderData *ho = getElem( elname );
      if( ho && ! ho->isObject() ) {
        errstr = QString("err: read elem failed \"%1\" is not a element: \"%2\" but \"%3\" in \"%4\"")
	         .arg(elname).arg(tagname).arg(ho->getType()).arg( getFullName() ); 
        DBG1q( errstr );
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
      ho = getElem( elname );
      if( !ho ) {
	errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 in %3")
		 .arg(cl_name).arg(elname).arg( objectName() ); 
        DBG1q( errstr );
	return false;
      }
      if( ! ho->isObject( cl_name ) ) {
	errstr = QString("TDataSet::fromDom: created obj %1 %2 in %3 is not given object ")
		 .arg(cl_name).arg(elname).arg( objectName() ); 
        DBG1q( errstr );
        DBGx( "ei: name: \"%s\" type: \"%s\"", qP(ho->objectName()), qP(ho->getType()) );
	return false;
      }

      TDataSet *ob = qobject_cast<TDataSet*>(ho);
      if( ! ob->fromDom( ee, errstr ) ) {
	return false;
      }
    
    } else if( tagname == "param" ) {  // ---------------- simple param
      QString tp_name = ee.attribute("otype");
      HolderData *ho = getElem( elname );
      if( ho && ho->isObject() ) {
	errstr = QString("TDataSet::fromDom: param \"%1\" is an object type \"%2\" ")
		 .arg(elname).arg(ho->getType()); 
        DBG1q( errstr );
	return false;
      }
      if( !ho ) {
	if( ! ( allow_add & allowParam ) ) {
	  DBGx( "WARN: creating param: \"%s\" disallowed in \"%s\"", qP(elname), qP(objectName()) );
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
      ho->fromString( getDomText(ee) );

    } else { // ----------- unknown element
      errstr = QString("TDataSet::fromDom: bad element %1 %2 ")
	       .arg(tagname).arg(elname); 
      DBG1q( errstr );
      return false;
    }
  }
  post_set();
  reportStructChanged();

  return true;
}

void TDataSet::check_guard() const
{
  if( guard != guard_val )  {
    DBG1( "ERR!!!!!!!!!! Guard value!!!");
    abort();
  }
}

void TDataSet::reportStructChanged()
{
  TDataSet *p = this, *np;
  while( (np = p->getParent()) != nullptr ) { // find root
    p = np;
  }
  // p is root now
  p->handleStructChanged();
}


void TDataSet::handleStructChanged()
{
  // TODO: handle suspend state: to simplify mass changes on load
  for( auto c : children() ) {
    TDataSet *ds = qobject_cast<TDataSet*>(c);
    if( ! ds )
      continue; // only datasets can handle 
    ds->handleStructChanged();
  }

  do_structChanged(); // functions to override
  
  if( ! par ) { // root signals about changes to world
    // DBG1( "dbg: root reports about change" );
    emit sigStructChanged();
  }
}

void TDataSet::do_structChanged()
{
}

void TDataSet::registerInput( InputSimple *inp )
{
  if( ! inp )
    return;
  if( inputs.indexOf( inp ) != -1 ) {
    DBG2q( "warn: input \"%s\" already registered", inp->objectName() );
    return;
  }
  inputs.push_back( inp );
}

void TDataSet::unregisterInput( InputSimple *inp )
{
  if( ! inp )
    return;
  int idx = inputs.indexOf( inp );
  if( idx == -1 ) {
    DBG2q( "warn: input \"%s\" not registered", inp->objectName() );
    return;
  }
  inputs.remove( idx );
}

const InputSimple* TDataSet::getInput (int n) const
{
  if( n < 0 || n >= inputs.size() ) {
    DBGx( "warn: bad input number %d, size= %d", n, inputs.size() );
    return nullptr;
  }
  return inputs[n];
}

void TDataSet::dumpStruct() const
{
  static int dump_lev = -1;
  ++dump_lev;
  DBGx( "* %d struct of %s %s this=%p", 
         dump_lev, qP(getType()), qP(objectName()), this );
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

// ------------------------------------ InputSimple ---------
STD_CLASSINFO(InputSimple,clpInput|clpSpecial);

const double InputSimple::fake_in {0};
const double InputSimple::one_in {1.0};

CTOR(InputSimple,TDataSet)
{
  if( par ) {
    par->registerInput( this );
  }
}

InputSimple::~InputSimple()
{
  if( par ) {
    par->unregisterInput( this );
  }
}


void InputSimple::post_set()
{
  TDataSet::post_set();
  reportStructChanged(); // changed link means changes structure
}

void InputSimple::do_structChanged()
{
  set_link();
}

void InputSimple::set_link()
{
  p = &fake_in;
  if ( source.cval() == ":one" ) { // special local case
    p = &one_in;
    return;
  }

  TDataSet *schem;
  if( !par || !(schem=par->getParent()) )
    return;
  ltype_t lt;
  const double *cp = schem->getDoublePtr( source, &lt, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;
    DBGx( "dbg: ptr set to target %p for \"%s\" in \"%s\"", 
        cp, qP(source), qP(getFullName())  );
  } else {
    DBGx( "dbg: ptr not found for \"%s\" in \"%s\"", 
        qP(source), qP(getFullName()) );
  }

  // more actions here
}


const char* InputSimple::helpstr { "Link to source of simple double data" };

DEFAULT_FUNCS_REG(InputSimple);


// end of dataset.cpp


