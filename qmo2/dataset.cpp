/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QColor>
#include "miscfun.h"
#include "dataset.h"

using namespace std;


#define ERR_ABS "ERR: abstract class function called"

// ================================================================
// ---------------- HolderData .... ----------------------
STD_CLASSINFO(HolderData,clpSpecial|clpPure);

HolderData::HolderData( ARGS_CTOR_MIN )
     :QAbstractItemModel( a_parent ),
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

HolderData* HolderData::create( const QString &, HolderData *,
         int, const QString &, const QString &, const QString & )
{
  DBG1( "Attempt to create abstract class" );
  return nullptr;
}

// QAbstractItemModel part
int HolderData::columnCount( const QModelIndex & /*par*/ ) const
{
  return 3;
}

// //* function to debug model indexes
// QString idx2s( const QModelIndex &idx )
// {
//   QString r = "[ ";
//   if( idx.isValid() ) {
//     r += "V ";
//   } else {
//     r += "X ";
//   }
//   void *p = idx.internalPointer();
//   r += QSN( idx.row() ) + " " + QSN( idx.column() ) + " " + QSN( (uint64_t)(p) );
//   if( p ) {
//     const HolderData *ho = static_cast<HolderData*>(p);
//     r += "=\"" + ho->getFullName() + "\"";
//   }
//
//   r += " ]";
//   return r;
// }

int HolderData::rowCount( const QModelIndex &par ) const
{
  int sz = size();
  // DBGx( "dbg: (rC0) par: %s sz: %d this: %s", qP(idx2s(par)), sz, qP( getFullName() ) );
  if( ! par.isValid() ) { // invalid: assume self (examples: root)
    return sz;
  }

  HolderData *po = static_cast<HolderData*>( par.internalPointer() );
  if( !po ) {
    // DBG1( "dbg: (rC1) po=0 sz=0" );
    return 0;
  }
  sz = po->size();
  // DBGx( "dbg: (rC2) sz=%d %s", sz, qP( po->getFullName() ) );
  return sz;
}

QVariant HolderData::data( const QModelIndex &idx, int role ) const
{
  // DBGx( "dbg: idx: %s this: %s role: %d", qP(idx2s(idx)), qP( getFullName() ), role );
  if( !idx.isValid() ) {
    return QVariant();
  }
  int c = idx.column();

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    return QVariant();
  }

  // DBGx( "dbg: ds: %s", qP( ds->getFullName() ) );
  return ds->dataObj( c, role );
}

QVariant HolderData::dataObj( int col, int role ) const
{
  // no switch: too complex
  if( role == Qt::DisplayRole ) {
    QString s;
    s = QString( "c " ) + QSN( col ); // fallback value
    switch( col ) {
      case 0:
        s = objectName(); break;
      case 1:
        s = getType(); break;
      case 2:
        if( size() == 0 ) {
          getData( "", s );
        } else {
          s = QSL("[") + QSN( size() ) + QSL("]");
        }
        break;
      default:
        break;
    }
    return s;
  }

  if( role == Qt::CheckStateRole ) {
    if( !show_active || col != 0 ) {
      return QVariant();
    }
    return ( par  &&  par->getActiveElem() == this );
  }

  if( role == Qt::DecorationRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    return getIcon();
  }

  return QVariant();
}


QModelIndex HolderData::index( int row, int column, const QModelIndex &par ) const
{
  //DBGx( "dbg: row=%d column=%d this: %s par: %s", row, column, qP( getFullName() ), qP(idx2s(par)) );

  if( ! hasIndex( row, column, par ) ) {
    // DBG1( "dbg: no index" );
    return QModelIndex();
  }

  const HolderData *d_par = static_cast<HolderData*>( par.internalPointer() );
  if( !d_par ) {
    // DBG1( "dbg: no d_par, set from this" );
    d_par = this;
  }
  // DBGx( "dbg: d_par: %s",  qP( d_par->getFullName() ) );

  const HolderData *d_t = qobject_cast<HolderData*>( d_par->getElem( row ) );
  if( ! d_t ) {
    // DBG1( "dbg: no d_t, return X" );
    return QModelIndex();
  }
  // DBGx( "dbg: d_t: %s",  qP( d_t->getFullName() ) );

  return createIndex( row, column, const_cast<HolderData*>(d_t) );
}

