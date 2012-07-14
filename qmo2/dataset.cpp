/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <vector>
#include <cstdio>
#include <limits>
#include <QColor>
#include "miscfun.h"
#include "dataset.h"

using namespace std;

// ---------------- TDataInfo -----------------------

// new really neeed only name type, and subtype -- all other fake
int TDataInfo::read( istream *is )
{
  int k, l, j, nline;
  char str[MAX_INPUTLEN];
  tp = subtp = 0; dlg_x = dlg_y = dlg_w = dlg_h = 0;
  v_max = 0; v_min = 1; // no limits
  flags = 0;
  name[0] = 0;
  descr = listdata = "";
  
  nline = 0;  // must consist of 4 lines of data (don't realy)
  while( 1 ) {
    is->getline( str, MAX_INPUTLEN );
    if( is->eof() || is->fail() )  
      return -1; 
    l = strlen( str );
    if( l < 1 ) continue;
    k = typeOfLine( str, 0, 0, 0, 0 );
    if( k == ltpComment ) continue;  // ordinary comment;
    if( k != ltpStruct )             // only @ -start strings;
       return -2;
    str[0] = ' ';
    switch( nline ) {
      case 0: j = sscanf( str, "%d %d %d %d %d %d %d %d %lf %lf",
                &tp, &subtp, &max_len, &dlg_x, &dlg_y, &dlg_w, &dlg_h,
                &flags, &v_min, &v_max );
	      if( j != 10 ) {
		qDebug( "ERR: TDataInfo::read(0): j=%d str=\"%s\"", j, str );
	      }
              k = ( j != 10 );  break;
      case 1: j = sscanf( str, "%32s" , name );
	      if( j != 1 ) {
		qDebug( "ERR: TDataInfo::read(1): j=%d", j );
	      }
              k =  (j != 1); 
	      break;
      case 2: k = 0; break;
      case 3: k = 0; break;
      default: fprintf( stderr, "Too many input lines (%d):TDataInfo::read", 
		        nline );
	       return -7;
    };
    nline++;
    if( nline >= 4 ) break;
    if( k ) { 
      qDebug( "ERR: TDataInfo::read(end): k=%d nline=%d", k, nline );
      return -3;
    };   
  };
  if( tp < 0 || !isGoodName(name) ) {
    return -1;
  };
  hval = hashVal( name );
  dyna = 1;
  return 0;
}

int TDataInfo::save( ostream *os ) const
{
  *os << "@ " << tp << ' ' << subtp << ' ' << max_len << ' '
      << dlg_x << ' ' << dlg_y << ' ' << dlg_w << ' ' << dlg_h << ' '
      << flags << ' ' << v_min << ' ' << v_max << '\n';
  *os << "@ " << name << '\n';
  *os << "@ \n@ \n";
  return 0;
}

// ================================================================
// ---------------- HolderData .... ----------------------

