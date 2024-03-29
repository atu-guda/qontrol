/***************************************************************************
 dataset.cpp  -   base definifions and classes implementation
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2022 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QColor>
#include <QStandardItemModel>

#include "miscfun.h"
#include "autoact.h"
#include "dataset.h"

using namespace std;


#define ERR_ABS "ERR: abstract class function called"

// ================================================================
// ---------------- HolderData .... ----------------------
STD_CLASSINFO(HolderData,clpSpecial|clpAbstract);

HolderData::HolderData( ARGS_CTOR_MIN )
     :QAbstractItemModel( a_parent ),
      flags(a_flags),
      par(a_parent)
{
  setObjectName( obj_name );

  setParm( QSL("vis_name"), autoVisName( a_v_name, obj_name ) ); // TODO: not always, see .....
  setParm( QSL("descr"), a_descr );
  setParm( QSL("extra"), a_extra );
  extraToParm();
}

HolderData::~HolderData()
{
}

HolderData* HolderData::create( const QString &, HolderData *,
         int, const QString &, const QString &, const QString & )
{
  qCritical() << "Attempt to create abstract class " << WHE;
  return nullptr;
}

// QAbstractItemModel part
int HolderData::columnCount( const QModelIndex & /*par*/ ) const
{
  return 4;
}

// //* function to debug model indexes
// QString idx2s( const QModelIndex &idx )
// {
//   QString r = QSL("[ ");
//   if( idx.isValid() ) {
//     r += QSL("V ");
//   } else {
//     r += QSL("X ");
//   }
//   void *p = idx.internalPointer();
//   r += QSN( idx.row() ) + QSL(" ") + QSN( idx.column() ) + QSL(" ") + QSN( (uint64_t)(p) );
//   if( p ) {
//     const HolderData *ho = static_cast<HolderData*>(p);
//     r += QSL("=\")" + ho->getFullName() + "\"";
//   }
//
//   r += QSL(" ]");
//   return r;
// }

int HolderData::rowCount( const QModelIndex &a_par ) const
{
  auto sz = size();
  if( ! a_par.isValid() ) { // invalid: assume self (examples: root)
    return sz;
  }

  HolderData *po = static_cast<HolderData*>( a_par.internalPointer() );
  if( !po ) {
    return 0;
  }
  sz = po->size();
  return (int)sz;
}

QVariant HolderData::data( const QModelIndex &idx, int role ) const
{
  if( !idx.isValid() ) {
    return QVariant();
  }
  int c = idx.column();

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    return QVariant();
  }

  return ds->dataObj( c, role );
}

QVariant HolderData::dataObj( int col, int role ) const
{
  // no switch: too complex
  if( role == Qt::DisplayRole ) {
    QString s;
    s = QSL( "c " ) % QSN( col ); // fallback value
    switch( col ) {
      case 0:
        s = objectName(); break;
      case 1:
        s = getType(); break;
      case 2:
        s = textVisual();
        if( s.size() > 80 ) {
          s.truncate( 72 );
          s += QSL( "..." );
          if( s[0] == '"' ) {
            s.append( '"' );
          }
        }
        break;
      case 3:
        s = getStateStr();
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
    return ( par  &&  par->getActiveObj() == this );
  }

  if( role == Qt::DecorationRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    return getIcon();
  }

  if( role == Qt::WhatsThisRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    return getParm( QSL("descr"),
        QSL("Object \"") % objectName() % QSL("\". type: ") % getType() );
  }

  if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
    if( col > 1 ) {
      return QVariant();
    }
    QString s = objectName() % QSL(" (") % getType() % QSL(") ");
    if( col == 1 ) {  // for button
      return s;
    }
    s += getStateStr(); // 0: real status tip
    return s;
  }

  return QVariant();
}


QModelIndex HolderData::index( int row, int column, const QModelIndex &o_par ) const
{

  if( ! hasIndex( row, column, o_par ) ) {
    return QModelIndex();
  }

  const HolderData *d_par = static_cast<HolderData*>( o_par.internalPointer() );
  if( !d_par ) {
    d_par = this;
  }

  const HolderData *d_t = qobject_cast<HolderData*>( d_par->getObj( row ) );
  if( ! d_t ) {
    return QModelIndex();
  }

  return createIndex( row, column, const_cast<HolderData*>(d_t) );
}

QModelIndex HolderData::parent( const QModelIndex & idx ) const
{
  if( ! idx.isValid() ) {
    return QModelIndex();
  }

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    return QModelIndex();
  }

  HolderData *ds_p = ds->getParent();
  if( !ds_p ) {
    return QModelIndex();
  }
  if( ds_p == this ) {
    return QModelIndex();
  }

  return createIndex( ds_p->getMyIndexInParent(), 0, ds_p );
}

// my part

int HolderData::indexOfHolder( const HolderData *ho ) const
{
  // children holds <QObject*>, but indexOf dont touch *it
  return children().indexOf( const_cast<HolderData*>(ho) );
}

const HolderData* HolderData::findChildBySubchild( const HolderData *subch ) const
{
  while( subch ) {
    const HolderData *prev = subch;
    subch = subch->getParent();
    if( subch == this ) {
      return prev;
    }
  };
  return nullptr;
}


QString HolderData::getStateStr() const
{
  char c1 = getModified() ? '*' : ' ';
  QString s = QChar(c1);
  if( dyn ) { s += QSL("."); };
  s += getStateString( state );
  s += QSL(" 0x");
  s += QSNX( flags );
  s += QSL(" iM=" ) % QSN( ignoreMod );
  return s;
}

void HolderData::setModified()
{
  // qWarning() << "SET: " << NWHE;
  if( ignoreMod ) {
    return;
  }
  if( modified ) {  // do not repeat on already modified
    return;
  }
  modified = 1;
  if( flags & efNoSave ) {    // ignore shadow and so on
    return;
  }
  if( par != nullptr ) {
    par->setModified();
  }
}

void HolderData::setUnModified()
{
  modified = 0;
  for_type<HolderData>( []( auto ho ) { ho->setUnModified();} );
}

void HolderData::reset()
{
  for_type<HolderData>( []( auto ho ) { ho->reset();} );
  do_reset();
}