QModelIndex HolderData::parent( const QModelIndex & idx ) const
{
  // DBGx( "dbg: this: %s idx: %s", qP( getFullName() ), qP(idx2s(idx)) );
  if( ! idx.isValid() ) {
    return QModelIndex();
  }

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    // DBG1( "dbg: ds=0, return X" );
    return QModelIndex();
  }
  // DBGx( "dbg: ds: %s", qP( ds->getFullName() ) );

  HolderData *ds_p = ds->getParent();
  if( !ds_p ) {
    // DBG1( "dbg: ds_p=0, return X" );
    return QModelIndex();
  }
  if( ds_p == this ) {
    // DBG1( "dbg: ds_p=this, return X" );
    return QModelIndex();
  }

  // DBGx( "dbg: ds_p: %s", qP( ds_p->getFullName() ) );

  // QModelIndex ret =  createIndex( ds_p->getMyIndexInParent(), 0, ds_p  );
  // DBGx("dbg: ret= %s", qP(idx2s(ret) ) );
  // return ret;
  return createIndex( ds_p->getMyIndexInParent(), 0, ds_p );
}

// my part

int HolderData::indexOfHolder( const HolderData *ho ) const
{
  // children holds <QObject*>, but indexOf dont touch *it
  return children().indexOf( const_cast<HolderData*>(ho) );
}


bool HolderData::isChildOf( const QString &cname ) const
{
  // DBGx( "dbg: this: \"%s\" type: \"%s\" cname: \"%s\"", qP( getFullName() ), qP( getType() ), qP( cname ) );
  const QMetaObject *mob = metaObject();
  while( mob ) {
    // DBGx( "dbg: className(): \"%s\" cmp \"%s\"", qP( mob->className() ), qP( cname ) );
    if( cname == mob->className() ) {
      // DBG1( "dbg: true" );
      return true;
    }
    mob = mob->superClass();
  }
  // DBG1( "dbg: false" );
  return false;
}

QStringList HolderData::elemNames() const
{
  QStringList cl;
  for( const auto c : children() ) {
    cl << c->objectName();
  }
  return cl;
}


HolderData* HolderData::getElem( int i ) const
{
  if( i < size() && i >=0 ) {
    return qobject_cast<HolderData*>(children().at(i));
  }
  return nullptr;
}


HolderData* HolderData::getElem( const QString &oname ) const
{
  QString first, rest;
  int idx = 0;

  NameType nt = splitName( oname, first, rest, idx );

  if( nt == badName ) {
    return nullptr;
  }

  HolderData *ho =  findChild<HolderData*>( first, Qt::FindDirectChildrenOnly );
  if( !ho ) { // need, as here both simpleName and complexName
    return nullptr;
  }

  if( nt == simpleName ) {
    return ho;
  }

  if( nt == complexName ) {
    return ho->getElem( rest );
  }

  DBGx( "warn: unknown name type: %d", nt );
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

bool HolderData::setParams( const QString params )
{
  bool was_set = false;
  QStringList sl = params.split( "\n", QString::SkipEmptyParts);
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );

  for( QString &s : sl ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      was_set = setData( nm, val ) || was_set; // Order!
    } else {
      DBGx( "warn: bad param string part: \"%s\"", qP( s ) );
    }
  }
  return was_set;
}

