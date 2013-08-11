/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <QColor>
#include "miscfun.h"
#include "dataset.h"

using namespace std;


// ================================================================
// ---------------- HolderData .... ----------------------

HolderData::HolderData( const QString &obj_name, TDataSet *a_parent, 
         int a_flags, const QString &a_v_name,
	 const QString &a_descr, const QString &a_extra )
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
  DBGx( "dbg: HolderData::~HolderData(): %s par: %p %s", 
      qP( objectName() ), par, qP( par->objectName() )  );
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
TClassInfo HolderInt::class_info = {
  "int", HolderInt::create, helpstr, clpData
};

CTOR(HolderInt) , v(0)
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
  // TODO: implement
}


bool HolderInt::set( const QVariant & x )
{
  bool ok;
  v = x.toInt( &ok );
  post_set();
  return ok;
}

QVariant HolderInt::get() const
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
  return QString::number( v );
}

bool HolderInt::fromString( const QString &s )
{
  bool ok;
  int vc = s.toInt( &ok, 0 ); // 0 = auto base
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


DEFAULT_FUNCS_REG(HolderInt)


// ---------------- HolderSwitch ---------
TClassInfo HolderSwitch::class_info = {
  "switch", HolderSwitch::create, helpstr, clpData
};

HolderSwitch::HolderSwitch( const QString &obj_name, 
               TDataSet *a_parent, int a_flags, const QString &a_v_name,
	      const QString &a_descr,
	      const QString &a_extra )
     :HolderInt( obj_name, a_parent, a_flags, a_v_name, a_descr, a_extra )
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

DEFAULT_FUNCS_REG(HolderSwitch)


// ---------------- HolderList ---------
TClassInfo HolderList::class_info = {
  "list", HolderList::create, helpstr, clpData
};

HolderList::HolderList( const QString &obj_name, 
     TDataSet *a_parent, int a_flags,
     const QString &v_name, const QString &a_descr,
     const QString &a_extra,  const QString &a_elems  )
   :HolderInt( obj_name, a_parent, a_flags, v_name, a_descr, a_extra )
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


QString HolderList::getType() const
{
  return "list";
}

DEFAULT_FUNCS_REG(HolderList)



// ---------------- HolderDouble ---------
TClassInfo HolderDouble::class_info = {
  "double", HolderDouble::create, helpstr, clpData
};

CTOR(HolderDouble), v(0)
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


bool HolderDouble::set( const QVariant & x )
{
  bool ok;
  v = x.toDouble( &ok );
  post_set();
  return ok;
}

QVariant HolderDouble::get() const
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
  return QString::number( v, 'g', 16 ); // TODO? format
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


DEFAULT_FUNCS_REG(HolderDouble)


// ---------------- HolderString ---------
TClassInfo HolderString::class_info = {
  "string", HolderString::create, helpstr, clpData
};

CTOR(HolderString)
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

bool HolderString::set( const QVariant & x )
{
  v = x.toString();
  post_set();
  return true;
}

QVariant HolderString::get() const
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

DEFAULT_FUNCS_REG(HolderString)


// ---------------- HolderColor ---------
TClassInfo HolderColor::class_info = {
  "color", HolderColor::create, helpstr, clpData
};

CTOR(HolderColor)
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


bool HolderColor::set( const QVariant & x )
{
  QRgb rgba = x.toInt();
  v.setRgba( rgba );
  post_set();
  return true;
}

QVariant HolderColor::get() const
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
  return QString::number( (int)(v.rgba()) );
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

DEFAULT_FUNCS_REG(HolderColor)


// ---------------- TDataSet ------------------------

TClassInfo TDataSet::class_info = 
 {  "TDataSet",  TDataSet::create, helpstr, clpSpecial };


const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

CTOR(TDataSet)
{
}

TDataSet::~TDataSet()
{
  DBGx( "dbg:  %p %s", this, qP( getFullName() ));
  state = stateBad; guard = 0;
}



QString TDataSet::getType() const
{ 
  return metaObject()->className();
}

DEFAULT_FUNCS_REG(TDataSet)


int TDataSet::getNumObj() const
{
  return children().size();
}

QStringList TDataSet::elemNames() const
{
  const QObjectList &chi = children();
  QStringList cl;
  for( auto c : chi ) {
    cl << c->objectName();
  }
  return cl;
}

HolderData* TDataSet::getElem( const QString &oname ) const
{
  return findChild<HolderData*>(oname);
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
  int nm_type = splitName( nm, first, rest );
  if( nm_type == -1 ) {
    DBGx( "warn: bad target name \"%s\"", qP( nm ) );
    return 0;
  }
  
  HolderData *ho = getElem( first );
  if( !ho ) {
    DBGx( "warn: fail to find name \"%s\"", qP( first ) );
    return 0;
  }
  if( nm_type == 1 ) { // first only 
    da = ho->get(); // TODO: check for slimple data?
    return 1;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    DBG2q( "warn: compound name required ", first );
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
  int nm_type = splitName( nm, first, rest );
  if( nm_type == -1 ) {
    DBG2q( "warn: bad target name", nm );
    return 0;
  }
  
  HolderData *ho = getElem( first );
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

  int nm_type = splitName( nmf, first, rest );
  if( nm_type == -1 ) {
    if( lt )
      *lt = LinkBad;
    DBG2q( "warn: bad target name", nmf );
    return 0;
  }

  HolderData *ho = getElem( first );
  
  if( !ho ) {
    if( lt )
      *lt = LinkBad;
    DBG2q( "warn: fail to find name ", first );
    return 0;
  }
  
  if( nm_type == 1 ) { // first only
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


HolderData* TDataSet::add_obj( const QString &cl_name, const QString &ob_name )
{
  if( ! ( allow_add & allowObject ) )
    return nullptr;
  if( getElem( ob_name ) != nullptr ) {
    DBGx( "ERR: name \"%s\" exist in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    DBGx( "WARN: type \"%s\" not allowed in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  HolderData *ob = EFACT.createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  structChanged();
  
  return ob;
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
  structChanged();
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
      HolderData *ho = getElem( elname );
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
      ho = getElem( elname );
      if( !ho || !ho->isObject( cl_name ) ) {
	errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 ")
		 .arg(cl_name).arg(elname); 
        DBG1q( errstr );
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

// end of dataset.cpp