bool HolderData::isChildOf( const QString &cname ) const
{
  const QMetaObject *mob = metaObject();
  while( mob ) {
    if( cname == mob->className() ) {
      return true;
    }
    mob = mob->superClass();
  }
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

QStringList HolderData::elemNamesByNameType( const QString &nm_templ, const QString &tp_name ) const
{
  QStringList cl;
  for( const auto c : children() ) {
    HolderData *ho = qobject_cast<HolderData*>( c );
    if( !ho ) {
      continue;
    }
    if( nm_templ.isEmpty() || ho->objectName().startsWith( nm_templ ) ) {
      if( tp_name.isEmpty() || ho->getType() == tp_name ) {
        cl << c->objectName();
      }
    }
  }
  return cl;
}


QString HolderData::ls() const
{
  QString r;
  r = getFullName() % QSL(" : ") % getType() % QSL(" dyn: ") % QSN(dyn)
    + QSL(" flags: 0x") % QSNX(flags) % QSL(" ptr: ") % QSNX( (unsigned long)(this));
  if( ! isObject() ) {
    r += QSL(" = \"") % textVisual() % QSL("\"");
  }
  r += QSL("\n;------------------------------------\n");
  int n_el = 0;
  QStringList sl;
  for( const auto c : children() ) {
    QString r1 = c->objectName() + ' ';
    ++n_el;
    const HolderData *ho = qobject_cast<HolderData*>( c );
    if( ho ) {
      r1 += ho->getType();
      if( ! ho->isObject() ) {
        QString vs = ho->textVisual();
        vs.truncate( 80 );
        r1 += QSL(" = \"") % vs + QSL("\"");
      }
    }
    sl << r1;
  }
  sl.sort();
  r += sl.join( '\n' );
  r += QSL("\n;------------------------------------");

  r += QSL("\n;n_el = ") + QSN(n_el) + QSL("\n;Params:\n");
  for( auto i= parms.constBegin(); i != parms.constEnd(); ++i ) {
    r += i.key() + QSL(" = \"") + i.value() + QSL("\"\n");
  }
  return r;
}

QString HolderData::lsf() const
{
  const QMetaObject *mo = metaObject();
  if( !mo ) {
    return QSL("ERROR_NO_METAOBJECT");
  }
  QStringList sl;
  int nf = mo->methodCount();
  for( int i=0; i<nf; ++i ) {
    QMetaMethod mm = mo->method( i );
    if( mm.isValid()  &&  mm.access() == QMetaMethod::Access::Public
        && ( mm.methodType() == QMetaMethod::MethodType::Method
          || mm.methodType() == QMetaMethod::MethodType::Slot ) ) {
      sl << mm.methodSignature();
    }
  }
  sl.sort();
  return sl.join( '\n' );
}

HolderData* HolderData::getObj( unsigned long i ) const
{
  if( isGoodIndex( i, *this ) ) {
    return qobject_cast<HolderData*>(children().at(i));
  }
  return nullptr;
}


HolderData* HolderData::getObj( const QString &oname ) const
{
  QString first, rest;
  int idx = 0;

  NameType nt = splitName( oname, first, rest, idx );

  if( nt == badName ) {
    qWarning() << "bad name" << oname << NWHE;
    return nullptr;
  }

  HolderData *ho =  findChild<HolderData*>( first, Qt::FindDirectChildrenOnly );
  if( !ho ) { // need, as here both simpleName and complexName
    // qWarning() << "object" << oname << "not found " << NWHE; // very noisy!
    return nullptr;
  }

  if( nt == simpleName ) {
    return ho;
  }

  if( nt == complexName ) {
    return ho->getObj( rest );
  }

  qWarning() << "unknown name type: " << nt << NWHE;
  return nullptr;
}

bool HolderData::hasObj( const QString &oname ) const
{
  return getObj( oname ) != nullptr;
}

void HolderData::setParm( const QString &pname, const QString &value )
{
  parms[pname] = value;
}


QString HolderData::getParm( const QString &pname, const QString &dfl ) const
{
  return parms.value( pname, dfl );
}

int HolderData::getParmInt( const QString &pname, int dfl ) const
{
  if( parms.contains( pname ) ) {
    const QString &p = parms[pname];
    bool ok;
    int v = p.toInt( &ok, 0 );
    if( ok ) {
      return v;
    }
  }
  return dfl;
}

long HolderData::getParmLong( const QString &pname, long dfl ) const
{
  if( parms.contains( pname ) ) {
    const QString &p = parms[pname];
    bool ok;
    long v = p.toLongLong( &ok, 0 );
    if( ok ) {
      return v;
    }
  }
  return dfl;
}

double HolderData::getParmDouble( const QString &pname, double dfl ) const
{
  if( parms.contains( pname ) ) {
    const QString &p = parms[pname];
    bool ok;
    double v = toDoubleEx( p, &ok );
    if( ok ) {
      return v;
    }
  }
  return dfl;
}

void HolderData::setParmIfEmpty( const QString &pname, const QString &val )
{
  if( getParm( pname ).isEmpty() ) {
    setParm( pname, val );
  }
}

bool HolderData::setDatas( const QString &datas )
{
  if( ! isObject() ) {
    return fromString( datas );
  }

  bool was_set = false;
  QStringList sl = datas.split( QSL("\n"), Qt::SkipEmptyParts );
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9.]*)\s*=(.+)$)" ); // '.' - for complex strings

  for( auto s : sl ) {
    s = s.trimmed();
    if( s.isEmpty() || s[0] == '#' || s[0] == ';' ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      val.replace( '\r', '\n' ); // newline representation
      if( val.startsWith( QSL("\"") ) && val.endsWith( QSL("\"") ) ) { // remove " ", if have (only one)
        val = val.mid( 1, val.size()-2 );
      }
      was_set = setData( nm, val ) || was_set; // Order!
    } else {
      qWarning() << QSL("bad param string part: ") << s << NWHE;
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

QString HolderData::hintName( const QString &ob_tp, const QString &nm_start ) const
{
  QString nm = nm_start;
  QString tpx = ob_tp;

  if( nm.isEmpty() ) {
    nm = QSL("xx_");
    if( tpx.isEmpty() ) {
      tpx = allowTypes();
      int coma_idx = tpx.indexOf( ',' );
      if( coma_idx > 0 ) { // first ',' do not have any sense
        tpx = tpx.left( coma_idx );
      }
    }
    const TClassInfo *ci = EFACT.getInfo( tpx );
    if( ci ) {
      const QMetaObject *mci = ci->meta;
      int ci_idx = mci->indexOfClassInfo( "nameHintBase" );
      if( ci_idx >= 0 ) {
        nm = mci->classInfo( ci_idx ).value();
      }
    }
  }

  if( nm.endsWith('_') ) {
    nm += '0';
  }

  if( !getObj( nm ) ) {
    return nm;
  }

  //             nameDIGITS
  QString bname;
  int ne = splitNameNum( nm, bname ) + 1; // -1 -> 0

  for( int i=ne; i<ne+2000; ++i ) {
    nm = bname + QSN( i );
    if( !getObj( nm ) ) {
      return nm;
    }
  }

  return bname + QSL(" _xx_");
}


bool HolderData::isRoTree( int flg ) const
{
  for( auto p = this; p; p=p->getParent() ) {
    if( p->hasAnyFlags( flg ) ) {
      return true;
    }
  }
  return false;
}

HolderData* HolderData::getRoot() noexcept
{
  HolderData *p = this, *np;
  while( (np = p->getParent()) != nullptr ) { // find root
    p = np;
  }
  return p;
}

const HolderData* HolderData::getRootConst() const noexcept
{
  const HolderData *p = this, *np;
  while( (np = p->getParent()) != nullptr ) { // find root
    p = np;
  }
  return p;
}

int HolderData::countObjsOfType( const QString &ob_tp, const QString &nm_start ) const
{
  int n_el = 0, l_nm = nm_start.size();

  for_type_c<HolderData>( [&n_el,l_nm,&ob_tp,&nm_start]( auto ho ) {
    if( ho->isChildOf( ob_tp ) ) {
      if( l_nm < 1 || ho->objectName().startsWith( nm_start ) ) {
        ++n_el;
      }
    }
  } );

  return n_el;
}

void HolderData::extraToParm()
{
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );
  QStringList el = getParm( QSL("extra") ).split( QSL("\n"), Qt::SkipEmptyParts );

  for( QString &s : el ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      setParm( nm, val );
    } else {
      qWarning() << "bad extra string part: " <<  s << NWHE;
    }
  }
  dyn = getParmInt( QSL("dyn"), dyn ); // mod if manualy set
  parms.remove( QSL("extra") ); // not need anymore
}

int HolderData::postFileRead()
{
  int rc = 0;
  for_type<HolderData>( [&rc]( auto ho ) { rc += ho->postFileRead(); } );
  rc += do_postFileRead();
  return rc;
}

int HolderData::do_postFileRead()
{
  return 0;
}

void HolderData::reportStructChanged()
{
  if( updSuspended > 0 ) {
    return;
  }

  HolderData *r_root = getRoot();
  r_root->handleStructChanged();
}