bool HolderData::isObject( const QString & cl_name ) const
{
  QString cn = cl_name;
  if( cn.isEmpty() ) {
    cn = QSL( "TDataSet" );
  }

  return isChildOf( cn );
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


QString HolderData::getTypeV() const // = 0;
{
  return "None";
}

int HolderData::checkData( int /* ni */ )
{
  return 0;
}

// void HolderData::check_guard() const
// {
//   if( guard != guard_val )  {
//     DBG1( "ERR!!!!!!!!!! Guard value!!!");
//     abort();
//   }
// }

void HolderData::reportStructChanged()
{
  HolderData *p = this, *np;
  while( (np = p->getParent()) != nullptr ) { // find root
    p = np;
  }
  // p is root now
  p->handleStructChanged();
}


void HolderData::handleStructChanged()
{
  if( updSuspended )
    return;

  for( auto c : children() ) {
    HolderData *ds = qobject_cast<HolderData*>(c);
    if( ! ds )
      continue; // what?
    ds->handleStructChanged();
  }

  do_structChanged(); // functions to override


  if( ! par ) { // root signals about changes to world? only root?
    // DBG1( "dbg: root reports about change" );
    emit sigStructChanged();
    beginResetModel(); // ???
    endResetModel();
  }
}

void HolderData::do_structChanged()
{
}

QIcon HolderData::getIcon() const
{
  QString iconName = QString( ":icons/elm_" ) + getType().toLower() + ".png";
  QIcon el_ico( iconName );
  return el_ico;
}

// TODO: more args
HolderData* HolderData::add_obj( const QString &cl_name, const QString &ob_name )
{
  //DBGx( "dbg: try to add \"%s\" type \"%s\" to \"%s\"",
  //    qP(ob_name), qP(cl_name), qP( getFullName() ) );
  // if( ! ( allow_add & allowObject ) )
  //   return nullptr;
  beginResetModel();

  HolderData *el = getElem( ob_name );
  if( el != nullptr ) {
    DBGx( "ERR: name \"%s\" (%s)  exist in \"%s\"!",
          qP(ob_name), qP(el->getType()), qP( getFullName() ) );
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    DBGx( "WARN: type \"%s\" for \"%s\" not allowed in \"%s\"!",
          qP(cl_name), qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  HolderData *ob = EFACT.createElem( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  endResetModel();
  reportStructChanged();

  return ob;
}

bool HolderData::add_obj_param( const QString &cl_name, const QString &ob_name,
     const QString &params )
{
  HolderData *ho = add_obj( cl_name, ob_name );
  if( ! ho )
    return false;

  ho->setParams( params );

  return true;
}

int HolderData::del_obj( const QString &ob_name )
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
  if( ho->getFlags() & efImmutable ) {
    DBGx( "warn: element \"%s\" in \"%s\" is Immutable", qP(ob_name), qP(getFullName()) );
    return 0;
  }

  HolderData *act_obj = getActiveElem();
  QString act_name;
  if( act_obj ) {
    act_name = act_obj->objectName();
  }

  beginResetModel();

  delete ho; // auto remove object and from parent lists
  if( !act_name.isEmpty() ) {
    setActiveElem( act_name );
  }

  endResetModel();

  reportStructChanged();
  return 1;
}

int HolderData::rename_obj( const QString &ob_name, const QString &new_name )
{
  if( ! isGoodName( new_name ) ) {
    DBGx( "warn: bad name \"%s\" to remame in \"%s\"", qP(new_name), qP(getFullName()) );
    return 0;
  }
  HolderData *ho = getElem( ob_name );
  if( !ho ) {
    DBGx( "warn: not found element \"%s\" in \"%s\"", qP(ob_name), qP(getFullName()) );
    return 0;
  }
  if( ho->getFlags() & efImmutable ) {
    DBGx( "warn: element \"%s\" in \"%s\" is Immutable", qP(ob_name), qP(getFullName()) );
    return 0;
  }

  if( getElem( new_name ) ) {
    DBGx( "warn: element \"%s\" already exists in \"%s\"", qP(new_name), qP(getFullName()) );
    return 0;
  }
  if( ! ho->isDyn() ) {
    DBGx( "warn: object \"%s\" is not created dynamicaly: ", qP(ob_name) );
    return 0;
  }
  ho->setObjectName( new_name );

  reportStructChanged();
  return 1;
}



void HolderData::setActiveIdx( int i )
{
  active_idx = i;
  if( par ) {
    QModelIndex mi1 = createIndex( 0, 0, par );
    QModelIndex mi2 = createIndex( par->size()-1, 0, par );
    emit dataChanged( mi1, mi2 );
  }
};

void HolderData::setActiveElem( const QString &nm )
{
  int a_idx = -1; HolderData *e = getElem( nm );
  if( e ) {
    a_idx = indexOfHolder( e );
  }
  setActiveIdx( a_idx );
};


HolderData* HolderData::add_param( const QString &tp_name, const QString &ob_name )
{
  if( getElem( ob_name ) ) {
    DBGx( "ERR: name \"%s\" exist in %s!", qP(ob_name), qP( getFullName() ) );
    return nullptr;
  }
  if( !isValidType( tp_name ) ) {
    return nullptr;
  }

  beginResetModel();
  HolderData *ho = EFACT.createElem( tp_name, ob_name, this );
  if( !ho ) {
    return nullptr;
  }
  endResetModel();

  reportStructChanged();
  return ho;
}



int HolderData::isValidType(  const QString &cl_name  ) const
{
  // DBGx( "dbg: allowTypes: \"%s\" for %s", allowTypes(), qP(getType()) );
  const TClassInfo *ci = EFACT.getInfo( cl_name );
  if( ! ci )
    return false;
  if( ci->props & clpPure )
    return false;

  QStringList atp = QString(allowTypes()).split(',');
  for( auto c : atp ) {
    if( EFACT.isChildOf( cl_name, c ) ) {
      return true;
    }
  }
  return false;
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

int HolderData::getMyIndexInParent() const
{
  if( !par ) {
    return -1;
  }
  return par->indexOfHolder( this );
}

QString HolderData::getFullName() const
{
  const HolderData *cob;
  QString res = objectName();
  QString tn;
  cob = par;
  while( cob != 0 ) {
    tn = cob->objectName();
    tn += '.';
    res.prepend( tn );
    cob = cob->par;
  };
  return res;
}

bool HolderData::getData( const QString &nm, QVariant &da, bool er ) const
{
  if( nm.isEmpty() ) {
    da = get();
    return true;
  }

  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    DBGx( "warn: bad target name \"%s\"", qP( nm ) );
    return 0;
  }

  HolderData *ho = getElem( first );
  if( !ho ) {
    if( er ) {
      DBGx( "warn: fail to find name \"%s\" in \"%s\"", qP(first), qP(getFullName()) );
    }
    return 0;
  }
  if( nm_type == simpleName ) { // first only
    da = ho->get( idx ); // TODO: check for simple data?
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

  return ho->getData( rest, da, er );
}


bool HolderData::getData( const QString &nm, int *da, bool er ) const
{
  if( !da ) {
    return false;
  }
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  *da = vda.toInt();
  return true;
}

bool HolderData::getData( const QString &nm, double *da, bool er ) const
{
  if( !da ) {
    return false;
  }
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  *da = vda.toDouble();
  return 1;
}


bool HolderData::getData( const QString &nm, QString &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  da = vda.toString();
  return true;
}


int HolderData::getDataD( const QString &nm, int dfl ) const
{
  int r = dfl;
  getData( nm, &r );
  return r;
}

double HolderData::getDataD( const QString &nm, double dfl ) const
{
  double r = dfl;
  getData( nm, &r );
  return r;
}

QString HolderData::getDataD( const QString &nm, const QString &dfl ) const
{
  QString r = dfl;
  getData( nm, r );
  return r;
}

QColor HolderData::getDataD( const QString &nm, const QColor &dfl ) const
{
  QColor r = dfl;
  int ri = 0;
  if( getData( nm,  &ri ) ) {
    r = QColor( QRgb( ri ) );
  }
  return QColor( QRgb( ri ) );
}



bool HolderData::setData( const QString &nm, const QVariant &da )
{
  if( nm.isEmpty() ) {
    return set( da );
  }

  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    DBG2q( "warn: bad target name", nm );
    return 0;
  }

  HolderData *ho = getElem( first );
  if( !ho ) {
    DBGx( "warn: fail to find name \"%s\" in \"%s\"",
          qP(first), qP(getFullName()) );
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
  if( ! nm.isEmpty() ) { // only w/o name
    DBGx( "warn: complex name \"%s\" req for simple obj %s",
        qP(nm), qP(objectName()) )
    return false;
  }
}

bool HolderData::getUpData( const QString &nm, int *da ) const
{
  if( !da ) {
    return false;
  }

  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  DBGx( "warn: fail to find name \"%s\" from \"%s\" up", qP(nm), qP(getFullName()) );
  return false;
}


bool HolderData::getUpData( const QString &nm, double *da ) const
{
  if( !da ) {
    return false;
  }

  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  DBGx( "warn: fail to find name \"%s\" from \"%s\" up", qP(nm), qP(getFullName()) );
  return false;
}

bool HolderData::getUpData( const QString &nm, QString &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  DBGx( "warn: fail to find name \"%s\" from \"%s\" up", qP(nm), qP(getFullName()) );
  return false;
}

bool HolderData::getUpData( const QString &nm, QVariant &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  DBGx( "warn: fail to find name \"%s\" from \"%s\" up", qP(nm), qP(getFullName()) );
  return false;
}



QStringList HolderData::getEnumStrings( const QString &enum_name ) const
{
  QStringList r;
  const QMetaObject *mci = metaObject();
  if( ! mci ) {
    return r;
  };
  int idx = mci->indexOfEnumerator( enum_name.toLocal8Bit().data() );
  if( idx < 0 ) {
    return r;
  }
  QMetaEnum me = mci->enumerator( idx );
  if( !me.isValid() ) {
    return r;
  }

  int n = me.keyCount();
  QString nm, snm, lbl;
  QMetaClassInfo ci;
  for( int i=0; i<n; ++i ) {
    int val = me.value( i ); // now the same as i, but ...
    snm = me.key( i );
    nm = QString( "enum_" ) + enum_name + "_" + QSN( val );
    int ci_idx = mci->indexOfClassInfo( nm.toLocal8Bit().data() );
    if( ci_idx < 0 ) {
      r << enum_name + "_" + QSN( val );
      continue;
    }
    ci = mci->classInfo( ci_idx );
    r << ci.value();
  }

  return r;
}


void HolderData::dumpStruct() const
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

void HolderData::post_set()
{
  // check_guard();
  for( auto e: children() ) { // propagate to childs
    HolderData* ho = qobject_cast<HolderData*>(e);
    if( !ho )
      continue;
    ho->post_set();
  }
}



const char* HolderData::helpstr { "Abstract data holder" };


// ---------------- HolderValue ---------
STD_CLASSINFO(HolderValue,clpData|clpPure);

CTOR(HolderValue,HolderData)
{
}

void HolderValue::reset_dfl()
{
  DBG1( ERR_ABS );
}

void HolderValue::post_set()
{
  DBG1( ERR_ABS );
}


bool HolderValue::set( const QVariant & /*x*/, int /* idx */ )
{
  DBG1( ERR_ABS );
  return false;
}

QVariant HolderValue::get( int /* idx */ ) const
{
  DBG1( ERR_ABS );
  return QVariant();
}

QString HolderValue::toString() const
{
  DBG1( ERR_ABS );
  return QString();
}

bool HolderValue::fromString( const QString & /*s */ )
{
  DBG1( ERR_ABS );
  return false;
}


QString HolderValue::getTypeV() const
{
  return "value?";
}

const char* HolderValue::helpstr { "Parent of all data" };


DEFAULT_FUNCS_REG(HolderValue);



// ---------------- HolderInt ---------
STD_CLASSINFO_ALIAS(HolderInt,clpData,int);

CTOR(HolderInt,HolderValue) , v(0)
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


QString HolderInt::getTypeV() const
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

QString HolderSwitch::getTypeV() const
{
  return "switch";
}

const char* HolderSwitch::helpstr { "Contains integer (bin) data - switch iface" };

DEFAULT_FUNCS_REG(HolderSwitch);


// ---------------- HolderList ---------
STD_CLASSINFO_ALIAS(HolderList,clpData,list);

CTOR(HolderList,HolderInt)
{
  setParm("min","0"); setParm("max","0");
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,LIST" );
  }

  // may be overkill, but one cone - one place
  QString enum_name = getParm( "enum" );
  QStringList sl;
  if( ! enum_name.isEmpty() ) {
    if( par ) {
      sl = par->getEnumStrings( enum_name );
    }
  }
  setParm( "max", QSN( sl.size()-1 ) );

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


QString HolderList::getTypeV() const
{
  return "list";
}

const char* HolderList::helpstr { "Contains integer data - list iface" };

DEFAULT_FUNCS_REG(HolderList);



// ---------------- HolderDouble ---------
STD_CLASSINFO_ALIAS(HolderDouble,clpData,double);

CTOR(HolderDouble,HolderValue), v(0)
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


QString HolderDouble::getTypeV() const
{
  return "double";
}

const char* HolderDouble::helpstr { "Contains double data" };

DEFAULT_FUNCS_REG(HolderDouble);


// ---------------- HolderString ---------
STD_CLASSINFO_ALIAS(HolderString,clpData,QString);

CTOR(HolderString,HolderValue)
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


QString HolderString::getTypeV() const
{
  return "string";
}

const char* HolderString::helpstr { "Contains QString data" };

DEFAULT_FUNCS_REG(HolderString);


// ---------------- HolderColor ---------
STD_CLASSINFO_ALIAS(HolderColor,clpData,QColor);

CTOR(HolderColor,HolderValue)
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
  QString s = x.toString();
  v = QColor( s );
  if( ! v.isValid() ) {
    QRgb rgba = x.toInt();
    v.setRgba( rgba );
  }
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
  //return QSN( (int)(v.rgba()) );
  return v.name();
}

bool HolderColor::fromString( const QString &s )
{
  v = QColor( s );
  if( ! v.isValid() ) {
    QRgb rgba = s.toInt();
    v.setRgba( rgba );
  }
  post_set();
  return true;
}


QString HolderColor::getTypeV() const
{
  return "color";
}

const char* HolderColor::helpstr { "Contains QColor data" };

DEFAULT_FUNCS_REG(HolderColor);

// ---------------- HolderIntArray ---------
STD_CLASSINFO_ALIAS(HolderIntArray,clpData|clpArray,int[]);

CTOR(HolderIntArray,HolderValue)
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


QString HolderIntArray::getTypeV() const
{
  return "int[]";
}

const char* HolderIntArray::helpstr { "Contains vector of integer data" };


DEFAULT_FUNCS_REG(HolderIntArray);


// ---------------- HolderDoubleArray ---------
STD_CLASSINFO_ALIAS(HolderDoubleArray,clpData|clpArray,double[]);

CTOR(HolderDoubleArray,HolderValue)
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


QString HolderDoubleArray::getTypeV() const
{
  return "double[]";
}

const char* HolderDoubleArray::helpstr { "Contains vector of double data" };


DEFAULT_FUNCS_REG(HolderDoubleArray);


// ---------------- HolderStringArray ---------
STD_CLASSINFO_ALIAS(HolderStringArray,clpData|clpArray,string[]);

CTOR(HolderStringArray,HolderValue)
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
  if( idx < 0 || idx >= v.size() ) // slow, but safe - not for fast code
    return false;
  v[idx] = x.toString();
  post_set();
  return ! v[idx].isEmpty();
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
  return v.join( QChar(0x2400) ); // NUL representation in Unicode
}

bool HolderStringArray::fromString( const QString &s )
{
  if( s.contains( QChar(0x2400) ) ) {
    v = s.split( QChar(0x2400), QString::KeepEmptyParts );
  } else {
    v = s.split('\x01', QString::KeepEmptyParts ); // old strings
  }

  post_set();
  return !v.isEmpty();
}


QString HolderStringArray::getTypeV() const
{
  return "string[]";
}

const char* HolderStringArray::helpstr { "Contains QStringList" };


DEFAULT_FUNCS_REG(HolderStringArray);



// ---------------- TDataSet ------------------------
STD_CLASSINFO(TDataSet,clpSpecial|clpPure);


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
  state = stateBad; //  guard = 0;
}


// ----------------------------------


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



QString TDataSet::getTypeV() const
{
  return metaObject()->className();
}

DEFAULT_FUNCS_REG(TDataSet);








const double* TDataSet::getDoublePtr( const QString &nm, ltype_t *lt,
              const TDataSet **targ, int lev  ) const
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
    // DBGx( "warn: fail to find name \"%s\" in \"%s\"", qP(first), qP(getFullName()) );
    return 0;
  }

  if( nm_type == simpleName ) { // first only
    if( ho->isObject() ) {
      TDataSet *ds= qobject_cast<TDataSet*>(ho);
      return ds->getDoublePtr( "out0", lt, targ, lev+1 );
    }
    if( ho->getTp() == QVariant::Double ) {
      HolderDouble *hod = qobject_cast<HolderDouble*>(ho);
      if( !hod )
        return nullptr;
      if( lt ) {
        *lt = ( lev == 1 ) ? LinkElm : LinkSpec;
      }
      if( targ ) {
        *targ = this;
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
    return nullptr;
  }
  TDataSet *ds = qobject_cast<TDataSet*>(ho);
  return ds->getDoublePtr( rest, lt, targ, lev+1 );

}

const double* TDataSet::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  ltype_t clt = LinkNone;
  ltype_t *plt = ( lt ) ? lt : &clt;  // failsafe link

  if( !par ) {  // parent-less object or root
    *plt = LinkBad;
    return nullptr;
  }
  TDataSet *ds = qobject_cast<TDataSet*>( par );
  if( !ds ) {
    return nullptr;
  }
  return ds->getSchemeDoublePtr( nm, lt, src_ob, lev );
}