HolderData::HolderData( const QString &obj_name, 
              const QString &v_name, QObject *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
           :QObject( a_parent ), old_tp(0), old_subtp(0), 
	    dyn(0), flags(a_flags),
	    v_min(DMIN), v_max(DMAX),
	    tp(QVariant::Invalid), ptr(0), target_name( obj_name )
{
  setObjectName( "_HO_" + obj_name );

  if( v_name.isNull() )  {
    setParm( "vis_name", obj_name );
  } else {
    setParm( "vis_name", v_name );
  }
  setParm( "descr", a_descr );
  setParm( "extra", a_extra );
  extraToParm();
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

// tmp: to remove, use only set/getParm
void HolderData::setVisName( const QString &av_name ) 
{ 
  setParm( "vis_name", av_name );
}

QString HolderData::getVisName() const 
{
  return getParm( "vis_name" );
}

void HolderData::setDescr( const QString &a_descr ) 
{
  setParm( "descr", a_descr );
}

QString HolderData::getDescr() const 
{
  return getParm( "descr" );
}

void HolderData::setProps( const QString &a_prop )
{
  setParm( "props", a_prop );
}

QString HolderData::getProps() const
{
  return getParm( "props" );
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

bool HolderData::toOldStream( std::ostream &os ) const // = 0
{
  os << qPrintable( targetName() ) << "=";
  return true;
}

// ---------------- HolderInt ---------

HolderInt::HolderInt( int *p, const QString &obj_name, 
              const QString &v_name, QObject *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderData( obj_name, v_name, a_parent, a_flags, a_descr, a_extra ),
	   val(p)
{
  if( !val ) {
    val = new int; *val = (int)(v_min); dyn = 1;
  }
  ptr = val; tp=QVariant::Int; old_tp = dtpInt; old_subtp = dtpsInt;
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

bool HolderInt::toOldStream( std::ostream &os ) const
{
  HolderData::toOldStream( os );
  os << (*val) << " ; \n";
  return true;
}


QString HolderInt::getType() const
{
  return "int";
}

// ---------------- HolderSwitch ---------
HolderSwitch::HolderSwitch( int *p, const QString &obj_name, 
              const QString &v_name, QObject *a_parent, int a_flags,
	      const QString &a_descr,
	      const QString &a_extra )
          :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  old_subtp = dtpsSwitch;
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
     const QString &v_name, QObject *a_parent, int a_flags,
     const QString &a_descr,
     const QString &a_extra,  const QString &a_elems  )
   :HolderInt( p, obj_name, v_name, a_parent, a_flags, a_descr, a_extra )
{
  old_subtp = dtpsList;
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
              const QString &v_name,  QObject *a_parent, int a_flags,
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

bool HolderDouble::toOldStream( std::ostream &os ) const
{
  HolderData::toOldStream( os );
  os << (*val) << " ; \n";
  return true;
}


QString HolderDouble::getType() const
{
  return "double";
}


// ---------------- HolderString ---------

HolderString::HolderString( QString *p, const QString &obj_name,
              const QString &v_name,  QObject *a_parent, int a_flags,
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

bool HolderString::toOldStream( std::ostream &os ) const
{
  HolderData::toOldStream( os );
  saveStr( &os, qPrintable(*val) ); 
  os << " ; \n";
  return true;
}

QString HolderString::getType() const
{
  return "string";
}

// ---------------- HolderStringArr ---------

HolderStringArr::HolderStringArr( QString *p, int an, const QString &obj_name,
              const QString &v_name,  QObject *a_parent, int a_flags,
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

bool HolderStringArr::toOldStream( std::ostream &os ) const
{
  HolderData::toOldStream( os );
  os <<  "?????;\n"; // TODO: !!!!
  return false;
}

QString HolderStringArr::getType() const
{
  return "string[]";
}

// ---------------- HolderColor ---------

HolderColor::HolderColor( QColor *p, const QString &obj_name,
              const QString &v_name,  QObject *a_parent, int a_flags,
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
  ptr = val; tp=QVariant::Color; old_tp = dtpInt; old_subtp = dtpsColor;
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

bool HolderColor::toOldStream( std::ostream &os ) const
{
  HolderData::toOldStream( os );
  os <<  (  (int)(val->rgba())  ) << " ; \n";
  return true;
}

QString HolderColor::getType() const
{
  return "color";
}


// ---------------- HolderObj ---------

HolderObj::HolderObj( TDataSet *p, const QString &obj_name,
              const QString &v_name,  QObject *a_parent, int a_flags,
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
  old_subtp = obj->getClassId();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "OBJ" );
  }
  obj->setObjectName( obj_name );
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

bool HolderObj::toOldStream( std::ostream &os ) const
{
  // name=1000(Type). 1000 is old subtp
  os << qPrintable( targetName() ) << '=' << old_subtp
      << '(' << obj->getClassName() << ")\n";
  obj->saveDatasOld( os );
  return true; 
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

TDataInfo TDataSet::dataset_d_i[1] = {
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};

const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

TDataSet::TDataSet( TDataSet* aparent )
         :QObject( aparent )
{
 guard = guard_val;
 parent = aparent;
 nelm = 0; allow_add = 0; d_i_alloc = 0; state = stateGood; modified = 0;
 ptrs.reserve( 64 ); // good value for simple objects
 d_i = dataset_d_i;
 initHash();
}

TDataSet::~TDataSet()
{
  int i;
  TDataSet* ob;
  for( i=0; i<nelm; i++ ) {
    if( d_i[i].dyna == 0 && d_i[i].tp != dtpObj ) continue;
    ob = static_cast<TDataSet*>( ptrs[i] ); 
    delete ob;
    ptrs[i] = 0;
  };
  nelm = 0; state = stateBad;
  if( d_i_alloc ) {
    delete d_i; d_i = 0;
  };
}

int TDataSet::getClassId(void) const 
{ 
  return CLASS_ID_TDataSet; 
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
  cob = parent; // TODO? is first dot need 
  while( cob != 0 ) {
    tn = cob->objectName();
    tn += '.';
    res.prepend( tn );
    cob = cob->parent;
  };
  return res;
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

int TDataSet::getN(void) const
{
  return nelm;
}

void* TDataSet::getObj( const QString &ename )
{
  HolderData *ho = getHolder( ename );
  if( !ho ) {
    qDebug( "ERR: TDataSet::getObj: not found element \"%s\"",
	qPrintable(ename) );
  } 
  return ho->getPtr();
}


HolderData* TDataSet::getHolder( const QString &oname ) const
{
  QString ho_name = "_HO_" + oname; 
  HolderData* ret;
  for(QObject* child : children() ) {  // FIXME: bad, use map?
    ret = qobject_cast<HolderData*>( child );
    if( !ret )
      continue;
    if( child->objectName() == ho_name ) {
      return ret;
    }
  }

  // HolderData *ho = findChild<HolderData*>(ho_name); BAD: recursive
  return nullptr;
}


// TODO: use real hash or map !!! TODO: DROP at all!
int TDataSet::getDataIdx( const char *nm ) const
{
  int i, hv;
  hv = hashVal( nm );
  for( i=0; i<nelm; i++ ) {
    if( hv == d_i[i].hval ) {
      if( strcmp( d_i[i].name, nm ) == 0 )
        return i;
    };
  };
  return -1;
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

const double* TDataSet::getDoublePtr( const QString &nm ) const
{
  if( nm.isEmpty() )
    return 0;
  QString nmf = nm, first, rest;
  
  if( nm[0] == ':' ) { // handle old ':name' from TModel
    nmf.remove( 0, 1 );
  }

  int nm_type = splitName( nmf, first, rest );
  if( nm_type == -1 ) {
    qDebug( "TDataSet::setDoublePtr: bad target name \"%s\"",
	qPrintable( nmf ) );
    return 0;
  }

  HolderData *ho = getHolder( first );
  
  if( !ho ) {
    qDebug( "TDataSet::getDoublePtr: fail to find name \"%s\"",
	qPrintable( first ) );
    return 0;
  }
  
  if( nm_type == 1 ) { // first only
    if( ho->isObject() ) {
      HolderObj *hob= qobject_cast<HolderObj*>(ho);
      return hob->getObj()->getDoublePtr( "out0" );
    }
    if( ho->getOldTp() == dtpDouble ) {
      return static_cast<const double*>( ho->getPtr() );
    }
    return 0;
  }

  // both part of name exists
  if( ! ho->isObject() ) {
    return 0;
  }
  HolderObj *hob= qobject_cast<HolderObj*>(ho);
  return hob->getObj()->getDoublePtr( rest );
  
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

int TDataSet::saveDatasOld( ostream &os )
{
  os << getClassId() << '(' << getClassName() << ")={\n";
  // some debug? values
  os << "%!d n=" << nelm << "; allow_add=" << allow_add << '\n';
  
  QObjectList childs = children();
  
  for( auto o : childs ) {
    QObject *xo = o;
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    if( ho->getFlags() & ( efNoSave | efStatic ) )
      continue;
    ho->toOldStream( os );
  }
  
  os << "}; -- end of " << getClassName() << " ; \n";
  modified = 0;
  return 0;
}

int TDataSet::loadDatas( istream *is )
{
  int k, r_id;
  //const char *mcn;
  char str[MAX_INPUTLEN], nm[MAX_NAMELEN];
  //mcn = getClassName(); //lm = strlen( mcn );
  while( 1 ) {                           // find: id(ClassName)={
    is->getline( str, MAX_INPUTLEN );  
    if( !is->good() ) return -1;
    k = typeOfLine( str, MAX_INPUTLEN, &r_id, nm, 0 );
    if( k == ltpComment ) continue;
    if( k != ltpStart ) return -2;
    break;
  };    
  if( r_id != getClassId() ) return -3;  // not my class ??
  // atu: debug:
  //fprintf( stderr, "TDataSet::loadDatas: reading object %d(%s)\n",
  //         r_id, nm );
  
  k = ltpComment; 
  while( k > 0 && k != ltpEnd ) {
    k = processElem( is );
  };    
  checkData(-1);
  state = 1; modified = 0;
  return 0;
}

// protected:
int TDataSet::initHash(void)
{
   int i, hv;
   nelm = 0; 
   for( nelm=0; nelm < MAX_DATAELEM && d_i[nelm].tp !=0; nelm++ ) { /*NOP */} ;
   for( i=0; i<nelm; i++ ) {
     hv = hashVal( d_i[i].name );
     d_i[i].hval = hv;
   };
   return 0;
}


int TDataSet::processElem( istream *is )
{
  int i, k, l;
  TDataSet* ob;
  QString val, delim, tbuf;
  char nm[MAX_NAMELEN];
  char str[MAX_INPUTLEN];
  TDataInfo inf;
  inf.descr = inf.listdata = 0; // safe values 
  is->getline( str, MAX_INPUTLEN );
  if( is->fail() ) return -1;
  k = typeOfLine( str, MAX_INPUTLEN, &l, nm, &val );
  if( k == ltpEnd ) {
    //qDebug( "dbg: TDataSet::processElem: ltpEnd in %s:\"%s\"", 
    //  qPrintable(getFullName()), str );
    return k;
  }
  if( k == ltpUnk ) {
    qDebug( "WARN: TDataSet::processElem: unknown line:\"%s\"", str );
    return k;
  };
  if( k == ltpComment ) return ltpComment;
  
  if( k == ltpVal ) {  // name=value
    
    HolderData *ho = getHolder( nm );
    if( !ho ) { // name not found
      //qDebug( "dbg: TDataSet::processElem: unknown name:\"%s\"=\"%s\" in %s %s;",
      //     nm, qPrintable(val), 
      //     getClassName(), qPrintable( getFullName() ) );
      if( ! allow_add ) {
        return ltpComment;
      }
      QRegExp re_objtype( "\\d+\\(([a-zA-Z0-9_]+)\\)" );
      if( re_objtype.indexIn( QString( val ) ) == -1 ) {
	qDebug( "ERR: TDataSet::processElem: bad object type string:\"%s\"=\"%s\"",
	       nm, qPrintable(val) );
	return ltpComment;
      }
      QString cl_name = re_objtype.cap(1);
      if( ! add_obj( cl_name, nm ) ) {
	qDebug( "ERR: TDataSet::processElem: fail to create obj %s %s ",
	       qPrintable(cl_name), nm );
	return ltpComment;
      }
      ho = getHolder( nm );
      //qDebug( "dbg: TDataSet::processElem: created %s %s ;",
      //       qPrintable(cl_name), nm );
    } else {
      //qDebug( "dbg: TDataSet::processElem: exists %s type %s in %s;",
      //       nm, qPrintable(ho->getType()), qPrintable( getFullName() ) );
    }
    
    if( !ho->isObject() ) { // simple object
      //qDebug( "dbg: TDataSet::processElem: simple object %s.%s = \"%s\" ;",
      //        qPrintable( getFullName() ), nm, qPrintable(val) );
      ho->set( val );
      return ltpVal;
    }

    // object
    HolderObj *hob = qobject_cast<HolderObj*>(ho);
    ob = hob->getObj();
    if( !ob ) {
      qDebug( "ERR: TDataSet::processElem: fail to find obj \"%s\";", nm );
      dumpStruct();
      abort();
      return -7;
    }
    ob->loadDatas( is );
    return ltpVal;
  };
  
  if( k == ltpValStart ) {  // multi-line string value
    delim = val;
    //qDebug( "dbg: TDataSet::processElem: ltpValStart :\"%s\" in %s %s; val= \"%s\"",
    //	   nm, getClassName(), qPrintable(objectName()), qPrintable(delim));
    HolderData *ho = getHolder( nm );
    if( ! ho ) {
      qDebug( "ERR: TDataSet::processElem: unknown longstr name:\"%s\" in %s %s;",
	     nm, getClassName(), qPrintable(objectName()));
      return -3;
    }
    if( ho->getOldTp() != dtpStr )  {
      qDebug( "ERR: TDataSet::processElem: bad type longstr \"%s\"  in %s %s;",
	     nm, getClassName(), qPrintable(objectName()));
      return -4; 
    }
    i = readMlStr( is, &tbuf, ho->getMax(), qPrintable(delim) );
    //qDebug( "dbg: TDataSet::processElem: readed long str i=%d \"%s\";",
    //	     i, qPrintable(tbuf) );
    if( i ) { 
      qDebug( "ERR: TDataSet::processElem: fail to read long str \"%s\"  in %s %s;",
	     nm, getClassName(), qPrintable(objectName()));
      return -8; 
    };
    ho->set( QString(tbuf) );
    return ltpValStart;
  };
  
  if( k == ltpStruct ) { // structure def comment -- first line empty
    // ignore @- lines now
    return ltpComment;
  };
  return 0;
}


void* TDataSet::add_obj( const QString &cl_name, const QString &ob_name )
{
  if( !allow_add )
    return nullptr;
  if( getHolder( ob_name ) != nullptr ) {
    qDebug( "ERR: TDataSet::add_obj: name \"%s\" exist int %s!",
	qPrintable(ob_name), qPrintable( getFullName() ) );
    return nullptr;
  }
  // TODO: no simple types for now!
  TDataSet *ob = ElemFactory::theFactory().createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  ob->check_guard();
  const TClassInfo *cl = ElemFactory::theFactory().getInfo( cl_name );
  
  // TODO: KILL! legacy part // most values is fake!
  ptrs.push_back(0);
  ptrs[nelm] = ob;
  d_i[nelm].tp = dtpObj; 
  d_i[nelm].subtp = cl->id;
  d_i[nelm].max_len = 0;
  d_i[nelm].dlg_x = 0; d_i[nelm].dlg_y = 0;
  d_i[nelm].dlg_w = 0; d_i[nelm].dlg_h = 0;
  d_i[nelm].v_min = 0; d_i[nelm].v_max = 0;
  d_i[nelm].dyna = 1; d_i[nelm].flags = 0;
  d_i[nelm].name[0] = 0;
  strncat( d_i[nelm].name, qPrintable(ob_name), MAX_NAMELEN-1 );
  d_i[nelm].descr = "";
  d_i[nelm].listdata = "";
  d_i[nelm].hval = hashVal( d_i[nelm].name );
  nelm++;
  d_i[nelm].tp = dtpEnd; ptrs[nelm]=0;
  return ob;
}

int TDataSet::del_obj( int n_ptrs )
{
  int i, *i_ob;
  double *d_ob; char *s_ob;
  TDataSet* ob;
  if( !allow_add || !d_i_alloc
       || n_ptrs >= nelm || n_ptrs < 0 ) return -1;
  if( d_i[n_ptrs].dyna == 0 ) return -2;
  switch( d_i[n_ptrs].tp ) {
    case dtpInt: i_ob = (int*)ptrs[n_ptrs];
                 delete i_ob; break;
    case dtpDbl: d_ob = (double*)ptrs[n_ptrs];
                 delete d_ob; break;
    case dtpStr: s_ob = (char*)ptrs[n_ptrs];
                 delete[] s_ob; break;
    case dtpObj: ob = static_cast<TDataSet*>( ptrs[n_ptrs] );
		 QString ho_name = "_HO_" + ob->objectName();
                 delete ob; 
		 HolderObj* hob = findChild<HolderObj*>(ho_name);
		 if( hob ) {
		   delete hob;
		 } else {
		   qDebug( "WARN: fail to find to del holder %s", qPrintable(ho_name) );
		 }
		 break;
  };
  ptrs[n_ptrs] = 0;
  //delete[] d_i[n_ptrs].descr;
  //delete[] d_i[n_ptrs].listdata;
  for( i=n_ptrs; i<nelm-1; i++ ) {
    ptrs[i] = ptrs[i+1];
    d_i[i] = d_i[i+1];
  };
  ptrs[nelm] = 0; d_i[nelm].tp = dtpEnd; nelm--;
  return 0;
}

int TDataSet::isValidType( int /* a_tp*/, int /* a_subtp */ ) const
{
  return 1;
}

int TDataSet::isChildOf( const char *cname )
{
  check_guard();
  const TClassInfo *ci, *pci;
  int l;
  l = strlen( cname );
  if( l < 1 || l >=MAX_NAMELEN ) return 0;
  if( strcmp( getClassName(), cname ) == 0 )
    return 1;
  ci = getClassInfo();    
  while( (pci = ci->parent_class) != 0 ) {
    if( strcmp( pci->className, cname ) == 0 )
      return 1;
    ci = pci;
  };
  return 0;
}

int TDataSet::isChildOf( int cid )
{
  check_guard();
  const TClassInfo *ci, *pci;
  if( cid == getClassId() )
    return 1;
  ci = getClassInfo();        
  while( (pci = ci->parent_class) != 0 ) {
    if( pci->id == cid )
      return 1;
    ci = pci;
  };
  return 0;
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
  static int lev = -1; 
  ++lev;
  QString buf;
  buf.reserve(4096); // TODO ?
  QObjectList childs = children();

  
  for( auto o : childs ) {
    QObject *xo = o;
    if( ! xo->inherits("HolderData" )) {
      continue;
    }
    HolderData *ho = qobject_cast<HolderData*>(xo);
    buf += QString( lev*2, QChar(' ') ); // for indent (human view). Is needed?
    if( ho->inherits( "HolderObj" ) ) {
      buf += ho->targetName() + " = {\n" + ho->toString() + "}\n"; 
    } else {
      buf += ho->targetName() + " = \"" + quoteString( ho->toString() ) + "\"\n"; 
    }
  }
  --lev;
  return buf; 
}

bool TDataSet::fromString( const QString & /*s*/ )
{
  return false; // TODO;
}

QDomElement TDataSet::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "obj" );
  de.setAttribute( "name", objectName() );
  de.setAttribute( "otype", getClassName() );
  
  QObjectList childs = children();
  for( auto o :  childs ) {
    QObject *xo = o;
    qDebug( "debug: TDataSet::toDom type: %s name: %s",
	qPrintable( xo->metaObject()->className() ),
	qPrintable( xo->objectName() )
    );
    if( xo->inherits("HolderData" )) {
      HolderData *ho = qobject_cast<HolderData*>(xo);
      if( !ho )
	continue; // but how?
      if( ho->getFlags() & efNoSave )
	continue;
      QDomElement cde = ho->toDom( dd );
      de.appendChild( cde );
    }

    // TODO: drop this! add/remove holder with object
    //if( xo->inherits("TDataSet" )) {
    //  TDataSet *ob = qobject_cast<TDataSet*>(xo);
    //  QDomElement cde = ob->toDom( dd );
    //  de.appendChild( cde );
    //}
  }
  
  return de;
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
  qDebug( "*%d struct of %s %s nelm=%d this=%p", 
         dump_lev, getClassName(), qPrintable(objectName()), nelm, this );
  for( int i=0; i<nelm; ++i ) {
    qDebug( "** [%d] %s %d.%d = %p", 
	i, d_i[i].name, d_i[i].tp, d_i[i].subtp, ptrs[i] );
  }
  // new part
  qDebug( "----- new part --" );
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