void HolderData::handleStructChanged()
{
  if( updSuspended > 0 ) {
    return;
  }

  for_type<HolderData>( []( auto ho ) { ho->handleStructChanged(); } );

  do_structChanged(); // functions to override


  if( ! par ) { // root signals about changes to world? only root?
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
  QString iconName = QSL( ":icons/elm_" ) % getType().toLower() % QSL(".png");
  QIcon el_ico( iconName );
  // todo: from files, if failed
  return el_ico;
}

HolderData* HolderData::addObjP( const QString &cl_name, const QString &ob_name )
{

  HolderData *el = getObj( ob_name );
  if( el != nullptr ) {
    qWarning() <<  "name " << ob_name << el->getType() << " exist in " << NWHE;
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    qWarning() <<  "type " << cl_name <<  " for " << ob_name
               << " not allowed in " << NWHE;
    return nullptr;
  }

  bool ok;
  toDoubleEx( ob_name, &ok );
  if( ok ) {
    qWarning() << "Cannot use special name " << ob_name << "for object " << NWHE;
    return nullptr;
  }

  beginResetModel();

  HolderData *ob = EFACT.createObj( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  setModified();
  endResetModel();
  reportStructChanged();

  return ob;
}

bool HolderData::addObj( const QString &cl_name, const QString &ob_name )
{
  HolderData *ho = addObjP( cl_name, ob_name );
  return ( ho != nullptr );
}

bool HolderData::addObjDatas( const QString &cl_name, const QString &ob_name,
     const QString &datas )
{
  HolderData *ho = addObjP( cl_name, ob_name );
  if( ! ho ) {
    return false;
  }

  ho->setDatas( datas );

  return true;
}

HolderData* HolderData::canDoActionObj( const QString &ob_name, const QString &act_name ) const
{
  HolderData *ho = getObj( ob_name );
  if( !ho ) {
    qWarning() << QSL("not found element ") << ob_name << QSL(" to " ) << act_name << NWHE;
    return nullptr;
  }
  if( ho->hasAnyFlags( efImmutable ) ) {
    qWarning() << "element " << ho->getFullName() << " is Immutable to " << act_name << NWHE;
    return nullptr;
  }

  if( ! ho->isDyn() ) {
    qWarning() << "element " << ho->getFullName() << " is not created dynamicaly" << NWHE;
    return nullptr;
  }
  return ho;
}


int HolderData::delObj( const QString &ob_name )
{
  HolderData *ho = canDoActionObj( ob_name, QSL("delete") );
  if( !ho ) {
    return 0;
  }

  HolderData *act_obj = getActiveObj();
  QString act_name;
  if( act_obj ) {
    act_name = act_obj->objectName();
  }

  bool ignored_obj = ho->hasAnyFlags( efNoSave );
  beginResetModel();

  delete ho; // auto remove object and from parent lists
  if( !act_name.isEmpty() ) {
    setActiveObj( act_name );
  }

  if( !ignored_obj ) {
    setModified();
  }
  endResetModel();

  reportStructChanged();
  return 1;
}

int HolderData::delAllDyn()
{
  QStringList nms;
  for_type<HolderData>( [&nms]( auto ho )
      {
        ho->delAllDyn();
        if( ho->isDyn() && !ho->hasAnyFlags( efImmutable ) ) {
          nms << ho->objectName();
        }
      }
  );


  int n = 0;
  for( const auto &nm : nms ) {
    n += delObj( nm );
  }
  return n;
}

int HolderData::delByNameType( const QString &nm_templ, const QString &tp_name )
{
  int n = 0;
  const QStringList els = elemNamesByNameType( nm_templ, tp_name );
  for( const auto &el : els ) {
    n += delObj( el );
  }
  return n;
}

int HolderData::renameObj( const QString &ob_name, const QString &new_name )
{
  HolderData *ho = canDoActionObj( ob_name, QSL("rename") );
  if( !ho ) {
    return 0;
  }

  if( ! isGoodName( new_name ) ) {
    qWarning() << "bad name " << new_name << " to rename" << NWHE;
    return 0;
  }
  if( getObj( new_name ) ) {
    qWarning() << "element " << new_name << " is already exists " << NWHE;
    return 0;
  }

  ho->setObjectName( new_name );
  setModified();

  reportStructChanged();
  return 1;
}

bool HolderData::cloneObj( const QString &old_name, const QString &new_name )
{
  HolderData *old_obj = getObj( old_name );
  if( !old_obj ) {
    qWarning() << " old object not exist: " << old_name << NWHE;
    return false;
  }

  QString ob_tp = old_obj->getType();
  QString s = old_obj->toString();

  HolderData *new_obj = addObjP( ob_tp, new_name );
  if( !new_obj ) {
    qWarning() << "fail to create new object" << new_name << " type " << ob_tp << NWHE;
    return false;
  }
  bool ok =  new_obj->fromString( s );
  new_obj->reportStructChanged();
  new_obj->reset();
  return ok;
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

bool HolderData::setActiveObj( const QString &nm )
{
  int a_idx = -1; HolderData *e = getObj( nm );
  if( e ) {
    a_idx = indexOfHolder( e );
  }
  setActiveIdx( a_idx );
  return ( a_idx >= 0 );
};



bool HolderData::isValidType(  const QString &cl_name  ) const
{
  const TClassInfo *ci = EFACT.getInfo( cl_name );
  if( ! ci ) {
    return false;
  }
  if( ci->props & clpAbstract ) {
    return false;
  }

  const QStringList atp = QString( allowTypes() ).split( ',' );
  for( const auto &c : atp ) {
    if( EFACT.isChildOf( cl_name, c ) ) {
      return true;
    }
  }
  return false;
}


QString HolderData::toXML( bool forceType ) const
{
  QString buf;
  buf.reserve(4096); // TODO ?
  QTextStream tstr( &buf, QIODevice::WriteOnly );
  tstr.setCodec( "UTF-8" ); // no QSL
  QDomDocument dd_tmp;
  QDomElement el_tmp = dd_tmp.createElement( QSL("tmp_xxx") );
  QDomElement dom = toDom( dd_tmp, forceType );
  dom.save( tstr, QDomNode::EncodingFromTextStream );

  return buf;
}


QDomElement HolderData::toDom( QDomDocument &dd, bool forceType ) const
{
  QDomElement de = dd.createElement( QSL("param") );
  de.setAttribute( QSL("name"), objectName() );
  if( dyn ) {
    de.setAttribute( QSL("dyn"), QSL("1") );
  }
  if( dyn || forceType) {
    de.setAttribute( QSL("otype"), getType() );
    saveParmsToDom( de );
  }
  QDomText tn = dd.createTextNode( toString() );
  de.appendChild( tn );
  return de;
}

void HolderData::saveParmsToDom( QDomElement &de ) const
{
  for( auto p = parms.cbegin(); p != parms.cend(); ++p ) {
    if( p.key() == QSL("props") && isObject() ) { // TODO: or default props
      continue;
    }
    de.setAttribute( QSL("prm_") + p.key(), p.value() );
  }
}

bool HolderData::fromDom( QDomElement &de, QString &errstr )
{
  IGNORE_STRUCT_CHANGE_HERE;
  ++cvtCount;

  if( ! isObject() ) {
    QString txt = getDomText( de );
    return fromString( txt );
  }

  for( QDomNode no = de.firstChild(); !no.isNull() ; no = no.nextSibling() ) {

    if ( ! no.isElement() ) {
      continue;
    }

    QDomElement ee = no.toElement();
    QString elname = ee.attribute( QSL("name") );
    QString tagname = ee.tagName();

    if( tagname == QSL("obj") ) {  // ------------------------------- object
      QString cl_name = ee.attribute(QSL("otype"));
      if( cl_name.isEmpty() ) {
        errstr = QSL( "err: element \"%1\" without type" ).arg(elname);
        qWarning() << errstr << NWHE;
        return false;
      }
      HolderData *ho = getObj( elname );
      if( ho && ! ho->isObject() ) {
        errstr = QSL("err: read elem \"%1\" failed. "
            "required: \"%2\" but have \"%3\" in \"%4\"" )
                .arg( elname, tagname, ho->getType(), getFullName() );
        qWarning() << errstr << NWHE;
        return false;
      }
      if( !ho ) { // name not found
        if( ! addObj( cl_name, elname ) ) {
          errstr = QSL("TDataSet::fromDom: fail to create obj %1 %2 ")
                   .arg( cl_name, elname );
          qWarning() << errstr << NWHE;
          continue; // for conversion: ignore unknown
          // return false;
        }
      }

      TDataSet* ob = getObjT<TDataSet*>( elname );
      if( !ob ) {
        errstr = QSL("TDataSet::fromDom: fail to find created obj %1 %2 in %3")
                 .arg( cl_name, elname, objectName() );
        qWarning() << errstr << NWHE;
        return false;
      }

      if( ! ob->fromDom( ee, errstr ) ) {
        return false;
      }

    } else if( tagname == QSL("param") ) {  // ---------------- simple param
      QString tp_name = ee.attribute(QSL("otype"));
      HolderData *ho = getObj( elname );
      QString txt = getDomText( ee );
      if( ho && ho->isObject() ) {
        //* TODO: remove after newlink conversion. or no: more converions
        if( ho->getType() == QSL("ParamDouble") ) {
          double vt = txt.toDouble(); // hack: limit number of digits on conversion
          txt = QSN( vt, 'g', DOUBLE_PREC );
          ho->setData( QSL("source"), txt );
          ho->setData( QSL("line_w"), 2 );
          ho->setData( QSL("line_color"), QSL("red") );
          continue;
        }
        // --------- end conversion (TMP) --------
        errstr = QString("TDataSet::fromDom: param \"%1\" is an object type \"%2\" ")
                 .arg( elname, ho->getType() );
        qWarning() << errstr << NWHE;
        return false;
      }
      if( !ho ) {
        ho =  addObjP( tp_name, elname );
        if( !ho  ) {
          errstr = QString("TDataSet::fromDom: fail to create param \"%1\" in \"%2\" ")
                   .arg( tp_name,elname );
          qWarning() << errstr << NWHE;
          // return false;
          continue; // ignore unused params
        }
      }
      ho->restoreParmsFromDom( ee );
      ho->reset_dfl();
      ho->fromString( txt );

    } else { // ----------- unknown element
      errstr = QString("TDataSet::fromDom: bad element %1 %2 ")
               .arg( tagname, elname );
      qWarning() << errstr << NWHE;
      return false;
    }
  }
  restoreParmsFromDom( de );
  post_set();
  reportStructChanged();

  return true;
}

bool HolderData::restoreParmsFromDom( QDomElement &de )
{
  if( ! isDyn() ) { // not for static objects
    return false;
  }
  // qDebug() << "dyn param:" << ho->objectName() <<  NWHE;
  auto attrs = de.attributes();
  int asz = attrs.size();
  for( int i=0; i<asz; ++i ) {
    QDomNode dann = attrs.item(i);
    QDomAttr dattr = dann.toAttr();
    if( dattr.isNull() ) { continue; }
    QString attr_name = dattr.name();
    // qDebug() << "dyn attr: " <<  attr_name << " to " << dattr.value() << NWHE;
    if( ! attr_name.startsWith( QSL("prm_") ) ) { continue; } // only special names
    attr_name.remove( 0, 4 ); // remove "prm_";
    if( attr_name == QSL("extra") ) { // ignore pseudo param, used only in static creation
      continue;
    }
    if( attr_name == QSL("props") && isObject() ) { // ignore props for object
      continue;
    }
    setParm( attr_name, dattr.value() );
    // qDebug() << "set dyn attr: " <<  attr_name << " to " << dattr.value() << NWHE;
  }
  return true;
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

double HolderData::getDoubleVal( const QString &nm ) const
{
  if( nm.isEmpty() ) {
    return getDouble();
  }

  HolderData *ho = getObj( nm );
  if( !ho ) {
    return 0.0;
  }

  return ho->getDouble();
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
    qWarning() << "bad target name " <<  nm << NWHE;
    return false;
  }

  HolderData *ho = getObj( first );
  if( !ho ) {
    if( er ) {
      qWarning() << "fail to find name " << first << NWHE;
    }
    return false;
  }
  if( nm_type == simpleName ) { // first only
    da = ho->get( idx ); // TODO: check for simple data?
    return true;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType  ||  ! ho->isObject()  ) {
    qWarning() << "compound name required. first: " << first
               << " rest:" << rest << " tp: " << (int)(ho->getTp()) << NWHE;
    return false;
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

bool HolderData::getData( const QString &nm, long *da, bool er ) const
{
  if( !da ) {
    return false;
  }
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  *da = (long)vda.toLongLong();
  return true;
}

bool HolderData::getData( const QString &nm, double *da, bool /*er*/ ) const
{
  if( !da ) {
    return false;
  }
  // special case!
  double a =  getDoubleVal( nm );
  *da = a;
  return true;
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

bool HolderData::getData( const QString &nm, QColor &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  if( vda.type() == QVariant::Color ) {
    da = vda.value<QColor>();
    return true;
  }
  QString s = vda.toString();
  QColor v = QColor( s );
  if( ! v.isValid() ) {
    QRgb rgba = vda.toInt();
    v.setRgba( rgba );
  }
  da = v;
  return true;
}

bool HolderData::getData( const QString &nm, QFont &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  if( vda.type() == QVariant::Font ) {
    da = vda.value<QFont>();
    return true;
  }
  QFont v;
  v.fromString( vda.toString() );
  da = v;
  return true;
}


bool HolderData::getData( const QString &nm, QTime &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  if( vda.type() == QVariant::Time ) {
    da = vda.toTime();
    return true;
  }
  QTime v;
  v.fromString( vda.toString() );
  da = v;
  return true;
}

bool HolderData::getData( const QString &nm, QDate &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  if( vda.type() == QVariant::Date ) {
    da = vda.toDate();
    return true;
  }
  QDate v;
  v.fromString( vda.toString() );
  da = v;
  return true;
}


int HolderData::getDataD( const QString &nm, int dfl ) const
{
  auto r = dfl;
  getData( nm, &r );
  return r;
}

long HolderData::getDataD( const QString &nm, long dfl ) const
{
  auto r = dfl;
  getData( nm, &r );
  return r;
}

double HolderData::getDataD( const QString &nm, double dfl ) const
{
  auto r = dfl;
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
  getData( nm, r );
  return r;
}

QFont  HolderData::getDataD( const QString &nm, const QFont &dfl ) const
{
  QFont r = dfl;
  getData( nm, r );
  return r;
}

QTime HolderData::getDataD( const QString &nm, const QTime &dfl ) const
{
  QTime r = dfl;
  getData( nm, r );
  return r;
}

QDate HolderData::getDataD( const QString &nm, const QDate &dfl ) const
{
  QDate r = dfl;
  getData( nm, r );
  return r;
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
    qWarning() << "bad target name " << nm << NWHE;
    return false;
  }

  HolderData *ho = getObj( first );
  if( !ho ) {
    qWarning() << "fail to find name " << first << NWHE;
    return false;
  }
  if( nm_type == simpleName ) { // first only
    return ho->set( da, idx );
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    qWarning() << "compound name required. first: " << first
               << " rest:" << rest << " tp: " << (int)(ho->getTp()) << NWHE;
    return false;
  }

  return ho->setData( rest, da );
}

bool HolderData::setD( const QString &nm, int da )
{
  return setData( nm, QVariant( da ) );
}

bool HolderData::setD( const QString &nm, double da )
{
  return setData( nm, QVariant( da ) );
}

bool HolderData::setD( const QString &nm, const QString &da )
{
  return setData( nm, QVariant( da ) );
}

bool HolderData::setD( const QString &nm, const QColor &da )
{
  return setData( nm, QVariant( da ) );
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
  qWarning() << "fail to find UP name " << nm << NWHE;
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
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}

bool HolderData::getUpData( const QString &nm, QString &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}

bool HolderData::getUpData( const QString &nm, QVariant &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
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
  QMetaClassInfo ci;
  for( int i=0; i<n; ++i ) {
    int val = me.value( i ); // now the same as i, but ...
    QString nm = QSL( "enum_" ) % enum_name % QSL("_") % QSN( val );
    int ci_idx = mci->indexOfClassInfo( nm.toLocal8Bit().data() );
    if( ci_idx < 0 ) {
      r << enum_name % QSL("_") % QSN( val );
      continue;
    }
    ci = mci->classInfo( ci_idx );
    r << ci.value();
  }

  return r;
}

void HolderData::fillListStrings()
{
  list_strings.clear();
  QString enum_name = getParm( QSL("enum") );
  if( ! enum_name.isEmpty() ) {
    if( par ) {
      list_strings = par->getEnumStrings( enum_name );
    }
  } else {
    QString s = getParm( QSL("list_strings" ) );
    if( !s.isEmpty() ) {
      list_strings = s.split(",");
    }
  }
  setParm( QSL("max"), QSN( list_strings.size()-1 ) );
}

HolderData* HolderData::addObjToSubP( const QString &subname, const QString &ob_tp, const QString &ob_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return nullptr;
  }
  HolderData *ob = sub->addObjP( ob_tp, ob_name );
  return ob;
}

bool HolderData::addObjToSub( const QString &subname, const QString &ob_tp, const QString &ob_name )
{
  HolderData *ho = addObjToSubP( subname, ob_tp, ob_name );
  return ( ho != nullptr );
}

bool HolderData::delObjFromSub( const QString &subname, const QString &ob_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->delObj( ob_name );
}

bool HolderData::cloneObjInSub( const QString &subname,
       const QString &old_name, const QString &new_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->cloneObj( old_name, new_name );
}

bool HolderData::renameObjInSub( const QString &subname,
       const QString &old_name, const QString &new_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->renameObj( old_name, new_name );
}

QAbstractItemModel* HolderData::getComplModel( const QString &targ, QObject *mdl_par ) const
{
  auto mdl = new QStandardItemModel( mdl_par );
  // qDebug() << "req: target: " << targ << NWHE;
  if( targ == QSL("in") ) {
    fillComplModelForInputs( mdl );
  } else if( targ == QSL("prm") ) {
    fillComplModelForParams( mdl );
  } else if( targ == QSL("out") ) {
    fillComplModelForOuts( mdl );
  }
  return mdl;
}

void HolderData::fillComplModelForInputs( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForInputs( mdl );
  }
}

void HolderData::fillComplModelForParams( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForParams( mdl );
  }
}

void HolderData::fillComplModelForOuts( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForOuts( mdl );
  }
}

int HolderData::fillComplForInputs( QStandardItem *it0, const QString &prefix ) const
{
  if( !it0 ) { return 0; }
  int n = 0;

  int ign = 0;
  getData( QSL("eprm.ignored"), &ign, false );
  if( ign ) {
    return 0;
  }

  // // debug: special fake name ==Full.name
  // auto it = new QStandardItem( QSL("==") % prefix % getFullName() );
  // it0->appendRow( it );

  auto mc = getMathConstants();
  for( auto e = mc.constBegin(); e != mc.constEnd(); ++e ) {
    auto it = new QStandardItem( e.key() );
    it0->appendRow( it );
  }

  for( auto e: children() ) {

    if( auto hd = qobject_cast<HolderDouble*>( e ) ) {
      auto it = new QStandardItem( prefix % hd->objectName() );
      it0->appendRow( it ); ++n;
      continue;
    }

    if( auto hda = qobject_cast<HolderDoubleArray*>( e ) ) {
      auto it = new QStandardItem( prefix % hda->objectName() + QSL("[0]") );
      it0->appendRow( it ); ++n;
      continue;
    }

    if( auto el = qobject_cast<TDataSet*>(e) ) {
      auto it = new QStandardItem( prefix % el->objectName() );
      int na = el->fillComplForInputs( it, prefix );
      if( na > 0 ) { // append row for elem only if at least one final completition
        it0->appendRow( it ); ++n;
      } else {
        delete it;
      }
    }

  }
  return n;
}

QStringList HolderData::getNamesOfType( const QString &subObjName,
       const QString ob_tp, bool withType ) const
{
  QStringList r;
  const HolderData *subObj = this;
  if( ! subObjName.isEmpty() && subObjName != QSL(".") ) {
    subObj = getObj( subObjName );
  }
  if( !subObj ) { return r; }
  // qWarning() << "Parent: " << subObj->getFullName() << NWHE;

  bool ignoreType = ob_tp.isEmpty();
  for( auto ho: subObj->TCHILD(HolderData*) ) {
    if( ignoreType || EFACT.isChildOf( ho->getType(), ob_tp ) ) { // not ho->isChildOf: aliases
      QString s = ho->objectName();
      if( withType ) {
        QString val;
        if( ! ho->isChildOf( QSL("TDataSet") ) ) {
          val = QSL(" = \"") % ho->toString() % QSL("\"");
        }
        s += QSL(" ") % ho->getType() % val;
      }
      r << s;
    }
  }
  return r;
}

void HolderData::dumpStruct() const
{
  qDebug() << toString() << NWHE;
}

void HolderData::post_set()
{
  do_post_set();
  if( hasAllFlags( efPostParent ) && par != nullptr ) {
    par->post_set();
  }
  if( hasAllFlags( efStructCh ) && par != nullptr ) {
    reportStructChanged();
  }
}

void HolderData::do_post_set()
{
  qWarning() << ERR_ABS << NWHE;
}



const char* HolderData::helpstr { "Abstract data holder" };

// ---------------- HolderValue ---------
STD_CLASSINFO(HolderValue,clpData|clpAbstract);

CTOR(HolderValue,HolderData)
{
}

void HolderValue::reset_dfl()
{
  set( getParm( QSL("def") ) );
}

void HolderValue::do_post_set()
{
  qWarning() << ERR_ABS << NWHE;
}


bool HolderValue::set( const QVariant & /*x*/, int /* idx */ )
{
  qWarning() << ERR_ABS << NWHE;
  return false;
}

QVariant HolderValue::get( int /* idx */ ) const
{
  qWarning() << ERR_ABS << NWHE;
  return QVariant();
}

double HolderValue::getDouble( int /*idx = 0*/ ) const
{
  qWarning() << ERR_ABS << NWHE;
  return 0.0;
}

QString HolderValue::toString() const
{
  qWarning() << ERR_ABS << NWHE;
  return QString();
}

QString HolderValue::textVisual() const
{
  return QSL("--HolderValue--");
}

bool HolderValue::fromString( const QString & /*s */ )
{
  qWarning() << ERR_ABS << NWHE;
  return false;
}

const char* HolderValue::helpstr { "Parent of all data" };


DEFAULT_FUNCS_REG(HolderValue);



// ---------------- HolderInt ---------
STD_CLASSINFO_ALIAS(HolderInt,clpData,int);

CTOR(HolderInt,HolderValue) , v(0)
{
  tp=QVariant::Int;
  setParmIfEmpty( QSL("props"), QSL("INT,SIMPLE") );
  reset_dfl();
}


bool HolderInt::set( const QVariant & x, int /* idx */ )
{
  auto v0 = v;
  bool ok;
  if( x.type() == QVariant::Int ) {
    v = x.toInt( &ok );
  } else {
    v = QString2IntEx( x.toString(), &ok );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderInt::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderInt::getDouble( int /* idx = 0 */ ) const
{
  return v;
}

void HolderInt::do_post_set()
{
  int v_min = getParmInt( QSL("min"), IMIN );
  int v_max = getParmInt( QSL("max"), IMAX );
  v = clamp( v, v_min, v_max );
}

QString HolderInt::toString() const
{
  return QSN( v );
}

QString HolderInt::textVisual() const
{
  return toString();
}

bool HolderInt::fromString( const QString &s )
{
  return set( s );
}

const char* HolderInt::helpstr { "Contains integer data" };


DEFAULT_FUNCS_REG(HolderInt);


// ---------------- HolderSwitch ---------
STD_CLASSINFO_ALIAS(HolderSwitch,clpData,switch);

CTOR(HolderSwitch,HolderInt)
{
  if( getParm( QSL("props") ) == QSL("INT,SIMPLE") ) {
    setParm( QSL("props"), QSL("INT,SWITCH") );
  }
  setParm( QSL("min"), QSL("0") ); setParm( QSL("max"), QSL("1") );
  reset_dfl();
}


void HolderSwitch::do_post_set()
{
  v = v ? 1 : 0;
}

const char* HolderSwitch::helpstr { "Contains integer (bin) data - switch iface" };

DEFAULT_FUNCS_REG(HolderSwitch);


// ---------------- HolderList ---------
STD_CLASSINFO_ALIAS(HolderList,clpData,list);

CTOR(HolderList,HolderInt)
{
  setParm( QSL("min"), QSL("0") ); setParm( QSL("max"), QSL("0") );
  if( getParm(QSL("props")) == QSL("INT,SIMPLE") ) {
    setParm( QSL("props"), QSL("INT,LIST") );
  }
  reset_dfl();
}

void HolderList::reset_dfl()
{
  fillListStrings();
  HolderInt::reset_dfl();
}

void HolderList::do_post_set()
{
  HolderInt::do_post_set();
}


QString HolderList::textVisual() const
{
  if( v < 0 || v >= list_strings.size() ) {
    return QSN(v) % QSL("?");
  }
  return list_strings[v] % QSL(" (") % QSN(v) % QSL(")");
}


const char* HolderList::helpstr { "Contains integer data - list iface" };

DEFAULT_FUNCS_REG(HolderList);


// ---------------- HolderLong ---------
STD_CLASSINFO_ALIAS(HolderLong,clpData,long);

CTOR(HolderLong,HolderValue) , v(0)
{
  tp=QVariant::LongLong;
  setParmIfEmpty( QSL("props"), QSL("LONG,SIMPLE") );
  reset_dfl();
}


bool HolderLong::set( const QVariant & x, int /* idx */ )
{
  auto v0 = v;
  bool ok;
  if( x.type() == QVariant::LongLong ) {
    v = (long)x.toLongLong( &ok );
  } else if( x.type() == QVariant::Int ) {
    v = x.toInt( &ok );
  } else {
    v = QString2LongEx( x.toString(), &ok );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderLong::get( int /* idx */ ) const
{
  return QVariant( (long long)v );
}

double HolderLong::getDouble( int /* idx = 0 */ ) const
{
  return v;
}

void HolderLong::do_post_set()
{
  auto v_min = getParmLong( QSL("min"), LMIN );
  auto v_max = getParmLong( QSL("max"), LMAX );
  v = clamp( v, v_min, v_max );
}

QString HolderLong::toString() const
{
  return QSN( v );
}

QString HolderLong::textVisual() const
{
  return toString();
}

bool HolderLong::fromString( const QString &s )
{
  return set( s );
}

const char* HolderLong::helpstr { "Contains long integer data" };


DEFAULT_FUNCS_REG(HolderLong);



// ---------------- HolderDouble ---------
STD_CLASSINFO_ALIAS(HolderDouble,clpData,double);

CTOR(HolderDouble,HolderValue), v(0)
{
  tp=QVariant::Double;
  post_set();
  setParmIfEmpty( QSL("props"), QSL("DOUBLE,SIMPLE") );
  reset_dfl();
}


bool HolderDouble::set( const QVariant & x, int /* idx */ )
{
  bool ok;
  auto v0 = v;
  if( x.type() == QVariant::String ) {
    v = toDoubleEx( x.toString(), &ok );
  } else {
    v = x.toDouble( &ok );
  }
  post_set();
  if( v != v0 ) { // sic! even one bit does metter
    setModified();
  }
  return ok;
}

QVariant HolderDouble::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderDouble::getDouble( int /* idx = 0 */ ) const
{
  return v;
}

void HolderDouble::do_post_set()
{
  double v_min = getParmDouble( QSL("min"), DMIN );
  double v_max = getParmDouble( QSL("max"), DMAX );
  v = clamp( v, v_min, v_max );
}

QString HolderDouble::toString() const
{
  return QSND( v );
}

QString HolderDouble::textVisual() const
{
  return toString();
}

bool HolderDouble::fromString( const QString &s )
{
  return set( s );
}


const char* HolderDouble::helpstr { "Contains double data" };

DEFAULT_FUNCS_REG(HolderDouble);


// ---------------- HolderString ---------
STD_CLASSINFO_ALIAS(HolderString,clpData,string);

CTOR(HolderString,HolderValue)
{
  tp=QVariant::String;
  post_set();
  setParmIfEmpty( QSL("props"), QSL("STRING,SIMPLE") );
  reset_dfl();
}


bool HolderString::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  v = x.toString();
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderString::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderString::getDouble( int /* idx = 0 */ ) const
{
  return toDoubleEx( v );
}

void HolderString::do_post_set()
{
  int v_max = getParmInt( QSL("max"), IMAX );
  v.truncate( v_max );
}

QString HolderString::toString() const
{
  return v;
}

QString HolderString::textVisual() const
{
  return QSL("\"") % v % QSL("\"");
}

bool HolderString::fromString( const QString &s )
{
  return set( s );
}


const char* HolderString::helpstr { "Contains QString data" };

DEFAULT_FUNCS_REG(HolderString);


// ---------------- HolderColor ---------
STD_CLASSINFO_ALIAS(HolderColor,clpData,color);

CTOR(HolderColor,HolderValue)
{
  tp=QVariant::Color;
  v = QColor(Qt::red);
  setParmIfEmpty( QSL("props"), QSL("COLOR,INT") );
  reset_dfl();
}


bool HolderColor::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  if( x.type() == QVariant::Color ) {
    v = x.value<QColor>();
  } else {
    QString s = x.toString();
    v = QColor( s );
    if( ! v.isValid() ) {
      QRgb rgba = x.toInt();
      v.setRgba( rgba );
    }
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderColor::get( int /* idx */ ) const
{
  // return QVariant( (int)(v.rgba()) );
  return QVariant( v );
}

double HolderColor::getDouble( int /* idx = 0 */ ) const
{
  return v.lightnessF();
}

void HolderColor::do_post_set()
{
  if( ! v.isValid() ) {
    v = QColor(Qt::red);
  }
}

QString HolderColor::toString() const
{
  if( v.alpha() == 255 ) {
    return v.name();
  }
  QString s = QString::asprintf( "#%08x", v.rgba() );
  return s;
}

QString HolderColor::textVisual() const
{
  return toString();
}

bool HolderColor::fromString( const QString &s )
{
  return set( s );
}


const char* HolderColor::helpstr { "Contains QColor data" };

DEFAULT_FUNCS_REG(HolderColor);

// ---------------- HolderFont ---------
STD_CLASSINFO_ALIAS(HolderFont,clpData,font);

CTOR(HolderFont,HolderValue)
{
  tp=QVariant::Font;
  setParmIfEmpty( QSL("props"), QSL("FONT,STRING") );
  reset_dfl();
}



bool HolderFont::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  QFont vt; // default
  if( x.type() == QVariant::Font ) {
    v = x.value<QFont>();
  } else {
    QString fn = x.toString();
    if(  fn == QSL("DEFAULT")  ||  fn.isEmpty()  ) {
      v = vt;
    } else if( fn == QSL("SMALL") ) {
      vt.setPointSize( vt.pointSize() * 7 / 10 );
      v = vt;
    } else if( fn == QSL("TINY") ) {
      vt.setPointSize( vt.pointSize() * 5 / 10 );
      v = vt;
    } else if( fn == QSL("LARGE") ) {
      vt.setPointSize( vt.pointSize() * 14 / 10 );
      v = vt;
    } else {
      v.fromString( fn );
    }
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderFont::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderFont::getDouble( int /* idx = 0 */ ) const
{
  return v.pointSize();
}

void HolderFont::do_post_set()
{
}

QString HolderFont::toString() const
{
  return v.toString();
}

QString HolderFont::textVisual() const
{
  return toString();
}

bool HolderFont::fromString( const QString &s )
{
  return set( s );
}


const char* HolderFont::helpstr { "Contains QFont data" };

DEFAULT_FUNCS_REG(HolderFont);


// ---------------- HolderDate ---------
STD_CLASSINFO_ALIAS(HolderDate,clpData,date);

CTOR(HolderDate,HolderValue)
{
  tp=QVariant::Date;
  setParmIfEmpty( QSL("props"), QSL("DATE,STRING") );
  reset_dfl();
}


bool HolderDate::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  if( x.type() == QVariant::Date ) {
    v = x.toDate();
  } else if( x.type() == QVariant::Int ) {
    v = QDate::fromJulianDay( x.toInt() );
  } else {
    v = QDate::fromString( x.toString(), DATE_FORMAT );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderDate::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderDate::getDouble( int /* idx = 0 */ ) const
{
  return (double)( v.toJulianDay() );
}

void HolderDate::do_post_set()
{
  if( ! v.isValid() ) {
    v = QDate( 1970, 0, 0 );
  }
  QString s_min = getParm( QSL("min") );
  if( ! s_min.isEmpty() ) {
    QDate d_min =  QDate::fromString( s_min, DATE_FORMAT );
    if( v < d_min ) { v = d_min; }
  }
  QString s_max = getParm( QSL("max") );
  if( ! s_max.isEmpty() ) {
    QDate d_max =  QDate::fromString( s_max, DATE_FORMAT );
    if( v > d_max ) { v = d_max; }
  }
}

QString HolderDate::toString() const
{
  return v.toString( DATE_FORMAT );
}

QString HolderDate::textVisual() const
{
  return toString();
}

bool HolderDate::fromString( const QString &s )
{
  return set( s );
}


const char* HolderDate::helpstr { "Contains QDate data" };

DEFAULT_FUNCS_REG(HolderDate);



// ---------------- HolderTime ---------
STD_CLASSINFO_ALIAS(HolderTime,clpData,time);

CTOR(HolderTime,HolderValue)
{
  tp=QVariant::Time;
  setParmIfEmpty( QSL("props"), QSL("TIME,STRING") );
  reset_dfl();
}


bool HolderTime::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  if( x.type() == QVariant::Time ) {
    v = x.toTime();
  } else if( x.type() == QVariant::Int ) {
    v = QTime::fromMSecsSinceStartOfDay( x.toInt() );
  } else {
    v = QTime::fromString( x.toString(), TIME_FORMAT );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderTime::get( int /* idx */ ) const
{
  return QVariant( v );
}

double HolderTime::getDouble( int /* idx = 0 */ ) const
{
  return 0.001 * (double)( v.msecsSinceStartOfDay() ); // to seconds
}

void HolderTime::do_post_set()
{
  if( ! v.isValid() ) {
    v = QTime( 0, 0, 0 );
  }
  QString s_min = getParm( QSL("min") );
  if( ! s_min.isEmpty() ) {
    QTime t_min =  QTime::fromString( s_min, TIME_FORMAT );
    if( v < t_min ) { v = t_min; }
  }
  QString s_max = getParm( QSL("max") );
  if( ! s_max.isEmpty() ) {
    QTime t_max =  QTime::fromString( s_max, TIME_FORMAT );
    if( v > t_max ) { v = t_max; }
  }
}

QString HolderTime::toString() const
{
  return v.toString( TIME_FORMAT );
}

QString HolderTime::textVisual() const
{
  return toString();
}

bool HolderTime::fromString( const QString &s )
{
  return set( s );
}


const char* HolderTime::helpstr { "Contains QTime data" };

DEFAULT_FUNCS_REG(HolderTime);


// ---------------- HolderIntArray ---------
STD_CLASSINFO_ALIAS(HolderIntArray,clpData|clpArray,int[]);

CTOR(HolderIntArray,HolderValue)
{
  tp=QVariant::UserType;
  setParmIfEmpty( QSL("props"), QSL("ARRAY_INT") );
  // all done by reset_dfl
  reset_dfl();
}

void HolderIntArray::reset_dfl()
{
  auto v0 = v;
  int n  = getParmInt( QSL("N"), 1 );
  int v1 = getParmInt( QSL("def"), 0 );
  v.assign( n, v1 );

  auto s = getParm(QSL("defs"));
  if( ! s.isEmpty() ) {
    QStringList sl = s.split( QSL(" "), Qt::SkipEmptyParts );
    if( sl.size() > (int)v.size() ) {
      v.assign( sl.size(), v1 );
    }

    int i = 0;
    bool ok;
    for( const auto &cs : sl ) {
      int vc = cs.toInt( &ok, 0 ); // 0 = auto base
      if( ok ) {
        v[i] = vc;
      }
      i++;
    }
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderIntArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( ! isGoodIndex( idx, v ) ) { // slow, but safe - not for fast code
    return false;
  }
  auto v0 = v[idx];
  v[idx] = x.toInt( &ok );
  post_set();
  if( v[idx] != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderIntArray::get( int idx ) const
{
  if( ! isGoodIndex( idx, v ) ) { // slow, but safe - not for fast code
    return QVariant();
  }
  return QVariant( v[idx] );
}

double HolderIntArray::getDouble( int idx ) const
{
  if( ! isGoodIndex( idx, v ) ) {
    return 0.0;
  }
  return v[idx];
  // return atd( v, idx, 0.0 );
}

void HolderIntArray::do_post_set()
{
  unsigned n = getParmInt( QSL("N"), 1 );
  if( n != v.size() ) {
    int v1 = getParmInt( QSL("def"), 0 );
    v.resize( n, v1 );
  }
  int v_min = getParmInt( QSL("min"), IMIN );
  int v_max = getParmInt( QSL("max"), IMAX );
  for( int& vc : v ) {
    vc = clamp( vc, v_min, v_max );
  }
}

QString HolderIntArray::toString() const
{
  QString s, sep = "";
  for( int vc : v ) {
    s += sep + QSN( vc );
    sep = QSL(" ");
  }
  return s;
}

QString HolderIntArray::textVisual() const
{
  QString s { QSL("[ ") }, sep;
  for( auto vc : v ) {
    s += sep + QSN( vc );
    sep = QSL(", ");
  }
  s += QSL(" ]");
  return s;
}

bool HolderIntArray::fromString( const QString &s )
{
  bool ok;
  const QStringList sl = s.split( QSL(" "), Qt::SkipEmptyParts );
  auto v0 = v;
  v.clear(); v.reserve( sl.size() );

  for( const auto &s : sl ) {
    int vc = s.toInt( &ok, 0 ); // 0 = auto base
    v.push_back( vc );
  }

  post_set();
  if( v != v0 ) { // slow
    setModified();
  }
  return ok; // ? only last
}


const char* HolderIntArray::helpstr { "Contains vector of integer data" };


DEFAULT_FUNCS_REG(HolderIntArray);


// ---------------- HolderDoubleArray ---------
STD_CLASSINFO_ALIAS(HolderDoubleArray,clpData|clpArray,double[]);

CTOR(HolderDoubleArray,HolderValue)
{
  tp=QVariant::UserType;
  setParmIfEmpty( QSL("props"), QSL("ARRAY_DOUBLE") );
  reset_dfl();
}

void HolderDoubleArray::reset_dfl()
{
  int n = getParmInt( QSL("N"), 1 );
  auto v0 = v;

  double v1 = getParmDouble( QSL("def"), 0.0 );
  v.assign( n, v1 );

  auto s = getParm( QSL("defs") );
  if( ! s.isEmpty() ) {
    const QStringList sl = s.split( QSL(" "), Qt::SkipEmptyParts );
    if( sl.size() > (int)v.size() ) {
      v.assign( sl.size(), v1 );
    }

    int i = 0;
    bool ok;
    for( const auto &cs : sl ) {
      double vc = toDoubleEx( cs, &ok );
      if( ok ) {
        v[i] = vc;
      }
      i++;
    }
  }

  post_set();
  if( v != v0 ) { // slow
    setModified();
  }
}


bool HolderDoubleArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( ! isGoodIndex( idx, v ) ) { // slow, but safe - not for fast code
    return false;
  }
  auto v0 = v[idx];
  if( x.type() == QVariant::String ) {
    v[idx] = toDoubleEx( x.toString(), &ok );
  } else {
    v[idx] = x.toDouble( &ok );
  }
  post_set();
  if( v[idx] != v0 ) { // sic! obe bit does metter
    setModified();
  }
  return ok;
}

QVariant HolderDoubleArray::get( int idx ) const
{
  if( ! isGoodIndex( idx, v ) ) { // slow, but safe - not for fast code
    return QVariant();
  }
  return QVariant( v[idx] );
}

double HolderDoubleArray::getDouble( int idx ) const
{
  // return atd( v, idx, 0.0 );
  if( ! isGoodIndex( idx, v ) ) {
    return 0.0;
  }
  return v[idx];
}

void HolderDoubleArray::do_post_set()
{
  unsigned n = getParmInt( QSL("N"), 1 );
  if( n != v.size() ) {
    double v1 = getParmDouble( QSL("def"), 0.0 );
    v.resize( n, v1 );
  }
  double v_min = getParmDouble( QSL("min"), DMIN );
  double v_max = getParmDouble( QSL("max"), DMAX );
  for( double& vc : v ) {
    vc = clamp( vc, v_min, v_max );
  }
}

QString HolderDoubleArray::toString() const
{
  QString s, sep = "";
  for( double vc : v ) {
    s += sep + QSND( vc );
    sep = QSL(" ");
  }
  return s;
}

QString HolderDoubleArray::textVisual() const
{
  QString s { QSL("[ ") }, sep;
  for( auto vc : v ) {
    s += sep + QSND( vc );
    sep = QSL(", ");
  }
  s += QSL(" ]");
  return s;
}

bool HolderDoubleArray::fromString( const QString &s )
{
  bool ok;
  const QStringList sl = s.split( QSL(" "), Qt::SkipEmptyParts );
  auto v0 = v;
  v.clear(); v.reserve( sl.size() );

  for( const auto &s : sl ) {
    double vc = toDoubleEx( s, &ok );
    v.push_back( vc );
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok; // ? only last
}

const char* HolderDoubleArray::helpstr { "Contains vector of double data" };


DEFAULT_FUNCS_REG(HolderDoubleArray);


// ---------------- HolderStringArray ---------
STD_CLASSINFO_ALIAS(HolderStringArray,clpData|clpArray,string[]);

CTOR(HolderStringArray,HolderValue)
{
  tp=QVariant::UserType;
  setParmIfEmpty( QSL("props"), QSL("ARRAY_STRING") );
  reset_dfl();
}

void HolderStringArray::reset_dfl()
{
  int n = getParmInt( QSL("N"), 1 );
  auto v0 = v;
  auto sd = getParm( QSL("def") );

  QStringList sl;
  auto s = getParm( QSL("defs") );
  if( ! s.isEmpty() ) {
    sl = s.split( '\x01', Qt::KeepEmptyParts ); // keep here
    if( sl.size() > n ) {
      n = sl.size();
    }
  }

  v.clear();
  int nds = sl.size();
  for( int i=0; i<n; ++i ) {
    QString vc;
    if( i < nds ) {
      vc = sl[i];
    } else {
      vc = sd;
    }
    v.append( vc );
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderStringArray::set( const QVariant & x, int idx )
{
  if( ! isGoodIndex( idx, v ) ) {
    return false;
  }
  auto v0 = v[idx];
  v[idx] = x.toString();
  post_set();
  if( v[idx] != v0 ) {
    setModified();
  }
  return ! v[idx].isEmpty();
}

QVariant HolderStringArray::get( int idx ) const
{
  if( idx < 0 ) {
    return v;
  }
  if( idx >= v.size() ) { // slow, but safe - not for fast code
    return QVariant();
  }
  return QVariant( v[idx] );
}

double HolderStringArray::getDouble( int idx ) const
{
  if( ! isGoodIndex( idx, v ) ) {
    return 0;
  }
  return toDoubleEx( v[idx] );
}

void HolderStringArray::do_post_set()
{
  int n = getParmInt( QSL("N"), 1 );
  auto v1 = getParm( QSL("def") );
  if( n > v.size() ) {
    v << v1;
  }
  int len_max = getParmInt( QSL("max"), -1 );
  if( len_max > -1 ) {
    for( QString& vc : v ) {
      vc.truncate(len_max);
    }
  }
}

QString HolderStringArray::toString() const
{
  return v.join( QChar(0x2400) ); // NUL representation in Unicode
}

QString HolderStringArray::textVisual() const
{
  QString s { QSL("[ ") }, sep;
  for( const auto &vc : qAsConst( v ) ) {
    s += sep % QSL("\"") % vc  % QSL("\"");
    if( sep.isEmpty() ) {
      sep = QSL(", ");
    }
  }
  s += QSL(" ]");
  return s;
}

bool HolderStringArray::fromString( const QString &s )
{
  auto v0 = v;
  if( s.contains( QChar(0x2400) ) ) {
    v = s.split( QChar(0x2400), Qt::KeepEmptyParts );
  } else {
    v = s.split('\x01', Qt::KeepEmptyParts ); // old strings
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
  return !v.isEmpty();
}


const char* HolderStringArray::helpstr { "Contains QStringList" };


DEFAULT_FUNCS_REG(HolderStringArray);


//=====================================================================
//=====================================================================
// ---------------- TDataSet ------------------------
STD_CLASSINFO(TDataSet,clpSpecial|clpAbstract);


const char* TDataSet::helpstr =
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

CTOR(TDataSet,HolderData)
{
  tp=QVariant::UserType;
  setParmIfEmpty( QSL("props"), QSL("OBJ") );
  reset_dfl();
}

TDataSet::~TDataSet()
{
  state = stateBad;
}


// ----------------------------------


void TDataSet::reset_dfl()
{
  for_type<HolderData>( []( auto ho ) { ho->reset_dfl(); } );
}


DEFAULT_FUNCS_REG(TDataSet);


bool TDataSet::set( const QVariant & x, int /* idx */  )
{
  bool ok =  fromString( x.toString() );
  post_set();
  return ok;
}

QVariant TDataSet::get( int /* idx */ ) const
{
  return QVariant( this->toString() );
}

double TDataSet::getDouble( int /* idx = 0 */ ) const
{
  return 0.0;
}

QString TDataSet::toString() const
{
  return toXML();
}

QString TDataSet::textVisual() const
{
  return QSL("{") % QSN( size() ) % QSL("}");
}

bool TDataSet::fromString( const QString &s )
{
  QString errstr;
  int err_line, err_column;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    qWarning() << "fail to parse str: line " << err_line << " col: " << err_column
               << " str: " << errstr << NWHE;
    return false;
  }
  QDomElement domroot = x_dd.documentElement();

  return fromDom( domroot, errstr );
}

QDomElement TDataSet::toDom( QDomDocument &dd, bool forceType ) const
{
  QDomElement de = dd.createElement( QSL("obj") );
  de.setAttribute( QSL("name"), objectName() );
  de.setAttribute( QSL("otype"), getType() );

  if( dyn || forceType ) { // func
    de.setAttribute( QSL("dyn"), QSL("1") );
    saveParmsToDom( de );
  }

  for_type_c<HolderData>( [&de,&dd,forceType]( auto ho ) {
      if( ! ho->hasAnyFlags( efNoSave ) && ! ho->hasClassProps( clpObsolete ) ) {
        QDomElement cde = ho->toDom( dd, forceType );
        de.appendChild( cde );
      }
  } );

  return de;
}


void TDataSet::do_post_set()
{
  // do nothing
}


// end of dataset.cpp