// not const - change param
double* TDataSet::getDoublePrmPtr( const QString &nm, int *flg )
{
  if( nm.isEmpty() )
    return nullptr;

  HolderDouble *hod = getElemT<HolderDouble*>( nm );

  if( !hod ) {
    DBGx( "warn: fail to find name \"%s\" in \"%s\"", qP(nm), qP(getFullName()) );
    return 0;
  }

  if( flg )
    *flg = hod->getFlags();
  return hod->addr();

  return 0;
}




bool TDataSet::set( const QVariant & x, int /* idx */  )
{
  // check_guard();
  return fromString( x.toString() );
}

QVariant TDataSet::get( int /* idx */ ) const
{
  // check_guard();
  return QVariant( this->toString() );
}

QString TDataSet::toString() const
{
  // check_guard();
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
    if( !ho ) {
      continue; // but how?
    }
    if( ho->getFlags() & efNoSave ) {
      continue;
    }

    const TClassInfo *ci = ho->getClassInfo(); // dont write obsoleted objects
    if( ci->props & clpObsolete ) {
      continue;
    }

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
        errstr = QString("err: read elem \"%1\" failed. "
            "required: \"%2\" but have \"%3\" in \"%4\"" )
                .arg(elname).arg(tagname).arg(ho->getType()).arg( getFullName() );
        DBG1q( errstr );
        return false;
      }
      if( !ho ) { // name not found
        if( ! add_obj( cl_name, elname ) ) {
          errstr = QString("TDataSet::fromDom: fail to create obj %1 %2 ")
                   .arg(cl_name).arg(elname);
          DBG1q( errstr );
          continue; // for conversion: ignore unknown
          // return false;
        }
      }

      TDataSet* ob = getElemT<TDataSet*>( elname );
      if( !ob ) {
        errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 in %3")
                 .arg(cl_name).arg(elname).arg( objectName() );
        DBG1q( errstr );
        return false;
      }

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
        ho =  add_param( tp_name, elname );
        if( !ho  ) {
          errstr = QString("TDataSet::fromDom: fail to create param \"%1\" in \"%2\" ")
                   .arg(tp_name).arg(elname);
          DBG1q( errstr );
          // return false;
          continue; // ignore unused params
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

void TDataSet::post_set() {
  return HolderData::post_set();
};

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
  // if( ! guard ) // BUG: bad order of destruction
  //   return;
  int idx = inputs.indexOf( inp );
  if( idx == -1 ) {
    DBG2q( "warn: input \"%s\" not registered", inp->objectName() );
    return;
  }
  inputs.remove( idx );
}

InputSimple* TDataSet::getInput( int n ) const
{
  if( n < 0 || n >= inputs.size() ) {
    DBGx( "warn: bad input number %d, in \"%s\" size= %d",
        n, qP(getFullName()), inputs.size() );
    return nullptr;
  }
  return inputs[n];
}


// ------------------------------------ InputAbstract ---------
//
STD_CLASSINFO(InputAbstract,clpInput|clpSpecial|clpPure);

const double InputAbstract::fake_in {0};
const double InputAbstract::one_in {1.0};

CTOR(InputAbstract,TDataSet)
{
  // child may register itself to parent
}

InputAbstract::~InputAbstract()
{
  // child may unregister itself from parent
}


void InputAbstract::post_set()
{
  HolderData::post_set();
  reportStructChanged(); // changed link means changes structure
}

void InputAbstract::do_structChanged()
{
  set_link();
}

void InputAbstract::set_link()
{
  p = &fake_in; src_obj = nullptr; linkType = LinkBad;
  if ( source.cval().isEmpty() ) {
    linkType = LinkNone;
    return;
  }
  if ( source.cval() == ":one" ) { // special local case
    linkType = LinkSpec;
    p = &one_in;
    return;
  }

  ltype_t lt;
  const TDataSet *srct = nullptr;
  const double *cp = getSchemeDoublePtr( source, &lt, &srct, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;  src_obj = srct; linkType = lt;
    //DBGx( "dbg: ptr set to target %p for \"%s\" in \"%s\"",
    //   cp, qP(source), qP(getFullName())  );
  } else {
    DBGx( "dbg: ptr not found for \"%s\" in \"%s\"",
        qP(source), qP(getFullName()) );
  }

}

const char* InputAbstract::helpstr { "Abstract link" };

DEFAULT_FUNCS_REG(InputAbstract);

// ------------------------------------ InputSimple ---------
STD_CLASSINFO(InputSimple,clpInput|clpSpecial);

CTOR(InputSimple,InputAbstract)
{
  TDataSet *ds = qobject_cast<TDataSet*>( par );
  if( ds ) {
    ds->registerInput( this );
  }
}

InputSimple::~InputSimple()
{
  TDataSet *ds = qobject_cast<TDataSet*>( par );
  if( ds ) {
    ds->unregisterInput( this );
  }
}


void InputSimple::post_set()
{
  InputAbstract::post_set(); // report is here
}


void InputSimple::set_link()
{
  InputAbstract::set_link();
}


const char* InputSimple::helpstr { "Link to source of simple double data" };

DEFAULT_FUNCS_REG(InputSimple);


// ------------------------------------ InputParam ---------
STD_CLASSINFO(InputParam,clpInput|clpSpecial);

CTOR(InputParam,InputAbstract)
{
}

InputParam::~InputParam()
{
}


void InputParam::post_set()
{
  InputAbstract::post_set(); // report is here
}


void InputParam::set_link()
{
  InputAbstract::set_link();
  target_flag = 0;;
  targ = &fake_target;
  if( !par ) // par seems to be InputParams, but may be dangling objects
    return;

  TDataSet *el = qobject_cast<TDataSet*>( par->getParent() ); // grang par may by TMiso, but ...
  if( !el )
    return;

  targ = el->getDoublePrmPtr( tparam, &target_flag );
  if( !targ ) {
    targ = &fake_target;
  }
}



const char* InputParam::helpstr { "Link from source to inner parameter" };

DEFAULT_FUNCS_REG(InputParam);


// end of dataset.cpp


